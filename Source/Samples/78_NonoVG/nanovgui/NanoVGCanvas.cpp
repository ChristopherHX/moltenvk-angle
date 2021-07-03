//
// Copyright (c) 2008-2020 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "../Precompiled.h"

#include "../Core/Context.h"
#include "../Input/InputEvents.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"
#include "../Graphics/GraphicsEvents.h"

#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>

#include "NanoVGSubSystem.h"
#include "NanoVGCanvas.h"

#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#include "nanovg.h"
#include "nanovg_gl_utils.h"

#include "../Demo.h"

#include "../DebugNew.h"

namespace Urho3D
{

static DemoData data;

NanoVGCanvas::NanoVGCanvas(Context* context)
    : NanoVGUIElement(context)
    , time_(0)
{
    SubscribeToEvent(E_ENDRENDERING, URHO3D_HANDLER(NanoVGCanvas, HandleRender));
    loadDemoData(GetSubsystem<ResourceCache>(), vg_, &data);
}

NanoVGCanvas::~NanoVGCanvas() = default;

void NanoVGCanvas::RegisterObject(Context* context) 
{
    context->RegisterFactory<NanoVGCanvas>("UI");
    URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
    URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);
}

void NanoVGCanvas::Update(float timeStep) 
{ 
    time_ += timeStep; 
}

/// Handle render event.
void NanoVGCanvas::HandleRender(StringHash eventType, VariantMap& eventData) 
{
    if (vg_ != nullptr)
    {
        BeginRender();
        renderDemo(vg_, 0, 0, textureSize_.x_, textureSize_.y_, time_, 0, &data);
        EndRender();
    }
}

}
  
