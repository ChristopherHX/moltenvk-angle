//
// Copyright (c) 2008-2017 the Urho3D project.
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



#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>
#ifdef URHO3D_ANGELSCRIPT
#include <Urho3D/AngelScript/ScriptFile.h>
#include <Urho3D/AngelScript/Script.h>
#endif

#include "%s.h"

#include <Urho3D/DebugNew.h>


URHO3D_DEFINE_APPLICATION_MAIN(%s)

void LoadCustomComponents(Context * context_);

%s::%s(Context* context) :
    Sample(context)
{
	configFileName = "";
	startingScene = "";
	isSceneLoading = false;
}

void %s::Setup()
{
	 // Execute base class Setup
	Sample::Setup();

    /*write your setup configurations here */
}

void %s::Start()
{
    // Execute base class startup
    Sample::Start();
	
#ifdef URHO3D_ANGELSCRIPT
	// Instantiate and register the AngelScript subsystem
	context_->RegisterSubsystem(new Script(context_));
#endif

	LoadCustomComponents(context_);



	LoadConfig();

	if (startingScene != "")
	{
		LoadScene(startingScene);
	}
	else
	{
		// Create the scene content
		CreateScene();
		// Setup the viewport for displaying the scene
		SetupViewport();
	}

}

/// Cleanup after the main loop. Called by Application.
void %s::Stop()
{
	// Execute base class Stop
	Sample::Stop();

    /// WRITE YOUR CODE HERE
	

}

void %s::LoadConfig()
{
	FileSystem* fileSystem = GetSubsystem<FileSystem>();
	Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

	configFileName = cache->GetResourceFileName("Config.xml");

	if (!fileSystem->FileExists(configFileName))
		return;

	SharedPtr<File> file(new File(context_, configFileName, FILE_READ));
	SharedPtr<XMLFile> config(new XMLFile(context_));

	config->Load(*file);

	XMLElement configElem = config->GetRoot();
	if (configElem.IsNull())
		return;

	XMLElement generalElem = configElem.GetChild("general");

	if (!generalElem.IsNull())
	{
		if (generalElem.HasAttribute("scene")) startingScene = generalElem.GetAttribute("scene");
	}
}

bool %s::LoadScene(Urho3D::String & fileName)
{

	Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
	FileSystem* fileSystem = GetSubsystem<FileSystem>();

	if (isSceneLoading == true)
	{
		return true;
	}

	isSceneLoading = true;
	String fullPathFileName = cache->GetResourceFileName(fileName);


	if (scene_ == NULL)
	{
		scene_ = new Scene(context_);
	}

	if (fullPathFileName.Empty())
	{
		//MessageBox("No such scene.\n" + fullPathFileName);
		return false;

	}


	// Always load the scene from the filesystem, not from resource paths
	if (!fileSystem->FileExists(fullPathFileName))
	{
		//MessageBox("No such scene.\n" + fullPathFileName);
		return false;
	}

	File file(context_,fullPathFileName, FILE_READ);
	if (!file.IsOpen())
	{
		//MessageBox("Could not open file.\n" + fullPathFileName);
		return false;
	}


	String extension = GetExtension(fullPathFileName);
	bool loaded = false;
	if (extension != ".xml")
		loaded = scene_->Load(file);
	else
		loaded = scene_->LoadXML(file);


	if (scene_ != NULL)
	{
		scene_->SetName(fileName);
	}




	if (loaded == true)
	{
		PODVector<Node*> cameraNodes = scene_->GetChildrenWithComponent("Camera", true);

		// if there is a camera in the scene
		// the first one found will be the main camera.
		if (cameraNodes.Size() > 0)
		{
			for (unsigned int  i = 0; i < cameraNodes.Size(); i++)
			{
				if (cameraNodes[i]->IsEnabled() == true)
				{
					cameraNode_ = cameraNodes[i];
					break;
				}
			}
		}
		else
		{
			// Create a scene node for the camera, which we will move around
			// The camera will use default settings (1000 far clip distance, 45 degrees FOV, set aspect ratio automatically)
			cameraNode_ = scene_->CreateChild("Camera");
			cameraNode_->CreateComponent<Camera>();

			// Set an initial position for the camera scene node above the plane
			cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -5.0f));
		}


		SetupViewport();
	}

	cache->ReleaseAllResources(false);

	isSceneLoading = false;
	return loaded;
}

void %s::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);

    // Create the Octree component to the scene. This is required before adding any drawable components, or else nothing will
    // show up. The default octree volume will be from (-1000, -1000, -1000) to (1000, 1000, 1000) in world coordinates; it
    // is also legal to place objects outside the volume but their visibility can then not be checked in a hierarchically
    // optimizing manner
    scene_->CreateComponent<Octree>();

    // Create a child scene node (at world origin) and a StaticModel component into it. Set the StaticModel to show a simple
    // plane mesh with a "stone" material. Note that naming the scene nodes is optional. Scale the scene node larger
    // (100 x 100 world units)
    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    // Create a directional light to the world so that we can see something. The light scene node's orientation controls the
    // light direction; we will use the SetDirection() function which calculates the orientation from a forward direction vector.
    // The light will use default settings (white light, no shadows)
    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f)); // The direction vector does not need to be normalized
    Light* light = lightNode->CreateComponent<Light>();
    light->SetLightType(LIGHT_DIRECTIONAL);

    // Create more StaticModel objects to the scene, randomly positioned, rotated and scaled. For rotation, we construct a
    // quaternion from Euler angles where the Y angle (rotation about the Y axis) is randomized. The mushroom model contains
    // LOD levels, so the StaticModel component will automatically select the LOD level according to the view distance (you'll
    // see the model get simpler as it moves further away). Finally, rendering a large number of the same object with the
    // same material allows instancing to be used, if the GPU supports it. This reduces the amount of CPU work in rendering the
    // scene.
    const unsigned NUM_OBJECTS = 200;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* mushroomNode = scene_->CreateChild("Mushroom");
        mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
        mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        mushroomNode->SetScale(0.5f + Random(2.0f));
        StaticModel* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
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



void %s::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();

    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen. We need to define the scene and the camera
    // at minimum. Additionally we could configure the viewport screen size and the rendering path (eg. forward / deferred) to
    // use, but now we just use full screen and default render path configured in the engine command line options
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}


