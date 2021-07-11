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
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
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
#include <Urho3D/Graphics/Technique.h>

#include "NVG.h"
#include "VGCanvas.h"
#include "VGEvents.h"
#include "VGFrameBuffer.h"
#include "VGElement.h"
#include "VGComponent.h"

#include "NonoVG.h"

#include "Demo.h"


static DemoData demoData_;


#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(NonoVG)


NonoVG::NonoVG(Context* context)
    : Sample(context)
    , time_(0)
{
}

void NonoVG::Setup()
{
    engineParameters_[EP_RESOURCE_PATHS] = "Data;CoreData;";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetProgramDir() + GetTypeName() + ".log";
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_WINDOW_WIDTH] = 1400;
    engineParameters_[EP_WINDOW_HEIGHT] = 1000;
    engineParameters_[EP_WINDOW_RESIZABLE] = true;
    engineParameters_[EP_WINDOW_TITLE] = GetTypeName();

    context_->RegisterSubsystem(new NanoVG(context_));
    VGElement::RegisterObject(context_);
    VGCanvas::RegisterObject(context_);
    VGFrameBuffer::RegisterObject(context_);
    VGComponent::RegisterObject(context_);

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

    
    NanoVG* nvg = GetSubsystem<NanoVG>();
    if (nvg)
    {
        nvg->Initialize();
        loadDemoData(nvg, &demoData_);

    }

    CreateScene();

    InitControls();

    SetupViewport();

    SubscribeToEvents();

    // Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);
}


void NonoVG::CreateScene()
{
    auto* cache = GetSubsystem<ResourceCache>();
    auto* graphics = GetSubsystem<Graphics>();
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



    // Create a "screen" like object for viewing the second scene. Construct it from two StaticModels, a box for the
    // frame and a plane for the actual view
    {
        Node* boxNode = scene_->CreateChild("ScreenBox");
        boxNode->SetPosition(Vector3(0.0f, 10.0f, 0.0f));
        boxNode->SetScale(Vector3(21.0f, 16.0f, 0.5f));
        auto* boxObject = boxNode->CreateComponent<StaticModel>();
        boxObject->SetModel(cache->GetResource<Model>("Models/Box.mdl"));
        boxObject->SetMaterial(cache->GetResource<Material>("Materials/Stone.xml"));

        Node* screenNode = scene_->CreateChild("Screen");
        screenNode->SetPosition(Vector3(0.0f, 10.0f, -0.27f));
        screenNode->SetRotation(Quaternion(-90.0f, 0.0f, 0.0f));
        screenNode->SetScale(Vector3(20.0f, 0.0f, 15.0f));
        auto* screenObject = screenNode->CreateComponent<StaticModel>();
        screenObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));

        VGFrameBuffer* vgFrameBuffer = scene_->CreateComponent<VGFrameBuffer>();
        vgFrameBuffer->CreateFrameBuffer(1024, 1024);
        vgFrameBuffer->SetClearColor(Color(0.4, 0.4, 0.4, 1.0));
        vgFrameBuffer->EnableRenderEvents();
        // Create a new material from scratch, use the diffuse unlit technique, assign the render texture
        // as its diffuse texture, then assign the material to the screen plane object
        SharedPtr<Material> renderMaterial(new Material(context_));
        renderMaterial->SetTechnique(0, cache->GetResource<Technique>("Techniques/DiffUnlit.xml"));
        renderMaterial->SetTexture(TU_DIFFUSE, vgFrameBuffer->GetRenderTarget());
        // Since the screen material is on top of the box model and may Z-fight, use negative depth bias
        // to push it forward (particularly necessary on mobiles with possibly less Z resolution)
        renderMaterial->SetDepthBias(BiasParameters(-0.001f, 0.0f));
        screenObject->SetMaterial(renderMaterial);
    }



    // Create a scene node for the camera, which we will move around
    // The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
    cameraNode_ = scene_->CreateChild("Camera");
    cameraNode_->CreateComponent<Camera>();

    // Set an initial position for the camera scene node above the plane
    cameraNode_->SetPosition(Vector3(0.0f, 7.0f, -30.0f));
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
    auto* graphics = GetSubsystem<Graphics>();
    NanoVG* nvg = GetSubsystem<NanoVG>();
    SharedPtr<Window> window_  = InitWindow();
    VGCanvas * vgCanvas = window_->CreateChild<VGCanvas>("VGCanvas");
    vgCanvas->SetClearColor(Color(0.5,0.5,0.5,1.0));
 
    window_ = InitWindow();
    vgCanvas = window_->CreateChild<VGCanvas>("VGCanvas2");
    vgCanvas->SetClearColor(Color(0.5, 0.5, 0.5, 1.0));
    window_->SetPosition(200, 200);


    vgComponents_.Clear();
    SharedPtr<VGComponent> vgComponent = VGComponent::Create(scene_, "vgComponentRoot");
    vgComponents_.Push(vgComponent);
    vgComponent = vgComponent->CreateChild("vgComponentChild");
    vgComponents_.Push(vgComponent);
    vgComponent = vgComponent->CreateChild("vgComponentChild2");
    vgComponents_.Push(vgComponent);
    vgComponent = vgComponent->CreateChild("vgComponentChild3");
    vgComponents_.Push(vgComponent);
 

    /*
    window_ = InitWindow();
    int winSize = Min(graphics->GetWidth() / 2.0, graphics->GetHeight() / 2.0);

    window_->SetMinWidth(winSize);
    window_->SetMinHeight(winSize);
    window_->SetWidth(winSize);
    window_->SetHeight(winSize);
    Sprite * sprite = window_->CreateChild<Sprite>("SVGSprite");
    svgTexture = nvg->LoadSVGIntoTexture("nanosvg/23_modified.svg"); 
    sprite->SetTexture(svgTexture);
    window_->SetPosition(300, 300);
    */
    
}

