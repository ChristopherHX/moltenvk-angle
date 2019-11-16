//
// Copyright 2019 Le Hoang Quyen. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#import "MGLLayer+Private.h"

#include <vector>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglext_angle.h>
#include <EGL/eglplatform.h>
#include <common/debug.h>
#include <libGLESv2/entry_points_gles_2_0_autogen.h>
#include <libGLESv2/entry_points_gles_ext_autogen.h>
#import "MGLContext+Private.h"
#import "MGLDisplay.h"

namespace
{

constexpr GLchar kBlitVS[] = R"(
attribute vec2 aPosition;

varying mediump vec2 vTexcoords;

void main()
{
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    vTexcoords = 0.5 * (aPosition + vec2(1.0, 1.0));
}
)";

constexpr GLchar kBlitFS[] = R"(
uniform sampler2D uTexture;

varying mediump vec2 vTexcoords;

void main()
{
    gl_FragColor = texture2D(uTexture, vTexcoords);
}
)";

template <GLenum State>
class ScopedGLEnable
{
  public:
    ScopedGLEnable(bool enable)
    {
        gl::GetIntegerv(State, &mPrevState);

        if (enable)
        {
            gl::Enable(State);
        }
        else
        {
            gl::Disable(State);
        }
    }
    ~ScopedGLEnable()
    {
        if (mPrevState)
        {
            gl::Enable(State);
        }
        else
        {
            gl::Disable(State);
        }
    }

  private:
    GLint mPrevState;
};

template <GLenum Target, GLenum TargetBindingGet, void (*BindFunc)(GLenum, GLuint)>
class ScopedGLBinding
{
  public:
    ScopedGLBinding(GLuint newObj)
    {
        gl::GetIntegerv(TargetBindingGet, &mPrevBoundObj);
        BindFunc(Target, newObj);
    }
    ~ScopedGLBinding() { BindFunc(Target, mPrevBoundObj); }

  private:
    GLint mPrevBoundObj;
};

using ScopedTextureBind = ScopedGLBinding<GL_TEXTURE_2D, GL_TEXTURE_BINDING_2D, gl::BindTexture>;
using ScopedBufferBind  = ScopedGLBinding<GL_ARRAY_BUFFER, GL_ARRAY_BUFFER_BINDING, gl::BindBuffer>;
using ScopedRenderbufferBind =
    ScopedGLBinding<GL_RENDERBUFFER, GL_RENDERBUFFER_BINDING, gl::BindRenderbuffer>;
using ScopedFBOBind = ScopedGLBinding<GL_FRAMEBUFFER, GL_FRAMEBUFFER_BINDING, gl::BindFramebuffer>;

class ScopedProgramBind
{
  public:
    ScopedProgramBind(GLuint program)
    {
        gl::GetIntegerv(GL_CURRENT_PROGRAM, &mPrevProgram);
        gl::UseProgram(program);
    }
    ~ScopedProgramBind() { gl::UseProgram(mPrevProgram); }

  private:
    GLint mPrevProgram;
};

class ScopedActiveTexture
{
  public:
    ScopedActiveTexture(GLenum unit)
    {
        gl::GetIntegerv(GL_ACTIVE_TEXTURE, &mPrevActiveTexture);
        gl::ActiveTexture(unit);
    }
    ~ScopedActiveTexture() { gl::ActiveTexture(mPrevActiveTexture); }

  private:
    GLint mPrevActiveTexture;
};

struct ScopedVAOBind
{
  public:
    ScopedVAOBind(GLuint vao)
    {
        gl::GetIntegerv(GL_VERTEX_ARRAY_BINDING_OES, &mPrevVAO);
        gl::BindVertexArrayOES(vao);
    }
    ~ScopedVAOBind() { gl::BindVertexArrayOES(mPrevVAO); }

  private:
    GLint mPrevVAO;
};

class ScopedTexture
{
  public:
    ScopedTexture() : mTexture(0) {}
    ScopedTexture(GLuint texture) : mTexture(texture) {}
    ~ScopedTexture()
    {
        if (mTexture)
        {
            gl::DeleteTextures(1, &mTexture);
        }
        mTexture = 0;
    }

    ScopedTexture &operator=(GLuint texture)
    {
        mTexture = texture;
        return *this;
    }

    operator GLuint() const { return get(); }

