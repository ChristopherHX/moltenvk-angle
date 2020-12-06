#include "../../Graphics/Viewport.h"
#include "../../DotNet/Defines.h"
using namespace Urho3D;
extern "C"
{

URHO3D_API Viewport* Viewport_Viewport(Context* nativeContext, Scene* scene, Camera* camera, RenderPath* renderPath = nullptr)
{
    return new Viewport(nativeContext, scene, camera, renderPath);
}

}
