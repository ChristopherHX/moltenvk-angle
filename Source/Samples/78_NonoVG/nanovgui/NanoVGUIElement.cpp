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

#include "NanoVGEvents.h"
#include "NanoVGSubSystem.h"
#include "NanoVGUIElement.h"

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#endif
#include "nanovg.h"
#include "nanovg_gl_utils.h"

#include "../Demo.h"

#include "../DebugNew.h"

namespace Urho3D
{

NanoVGUIElement::NanoVGUIElement(Context* context)
    : BorderImage(context)
    , nvgFrameBuffer_(NULL)
{
    NanoVG* nanovg = GetSubsystem<NanoVG>();
    vg_ = nanovg->GetNVGContext();
    graphics_ = GetSubsystem<Graphics>();
    drawTexture_ = NULL;

    SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NanoVGUIElement, HandleRender));
}

NanoVGUIElement::~NanoVGUIElement()
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

void NanoVGUIElement::RegisterObject(Context* context)
{
    context->RegisterFactory<NanoVGUIElement>("UI");
    URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
    URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);
}

void NanoVGUIElement::BeginRender()
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

void NanoVGUIElement::EndRender()
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

void NanoVGUIElement::HandleRender(StringHash eventType, VariantMap& eventData)
{
    if (vg_ != nullptr)
    {
        BeginRender();
        
        using namespace NVGRender;

        VariantMap& eventData = GetEventDataMap();

        eventData[P_NVGELEMENT] = this;
        eventData[P_NVGCONTEXT] = GetSubsystem<NanoVG>();

        this->SendEvent(E_NVGRENDER, eventData);

        EndRender();
    }
}

void NanoVGUIElement::OnResize(const IntVector2& newSize, const IntVector2& delta)
{
    CreateFrameBuffer(newSize.x_, newSize.y_);
}

void NanoVGUIElement::CreateFrameBuffer(int width, int height)
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

} // namespace Urho3D
