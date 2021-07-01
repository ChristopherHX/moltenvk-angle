//
// Copyright (c) 2008-2021 the Urho3D project.
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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>


#include "NanoGUI.h"

#include "NonoVG.h"

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(NonoVG)

NonoVG::NonoVG(Context* context) :
    Sample(context)
{

}

void NonoVG::Setup()
{

    engineParameters_[EP_RESOURCE_PATHS] = "Data/nanovg;Data;CoreData;";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetProgramDir() + GetTypeName() + ".log";
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_WINDOW_WIDTH] = 1280;
    engineParameters_[EP_WINDOW_HEIGHT] = 720;
    engineParameters_[EP_WINDOW_TITLE] = GetTypeName();

    NanoGUI::RegisterObject(context_);
    context_->RegisterSubsystem(new NanoGUI(context_));
}

void NonoVG::Start()
{
    // Execute base class startup
    Sample::Start();
    
    uiRoot_ = GetSubsystem<UI>()->GetRoot();
    // Load XML file containing default UI style sheet
    auto* cache = GetSubsystem<ResourceCache>();
    auto* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Set the loaded style as default style
    uiRoot_->SetDefaultStyle(style);

    InitWindow();

    InitControls();
    
    CreateScene();
    SetupViewport();
    

    // Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);
}

void NonoVG::CreateScene()
{
    auto* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);

    // Create the Octree component to the scene. This is required before adding any drawable components, or else nothing
    // will show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world
    // coordinates; it is also legal to place objects outside the volume but their visibility can then not be checked in
    // a hierarchically optimizing manner
    scene_->CreateComponent<Octree>();

    // Create a child scene node (at world origin) and a StaticModel component into it. Set the StaticModel to show a
    // simple plane mesh with a "stone" material. Note that naming the scene nodes is optional. Scale the scene node
    // larger (100 x 100 world units)
    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    auto* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    // Create a directional light to the world so that we can see something. The light scene node's orientation controls
    // the light direction; we will use the SetDirection() function which calculates the orientation from a forward
    // direction vector. The light will use default settings (white light, no shadows)
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    auto* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    // Create more StaticModel objects to the scene, randomly positioned, rotated and scaled. For rotation, we construct
    // a quaternion from Euler angles where the Y angle (rotation about the Y axis) is randomized. The mushroom model
    // contains LOD levels, so the StaticModel component will automatically select the LOD level according to the view
    // distance (you'll see the model get simpler as it moves further away). Finally, rendering a large number of the
    // same object with the same material allows instancing to be used, if the GPU supports it. This reduces the amount
    // of CPU work in rendering the scene.
    const unsigned NUM_OBJECTS = 200;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* mushroomNode = scene_->CreateChild("Mushroom");
        mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
        mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        mushroomNode->SetScale(0.5f + Random(2.0f));
        auto* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
        mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
        mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
    }

    // Create a scene node for the camera, which we will move around
    // The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();

    // Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0f, 5.0f, 0.0f));
}


void NonoVG::SetupViewport()
{
    auto* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the
    // camera at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward /
    // deferred) to use, but now we just use full screen and default render path configured in the engine command line
    // options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}


void NonoVG::InitControls()
{

  
    drawTexture_ = new Texture2D(context_);
    textureSize_ = IntVector2(CWIDTH, CHEIGHT);
    drawTexture_->SetMipsToSkip(QUALITY_HIGH, 0);
    drawTexture_->SetNumLevels(1);
    drawTexture_->SetSize(textureSize_.x_, textureSize_.y_, Graphics::GetRGBAFormat(), TEXTURE_RENDERTARGET);

    NanoGUI* nanogui = GetSubsystem<NanoGUI>();
    if (nanogui)
    {
        nanogui->Initialize(drawTexture_,CWIDTH, CHEIGHT);
    }

  
    // Create a NanoVG Presentation
    NanoVGPresentation_ = new BorderImage(context_);
    NanoVGPresentation_->SetName("NanoVGPresentation");
    NanoVGPresentation_->SetMinHeight(CHEIGHT);
    NanoVGPresentation_->SetMinWidth(CWIDTH);
    NanoVGPresentation_->SetTexture(drawTexture_);
    window_->AddChild(NanoVGPresentation_);
 
}

void NonoVG::InitWindow()
{
    // Create the Window and add it to the UI's root node
    window_ = new Window(context_);
    uiRoot_->AddChild(window_);

    // Set Window size and layout settings
    window_->SetMinWidth(384);
    window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    window_->SetAlignment(HA_CENTER, VA_CENTER);
    window_->SetName("NanoVG Window");

    // Create Window 'titlebar' container
    auto* titleBar = new UIElement(context_);
    titleBar->SetMinSize(0, 24);
    titleBar->SetVerticalAlignment(VA_TOP);
    titleBar->SetLayoutMode(LM_HORIZONTAL);

    // Create the Window title Text
    auto* windowTitle = new Text(context_);
    windowTitle->SetName("WindowTitle");
    windowTitle->SetText("Hello NanoVG!");

    // Create the Window's close button
    auto* buttonClose = new Button(context_);
    buttonClose->SetName("CloseButton");

    // Add the controls to the title bar
    titleBar->AddChild(windowTitle);
    titleBar->AddChild(buttonClose);

    // Add the title bar to the Window
    window_->AddChild(titleBar);

    // Apply styles
    window_->SetStyleAuto();
    windowTitle->SetStyleAuto();
    buttonClose->SetStyle("CloseButton");

}

void NonoVG::Stop() {

    // Execute base class startup
    Sample::Stop();
    NanoGUI* nanogui = GetSubsystem<NanoGUI>();
    if (nanogui)
    {
        nanogui->Clear();
    }
}

