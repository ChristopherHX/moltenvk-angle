#pragma once
#include "GLHeaders.h"

#include <Urho3D/UI/BorderImage.h>

struct NVGcontext;
struct NVGLUframebuffer;

namespace Urho3D
{

class Texture2D;

class URHO3D_API NanoVGUIElement : public BorderImage
{
    URHO3D_OBJECT(NanoVGUIElement, BorderImage);

    /// Construct.
    explicit NanoVGUIElement(Context* context);
    /// Destruct.
    ~NanoVGUIElement() override;
    /// Register object factory.
    /// @nobind
    static void RegisterObject(Context* context);

    /// React to resize.
    void OnResize(const IntVector2& newSize, const IntVector2& delta) override;

    void BeginRender();
    void EndRender();

protected:

    void CreateFrameBuffer(int mWidth, int mHeight);

protected:
    WeakPtr<Graphics> graphics_; 
    Urho3D::SharedPtr<Texture2D> drawTexture_;
    IntVector2 textureSize_;
    NVGLUframebuffer* nvgFrameBuffer_;
    NVGcontext* vg_;
    GLint previousVBO;
    GLint previousFBO;
    ShaderVariation* previousVS;
    ShaderVariation* previousPS;
};

} // namespace Urho3D
