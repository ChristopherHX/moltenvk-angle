/*!
 * \file NanoGUI.h
 *
 * \author vitali
 * \date Februar 2015
 *
 * 
 */

#pragma once
#include "..\Core\Object.h"

struct NVGcontext;
struct NVGLUframebuffer;


namespace Urho3D
{
	class Graphics;
	class VertexBuffer;
	class Cursor;
	class ResourceCache;
	class Timer;
	class UIBatch;
	class UIElement;
	class XMLElement;
	class XMLFile;

	struct Theme {
	public:
		/* Fonts */
		int mFontNormal;
		int mFontBold;
		int mFontIcons;

		/* Spacing-related parameters */
		int mStandardFontSize;
		int mButtonFontSize;
		int mWindowCornerRadius;
		int mWindowHeaderHeight;
		int mWindowDropShadowSize;
		int mButtonCornerRadius;

		/* Generic colors */
		Color mDropShadow;
		Color mTransparent;
		Color mBorderDark;
		Color mBorderLight;
		Color mBorderMedium;
		Color mTextColor;
		Color mDisabledTextColor;
		Color mTextColorShadow;
		Color mIconColor;

		/* Button colors */
		Color mButtonGradientTopFocused;
		Color mButtonGradientBotFocused;
		Color mButtonGradientTopUnfocused;
		Color mButtonGradientBotUnfocused;
		Color mButtonGradientTopPushed;
		Color mButtonGradientBotPushed;

		/* Window colors */
		Color mWindowFillUnfocused;
		Color mWindowFillFocused;
		Color mWindowTitleUnfocused;
		Color mWindowTitleFocused;

		Color mWindowHeaderGradientTop;
		Color mWindowHeaderGradientBot;
		Color mWindowHeaderSepTop;
		Color mWindowHeaderSepBot;

		Color mWindowPopup;
		Color mWindowPopupTransparent;

		Theme(NVGcontext *ctx);
	};

class NanoGUI : public Object
{
    URHO3D_OBJECT(NanoGUI, Object);

public:
	NanoGUI(Context* context);
	virtual ~NanoGUI();
	static void RegisterObject(Context* context);
	/// Initialize when screen mode initially set.
	void Initialize();
	/// Clear the UI (excluding the cursor.)
	void Clear();
	/// Update the UI logic. Called by HandlePostUpdate().
	void Update(float timeStep);
	/// Update the UI for rendering. Called by HandleRenderUpdate().
	void RenderUpdate();
	/// Render the UI. If resetRenderTargets is true, is assumed to be the default UI render to backbuffer called by Engine, and will be performed only once. Additional UI renders to a different rendertarget may be triggered from the renderpath.
	void Render(bool resetRenderTargets = true);


protected:
	/// Handle frame begin event.
	void HandleBeginFrame(StringHash eventType, VariantMap& eventData);
	/// Handle logic post-update event.
	void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle render update event.
	void HandleRenderUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle render event.
	void HandleRender(StringHash eventType, VariantMap& eventData);
	/// Upload UI geometry into a vertex buffer.
	void SetVertexData(VertexBuffer* dest, const PODVector<float>& vertexData);
	/// Render UI batches. Geometry must have been uploaded first.
	void Render(bool resetRenderTargets, VertexBuffer* buffer, const PODVector<UIBatch>& batches, unsigned batchStart, unsigned batchEnd);
	/// Initialized flag.
	bool initialized_;
	/// Graphics subsystem.
	WeakPtr<Graphics> graphics_;
	/// UI rendering vertex data.
	PODVector<float> vertexData_;
	/// UI vertex buffer.
	SharedPtr<VertexBuffer> vertexBuffer_;
	/// UI rendering batches.
	PODVector<UIBatch> batches_;
	/// Non-modal batch size (used internally for rendering).
	unsigned nonModalBatchSize_;
	/// UI root element.
	SharedPtr<UIElement> rootElement_;

	/// nanovg context
	NVGcontext* vg_;
	Theme* theme_;
private:
};
}