    GLuint get() const { return mTexture; }

  private:
    GLuint mTexture;
};

class ScopedViewport
{
  public:
    ScopedViewport(GLint x, GLint y, GLint width, GLint height)
    {
        gl::GetIntegerv(GL_VIEWPORT, mPrevViewport);
        gl::Viewport(x, y, width, height);
    }
    ~ScopedViewport()
    {
        gl::Viewport(mPrevViewport[0], mPrevViewport[1], mPrevViewport[2], mPrevViewport[3]);
    }

  private:
    GLint mPrevViewport[4];
};

void Throw(NSString *msg)
{
    [NSException raise:@"MGLSurfaceException" format:@"%@", msg];
}

GLint CompileShader(GLenum target, const GLchar *source, GLuint *shader)
{
    GLint logLength, status;

    *shader                 = gl::CreateShader(target);
    const GLchar *sources[] = {source};
    gl::ShaderSource(*shader, 1, sources, NULL);
    gl::CompileShader(*shader);
    gl::GetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        gl::GetShaderInfoLog(*shader, logLength, &logLength, log);
        NSLog(@"Shader compile log:\n%s", log);
        free(log);
    }

    gl::GetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        NSLog(@"Failed to compile shader:\n");
        NSLog(@"%s", source);
    }

    return status && *shader;
}

GLint LinkProgram(GLuint program)
{
    GLint logLength, status;

    gl::LinkProgram(program);
    gl::GetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        gl::GetProgramInfoLog(program, logLength, &logLength, log);
        NSLog(@"Program link log:\n%s", log);
        free(log);
    }

    gl::GetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == 0)
        NSLog(@"Failed to link program %d", program);

    return status && program;
}
}

// MGLLayer implementation
@implementation MGLLayer

- (id)init
{
    if (self = [super init])
    {
        [self constructor];
    }
    return self;
}

- (id)initWithLayer:(id)layer
{
    if (self = [super initWithLayer:layer])
    {
        [self constructor];
    }
    return self;
}

- (id)initWithCoder:(NSCoder *)coder
{
    if (self = [super initWithCoder:coder])
    {
        [self constructor];
    }
    return self;
}

- (void)constructor
{
    _drawableColorFormat   = MGLDrawableColorFormatRGBA8888;
    _drawableDepthFormat   = MGLDrawableDepthFormatNone;
    _drawableStencilFormat = MGLDrawableStencilFormatNone;

    _display = [MGLDisplay defaultDisplay];

    _eglSurface = EGL_NO_SURFACE;

    _metalLayer       = [[CAMetalLayer alloc] init];
    _metalLayer.frame = self.bounds;
    [self addSublayer:_metalLayer];
}

- (void)dealloc
{
    [self releaseSurface];

    _display = nil;
}

- (void)setContentsScale:(CGFloat)contentsScale
{
    [super setContentsScale:contentsScale];
    _metalLayer.contentsScale = contentsScale;
}

- (CGSize)drawableSize
{
    return _metalLayer.drawableSize;
}

- (BOOL)setCurrentContext:(MGLContext *)context
{
    if (eglGetCurrentContext() != context.eglContext ||
        eglGetCurrentSurface(EGL_READ) != self.eglSurface ||
        eglGetCurrentSurface(EGL_DRAW) != self.eglSurface)
    {
        if (!eglMakeCurrent(_display.eglDisplay, self.eglSurface, self.eglSurface,
                            context.eglContext))
        {
            return NO;
        }
    }

    if (_retainedBacking)
    {
        GLint currentFBO;
        gl::GetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
        BOOL offscreenFBOWasBound = currentFBO == _defaultOpenGLFrameBufferID;

        if (![self ensureOffscreenFBOCreated])
        {
            return NO;
        }

        if (offscreenFBOWasBound)
        {
            // Draw to offscreen texture instead of eglSurface
            gl::BindFramebuffer(GL_FRAMEBUFFER, _defaultOpenGLFrameBufferID);
        }
    }

    return YES;
}

- (BOOL)present
{
    if (_retainedBacking)
    {
        if (![self blitOffscreenTexture:_offscreenTexture toFBO:0])
        {
            return NO;
        }
    }

    if (!eglSwapBuffers(_display.eglDisplay, self.eglSurface))
    {
        return NO;
    }

    [self checkLayerSize];

    return YES;
}

