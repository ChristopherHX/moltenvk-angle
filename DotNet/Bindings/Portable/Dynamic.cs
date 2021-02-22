using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Threading;
using System.Runtime.InteropServices;
using Urho.Physics;
using Urho.Gui;
using Urho.Urho2D;
using Urho.Resources;

namespace Urho
{

    public unsafe class Dynamic
    {
        public IntPtr Handle { get; private set; }

        public Variant variant;


        public Dynamic(byte[] data)
        {
            fixed (byte* bptr = data)
            {
                Handle = Dynamic_CreateBuffer(bptr, data.Length);
            }

            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(Variant v)
        {
            Handle = Dynamic_CreateVariant(ref v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }


        public Dynamic(bool v)
        {
            Handle = Dynamic_CreateBool(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }


        public Dynamic(int v)
        {
            Handle = Dynamic_CreateInt(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(float v)
        {
            Handle = Dynamic_CreateFloat(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(double v)
        {
            Handle = Dynamic_CreateDouble(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }


        public Dynamic(Vector2 v)
        {
            Handle = Dynamic_CreateVector2(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(Vector3 v)
        {
            Handle = Dynamic_CreateVector3(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(Vector4 v)
        {
            Handle = Dynamic_CreateVector4(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(Quaternion v)
        {
            Handle = Dynamic_CreateQuaternion(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }
        public Dynamic(Color v)
        {
            Handle = Dynamic_CreateColor(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(string v)
        {
            Handle = Dynamic_CreateString(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }


        public static implicit operator Dynamic(bool b)
        {
            return new Dynamic(b);
        }

        public static implicit operator bool(Dynamic v)
        {
            return v.variant.Value._bool;
        }

        public static implicit operator Dynamic(int i)
        {
            return new Dynamic(i);
        }

        public static implicit operator int(Dynamic v)
        {
            return v.variant.Value._int;
        }

        public static implicit operator Dynamic(float val)
        {
            return new Dynamic(val);
        }

        public static implicit operator float(Dynamic v)
        {
            return v.variant.Value._float;
        }

        public static implicit operator Dynamic(double val)
        {
            return new Dynamic(val);
        }

        public static implicit operator double(Dynamic v)
        {
            return v.variant.Value._double;
        }

        public static implicit operator Dynamic(Vector2 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Vector2(Dynamic v)
        {
            return v.variant.Value.vector2;
        }

        public static implicit operator Dynamic(Vector3 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Vector3(Dynamic v)
        {
            return v.variant.Value.vector3;
        }

        public static implicit operator Dynamic(Vector4 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Vector4(Dynamic v)
        {
            return v.variant.Value.vector4;
        }

        public static implicit operator Dynamic(Quaternion val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Quaternion(Dynamic v)
        {
            return v.variant.Value.quaternion;
        }

        public static implicit operator Dynamic(Color val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Color(Dynamic v)
        {
            return v.variant.Value.color;
        }

        public static implicit operator Dynamic(string val)
        {
            return new Dynamic(val);
        }

        public static implicit operator string(Dynamic v)
        {
            IntPtr nativeCString = Dynamic_GetString(v.Handle);
            string result = Marshal.PtrToStringAnsi(nativeCString);
            NativeString.Free(nativeCString);
            return result;
        }


        ~Dynamic()
        {
            Dynamic_Dispose(Handle);
        }

        public static implicit operator Dynamic(byte[] data)
        {
            return new Dynamic(data);
        }


        public static implicit operator byte[](Dynamic v)
        {
            int size;
            var bytesPtr = Dynamic_GetBuffer(v.Handle, out size);
            if (bytesPtr == IntPtr.Zero)
                return new byte[0];
            byte[] result = new byte[size];
            Marshal.Copy(bytesPtr, result, 0, size);
            return result;
        }



        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateBuffer(byte* data, int size);


        /////////////////////////////////////////////////////////////////////

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateVariant(ref Variant v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateBool(bool v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateInt(int i);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateFloat(float f);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateVector2(Vector2 v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateVector3(Vector3 v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateVector4(Vector4 v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateQuaternion(Quaternion v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateColor(Color v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateDouble(double v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateString(string s);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_GetString(IntPtr h);

        ////////////////////////////////////////////////////////////////////


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_Dispose(IntPtr handle);


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_GetBuffer(IntPtr v, out int count);
    }

}