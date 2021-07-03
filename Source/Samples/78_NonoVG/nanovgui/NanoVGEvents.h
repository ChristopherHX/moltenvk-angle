#pragma once

#include "../Core/Object.h"

namespace Urho3D
{

	/// Update of NanoVG rendering.
URHO3D_EVENT(E_NVGRENDER, NVGRender)
{
    URHO3D_PARAM(P_NVGELEMENT, NanoVGElement);       // NanoVGUIElement Element pointer
    URHO3D_PARAM(P_NVGCONTEXT, NanoVGcontext);        // NVGcontext pointer
}

}