- (BOOL)blitOffscreenTexture:(GLuint)texture toFBO:(GLuint)fbo
{
    auto currentCtx   = [MGLContext currentContext];
    auto currentLayer = [MGLContext currentLayer];
    [MGLContext setCurrentContext:_offscreenFBOCreatorContext forLayer:self];

    ScopedFBOBind bindFBO(fbo);
    ScopedProgramBind bindProgram(_offscreenBlitProgram);
    ScopedActiveTexture activeTexture(GL_TEXTURE0);
    ScopedTextureBind bindTexture(texture);
    ScopedVAOBind bindVAO(_offscreenBlitVAO);

    ScopedGLEnable<GL_CULL_FACE> disableCull(false);
    ScopedGLEnable<GL_DEPTH_TEST> disableDepthTest(false);
    ScopedGLEnable<GL_STENCIL_TEST> disableStencilTest(false);
    ScopedGLEnable<GL_BLEND> disableBlend(false);
    ScopedGLEnable<GL_SCISSOR_TEST> disableScissorTest(false);

    ScopedViewport setViewport(0, 0, static_cast<GLint>(_offscreenFBOSize.width),
                               static_cast<GLint>(_offscreenFBOSize.height));

    gl::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    BOOL re = gl::GetError() == GL_NO_ERROR;

    [MGLContext setCurrentContext:currentCtx forLayer:currentLayer];

    return re;
}

- (EGLSurface)eglSurface
{
    [self ensureSurfaceCreated];

    return _eglSurface;
}

- (void)setDrawableColorFormat:(MGLDrawableColorFormat)drawableColorFormat
{
    _drawableColorFormat = drawableColorFormat;
    [self releaseSurface];
}

- (void)setDrawableDepthFormat:(MGLDrawableDepthFormat)drawableDepthFormat
{
    _drawableDepthFormat = drawableDepthFormat;
    [self releaseSurface];
}

- (void)setDrawableStencilFormat:(MGLDrawableStencilFormat)drawableStencilFormat
{
    _drawableStencilFormat = drawableStencilFormat;
    [self releaseSurface];
}

