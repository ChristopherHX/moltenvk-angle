//
// Copyright (c) 2008-2021 the Urho3D project.
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

#include "../Urho3D.h"
#include "NanoGUI.h"
#include "../UI/CheckBox.h"
#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../UI/Cursor.h"
#include "../UI/DropDownList.h"
#include "../UI/FileSelector.h"
#include "../UI/Font.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../Input/Input.h"
#include "../Input/InputEvents.h"
#include "../UI/LineEdit.h"
#include "../UI/ListView.h"
#include "../IO/Log.h"
#include "../Math/Matrix3x4.h"
#include "../UI/MessageBox.h"
#include "../Core/Profiler.h"
#include "../Resource/ResourceCache.h"
#include "../UI/ScrollBar.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ShaderVariation.h"
#include "../UI/Slider.h"
#include "../Container/Sort.h"
#include "../UI/Sprite.h"
#include "../UI/Text.h"
#include "../UI/Text3D.h"
#include "../UI/ToolTip.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"
#include "../Graphics/VertexBuffer.h"
#include "../UI/Window.h"
#include "../UI/View3D.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Timer.h"

#include <Urho3D/Graphics/Texture2D.h>

#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#include "nanovg/nanovg.h"
#include "nanovg/nanovg_gl.h"
#include "nanovg/nanovg_gl_utils.h"


#include "Demo.h"


static DemoData data;

namespace Urho3D
{

	void Urho3D::NanoGUI::RegisterObject(Context* context)
	{
		context->RegisterFactory<NanoGUI>();
	}

	Urho3D::NanoGUI::~NanoGUI()
	{
	
		Clear();
	}

	Urho3D::NanoGUI::NanoGUI(Context* context) : Object(context),
		initialized_(false),
		vg_(NULL),
        texture2D_(NULL),
        nvgFrameBuffer_(NULL)
	{

	}



	void NanoGUI::Initialize(Texture2D * texture2D,int Width, int Height)
	{
		Graphics* graphics = GetSubsystem<Graphics>();

		if (!graphics || !graphics->IsInitialized())
			return;
     

		time_ = 0.0f;
	
	//	OpenConsoleWindow();
		graphics_ = graphics;

		initialized_ = true;

        SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(NanoGUI, HandlePostUpdate));
        SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NanoGUI, HandleRender));


//NVG_DEBUG
#if defined(NANOVG_GL3_IMPLEMENTATION)
		vg_ = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#else
        vg_ = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#endif

		if (vg_ == NULL)
            URHO3D_LOGERROR("Could not init nanovg.");
		

		if (vg_)
		{
            loadDemoData(GetSubsystem<ResourceCache>(),vg_, &data);
		}
        

         unsigned TextureID = texture2D->GetGPUObjectName();
         CreateFrameBuffer(Width, Height, TextureID);

		 URHO3D_LOGINFO("Initialized NanoGUI interface");
	}

	void NanoGUI::Clear()
	{ 
		freeDemoData(vg_, &data);
        if (vg_)
        {
            nvgluDeleteFramebuffer(nvgFrameBuffer_);
            
#if defined(NANOVG_GL3_IMPLEMENTATION)
            nvgDeleteGL3(vg_);
#else
            nvgDeleteGLES2(vg_);
#endif
            vg_ = nullptr;
        }
  
	}


    void NanoGUI::CreateFrameBuffer(int w, int h, unsigned & TextureID)
    {
        GLint defaultFBO;
        GLint defaultRBO;
        NVGLUframebuffer* fb = NULL;

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

        fb = (NVGLUframebuffer*)malloc(sizeof(NVGLUframebuffer));
        if (fb == NULL) goto error;
        memset(fb, 0, sizeof(NVGLUframebuffer));

     
        fb->texture = TextureID;

        fb->ctx = vg_;

        // frame buffer object
        glGenFramebuffers(1, &fb->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);

        // render buffer object
        glGenRenderbuffers(1, &fb->rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);

        // combine all
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    #ifdef GL_DEPTH24_STENCIL8
            // If GL_STENCIL_INDEX8 is not supported, try GL_DEPTH24_STENCIL8 as a fallback.
            // Some graphics cards require a depth buffer along with a stencil.
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    #endif // GL_DEPTH24_STENCIL8
                goto error;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
        nvgFrameBuffer_ =  fb;
        return;
    error:
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
        nvgluDeleteFramebuffer(fb);
    }

	void NanoGUI::Update(float timeStep)
	{ 
		URHO3D_PROFILE(UpdateNanoGUI);
        time_ += timeStep;
	}

	void NanoGUI::Render(bool resetRenderTargets /*= true*/)
	{
        URHO3D_PROFILE(RenderNanoGUI);
        
       
		if (vg_)
        {

            
            if(nvgFrameBuffer_ != nullptr)
            {
                nvgluBindFramebuffer(nvgFrameBuffer_);
                glViewport(0, 0, 1400, 1200);
                glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
            }
            nvgBeginFrame(vg_, 1400, 1200, 1.0f);
            
            // TBD ELI , hack to flip the image horizontal
            
            nvgTranslate(vg_, 700, 600);
            nvgScale(vg_,1.0,-1.0);
            nvgTranslate(vg_, -700, -600);
            
            // TBD ELI , hack to clear the frame
            nvgBeginPath(vg_);
            nvgRect(vg_, 0,0, 1400,1200);
            nvgFillColor(vg_, nvgRGBA(0,0,0,255));
            nvgFill(vg_);

            renderDemo(vg_, 0, 0, 1400, 1200, time_, 0, &data);
        

            nvgEndFrame(vg_);
            
            if(nvgFrameBuffer_ != nullptr)
            {
                nvgluBindFramebuffer(nullptr);
            }

          //  graphics_->ResetCachedState();
        }
	}


	void NanoGUI::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace PostUpdate;
 
		Update(eventData[P_TIMESTEP].GetFloat());
	}


	void NanoGUI::HandleRender(StringHash eventType, VariantMap& eventData)
	{
		Render();
	}





}
