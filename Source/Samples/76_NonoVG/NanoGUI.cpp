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
	const unsigned NANOGUI_VERTEX_SIZE = 6;


	Theme::Theme(NVGcontext *ctx)
	{
		
			mStandardFontSize = 16;
			mButtonFontSize = 20;
			mWindowCornerRadius = 2;
			mWindowHeaderHeight = 30;
			mWindowDropShadowSize = 10;
			mButtonCornerRadius = 2;

			mDropShadow = Color(0.0f,0.0f,0.0f, 128 / 255.f);
			mTransparent = Color(0.0f, 0.0f, 0.0f, 0.0f);
			mBorderDark = Color(29 / 255.f, 29 / 255.f, 29 / 255.f, 255 / 255.f);
			mBorderLight = Color(92 / 255.f, 92 / 255.f, 92 / 255.f, 255 / 255.f);
			mBorderMedium = Color(35 / 255.f, 35 / 255.f, 35 / 255.f, 255 / 255.f);
			mTextColor = Color(1.0f,1.0f,1.0f, 160 / 255.f);
			mDisabledTextColor = Color(1.0f, 1.0f, 1.0f, 80 / 255.f);
			mTextColorShadow = Color(0.0f, 0.0f, 0.0f, 160 / 255.f);
			mIconColor = mTextColor;

			mButtonGradientTopFocused = Color(64 / 255.f, 64 / 255.f, 64 / 255.f, 1.0f);
			mButtonGradientBotFocused = Color(48 / 255.f, 48 / 255.f, 48 / 255.f, 1.0f);
			mButtonGradientTopUnfocused = Color(74 / 255.f, 74 / 255.f, 74 / 255.f, 1.0f);
			mButtonGradientBotUnfocused = Color(58 / 255.f, 58 / 255.f, 58 / 255.f, 1.0f);
			mButtonGradientTopPushed = Color(41 / 255.f, 41 / 255.f, 41 / 255.f, 1.0f);
			mButtonGradientBotPushed = Color(29 / 255.f, 29 / 255.f, 29 / 255.f, 1.0f);

			/* Window-related */
			mWindowFillUnfocused = Color(43 / 255.f, 43 / 255.f, 43 / 255.f, 230 / 255.f);
			mWindowFillFocused = Color(45 / 255.f, 45 / 255.f, 45 / 255.f, 230 / 255.f);
			mWindowTitleUnfocused = Color(220 / 255.f, 220 / 255.f, 220 / 255.f, 160 / 255.f);
			mWindowTitleFocused = Color(1.0f, 1.0f, 1.0f, 190 / 255.f);

			mWindowHeaderGradientTop = mButtonGradientTopUnfocused;
			mWindowHeaderGradientBot = mButtonGradientBotUnfocused;
			mWindowHeaderSepTop = mBorderLight;
			mWindowHeaderSepBot = mBorderDark;

			mWindowPopup = Color(50 / 255.f, 50 / 255.f, 50 / 255.f, 255);
			mWindowPopupTransparent = Color(50 / 255.f, 50 / 255.f, 50 / 255.f, 0);
			
			mFontNormal = nvgCreateFont(ctx, "sans", "Roboto-Regular.ttf"); //nvgCreateFontMem(ctx, "sans", roboto_regular_ttf,	roboto_regular_ttf_size, 0);
			mFontBold = nvgCreateFont(ctx, "sans-bold", "Roboto-Bold.ttf"); //nvgCreateFontMem(ctx, "sans-bold", roboto_bold_ttf,roboto_bold_ttf_size, 0);
			mFontIcons = nvgCreateFont(ctx, "icons", "entypo.ttf"); //nvgCreateFontMem(ctx, "icons", entypo_ttf,	entypo_ttf_size, 0);

			if (mFontNormal == -1 || mFontBold == -1 || mFontIcons == -1)
			{
                URHO3D_LOGERROR("Could not load fonts!");
			}
				
		
	}


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
		theme_(NULL)
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

		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(NanoGUI, HandleBeginFrame));
        SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(NanoGUI, HandlePostUpdate));
        SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(NanoGUI, HandleRenderUpdate));
        SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NanoGUI, HandleRender));




	//	vg_ = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
#if defined(NANOVG_GL3_IMPLEMENTATION)
		vg_ = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#else
        vg_ = nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#endif

		if (vg_ == NULL)
            URHO3D_LOGERROR("Could not init nanovg.");
		

		if (vg_)
		{
			theme_ = new Theme(vg_);
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
        if (theme_)
        {
            delete theme_;
            theme_ = nullptr;
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

		nvgBeginFrame(vg_, graphics_->GetWidth(), graphics_->GetHeight(), 1.0f);

		renderDemo(vg_, 0, 0, graphics_->GetWidth(), graphics_->GetHeight(), time_, 0,&data);
   
		nvgEndFrame(vg_);

		graphics_->ResetCachedState();
	}

	
	void NanoGUI::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
	{

	}

	void NanoGUI::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace PostUpdate;

		Update(eventData[P_TIMESTEP].GetFloat());
	}

	void NanoGUI::HandleRenderUpdate(StringHash eventType, VariantMap& eventData)
	{
	}

	void NanoGUI::HandleRender(StringHash eventType, VariantMap& eventData)
	{
		Render();
	}





}