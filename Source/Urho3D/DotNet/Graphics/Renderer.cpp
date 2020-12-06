#include "../../Graphics/Renderer.h"
#include "../../DotNet/Defines.h"
using namespace Urho3D;
extern "C"
{

URHO3D_API void Renderer_SetViewport(Renderer* nativeInstance, unsigned index, Viewport* viewport)
{
    nativeInstance->SetViewport(index, viewport);
}

}