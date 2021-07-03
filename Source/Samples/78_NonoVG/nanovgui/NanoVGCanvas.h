#pragma once

#include "GLHeaders.h"
#include "NanoVGUIElement.h"


struct NVGcontext;
struct NVGLUframebuffer;

namespace Urho3D
{

class Texture2D;

class URHO3D_API NanoVGCanvas : public NanoVGUIElement
{
    URHO3D_OBJECT(NanoVGCanvas, NanoVGUIElement);

    /// Construct.
    explicit NanoVGCanvas(Context* context);
    /// Destruct.
    ~NanoVGCanvas() override;
    /// Register object factory.
    /// @nobind
    static void RegisterObject(Context* context);

    /// Perform UI element update.
    void Update(float timeStep) override;


private:
    /// Handle render event.
    void HandleRender(StringHash eventType, VariantMap& eventData);

private:
    float time_;
};

}
