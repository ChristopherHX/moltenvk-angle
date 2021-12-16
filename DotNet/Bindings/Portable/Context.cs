using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using Urho.Urho2D;
using Urho.Gui;
using Urho.Resources;
using Urho.IO;
using Urho.Navigation;
using Urho.Network;

namespace Urho
{

    public class AttributesVector
    {
        public IntPtr Handle { get; private set; } = IntPtr.Zero;

        public AttributesVector(IntPtr handle)
        {
            Handle = handle;
        }

        public int Count
        {
            get
            {
                return AttributeVector_GetSize(Handle);
            }
        }

        public AttributeInfo this[uint index]
        {
            get
            {
                AttributeInfo attributeInfo = new AttributeInfo();
                attributeInfo.Type = AttributeVector_Attribute_GetType(Handle, index);
                IntPtr nativeString = AttributeVector_Attribute_GetName(Handle, index);
                string result = Marshal.PtrToStringAnsi(nativeString);
                attributeInfo.Name = result;
                return attributeInfo;
            }
        }

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern int AttributeVector_GetSize(IntPtr handle);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern VariantType AttributeVector_Attribute_GetType(IntPtr handle, uint index);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr AttributeVector_Attribute_GetName(IntPtr handle, uint index);
    }

    /// <summary>
    /// Urho3D execution context. Provides access to subsystems, object factories and attributes, and event receivers.
    /// </summary>
    public unsafe partial class Context : RefCounted
    {
        public List<string> Categories
        {
            get
            {
                List<string> categories = new List<string>();
                int size = GetCetegoriesSize();
                for (int i = 0; i < size; i++)
                {
                    categories.Add(GetCategory(i));
                }
                return categories;
            }
        }

        public List<string> GetObjectsByCategory(string category)
        {
            List<string> objects = new List<string>();
            PopulateByCategory(category);
            int size = GetObjectCountInLastPopulatedCetegory();
            for (int i = 0; i < size; i++)
            {

                objects.Add(GetObjectInLastPopulatedCetegory(i));
            }
            ClearLastPopulatedCategory();
            return objects;
        }

        public AttributesVector GetAttributes(StringHash type)
        {
            return new AttributesVector(Context_GetAttributes(handle, type.Code));
        }

        public AttributesVector GetAttributes(string type)
        {
            return new AttributesVector(Context_GetAttributes(handle, new StringHash(type).Code));
        }

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Context_GetAttributes(IntPtr handle, int type);
    }

}