SharedPtr<Window> NonoVG::InitWindow()
{
    // Create the Window and add it to the UI's root node
        
    /// The Window.
    SharedPtr<Window> window_(new Window(context_));
    uiRoot_->AddChild(window_);

    auto* graphics = GetSubsystem<Graphics>();

    // Set Window size and layout settings
    window_->SetMinWidth(graphics->GetWidth() / 1.5);
    window_->SetMinHeight(graphics->GetHeight() / 1.5);
    window_->SetLayout(LM_VERTICAL, 6, IntRect(6, 6, 6, 6));
    window_->SetAlignment(HA_LEFT, VA_TOP);
    window_->SetName("NanoVG Window");
    window_->SetMovable(true);
    window_->SetResizable(true);
    window_->SetFocusMode(FocusMode::FM_FOCUSABLE);

    // Create Window 'titlebar' container
    auto* titleBar = new UIElement(context_);
    titleBar->SetMinSize(0, 24);
    titleBar->SetMaxHeight(24);
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

    return window_;
}

void NonoVG::MoveCamera(float timeStep)
{
    // Do not move if the UI has a focused element (the console)
    if (GetSubsystem<UI>()->GetFocusElement())
        return;

    auto* input = GetSubsystem<Input>();

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    IntVector2 mouseMove = input->GetMouseMove();
    yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
    pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
    pitch_ = Clamp(pitch_, -90.0f, 90.0f);

    // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
    cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    // Use the Translate() function (default local space) to move relative to the node's orientation.
    if (input->GetKeyDown(KEY_W))
        cameraNode_->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_S))
        cameraNode_->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_A))
        cameraNode_->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
    if (input->GetKeyDown(KEY_D))
        cameraNode_->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);
}

void NonoVG::SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(NonoVG, HandleUpdate));
    SubscribeToEvent(E_VGRENDER, URHO3D_HANDLER(NonoVG, HandleNVGRender));
    SubscribeToEvent(E_VGFBRENDER, URHO3D_HANDLER(NonoVG, HandleVGFBRender));
}

void NonoVG::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    time_ += timeStep;
    // Move the camera, scale movement with time step
    MoveCamera(timeStep);
}

void NonoVG::HandleNVGRender(StringHash eventType, VariantMap& eventData)
{
    using namespace VGRender;

    VGElement* nanoVGUIElement = static_cast<VGElement*>(eventData[P_VGELEMENT].GetPtr());
    IntVector2 size = nanoVGUIElement->GetSize();

    String canvasName = nanoVGUIElement->GetName();

    if (canvasName == "VGCanvas")
    {
        renderVGElement(nanoVGUIElement, 0, 0, size.x_, size.y_, time_, 0, &demoData_);
    }

    if (canvasName == "VGCanvas2")
    {
        RenderVGComponents();
    }
}


