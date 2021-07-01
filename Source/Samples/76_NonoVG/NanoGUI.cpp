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
#include "../Graphics/Texture2D.h"
#include "../UI/ToolTip.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"
#include "../Graphics/VertexBuffer.h"
#include "../UI/Window.h"
#include "../UI/View3D.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Timer.h"


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
		vg_(NULL)
	{

	}



	void NanoGUI::Initialize()
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

		 URHO3D_LOGINFO("Initialized NanoGUI interface");
	}

	void NanoGUI::Clear()
	{ 
		freeDemoData(vg_, &data);
        if (vg_)
        {
#if defined(NANOVG_GL3_IMPLEMENTATION)
            nvgDeleteGL3(vg_);
#else
            nvgDeleteGLES2(vg_);
#endif
            vg_ = nullptr;
        }
  
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
            GLint previousVBO = 0;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousVBO);

            ShaderVariation* previousVS = graphics_->GetVertexShader();
            ShaderVariation* previousPS = graphics_->GetPixelShader();

            nvgBeginFrame(vg_, graphics_->GetWidth(), graphics_->GetHeight(), 1.0f);

            renderDemo(vg_, 0, 0, graphics_->GetWidth(), graphics_->GetHeight(), time_, 0, &data);

            nvgEndFrame(vg_);

            glBindBuffer(GL_ARRAY_BUFFER, previousVBO);
            glEnable(GL_DEPTH_TEST);
            graphics_->SetCullMode(CULL_CCW);
            graphics_->SetDepthTest(CMP_LESSEQUAL);
            graphics_->SetDepthWrite(true);
            graphics_->SetShaders(previousVS, previousPS);
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