//
// Copyright (c) 2008-2020 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../Precompiled.h"

#include "../Core/Context.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Input/InputEvents.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>

#include "NVG.h"
#include "VGElement.h"
#include "VGEvents.h"

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#endif

#include "nanovg_gl_utils.h"

#include "../Demo.h"

#include "../DebugNew.h"

namespace Urho3D
{

VGElement::VGElement(Context* context)
    : BorderImage(context)
    , nvgFrameBuffer_(NULL)
{
    NanoVG* nanovg = GetSubsystem<NanoVG>();
    vg_ = nanovg->GetNVGContext();
    graphics_ = GetSubsystem<Graphics>();
    drawTexture_ = NULL;

    SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(VGElement, HandleRender));
}

VGElement::~VGElement()
{
    NanoVG* nanovg = GetSubsystem<NanoVG>();
    NVGcontext* vg = nanovg->GetNVGContext();

    if (vg)
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
        SetTexture(nullptr);
        drawTexture_->Release();
        drawTexture_ = NULL;
        imageRect_ = IntRect::ZERO;
    }
}

void VGElement::RegisterObject(Context* context)
{
    context->RegisterFactory<VGElement>("UI");
    URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
    URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);
}

void VGElement::BeginRender()
{
    if (nvgFrameBuffer_ != nullptr && vg_ != nullptr)
    {

        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousVBO);
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);

        previousVS = graphics_->GetVertexShader();
        previousPS = graphics_->GetPixelShader();

        graphics_->SetShaders(NULL, NULL);

        glBindFramebuffer(GL_FRAMEBUFFER, nvgFrameBuffer_->fbo);

        glViewport(0, 0, textureSize_.x_, textureSize_.y_);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        nvgBeginFrame(vg_, textureSize_.x_, textureSize_.y_, 1.0f);

        // TBD ELI , hack to flip the image horizontal
        nvgTranslate(vg_, textureSize_.x_ / 2, textureSize_.y_ / 2);
        nvgScale(vg_, 1.0, -1.0);
        nvgTranslate(vg_, -textureSize_.x_ / 2, -textureSize_.y_ / 2);

        // TBD ELI , hack to clear the frame
        nvgBeginPath(vg_);
        nvgRect(vg_, 0, 0, textureSize_.x_, textureSize_.y_);
        nvgFillColor(vg_, nvgRGBA(128, 128, 128, 255));
        nvgFill(vg_);
    }
}

void VGElement::EndRender()
{
    if (nvgFrameBuffer_ != nullptr && vg_ != nullptr)
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
    }
}

void VGElement::HandleRender(StringHash eventType, VariantMap& eventData)
{
    if (vg_ != nullptr)
    {
        BeginRender();

        using namespace NVGRender;

        VariantMap& eventData = GetEventDataMap();

        eventData[P_VGELEMENT] = this;
        eventData[P_VGCONTEXT] = GetSubsystem<NanoVG>();

        this->SendEvent(E_NVGRENDER, eventData);

        EndRender();
    }
}

void VGElement::OnResize(const IntVector2& newSize, const IntVector2& delta)
{
    CreateFrameBuffer(newSize.x_, newSize.y_);
}

