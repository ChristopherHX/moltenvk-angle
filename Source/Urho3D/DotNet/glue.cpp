#include <stdio.h>
#include <string.h>
#ifndef _MSC_VER
#   include <unistd.h>
#endif
#include <stdlib.h>
#include "AllUrho.h"
#include "glue.h"
#include "interop.h"

#ifdef __ANDROID__
#include <android/log.h>
#include <jni.h>
#endif

using namespace Urho3D;

//
// This is just an implemention of EventHandler that can be used with function
// pointers, so we can register delegates from C#
//

extern "C" {

	DllExport void *urho_subscribe_event(void *_receiver, HandlerFunctionPtr callback, void *data, int eventNameHash)
	{
		StringHash h(eventNameHash);
		Urho3D::Object *receiver = (Urho3D::Object *) _receiver;
		NotificationProxy *proxy = new NotificationProxy(receiver, callback, data, h);
		receiver->SubscribeToEvent(receiver, h, proxy);
		return proxy;
	}
    
    DllExport
    void * VariantMap_VariantMap()
    {
        return (void*)(new VariantMap());
    }
    
    DllExport
    void VariantMap_Dispose(VariantMap * variantMap)
    {
        if(variantMap)
        {
            delete variantMap;
        }
    }

    DllExport
    void * urho_map_get_variantmap (VariantMap &map, int hash)
    {
        StringHash h (hash);
        return (void*)(&map [h].GetVariantMap());
    }


	DllExport
	void * urho_map_get_ptr (VariantMap &map, int hash)
	{
		StringHash h (hash);
		return map [h].GetVoidPtr ();
	}

	DllExport
	void * urho_map_get_object(VariantMap &map, int hash, int* objHash)
	{
		StringHash h(hash);
		void* ptr = map[h].GetVoidPtr();
		Object * object = static_cast<Object *>(ptr);
		*objHash = object->GetType().Value(); //GetType is virtual
		return ptr;
	}

	DllExport
	const char * urho_map_get_String (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return stringdup(map [h].GetString ().CString());
	}
	
	DllExport
	int urho_map_get_StringHash (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return map [h].GetStringHash ().Value ();
	}
	
	DllExport
	Variant urho_map_get_Variant (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return map [h];
	}
	
	DllExport
	Interop::Vector3 urho_map_get_Vector3 (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return *((Interop::Vector3  *) &(map [h].GetVector3 ()));
	}
	
	DllExport
	Interop::IntVector2 urho_map_get_IntVector2 (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return *((Interop::IntVector2  *) &(map [h].GetIntVector2 ()));
	}

	DllExport
	int urho_map_get_bool (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return map [h].GetBool ();
	}
	
	DllExport
	float urho_map_get_float (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return map [h].GetFloat ();
	}
		
	DllExport
	int urho_map_get_int (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return map [h].GetInt ();
	}
	
	DllExport
	unsigned int urho_map_get_uint (VariantMap& map, int hash)
	{
		StringHash h (hash);
		return map [h].GetUInt ();
	}

	DllExport unsigned char *
	urho_map_get_buffer (VariantMap &map, int hash, unsigned *size)
	{
		StringHash h (hash);
		PODVector<unsigned char> p (map [h].GetBuffer ());
		*size = p.Size();
		unsigned char * result = new unsigned char[p.Size()];
		for (int i = 0; i < p.Size(); i++) {
			result[i] = p[i];
		}
		return result;
	}

	DllExport
	void urho_unsubscribe (NotificationProxy *proxy)
	{
		proxy->Unsub ();
	}

	DllExport void
	UI_LoadLayoutToElement(Urho3D::UI *_target, Urho3D::UIElement *to, Urho3D::ResourceCache *cache, const char * name)
	{		
		SharedPtr<UIElement> layoutRoot = _target->LoadLayout(cache->GetResource<XMLFile>(name));
		to->AddChild(layoutRoot);
	}

	DllExport int
	Scene_LoadXMLFromCache(Urho3D::Scene *_target, Urho3D::ResourceCache *cache, const char * name)
	{
		SharedPtr<File> file = cache->GetFile(name);
		return _target->LoadXML(*file);
	}
	
	DllExport
	void *TouchState_GetTouchedElement (TouchState *state)
	{
		return (void *) state->GetTouchedElement ();
	}

	DllExport
	const char *Urho_GetPlatform ()
	{
		return stringdup (GetPlatform().CString ());
	}

	DllExport
	unsigned urho_stringhash_from_string (const char *p)
	{
		StringHash foo (p);
		return foo.Value ();
	}
	
	DllExport
	Skeleton *AnimatedModel_GetSkeleton (AnimatedModel *model)
	{
		return &model->GetSkeleton ();
	}

	DllExport
	unsigned Controls_GetButtons (Controls *controls)
	{
		return controls->buttons_;
	}

	DllExport
	void* Graphics_GetSdlWindow(Graphics* target)
	{
		return target->GetWindow();
	}

	DllExport
	void Controls_SetButtons (Controls *controls, unsigned value)
	{
		controls->buttons_ = value;
	}
	
	DllExport
	float Controls_GetYaw (Controls *controls)
	{
		return controls->yaw_;
	}

	DllExport
	void Controls_SetYaw (Controls *controls, float value)
	{
		controls->yaw_ = value;
	}

	DllExport
	float Controls_GetPitch (Controls *controls)
	{
		return controls->pitch_;
	}

	DllExport
	void Controls_SetPitch (Controls *controls, float value)
	{
		controls->pitch_ = value;
	}

	DllExport void
	Controls_Reset (Urho3D::Controls *_target)
	{
		_target->Reset ();
	}
	
	DllExport void
	Controls_Set (Urho3D::Controls *_target, unsigned int buttons, int down)
	{
		_target->Set (buttons, down);
	}
	
	DllExport int
	Controls_IsDown (Urho3D::Controls *_target, unsigned int button)
	{
		return _target->IsDown (button);
	}
	
#if !defined(UWP)
	DllExport int 
	Network_Connect(Network *net, const char *ptr, short port, Scene *scene)
	{
		String s(ptr);
		return net->Connect(s, port, scene) ? 1 : 0;
	}

	DllExport const Controls *
	Connection_GetControls (Connection *conn)
	{
		return &conn->GetControls ();
	}
	DllExport void
	Connection_SetControls(Connection *conn, Controls *ctl)
	{
		conn->SetControls(*ctl);
	}
#endif

	DllExport Controls *
	Controls_Create ()
	{
		return new Controls ();
	}

	DllExport void
	Controls_Destroy (Controls *controls)
	{
		delete controls;
	}

	DllExport RayQueryResult *
	Octree_Raycast(Octree *octree, const Urho3D::Ray & ray, const Urho3D::RayQueryLevel & level, float maxDistance, unsigned int flags, unsigned int viewMask, bool single, int *count) {
		PODVector<RayQueryResult> results;
		auto size = sizeof(RayQueryResult);
		RayOctreeQuery query(results, ray, level, maxDistance, flags, viewMask);
		if (single)
			octree->RaycastSingle(query);
		else
			octree->Raycast(query);

		if (results.Size() == 0)
			return NULL;

		RayQueryResult * result = new RayQueryResult[results.Size()];
		*count = results.Size();
		for (int i = 0; i < results.Size(); i++) {
			result[i] = results[i];
		}
		return result;
	}

	DllExport void 
	Console_OpenConsoleWindow()
	{
		OpenConsoleWindow();
	}

	DllExport const char * 
	Console_GetConsoleInput()
	{
		return stringdup(GetConsoleInput().CString());
	}

	//
	// returns: null on no matches
	// otherwise, a pointer that should be released with free() that
	// contains a first element (pointer sized) with the number of elements
	// followed by the number of pointers
	//
	DllExport
	void *urho_node_get_components (Node *node, int code, int recursive, int *count)
	{
		PODVector<Node*> dest;
		node->GetChildrenWithComponent (dest, StringHash(code), recursive);
		if (dest.Size () == 0)
			return NULL;
		*count = dest.Size ();
		void **t = (void **) malloc (sizeof(Node*)*dest.Size());
		for (int i = 0; i < dest.Size (); i++){
			t [i] = dest [i];
		}
		return t;
	}
	
	DllExport
	void* Node_GetChildrenWithTag(Node *node, const char* tag, bool recursive, int *count)
	{
		PODVector<Node*> dest;
		node->GetChildrenWithTag(dest, String(tag), recursive);
		if (dest.Size() == 0)
			return NULL;
		*count = dest.Size();
		void **t = (void **)malloc(sizeof(Node*)*dest.Size());
		for (int i = 0; i < dest.Size(); i++) {
			t[i] = dest[i];
		}
		return t;
	}

    DllExport
    void* Node_GetChildren2(Node *node, bool recursive, int *count)
    {
        PODVector<Node*> dest;
        node->GetChildren(dest, recursive);
        if (dest.Size() == 0)
            return NULL;
        *count = dest.Size();
        void **t = (void **)malloc(sizeof(Node*)*dest.Size());
        for (int i = 0; i < dest.Size(); i++) {
            t[i] = dest[i];
        }
        return t;
    }

	DllExport Interop::Vector3 *
	urho_navigationmesh_findpath(NavigationMesh * navMesh, const class Urho3D::Vector3 & start, const class Urho3D::Vector3 & end, int *count)
	{
		PODVector<Vector3> dest;
		navMesh->FindPath(dest, start, end);
		if (dest.Size() == 0)
			return NULL;
		*count = dest.Size();
		Interop::Vector3 * results = new Interop::Vector3[dest.Size()];
		for (int i = 0; i < dest.Size(); i++) {
			auto vector = *((Interop::Vector3  *) &(dest[i]));
			results[i] = vector;
		}
		return results;
	}

	DllExport int*
	Graphics_GetMultiSampleLevels(Graphics* target, int* count)
	{
		PODVector<int> levels = target->GetMultiSampleLevels();
		*count = levels.Size();
		int* result = new int[levels.Size()];
		for (int i = 0; i < levels.Size(); i++) {
			result[i] = levels[i];
		}
		return result;
	}
	
	DllExport MemoryBuffer* MemoryBuffer_MemoryBuffer(void* data, int size)
	{
		auto buffer = new MemoryBuffer(data, size);
		return buffer;
	}

	DllExport unsigned char* MemoryBuffer_GetData(MemoryBuffer* target, int *count)
	{
		*count = target->GetSize();
		return target->GetData();
	}

	DllExport void MemoryBuffer_Dispose(MemoryBuffer* target)
	{
		delete target;
	}

	DllExport unsigned MemoryBuffer_GetSize(MemoryBuffer* target)
	{
		return target->GetSize();
	}

    DllExport char* MemoryBuffer_GetString(MemoryBuffer* target)
    {
        return stringdup(target->ReadString().CString());
    }


	DllExport unsigned File_GetSize(File* target)
	{
		return target->GetSize();
	}

	DllExport Interop::Vector3 *
	Frustum_GetVertices(Frustum * frustum, int* count)
	{
		int size = NUM_FRUSTUM_VERTICES;
		Interop::Vector3 * results = new Interop::Vector3[size];
		for (int i = 0; i < size; i++) {
			auto vector = *((Interop::Vector3  *) &(frustum->vertices_[i]));
			results[i] = vector;
		}
		*count = size;
		return results;
	}

	DllExport Interop::Plane *
	Frustum_GetPlanes(Frustum * frustum, int* count)
	{
		int size = NUM_FRUSTUM_PLANES;
		Interop::Plane * results = new Interop::Plane[size];
		for (int i = 0; i < size; i++) {
			auto plane = *((Interop::Plane  *) &(frustum->planes_[i]));
			results[i] = plane;
		}
		*count = size;
		return results;
	}

	DllExport Interop::Color
	Material_GetShaderParameterColor(Urho3D::Material *_target, const char* paramName)
	{
		return *((Interop::Color *) &(_target->GetShaderParameter(Urho3D::String(paramName)).GetColor()));
	}

	DllExport unsigned char*
	Image_GetDataBytes(Urho3D::Image *_target, int* len)
	{
		*len = _target->GetWidth() * _target->GetHeight() * _target->GetDepth() * _target->GetComponents();
		return _target->GetData();
	}

	DllExport unsigned char*
	Image_SavePNG2(Urho3D::Image *_target, int* len)
	{
		return _target->SavePNG(len);
	}

	DllExport
	void FreeBuffer(unsigned char* myBuffer)
	{
		delete myBuffer;
	}

	DllExport
	void RenderPathCommand_SetShaderParameter_float(RenderPathCommand* rpc, const char* parameter, float value)
	{
		rpc->SetShaderParameter(String(parameter), value);
	}

	DllExport
	void RenderPathCommand_SetShaderParameter_Matrix4(RenderPathCommand* rpc, const char* parameter, const class Urho3D::Matrix4 & value)
	{
		rpc->SetShaderParameter(String(parameter), value);
	}

	DllExport
	void RenderPathCommand_SetOutput(RenderPathCommand* rpc, int index, const char* name)
	{
		rpc->SetOutput(index, String(name));
	}

	DllExport int
	Input_GetMouseButtonDown (Urho3D::Input *_target, int button)
	{
		return _target->GetMouseButtonDown (MouseButtonFlags(button));
	}


	DllExport int
	Input_GetMouseButtonPress (Urho3D::Input *_target, int button)
	{
		return _target->GetMouseButtonPress (MouseButtonFlags(button));
	}

	DllExport Urho3D::Bone *
	Skeleton_GetBone0 (Urho3D::Skeleton *_target, int boneNameHash)
	{
		return _target->GetBone (Urho3D::StringHash(boneNameHash));
	}


	DllExport unsigned int
	Camera_GetViewOverrideFlags (Urho3D::Camera *_target)
	{
		return _target->GetViewOverrideFlags ();
	}

	DllExport void
	Camera_SetViewOverrideFlags (Urho3D::Camera *_target, unsigned int flags)
	{
		_target->SetViewOverrideFlags (ViewOverrideFlags(flags));
	}

#if defined(__ANDROID__)
	JavaVM* SDL_Android_GetJVM();
	DllExport int java_interop_jvm_list(JavaVM * *vmBuf, int bufLen, int* nVMs)
	{
		__android_log_print(2, "Urho3D", "java_interop_jvm_list vmBuf=%p bufLen=%d nVMs=%p", vmBuf, bufLen, nVMs);
		if (vmBuf != 0)
		{
			vmBuf[0] = SDL_Android_GetJVM();
		}

		if (nVMs != NULL)
		{
			*nVMs = 1;
		}

		return 0;
	}
#endif

    DllExport unsigned int VertexBuffer_GetElementMask(Urho3D::VertexBuffer *_target)
    {
        unsigned int mask = _target->GetElementMask();
        return mask;
    }

	DllExport void String_FreeNativeString(char * str)
    {
        if(str)
        {
            free(str);
        }
    }

    DllExport
    Variant  Variant_CreateInt(int i)
    {
        Variant v = i ;
        return v;
    }

    DllExport
    int Variant_GetInt(Variant & v)
    {
        return v.GetInt();
    }


    DllExport
    Variant  Variant_CreateBool(bool val)
    {
        Variant v = val ;
        return v;
    }

    DllExport
    bool Variant_GetBool(Variant & v)
    {
        return v.GetBool();
    }


    DllExport
    Variant  Variant_CreateFloat(float f)
    {
        Variant v = f;
        return v;
    }

    DllExport
    float Variant_GetFloat(Variant & v)
    {
        return v.GetFloat();
    }


    DllExport
    Variant  Variant_CreateVector2(Interop::Vector2 vec)
    {
        Variant v = Vector2(vec.x,vec.y);
        return v;
    }

    DllExport
    Interop::Vector2 Variant_GetVector2(Variant & v)
    {
        return *((Interop::Vector2  *) &(v.GetVector2 ()));
    }

    DllExport
    Variant  Variant_CreateVector3(Interop::Vector3 vec)
    {
        Variant v = Vector3(vec.x,vec.y,vec.z);
        return v;
    }

    DllExport
    Interop::Vector3 Variant_GetVector3(Variant & v)
    {
        return *((Interop::Vector3  *) &(v.GetVector3 ()));
    }

    DllExport
    Variant  Variant_CreateVector4(Interop::Vector4 vec)
    {
        Variant v =  Vector4(vec.x,vec.y,vec.z,vec.w);
        return v;
    }

    DllExport
    Interop::Vector4  Variant_GetVector4(Variant & v)
    {
     
        return *((Interop::Vector4  *) &(v.GetVector4 ()));
    }




    DllExport
    Variant  Variant_CreateQuaternion(Interop::Quaternion q)
    {
        Variant v = Quaternion(q.w,q.x,q.y,q.z);
        return v;
    }

    DllExport
    Interop::Quaternion Variant_GetQuaternion(Variant & v)
    {
        return *((Interop::Quaternion *) &(v.GetQuaternion()));
    }



    DllExport
    Variant  Variant_CreateColor(Interop::Color c)
    {
        Variant v = Color(c.r,c.g,c.b,c.a);
        return v;
    }

    DllExport
    Interop::Color Variant_GetColor(Variant & v)
    {
       return  *((Interop::Color *) &(v.GetColor()));
    }


    DllExport
    Variant Variant_CreateDouble(double f)
    {
        Variant v = f;
        return v;
    }

    DllExport
    double Variant_GetDouble(Variant & v)
    {
        return v.GetDouble();
    }


    DllExport Variant Variant_CreateString(const char* value)
    {
        Variant v = value;
        return v;
    }


    DllExport char* Variant_GetString(Variant& variant)
    {
        String urhoString = variant.GetString();
        return stringdup(urhoString.CString());
    }




    DllExport Variant Variant_CreateBuffer(void* data, int size)
    {
        Variant v =  VectorBuffer(data,size);
        return v;
        
   
    }

    DllExport unsigned char* Variant_GetBuffer(Variant& v, int *count)
    {
        const PODVector<unsigned char>& pod = v.GetBuffer();
        *count = pod.Size();
        return pod.Buffer();
    }


}
