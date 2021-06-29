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


#define NANOVG_GLEW

#ifdef NANOVG_GLEW
#include <glew/glew.h>
#endif
#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#include "nanovg/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"
#include "nanovg/nanovg_gl_utils.h"




void renderPattern(NVGcontext* vg, NVGLUframebuffer* fb, float t, float pxRatio)
{
	int winWidth, winHeight;
	int fboWidth, fboHeight;
	int pw, ph, x, y;
	float s = 20.0f;
	float sr = (cosf(t) + 1)*0.5f;
	float r = s * 0.6f * (0.2f + 0.8f * sr);

	if (fb == NULL) return;

	nvgImageSize(vg, fb->image, &fboWidth, &fboHeight);
	winWidth = (int)(fboWidth / pxRatio);
	winHeight = (int)(fboHeight / pxRatio);

	// Draw some stuff to an FBO as a test
	nvgluBindFramebuffer(fb);
	glViewport(0, 0, fboWidth, fboHeight);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	nvgBeginFrame(vg, winWidth, winHeight, pxRatio);

	pw = (int)ceilf(winWidth / s);
	ph = (int)ceilf(winHeight / s);

	nvgBeginPath(vg);
	for (y = 0; y < ph; y++) {
		for (x = 0; x < pw; x++) {
			float cx = (x + 0.5f) * s;
			float cy = (y + 0.5f) * s;
			nvgCircle(vg, cx, cy, r);
		}
	}
	nvgFillColor(vg, nvgRGBA(220, 160, 0, 200));
	nvgFill(vg);

	nvgEndFrame(vg);
	nvgluBindFramebuffer(NULL);
}
void drawWindow(NVGcontext* vg, const char* title, float x, float y, float w, float h)
{
	float cornerRadius = 6.0f;
	NVGpaint shadowPaint;
	NVGpaint headerPaint;

//	nvgSave(vg);
	//	nvgClearState(vg);

	// Window
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x, y, w, h, cornerRadius);
	nvgFillColor(vg, nvgRGBA(90, 30, 34, 255));
	//	nvgFillColor(vg, nvgRGBA(0,0,0,128));
	nvgFill(vg);

	// Drop shadow
	shadowPaint = nvgBoxGradient(vg, x, y + 2, w, h, cornerRadius * 2, 10, nvgRGBA(0, 0, 0, 128), nvgRGBA(0, 0, 0, 0));
	nvgBeginPath(vg);
	nvgRect(vg, x - 10, y - 10, w + 20, h + 30);
	nvgRoundedRect(vg, x, y, w, h, cornerRadius);
	nvgPathWinding(vg, NVG_HOLE);
	nvgFillPaint(vg, shadowPaint);
	nvgFill(vg);

	// Header
	headerPaint = nvgLinearGradient(vg, x, y, x, y + 15, nvgRGBA(255, 255, 255, 8), nvgRGBA(0, 0, 0, 16));
	nvgBeginPath(vg);
	nvgRoundedRect(vg, x + 1, y + 1, w - 2, 30, cornerRadius - 1);
	nvgFillPaint(vg, headerPaint);
	nvgFill(vg);
	nvgBeginPath(vg);
	nvgMoveTo(vg, x + 0.5f, y + 0.5f + 30);
	nvgLineTo(vg, x + 0.5f + w - 1, y + 0.5f + 30);
	nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 32));
	nvgStroke(vg);

// 	nvgFontSize(vg, 18.0f);
// 	nvgFontFace(vg, "sans-bold");
// 	nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
// 
// 	nvgFontBlur(vg, 2);
// 	nvgFillColor(vg, nvgRGBA(0, 0, 0, 128));
// 	nvgText(vg, x + w / 2, y + 16 + 1, title, NULL);
// 
// 	nvgFontBlur(vg, 0);
// 	nvgFillColor(vg, nvgRGBA(220, 220, 220, 160));
// 	nvgText(vg, x + w / 2, y + 16, title, NULL);

//	nvgRestore(vg);
}