- (void)releaseSurface
{
    if (_eglSurface == eglGetCurrentSurface(EGL_READ) ||
        _eglSurface == eglGetCurrentSurface(EGL_DRAW))
    {
        eglMakeCurrent(_display.eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if (_eglSurface != EGL_NO_SURFACE)
    {
        eglDestroySurface(_display.eglDisplay, _eglSurface);
        _eglSurface = EGL_NO_SURFACE;
    }
    [self releaseOffscreenRenderingResources];
}

- (void)releaseOffscreenRenderingResources
{
    if (_defaultOpenGLFrameBufferID)
    {
        auto oldRetainBackingFlag = _retainedBacking;
        // Avoid the buffer being created again inside setCurrentContext:
        _retainedBacking  = NO;
        auto currentCtx   = [MGLContext currentContext];
        auto currentLayer = [MGLContext currentLayer];
        [MGLContext setCurrentContext:_offscreenFBOCreatorContext];

        gl::DeleteFramebuffers(1, &_defaultOpenGLFrameBufferID);
        _defaultOpenGLFrameBufferID = 0;

        gl::DeleteTextures(1, &_offscreenTexture);
        _offscreenTexture = 0;
        gl::DeleteRenderbuffers(1, &_offscreenDepthStencilBuffer);
        _offscreenDepthStencilBuffer = 0;
        gl::DeleteVertexArraysOES(1, &_offscreenBlitVAO);
        _offscreenBlitVAO = 0;
        gl::DeleteBuffers(1, &_offscreenBlitVBO);
        _offscreenBlitVBO = 0;
        gl::DeleteBuffers(1, &_offscreenBlitProgram);
        _offscreenBlitProgram = 0;

        [MGLContext setCurrentContext:currentCtx forLayer:currentLayer];

        _retainedBacking = oldRetainBackingFlag;
    }

    _offscreenFBOSize.width = _offscreenFBOSize.height = 0;
}

- (void)checkLayerSize
{
    // Resize the metal layer
    _metalLayer.frame = self.bounds;
    _metalLayer.drawableSize =
        CGSizeMake(_metalLayer.bounds.size.width * _metalLayer.contentsScale,
                   _metalLayer.bounds.size.height * _metalLayer.contentsScale);
}

- (void)ensureSurfaceCreated
{
    if (_eglSurface != EGL_NO_SURFACE)
    {
        return;
    }

    [self checkLayerSize];

    int red = 8, green = 8, blue = 8, alpha = 8;
    switch (_drawableColorFormat)
    {
        case MGLDrawableColorFormatRGBA8888:
            red = green = blue = alpha = 8;
            break;
        case MGLDrawableColorFormatRGB565:
            red = blue = 5;
            green      = 6;
            alpha      = 0;
            break;
        default:
            UNREACHABLE();
            break;
    }

    // Init surface
    std::vector<EGLint> surfaceAttribs = {
        EGL_RED_SIZE,       red,
        EGL_GREEN_SIZE,     green,
        EGL_BLUE_SIZE,      blue,
        EGL_ALPHA_SIZE,     alpha,
        EGL_DEPTH_SIZE,     _retainedBacking ? 0 : _drawableDepthFormat,
        EGL_STENCIL_SIZE,   _retainedBacking ? 0 : _drawableStencilFormat,
        EGL_SAMPLE_BUFFERS, 0,
        EGL_SAMPLES,        EGL_DONT_CARE,
    };
    surfaceAttribs.push_back(EGL_NONE);
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(_display.eglDisplay, surfaceAttribs.data(), &config, 1, &numConfigs) ||
        numConfigs < 1)
    {
        Throw(@"Failed to call eglChooseConfig()");
    }

    EGLint creationAttribs[] = {EGL_FLEXIBLE_SURFACE_COMPATIBILITY_SUPPORTED_ANGLE, EGL_TRUE,
                                EGL_NONE};

    _eglSurface = eglCreateWindowSurface(
        _display.eglDisplay, config, (__bridge EGLNativeWindowType)_metalLayer, creationAttribs);
    if (_eglSurface == EGL_NO_SURFACE)
    {
        Throw(@"Failed to call eglCreateWindowSurface()");
    }
}

- (BOOL)ensureOffscreenFBOCreated
{
    ASSERT([MGLContext currentContext]);

    ScopedTexture oldOffscreenTexture = 0;

    if (_offscreenFBOCreatorContext != [MGLContext currentContext] || !
                                                                      [self verifyOffscreenFBOSize])
    {
        // We need to copy the old texture to current texture
        oldOffscreenTexture = _offscreenTexture;
        _offscreenTexture   = 0;
        [self releaseOffscreenRenderingResources];
    }

    if (_defaultOpenGLFrameBufferID)
    {
        // Already created.
        return YES;
    }

    if (![self createOffscreenRenderingResources])
    {
        [self releaseOffscreenRenderingResources];
        return NO;
    }

    if (oldOffscreenTexture.get())
    {
        return [self blitOffscreenTexture:oldOffscreenTexture toFBO:_defaultOpenGLFrameBufferID];
    }

    return YES;
}

- (BOOL)createOffscreenRenderingResources
{
    if (![self createOffscreenBlitVBO])
    {
        return NO;
    }

    if (![self createOffscreenBlitProgram])
    {
        return NO;
    }

    return [self createOffscreenFBO];
}

- (BOOL)createOffscreenFBO
{
    // Clear pending errors
    gl::GetError();

    _offscreenFBOCreatorContext = [MGLContext currentContext];
    _offscreenFBOSize           = self.drawableSize;

    gl::GenFramebuffers(1, &_defaultOpenGLFrameBufferID);

    ScopedFBOBind bindFBO(_defaultOpenGLFrameBufferID);

    if (![self createOffscreenTexture])
    {
        return NO;
    }
    if (![self createOffscreenDepthStencilbuffer])
    {
        return NO;
    }

    if (gl::CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        // Fatal error
        Throw(@"Offscreen texture is not complete");
    }

    return _defaultOpenGLFrameBufferID && gl::GetError() == GL_NO_ERROR;
}

- (BOOL)createOffscreenDepthStencilbuffer
{
    // Clear pending errors
    gl::GetError();

    int depthBits   = _drawableDepthFormat;
    int stencilBits = _drawableStencilFormat;

    if (!depthBits && !stencilBits)
    {
        // No depth & stencil buffer
        return YES;
    }

    GLenum depthStencilFormat = 0;
    if (depthBits && stencilBits)
    {
        depthStencilFormat = GL_DEPTH24_STENCIL8_OES;
    }
    else if (depthBits)
    {
        depthStencilFormat = GL_DEPTH_COMPONENT24_OES;
    }
    else if (stencilBits)
    {
        depthStencilFormat = GL_STENCIL_INDEX8;
    }

    gl::GenRenderbuffers(1, &_offscreenDepthStencilBuffer);
    ScopedRenderbufferBind bindRenderbuffer(_offscreenDepthStencilBuffer);

    gl::RenderbufferStorage(GL_RENDERBUFFER, depthStencilFormat,
                            static_cast<GLsizei>(_offscreenFBOSize.width),
                            static_cast<GLsizei>(_offscreenFBOSize.height));

    if (depthBits)
    {
        gl::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                                    _offscreenDepthStencilBuffer);
    }
    if (stencilBits)
    {
        gl::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                    _offscreenDepthStencilBuffer);
    }

    return _offscreenDepthStencilBuffer && gl::GetError() == GL_NO_ERROR;
}