void NonoVG::RenderVGComponents() 
{
    NanoVG* nvg = GetSubsystem<NanoVG>();
    VGFrameBuffer* frameBuffer = nvg->GetCurrentFrameBuffer();

    if (frameBuffer == nullptr)
        return;

    IntVector2  frameBufferSize = frameBuffer->GetSize();
    int screenWidth = frameBufferSize.x_;
    int screenHeight = frameBufferSize.y_;

    for (VGComponent * vgComponent : vgComponents_)
    {
        String name = vgComponent->GetName();
        if (name == "vgComponentRoot")
        {
            float width = screenWidth * 0.04;
            float height = screenWidth * 0.02;
            float hotspot_x = width / 2.0;
            float hotspot_y = height / 2.0;

            vgComponent->BeginDraw();
            NVGpaint bg = vgComponent->LinearGradient(0, 0, 60, 30, nvgRGBA(255, 255, 255, 32), nvgRGBA(0, 0, 0, 32));
            vgComponent->SetPosition(screenWidth / 2.0, screenHeight/2.0);
            vgComponent->SetRotation(time_);
            //  vgComponent->SetScale(2.0f, 1.0f);
            vgComponent->BeginPath();
            vgComponent->Ellipse(width, height);
            vgComponent->SetHotSpot(hotspot_x, hotspot_y);
            vgComponent->FillColor(nvgRGBA(0, 96, 128, 255));
            vgComponent->Fill();
            vgComponent->FillPaint(bg);
            vgComponent->Fill();
            vgComponent->EndDraw();
        }
        else if (name == "vgComponentChild")
        {
            float width = screenWidth * 0.1;
            float height = screenWidth * 0.1;
            float hotspot_x = width / 2.0;
            float hotspot_y = height / 2.0;

            vgComponent->BeginDraw();
            /*position is set relative to the parent ,vgComponentRoot  */
            vgComponent->SetPosition(screenWidth * 0.17, screenHeight * 0.17);
            vgComponent->SetRotation(-2 * time_);
            vgComponent->SetHotSpot(hotspot_x, hotspot_y);
            drawColorwheelOnVGComponent(vgComponent, 0, 0, width, height, 0);
            vgComponent->EndDraw();
        }
        else if (name == "vgComponentChild2")
        {
            float width = screenWidth * 0.02;
            float height = screenWidth * 0.02;
            float hotspot_x = width / 2.0;
            float hotspot_y = height / 2.0;

            vgComponent->BeginDraw();
            NVGpaint bg = vgComponent->LinearGradient(0, 0, 15, 30, nvgRGBA(255, 255, 255, 32), nvgRGBA(0, 0, 0, 32));
            /*position is set relative to the parent ,vgComponentChild  */
            vgComponent->SetPosition(screenWidth * 0.17, screenHeight * 0.17);
            vgComponent->SetRotation(2 * time_);
            vgComponent->BeginPath();
            vgComponent->RoundedRect(width, height, 3);
            vgComponent->SetHotSpot(hotspot_x, hotspot_y);
            vgComponent->FillColor(nvgRGBA(125, 45, 200, 255));
            vgComponent->Fill();
            vgComponent->FillPaint(bg);
            vgComponent->Fill();
            vgComponent->EndDraw();
        }
        else if (name == "vgComponentChild3")
        {
            float width = screenWidth * 0.07;
            float height = screenWidth * 0.07;
            float hotspot_x = width / 2.0;
            float hotspot_y = height / 2.0;

            vgComponent->BeginDraw();
            /*position is set relative to the parent ,vgComponentChild2  */
            vgComponent->SetPosition(screenWidth * 0.03, screenHeight * 0.03);
            vgComponent->SetRotation(-time_);
          
            NVGpaint imgPaint = vgComponent->ImagePattern(0, 0, width, height, 0, demoData_.svgImage, 1.0);
            vgComponent->BeginPath();
            vgComponent->RoundedRect(width, width, 3);
            vgComponent->SetHotSpot(hotspot_x, hotspot_y);
            vgComponent->FillPaint(imgPaint);
            vgComponent->Fill();
            vgComponent->EndDraw();
        }
    }
}

void NonoVG::HandleVGFBRender(StringHash eventType, VariantMap& eventData) {

    using namespace VGFBRender;
    VGFrameBuffer* vgFrameBuffer = static_cast<VGFrameBuffer*>(eventData[P_VGFRAMEBUFFER].GetPtr());
    IntVector2 size = vgFrameBuffer->GetSize();
   renderVGFrameBuffer(vgFrameBuffer, 0, 0, size.x_, size.y_, time_, 0, &demoData_);
}

void NonoVG::Stop()
{

    Sample::Stop();
    NanoVG* nvg = GetSubsystem<NanoVG>();
    if (nvg)
    {
        nvg->Clear();
    }

    svgTexture.Reset();

}