void VGElement::CreateFrameBuffer(int width, int height)
{

    if (width == 0 || height == 0)
        return;

    NanoVG* nanovg = GetSubsystem<NanoVG>();
    NVGcontext* vg_ = nanovg->GetNVGContext();

    nvgluDeleteFramebuffer(nvgFrameBuffer_);
    nvgFrameBuffer_ = NULL;
    if (drawTexture_ != NULL)
    {
        SetTexture(nullptr);
        drawTexture_->Release();
        drawTexture_ = NULL;
        imageRect_ = IntRect::ZERO;
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
    SetTexture(drawTexture_);
    return;
error:
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    nvgluDeleteFramebuffer(fb);
    nvgFrameBuffer_ = NULL;
    delete drawTexture_;
    drawTexture_ = NULL;
}

void VGElement::BeginFrame(float windowWidth, float windowHeight, float devicePixelRatio)
{
    nvgBeginFrame(vg_, windowWidth, windowHeight, devicePixelRatio);
}

void VGElement::CancelFrame() { nvgCancelFrame(vg_); }

void VGElement::EndFrame() { nvgEndFrame(vg_); }

void VGElement::GlobalCompositeOperation(int op) { nvgGlobalCompositeOperation(vg_, op); }

void VGElement::GlobalCompositeBlendFunc(int sfactor, int dfactor)
{
    nvgGlobalCompositeBlendFunc(vg_, sfactor, dfactor);
}

void VGElement::GlobalCompositeBlendFuncSeparate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha)
{
    nvgGlobalCompositeBlendFuncSeparate(vg_, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

NVGcolor VGElement::RGB(unsigned char r, unsigned char g, unsigned char b) { return nvgRGB(r, g, b); }

NVGcolor VGElement::RGBf(float r, float g, float b) { return nvgRGBf(r, g, b); }

NVGcolor VGElement::RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return nvgRGBA(r, g, b, a);
}

NVGcolor VGElement::RGBAf(float r, float g, float b, float a) { return nvgRGBAf(r, g, b, a); }

NVGcolor VGElement::LerpRGBA(NVGcolor c0, NVGcolor c1, float u) { return nvgLerpRGBA(c0, c1, u); }

NVGcolor VGElement::TransRGBA(NVGcolor c0, unsigned char a) { return nvgTransRGBA(c0, a); }

NVGcolor VGElement::TransRGBAf(NVGcolor c0, float a) { return nvgTransRGBAf(c0, a); }

NVGcolor VGElement::HSL(float h, float s, float l) { return nvgHSL(h, s, l); }

NVGcolor VGElement::HSLA(float h, float s, float l, unsigned char a) { return nvgHSLA(h, s, l, a); }

void VGElement::SaveState() { nvgSave(vg_); }

void VGElement::RestoreState() { nvgRestore(vg_); }

void VGElement::ResetState() { nvgReset(vg_); }

void VGElement::ShapeAntiAlias(int enabled) { nvgShapeAntiAlias(vg_, enabled); }

void VGElement::StrokeColor(NVGcolor color) { nvgStrokeColor(vg_, color); }

void VGElement::StrokePaint(NVGpaint paint) { nvgStrokePaint(vg_, paint); }

void VGElement::FillColor(NVGcolor color) { nvgFillColor(vg_, color); }

void VGElement::FillPaint(NVGpaint paint) { nvgFillPaint(vg_, paint); }

void VGElement::MiterLimit(float limit) { nvgMiterLimit(vg_, limit); }

void VGElement::StrokeWidth(float size) { nvgStrokeWidth(vg_, size); }

void VGElement::LineCap(int cap) { nvgLineCap(vg_, cap); }

void VGElement::LineJoin(int join) { nvgLineJoin(vg_, join); }

void VGElement::GlobalAlpha(float alpha) { nvgGlobalAlpha(vg_, alpha); }

void VGElement::ResetTransform() { nvgResetTransform(vg_); }

void VGElement::Transform(float a, float b, float c, float d, float e, float f) { nvgTransform(vg_, a, b, c, d, e, f); }

void VGElement::Translate(float x, float y) { nvgTranslate(vg_, x, y); }

void VGElement::Rotate(float angle) { nvgRotate(vg_, angle); }

void VGElement::SkewX(float angle) { nvgSkewX(vg_, angle); }

void VGElement::SkewY(float angle) { nvgSkewY(vg_, angle); }

void VGElement::Scale(float x, float y) { nvgScale(vg_, x, y); }

void VGElement::CurrentTransform(float* xform) { nvgCurrentTransform(vg_, xform); }

void VGElement::TransformIdentity(float* dst) { nvgTransformIdentity(dst); }

void VGElement::TransformTranslate(float* dst, float tx, float ty) { nvgTransformTranslate(dst, tx, ty); }

void VGElement::TransformScale(float* dst, float sx, float sy) { nvgTransformScale(dst, sx, sy); }

void VGElement::TransformRotate(float* dst, float a) { nvgTransformRotate(dst, a); }

void VGElement::TransformSkewX(float* dst, float a) { nvgTransformSkewX(dst, a); }

void VGElement::TransformSkewY(float* dst, float a) { nvgTransformSkewY(dst, a); }

void VGElement::TransformMultiply(float* dst, const float* src) { nvgTransformMultiply(dst, src); }

void VGElement::TransformPremultiply(float* dst, const float* src) { nvgTransformPremultiply(dst, src); }

int VGElement::TransformInverse(float* dst, const float* src) { return nvgTransformInverse(dst, src); }

void VGElement::TransformPoint(float* dstx, float* dsty, const float* xform, float srcx, float srcy)
{
    nvgTransformPoint(dstx, dsty, xform, srcx, srcy);
}

float VGElement::DegToRad(float deg) { return nvgDegToRad(deg); }

float VGElement::RadToDeg(float rad) { return nvgRadToDeg(rad); }

void VGElement::BeginPath() { nvgBeginPath(vg_); }

void VGElement::MoveTo(float x, float y) { nvgMoveTo(vg_, x, y); }

void VGElement::LineTo(float x, float y) { nvgLineTo(vg_, x, y); }

void VGElement::BezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    nvgBezierTo(vg_, c1x, c1y, c2x, c2y, x, y);
}

void VGElement::QuadTo(float cx, float cy, float x, float y) { nvgQuadTo(vg_, cx, cy, x, y); }

void VGElement::ArcTo(float x1, float y1, float x2, float y2, float radius) { nvgArcTo(vg_, x1, y1, x2, y2, radius); }

void VGElement::ClosePath() { nvgClosePath(vg_); }

void VGElement::PathWinding(int dir) { nvgPathWinding(vg_, dir); }

void VGElement::Arc(float cx, float cy, float r, float a0, float a1, int dir) { nvgArc(vg_, cx, cy, r, a0, a1, dir); }

void VGElement::Rect(float x, float y, float w, float h) { nvgRect(vg_, x, y, w, h); }

void VGElement::RoundedRect(float x, float y, float w, float h, float r) { nvgRoundedRect(vg_, x, y, w, h, r); }

void VGElement::RoundedRectVarying(float x, float y, float w, float h, float radTopLeft, float radTopRight,
                                   float radBottomRight, float radBottomLeft)
{
    nvgRoundedRectVarying(vg_, x, y, w, h, radTopLeft, radTopRight, radBottomRight, radBottomLeft);
}

void VGElement::Ellipse(float cx, float cy, float rx, float ry) { nvgEllipse(vg_, cx, cy, rx, ry); }

void VGElement::Circle(float cx, float cy, float r) { nvgCircle(vg_, cx, cy, r); }

void VGElement::Fill() { nvgFill(vg_); }

void VGElement::Stroke() { nvgStroke(vg_); }

} // namespace Urho3D