- (BOOL)createOffscreenTexture
{
    // Clear pending errors
    gl::GetError();

    // NOTE(hqle): MSAA option
    gl::GenTextures(1, &_offscreenTexture);

    ScopedTextureBind bindTexture(_offscreenTexture);

    GLenum textureFormat;
    switch (_drawableColorFormat)
    {
        case MGLDrawableColorFormatRGBA8888:
            textureFormat = GL_RGBA8_OES;
            break;
        case MGLDrawableColorFormatRGB565:
            textureFormat = GL_RGB8_OES;
            break;
        default:
            UNREACHABLE();
            break;
    }

    gl::TexStorage2DEXT(GL_TEXTURE_2D, 1, textureFormat,
                        static_cast<GLsizei>(_offscreenFBOSize.width),
                        static_cast<GLsizei>(_offscreenFBOSize.height));

    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    gl::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _offscreenTexture,
                             0);

    return _offscreenTexture && gl::GetError() == GL_NO_ERROR;
}

- (BOOL)verifyOffscreenFBOSize
{
    if (!_defaultOpenGLFrameBufferID)
    {
        return YES;
    }

    return static_cast<NSUInteger>(_offscreenFBOSize.width) ==
               static_cast<NSUInteger>(self.drawableSize.width) &&
           static_cast<NSUInteger>(_offscreenFBOSize.height) ==
               static_cast<NSUInteger>(self.drawableSize.height);
}

- (BOOL)createOffscreenBlitProgram
{
    GLenum vshader, fshader;
    if (!CompileShader(GL_VERTEX_SHADER, kBlitVS, &vshader))
    {
        return NO;
    }

    if (!CompileShader(GL_FRAGMENT_SHADER, kBlitFS, &fshader))
    {
        return NO;
    }

    _offscreenBlitProgram = gl::CreateProgram();
    gl::AttachShader(_offscreenBlitProgram, vshader);
    gl::AttachShader(_offscreenBlitProgram, fshader);
    gl::BindAttribLocation(_offscreenBlitProgram, 0, "aPosition");

    if (!LinkProgram(_offscreenBlitProgram))
    {
        return NO;
    }
    GLint textureLocation = -1;
    textureLocation       = gl::GetUniformLocation(_offscreenBlitProgram, "uTexture");
    ASSERT(textureLocation != -1);

    ScopedProgramBind bindProgram(_offscreenBlitProgram);
    gl::Uniform1i(textureLocation, 0);

    return YES;
}

- (BOOL)createOffscreenBlitVBO
{
    // Clear pending errors
    gl::GetError();

    constexpr float kBlitVertices[] = {-1, 1, -1, -1, 1, 1, 1, -1};

    gl::GenVertexArraysOES(1, &_offscreenBlitVAO);
    gl::GenBuffers(1, &_offscreenBlitVBO);
    ScopedVAOBind bindVAO(_offscreenBlitVAO);
    ScopedBufferBind bindVBO(_offscreenBlitVBO);
    gl::BufferData(GL_ARRAY_BUFFER, sizeof(kBlitVertices), kBlitVertices, GL_STATIC_DRAW);

    gl::EnableVertexAttribArray(0);
    gl::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    return _offscreenBlitVAO && _offscreenBlitVBO && gl::GetError() == GL_NO_ERROR;
}

@end
