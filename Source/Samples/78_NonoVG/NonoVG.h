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

private:
    /// Create and initialize a Window control.
    SharedPtr<Window> InitWindow();
    /// Create and add various common controls for demonstration purposes.
    void InitControls();
    
    void CreateScene();
    /// Set up a viewport for displaying the scene.
    void SetupViewport();

        /// Read input and moves the camera.
    void MoveCamera(float timeStep);
    /// Subscribe to application-wide logic update events.
    void SubscribeToEvents();
    /// Handle the logic update event.
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

     /// Handle NVG render event.
    void HandleNVGRender(StringHash eventType, VariantMap& eventData);
    
    /// The UI's root UIElement.
    SharedPtr<UIElement> uiRoot_;

    float time_;



};