void drawEyes(NVGcontext* vg, float x, float y, float w, float h, float mx, float my, float t)
{
	NVGpaint gloss, bg;
	float ex = w *0.23f;
	float ey = h * 0.5f;
	float lx = x + ex;
	float ly = y + ey;
	float rx = x + w - ex;
	float ry = y + ey;
	float dx, dy, d;
	float br = (ex < ey ? ex : ey) * 0.5f;
	float blink = 1 - pow(sinf(t*0.5f), 200)*0.8f;

	bg = nvgLinearGradient(vg, x, y + h*0.5f, x + w*0.1f, y + h, nvgRGBA(0, 0, 0, 32), nvgRGBA(0, 0, 0, 16));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx + 3.0f, ly + 16.0f, ex, ey);
	nvgEllipse(vg, rx + 3.0f, ry + 16.0f, ex, ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	bg = nvgLinearGradient(vg, x, y + h*0.25f, x + w*0.1f, y + h, nvgRGBA(220, 220, 220, 255), nvgRGBA(128, 128, 128, 255));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx, ly, ex, ey);
	nvgEllipse(vg, rx, ry, ex, ey);
	nvgFillPaint(vg, bg);
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx + dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, lx + dx, ly + dy + ey*0.25f*(1 - blink), br, br*blink);
	nvgFillColor(vg, nvgRGBA(32, 32, 32, 255));
	nvgFill(vg);

	dx = (mx - rx) / (ex * 10);
	dy = (my - ry) / (ey * 10);
	d = sqrtf(dx*dx + dy*dy);
	if (d > 1.0f) {
		dx /= d; dy /= d;
	}
	dx *= ex*0.4f;
	dy *= ey*0.5f;
	nvgBeginPath(vg);
	nvgEllipse(vg, rx + dx, ry + dy + ey*0.25f*(1 - blink), br, br*blink);
	nvgFillColor(vg, nvgRGBA(32, 32, 32, 255));
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, lx - ex*0.25f, ly - ey*0.5f, ex*0.1f, ex*0.75f, nvgRGBA(255, 255, 255, 128), nvgRGBA(255, 255, 255, 0));
	nvgBeginPath(vg);
	nvgEllipse(vg, lx, ly, ex, ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);

	gloss = nvgRadialGradient(vg, rx - ex*0.25f, ry - ey*0.5f, ex*0.1f, ex*0.75f, nvgRGBA(255, 255, 255, 128), nvgRGBA(255, 255, 255, 0));
	nvgBeginPath(vg);
	nvgEllipse(vg, rx, ry, ex, ey);
	nvgFillPaint(vg, gloss);
	nvgFill(vg);
}


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
	
		if (vg_)
		{
			nvgDeleteGL3(vg_);
		}
		if (theme_)
		{
			delete theme_;
		}
	}

	Urho3D::NanoGUI::NanoGUI(Context* context) : Object(context),
		initialized_(false),
		nonModalBatchSize_(0),
		vg_(NULL),
		theme_(NULL)
	{

	}



	void NanoGUI::Initialize()
	{
		Graphics* graphics = GetSubsystem<Graphics>();

		if (!graphics || !graphics->IsInitialized())
			return;
	
		OpenConsoleWindow();
		graphics_ = graphics;

		vertexBuffer_ = new VertexBuffer(context_);

		initialized_ = true;

		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(NanoGUI, HandleBeginFrame));
        SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(NanoGUI, HandlePostUpdate));
        SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(NanoGUI, HandleRenderUpdate));
        SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NanoGUI, HandleRender));


		rootElement_ = new UIElement(context_);
		rootElement_->SetSize(graphics->GetWidth(), graphics->GetHeight());
		rootElement_->SetPosition(0, 0);

	//	vg_ = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
		vg_ = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

		if (vg_ == NULL)
            URHO3D_LOGERROR("Could not init nanovg.");
		

		if (vg_)
		{
			theme_ = new Theme(vg_);
		}

		 URHO3D_LOGINFO("Initialized NanoGUI interface");
	}

	void NanoGUI::Clear()
	{

	}

	void NanoGUI::Update(float timeStep)
	{ URHO3D_PROFILE(UpdateNanoGUI);

	}

	void NanoGUI::RenderUpdate()
	{
        URHO3D_PROFILE(GetNanoGUIBatches);

		// Get rendering batches from the non-modal UI elements
		batches_.Clear();
		vertexData_.Clear();
		const IntVector2& rootSize = rootElement_->GetSize();
		IntRect currentScissor = IntRect(0, 0, rootSize.x_, rootSize.y_);

		/// getbatches
		// create a test white quad on screen 
// 		UIBatch batch(rootElement_, BLEND_REPLACE, currentScissor, NULL, &vertexData_);
// 		batch.SetColor(Color::WHITE);
// 		batch.AddQuad(0, 0, graphics_->GetWidth(), graphics_->GetHeight(), 0, 0);
// 		UIBatch::AddOrMerge(batch, batches_);

		// Save the batch size of the non-modal batches for later use
		nonModalBatchSize_ = batches_.Size();
	}

	void NanoGUI::Render(bool resetRenderTargets /*= true*/)
	{
        URHO3D_PROFILE(RenderNanoGUI);

		SetVertexData(vertexBuffer_, vertexData_);
		// Render non-modal batches
		Render(resetRenderTargets, vertexBuffer_, batches_, 0, nonModalBatchSize_);


		nvgBeginFrame(vg_, graphics_->GetWidth(), graphics_->GetHeight(), 1.0f);

		for (int i = 0; i < 20; i++) {
			nvgBeginPath(vg_);
			nvgRect(vg_, 10 + i * 30, 10, 10, graphics_->GetHeight() - 20);
			nvgFillColor(vg_, nvgHSLA(1 / 19.0f, 0.5f, 0.5f, 255));
			nvgFill(vg_);
		}


		drawWindow(vg_,"hallloooasdsad",100,0,500,500);

		drawEyes(vg_, 100, 150, 290, 200, 3, 4, 6);


		nvgEndFrame(vg_);
	}

	void NanoGUI::Render(bool resetRenderTargets, VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd)
	{
		// Engine does not render when window is closed or device is lost
		assert(graphics_ && graphics_->IsInitialized() && !graphics_->IsDeviceLost());

		if (batches.Empty())
			return;


		Vector2 invScreenSize(1.0f / (float)graphics_->GetWidth(), 1.0f / (float)graphics_->GetHeight());
		Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);
		Vector2 offset(-1.0f, 1.0f);

		Matrix4 projection(Matrix4::IDENTITY);
		projection.m00_ = scale.x_;
		projection.m03_ = offset.x_;
		projection.m11_ = scale.y_;
		projection.m13_ = offset.y_;
		projection.m22_ = 1.0f;
		projection.m23_ = 0.0f;
		projection.m33_ = 1.0f;

		graphics_->ClearParameterSources();
		graphics_->SetColorWrite(true);
		graphics_->SetCullMode(CULL_CCW);
		graphics_->SetDepthTest(CMP_ALWAYS);
		graphics_->SetDepthWrite(false);
        graphics_->SetLineAntiAlias(false);
		graphics_->SetFillMode(FILL_SOLID);
		graphics_->SetStencilTest(false);
		if (resetRenderTargets)
			graphics_->ResetRenderTargets();
		graphics_->SetVertexBuffer(buffer);

		ShaderVariation* noTextureVS = graphics_->GetShader(VS, "Basic", "VERTEXCOLOR");
		ShaderVariation* diffTextureVS = graphics_->GetShader(VS, "Basic", "DIFFMAP VERTEXCOLOR");
		ShaderVariation* noTexturePS = graphics_->GetShader(PS, "Basic", "VERTEXCOLOR");
		ShaderVariation* diffTexturePS = graphics_->GetShader(PS, "Basic", "DIFFMAP VERTEXCOLOR");
		ShaderVariation* diffMaskTexturePS = graphics_->GetShader(PS, "Basic", "DIFFMAP ALPHAMASK VERTEXCOLOR");
		ShaderVariation* alphaTexturePS = graphics_->GetShader(PS, "Basic", "ALPHAMAP VERTEXCOLOR");

		unsigned alphaFormat = Graphics::GetAlphaFormat();

		for (unsigned i = batchStart; i < batchEnd; ++i)
		{
			const UIBatch& batch = batches[i];
			if (batch.vertexStart_ == batch.vertexEnd_)
				continue;

			ShaderVariation* ps;
			ShaderVariation* vs;

			if (!batch.texture_)
			{
				ps = noTexturePS;
				vs = noTextureVS;
			}
			else
			{
				// If texture contains only an alpha channel, use alpha shader (for fonts)
				vs = diffTextureVS;

				if (batch.texture_->GetFormat() == alphaFormat)
					ps = alphaTexturePS;
				else if (batch.blendMode_ != BLEND_ALPHA && batch.blendMode_ != BLEND_ADDALPHA && batch.blendMode_ != BLEND_PREMULALPHA)
					ps = diffMaskTexturePS;
				else
					ps = diffTexturePS;
			}

			graphics_->SetShaders(vs, ps);
            if (graphics_->NeedParameterUpdate(SP_OBJECT, this))
				graphics_->SetShaderParameter(VSP_MODEL, Matrix3x4::IDENTITY);
			if (graphics_->NeedParameterUpdate(SP_CAMERA, this))
				graphics_->SetShaderParameter(VSP_VIEWPROJ, projection);
			if (graphics_->NeedParameterUpdate(SP_MATERIAL, this))
				graphics_->SetShaderParameter(PSP_MATDIFFCOLOR, Color(1.0f, 1.0f, 1.0f, 1.0f));

			graphics_->SetBlendMode(batch.blendMode_);
			graphics_->SetScissorTest(true, batch.scissor_);
			graphics_->SetTexture(0, batch.texture_);
			graphics_->Draw(TRIANGLE_LIST, batch.vertexStart_ / UI_VERTEX_SIZE, (batch.vertexEnd_ - batch.vertexStart_) /
				UI_VERTEX_SIZE);
		}
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
		RenderUpdate();
	}

	void NanoGUI::HandleRender(StringHash eventType, VariantMap& eventData)
	{
		Render();
	}

	void NanoGUI::SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData)
	{
		if (vertexData.Empty())
			return;

		// Update quad geometry into the vertex buffer
		// Resize the vertex buffer first if too small or much too large
		unsigned numVertices = vertexData.Size() / NANOGUI_VERTEX_SIZE;
		if (dest->GetVertexCount() < numVertices || dest->GetVertexCount() > numVertices * 2)
			dest->SetSize(numVertices, MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1, true);

		dest->SetData(&vertexData[0]);
	}






}