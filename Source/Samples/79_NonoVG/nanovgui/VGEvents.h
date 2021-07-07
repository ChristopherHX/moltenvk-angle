#pragma once

#include "../Core/Object.h"

namespace Urho3D
{

	/// Update of NanoVG rendering.
URHO3D_EVENT(E_VGRENDER, VGRender)
{
    URHO3D_PARAM(P_VGELEMENT, VGElement);       // NanoVGUIElement Element pointer
}

URHO3D_EVENT(E_VGFBRENDER, VGFBRender)
{
    URHO3D_PARAM(P_VGFRAMEBUFFER, VGFrameBuffer); // NanoVGUIElement Element pointer
}

}