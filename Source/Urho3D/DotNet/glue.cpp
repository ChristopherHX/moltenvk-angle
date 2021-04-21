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

#ifdef _WINDOWS
typedef unsigned int uint;
#endif

//
// This is just an implemention of EventHandler that can be used with function
// pointers, so we can register delegates from C#
//


static String StringDup;
const char *stringdup(const char *s)
{
    StringDup = String(s);
    return StringDup.CString();
}


extern "C" {

    // reciever and sender are the same
	DllExport void *urho_subscribe_event(void *_receiver, HandlerFunctionPtr callback, void *data, int eventNameHash)
	{
		StringHash h(eventNameHash);
		Urho3D::Object *receiver = (Urho3D::Object *) _receiver;
		NotificationProxy *proxy = new NotificationProxy(receiver, callback, data, h);
		receiver->SubscribeToEvent(receiver, h, proxy);
		return proxy;
	}
    
    // sender is unknown here , can be anyone
    DllExport void *urho_subscribe_global_event(void *_receiver, HandlerFunctionPtr callback, void *data, int eventNameHash)
    {
        StringHash h(eventNameHash);
        Urho3D::Object *receiver = (Urho3D::Object *) _receiver;
        WeakPtr<Urho3D::Object> weak_receiver(receiver);
        NotificationProxy *proxy = new NotificationProxy(weak_receiver, callback, data, h);
        receiver->SubscribeToEvent(h, proxy);
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
            variantMap->Clear();
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
	unsigned int urho_stringhash_from_string (const char *p)
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

    DllExport void
    Node_RemoveComponent22 (Urho3D::Node *_target, Urho3D::Component * component)
    {
        _target->RemoveComponent (component);
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

    DllExport const char * MemoryBuffer_GetString(MemoryBuffer* target)
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
        /*
        if(str)
        {
            free(str);
        }
         */
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


    DllExport const char * Variant_GetString(Variant& variant)
    {
        String urhoString = variant.GetString();
        return stringdup(urhoString.CString());
    }

    DllExport void Variant_CreateBuffer(void* data, int size , Variant & v)
    {
        v =  VectorBuffer(data,size);
    }

    DllExport unsigned char* Variant_GetBuffer(Variant& v, int *count)
    {
        const PODVector<unsigned char>& pod = v.GetBuffer();
        *count = pod.Size();
        return pod.Buffer();
    }




    DllExport void urho_map_get_value(VariantMap& nativeInstance, int key, Variant& value)
    {
        value = nativeInstance[StringHash(key)];
    }

    DllExport void urho_map_set_value(VariantMap& nativeInstance, int key, Variant& value)
    {
        nativeInstance[StringHash(key)] = value;
    }

    DllExport void urho_map_set_value_ptr(VariantMap& nativeInstance, int key, Variant* value)
    {
        nativeInstance[StringHash(key)] = *value;
    }


    static   char conversionNumbersBuffer[CONVERSION_BUFFER_LENGTH];

    DllExport  char* float_convert_to_string(float value)
    {
        sprintf(conversionNumbersBuffer, "%g", value);
        return conversionNumbersBuffer;
    }

    DllExport  char* double_convert_to_string(double value)
    {
      
        sprintf(conversionNumbersBuffer, "%g", value);
        return conversionNumbersBuffer;
    }


/*DYNAMIC*/

//
    DllExport Variant* Dynamic_CreateVariant(Variant& value)
    {
        Variant * v = new Variant(value);
        return v;
    }

    DllExport Variant* Dynamic_CreateBool(bool val)
    {
        Variant * v = new Variant(val);
        return v;
    }

    DllExport Variant* Dynamic_CreateInt(int val)
    {
        Variant * v = new Variant(val);
        return v;
    }

    DllExport Variant* Dynamic_CreateUInt(unsigned int val)
    {
        Variant * v = new Variant(val);
        return v;
    }

    DllExport Variant* Dynamic_CreateInt64(long long val)
    {
        Variant * v = new Variant(val);
        return v;
    }

    DllExport Variant* Dynamic_CreateUInt64(unsigned long long val)
    {
        Variant * v = new Variant(val);
        return v;
    }



    DllExport Variant* Dynamic_CreateFloat(float val)
    {
        Variant * v = new Variant(val);
        return v;
    }

    DllExport Variant* Dynamic_CreateDouble(double val)
    {
        Variant * v = new Variant(val);
        return v;
    }

    
    DllExport Variant* Dynamic_CreateVector2(Interop::Vector2 val)
    {
        Variant * v = new Variant(Vector2(val.x,val.y));
        return v;
    }

    DllExport Variant* Dynamic_CreateVector3(Interop::Vector3 val)
    {
        Variant * v = new Variant(Vector3(val.x,val.y,val.z));
        return v;
    }

    DllExport Variant* Dynamic_CreateVector4(Interop::Vector4 val)
    {
        Variant * v = new Variant(Vector4(val.x,val.y,val.z,val.w));
        return v;
    }

    DllExport Variant*  Dynamic_CreateQuaternion(Interop::Quaternion q)
    {
        Variant * v = new Variant(Quaternion(q.w,q.x,q.y,q.z));
        return v;
    }


    DllExport
    Variant *  Dynamic_CreateColor(Interop::Color c)
    {
        Variant * v = new Variant(Color(c.r,c.g,c.b,c.a));
        return v;
    }

    DllExport Variant*  Dynamic_CreateIntVector2(Interop::IntVector2 val)
    {
        Variant * v = new Variant(IntVector2(val.x,val.y ));
        return v;
    }

    DllExport Variant*  Dynamic_CreateIntVector3(Interop::IntVector3 val)
    {
        Variant * v = new Variant(IntVector3(val.x,val.y,val.z));
        return v;
    }

    DllExport Variant*  Dynamic_CreateIntRect(Interop::IntRect val)
    {
        //int left, int top, int right, int bottom
        Variant * v = new Variant(IntRect(val.left,val.top,val.right,val.bottom));
        return v;
    }

    DllExport Variant*  Dynamic_CreateRect(Interop::Rect val)
    {
        Variant * v = new Variant(Rect(val.min.x,val.min.y,val.max.x,val.max.y));
        return v;
    }

    DllExport Variant*  Dynamic_CreateMatrix3(Interop::Matrix3 val)
    {
        Variant * v = new Variant(Matrix3(val.m00,val.m01,val.m02,val.m10,val.m11,val.m12,val.m20,val.m21,val.m22));
        
        Matrix3 mat = v->GetMatrix3();
        
      //  printf("Dynamic_CreateMatrix3 : %g:%g:%g %g:%g:%g %g:%g:%g \n",mat.m00_,mat.m01_,mat.m02_, mat.m10_,mat.m11_,mat.m12_ , mat.m20_,mat.m21_,mat.m22_);
        
        return v;
    }

    DllExport
    Interop::Matrix3 Dynamic_GetMatrix3 ( Variant * v)
    {
        return *((Interop::Matrix3  *) &(v->GetMatrix3()));
    }

    DllExport Variant*  Dynamic_CreateMatrix4(Interop::Matrix4 val)
    {
        Variant * v = new Variant(Matrix4(val.m00,val.m01,val.m02,val.m03,val.m10,val.m11,val.m12,val.m13,val.m20,val.m21,val.m22,val.m23,val.m30,val.m31,val.m32,val.m33));
        return v;
    }

    DllExport
    Interop::Matrix4 Dynamic_GetMatrix4 ( Variant * v)
    {
        return *((Interop::Matrix4  *) &(v->GetMatrix4()));
    }


    DllExport Variant*  Dynamic_CreateMatrix3x4(Interop::Matrix3x4 val)
    {
        Variant * v = new Variant(Matrix3x4(val.m00,val.m01,val.m02,val.m03,val.m10,val.m11,val.m12,val.m13,val.m20,val.m21,val.m22,val.m23));
        return v;
    }

    DllExport
    Interop::Matrix3x4 Dynamic_GetMatrix3x4 ( Variant * v)
    {
        return *((Interop::Matrix3x4  *) &(v->GetMatrix3x4()));
    }


    DllExport Variant * Dynamic_CreateString(const char* value)
    {
        Variant *v = new Variant(value);
        return v;
    }


    DllExport const char * Dynamic_GetString(Variant* variant)
    {
        String urhoString = variant->GetString();
        return stringdup(urhoString.CString());
    }


    DllExport Variant* Dynamic_CreateBuffer(void* data, int size)
    {
        Variant * v = new Variant(VectorBuffer(data,size));
        return v;
    }

    DllExport unsigned char* Dynamic_GetBuffer(Variant* v, int *count)
    {
        const PODVector<unsigned char>& pod = v->GetBuffer();
        *count = pod.Size();
        return pod.Buffer();
    }


    DllExport void Dynamic_Dispose(Variant* target)
    {
        delete target;
    }

    DllExport void
    Connection_SendRemoteEvent(Connection *conn,int eventType, bool inOrder, VariantMap& eventData)
    {
        conn->SendRemoteEvent(StringHash(eventType),inOrder,eventData);
    }

    DllExport void
    Connection_SendRemoteEvent2(Connection *conn,Node *node,int eventType, bool inOrder, VariantMap& eventData)
    {
        conn->SendRemoteEvent(node,StringHash(eventType),inOrder,eventData);
    }


DllExport void *
Network_GetClientConnections(Network *network, int *count)
{
    const Vector<SharedPtr<Connection> >& dest = network->GetClientConnections();
    *count = 0;
    
    if (dest.Size () == 0)
        return NULL;
    
    *count = dest.Size ();
    
    void **t = (void **) malloc (sizeof(Connection*)*dest.Size());
    for (int i = 0; i < dest.Size (); i++){
        t [i] = dest [i];
    }
    return t;
}

DllExport void VoidPtr_Free(void * ptr)
{
    free(ptr);
}


DllExport void
KinematicCharacterController_GetTransform (Urho3D::KinematicCharacterController *_target,Interop::Vector3 * position, Interop::Quaternion * rotation)
{
    Vector3 _pos;
    Quaternion _rot;
    
    _target->GetTransform(_pos, _rot);
    
    *position = *((Interop::Vector3  *) &(_pos));
    *rotation = *((Interop::Quaternion  *) &(_rot));
}


DllExport Variant
Node_GetVar (Urho3D::Node *_target, int key)
{
    return _target->GetVar (Urho3D::StringHash(key));
}

DllExport Interop::Matrix3x4  Matrix3x4_Create( Vector3& translation,  Quaternion& rotation,  Vector3& scale)
{
    Matrix3x4 matrix3x4(translation,rotation,scale);
    return *((Interop::Matrix3x4  *) &(matrix3x4));
}

DllExport Interop::Matrix3x4  Matrix3x4_Multiply( Matrix3x4 & left , Matrix3x4 & right)
{
    Matrix3x4 matrix3x4 = left * right ;
    return *((Interop::Matrix3x4  *) &(matrix3x4));
}

DllExport Interop::Vector3  Matrix3x4_Translation( Matrix3x4 & matrix3x4)
{
    Vector3 translation = matrix3x4.Translation();
    return *((Interop::Vector3  *) &(translation));
}

DllExport Interop::Quaternion  Matrix3x4_Rotation( Matrix3x4 & matrix3x4)
{
    Quaternion rotation = matrix3x4.Rotation();
    return *((Interop::Quaternion  *) &(rotation));
}



DllExport void
Profiler_BeginBlock (Urho3D::Profiler *_target,const char* name)
{
    _target->BeginBlock(name);
}

DllExport 
uint JoystickState_GetNumButtons (Urho3D::JoystickState *_target)
{
	return _target->GetNumButtons();
}

DllExport
bool JoystickState_GetButtonDown (Urho3D::JoystickState *_target , int position)
{
    return _target->GetButtonDown(position);
}

DllExport
uint JoystickState_GetNumHats (Urho3D::JoystickState *_target)
{
    return _target->GetNumHats();
}

DllExport
int JoystickState_GetHatPosition (Urho3D::JoystickState *_target ,uint index)
{
    return _target->GetHatPosition(index);
}

DllExport
uint JoystickState_GetNumAxes (Urho3D::JoystickState *_target)
{
    return _target->GetNumAxes();
}

DllExport
float JoystickState_GetAxisPosition (Urho3D::JoystickState *_target,uint index)
{
    return _target->GetAxisPosition(index);
}


DllExport
void * PhysicsWorld2D_GetRigidBodies(Urho3D::PhysicsWorld2D *_target,const class Urho3D::Rect & aabb, unsigned collisionMask, int *count)
{
    PODVector<RigidBody2D*> results;
    _target->GetRigidBodies(results, aabb, collisionMask);
    if (results.Size() == 0)
        return NULL;
    *count = results.Size();
    void **t = (void **)malloc(sizeof(RigidBody2D*)*results.Size());
    for (int i = 0; i < results.Size(); i++) {
        t[i] = results[i];
    }
    return t;
}

/*
 
 #if UWP
 #define stringdup _strdup
 #else
 #define stringdup strdup
 #endif
 */


}


