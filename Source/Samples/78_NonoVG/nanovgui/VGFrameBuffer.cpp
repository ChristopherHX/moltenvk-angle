
#include "NVG.h"
#include "nanovg.h"
#include "VGFrameBuffer.h"
#include <Urho3D/Core/Context.h>


#include "nanovg_gl_utils.h"

namespace Urho3D
{

static VGFrameBuffer* CurrenVGFrameBuffer_ = nullptr;

VGFrameBuffer::VGFrameBuffer(Context* context, int Width, int Height)
    : Object(context)
    , nvgFrameBuffer_(nullptr)
{
    nanoVG_ = GetSubsystem<NanoVG>();
    vg_ = nanoVG_->GetNVGContext();
    graphics_ = GetSubsystem<Graphics>();

    drawTexture_ = nullptr;
    clearColor_ = Color(0.0, 0.0, 0.0, 1.0);

    CreateFrameBuffer(Width, Height);
}

VGFrameBuffer::~VGFrameBuffer() 
{
  
    if (nanoVG_.Get() && vg_)
    {
        nvgluDeleteFramebuffer(nvgFrameBuffer_);
        nvgFrameBuffer_ = NULL;
    }
    else
    {
        if (nvgFrameBuffer_->fbo != 0)
            glDeleteFramebuffers(1, &nvgFrameBuffer_->fbo);
        if (nvgFrameBuffer_->rbo != 0)
            glDeleteRenderbuffers(1, &nvgFrameBuffer_->rbo);
    }

    if (drawTexture_ != NULL)
    {
        drawTexture_->Release();
        drawTexture_.Reset();
        drawTexture_ = NULL;
    }
}

void VGFrameBuffer::SetClearColor(Color color) { clearColor_ = color; }

Color VGFrameBuffer::GetClearColor() { return clearColor_; }

bool VGFrameBuffer::CreateFrameBuffer(int width, int height)
{

   
     if (width == 0 || height == 0)
        return false;

     if (nanoVG_ == nullptr || vg_ == nullptr)
     {
         return false;
     }

    nvgluDeleteFramebuffer(nvgFrameBuffer_);
    nvgFrameBuffer_ = NULL;
    if (drawTexture_ != NULL)
    {
    
        drawTexture_->Release();
        drawTexture_.Reset();
        drawTexture_ = NULL;
    }

    drawTexture_ = new Texture2D(context_);
    textureSize_ = IntVector2(width, height);
    drawTexture_->SetMipsToSkip(QUALITY_HIGH, 0);
    drawTexture_->SetNumLevels(1);
    drawTexture_->SetSize(textureSize_.x_, textureSize_.y_, Graphics::GetRGBAFormat(), TEXTURE_RENDERTARGET);

    unsigned TextureID = drawTexture_->GetGPUObjectName();

    GLint defaultFBO;
    GLint defaultRBO;
    NVGLUframebuffer* fb = NULL;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

    fb = (NVGLUframebuffer*)malloc(sizeof(NVGLUframebuffer));
    if (fb == NULL)
        goto error;
    memset(fb, 0, sizeof(NVGLUframebuffer));

    fb->image = -1;
    fb->texture = TextureID;

    fb->ctx = vg_;

    // frame buffer object
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);

    // render buffer object
    glGenRenderbuffers(1, &fb->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);

    // combine all
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
#ifdef GL_DEPTH24_STENCIL8
        // If GL_STENCIL_INDEX8 is not supported, try GL_DEPTH24_STENCIL8 as a fallback.
        // Some graphics cards require a depth buffer along with a stencil.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
#endif // GL_DEPTH24_STENCIL8
            goto error;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    nvgFrameBuffer_ = fb;
    return true;
error:
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    nvgluDeleteFramebuffer(fb);
    nvgFrameBuffer_ = NULL;
    delete drawTexture_;
    drawTexture_ = NULL;
    return false;

}

void VGFrameBuffer::Bind() {
    if (nvgFrameBuffer_ != nullptr && nanoVG_ != nullptr && vg_ != nullptr)
    {

        if (CurrenVGFrameBuffer_)
        {
            CurrenVGFrameBuffer_->UnBind();
            CurrenVGFrameBuffer_ = nullptr;
        }
        CurrenVGFrameBuffer_ = this;

        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousVBO);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);

        previousVS = graphics_->GetVertexShader();
        previousPS = graphics_->GetPixelShader();

        graphics_->SetShaders(NULL, NULL);

        glBindFramebuffer(GL_FRAMEBUFFER, nvgFrameBuffer_->fbo);

        graphics_->ClearParameterSources();
        graphics_->SetColorWrite(true);
        graphics_->SetCullMode(CULL_NONE);
        graphics_->SetDepthTest(CMP_ALWAYS);
        graphics_->SetDepthWrite(false);
        graphics_->SetFillMode(FILL_SOLID);
        graphics_->SetStencilTest(false);
        graphics_->SetScissorTest(false);
        graphics_->Clear(CLEAR_COLOR | CLEAR_DEPTH | CLEAR_STENCIL, clearColor_, 0, 0);

        glViewport(0, 0, textureSize_.x_, textureSize_.y_);

        nvgBeginFrame(vg_, textureSize_.x_, textureSize_.y_, 1.0f);

        // Flip the image on the horizontal axis
        float midx = textureSize_.x_ / 2.0f;
        float midy = textureSize_.y_ / 2.0f;
        nvgTranslate(vg_, midx, midy);
        nvgScale(vg_, 1.0, -1.0);
        nvgTranslate(vg_, -midx, -midy);
    }
}

void VGFrameBuffer::UnBind() {
    if (nvgFrameBuffer_ != nullptr && nanoVG_ != nullptr && vg_ != nullptr)
    {
        if (CurrenVGFrameBuffer_ == this)
        {
            nvgEndFrame(vg_);
            //  Urho3D restore old values
            glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
            glBindBuffer(GL_ARRAY_BUFFER, previousVBO);
            glEnable(GL_DEPTH_TEST);
            graphics_->SetCullMode(CULL_CCW);
            graphics_->SetDepthTest(CMP_LESSEQUAL);
            graphics_->SetDepthWrite(true);
            graphics_->SetShaders(previousVS, previousPS);
            CurrenVGFrameBuffer_ = nullptr;
        }
    }
}

Texture2D* VGFrameBuffer::GetRenderTarget() 
{ 
    return drawTexture_; 
}


} // namespace Urho3D
