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

#pragma once

#include "Sample.h"


const int CWIDTH = 1400;
const int CHEIGHT = 1200;


namespace Urho3D
{

class Window;

}


class NonoVG : public Sample
{
    URHO3D_OBJECT(NonoVG, Sample);

public:
    /// Construct.
    explicit NonoVG(Context* context);

    void Setup() override;
    /// Setup after engine initialization and before running the main loop.
    void Start() override;

    /// Cleanup after the main loop. Called by Application.
    void Stop() override;

protected:
    /// Return XML patch instructions for screen joystick layout for a specific sample app, if any.
    String GetScreenJoystickPatchString() const override { return
        "<patch>"
        "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Hat0']]\">"
        "        <attribute name=\"Is Visible\" value=\"false\" />"
        "    </add>"
        "</patch>";
    }

private:
    /// Create and initialize a Window control.
    void InitWindow();
    /// Create and add various common controls for demonstration purposes.
    void InitControls();
    
    void CreateScene();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();
    
    /// The Window.
    SharedPtr<Window> window_;
    /// The UI's root UIElement.
    SharedPtr<UIElement> uiRoot_;


    SharedPtr<Texture2D> drawTexture_;

    SharedPtr<BorderImage> NanoVGPresentation_;

    IntVector2 textureSize_;
    Vector2 textureScale_;
};


