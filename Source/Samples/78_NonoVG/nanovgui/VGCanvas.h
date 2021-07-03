#pragma once

#include "GLHeaders.h"
#include "VGElement.h"


struct NVGcontext;
struct NVGLUframebuffer;

namespace Urho3D
{

class Texture2D;

class URHO3D_API VGCanvas : public VGElement
{
    URHO3D_OBJECT(VGCanvas, VGElement);

    /// Construct.
    explicit VGCanvas(Context* context);
    /// Destruct.
    ~VGCanvas() override;
    /// Register object factory.
    /// @nobind
    static void RegisterObject(Context* context);


};

}
