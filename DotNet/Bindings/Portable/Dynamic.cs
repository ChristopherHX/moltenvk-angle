//
// The MIT License (MIT)
//
// Copyright (c) 2021 Eli Aloni (A.K.A elix22)
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
        
        public Dynamic(uint v)
        {
            Handle = Dynamic_CreateUInt(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }


        public Dynamic(System.Int64 v)
        {
            Handle = Dynamic_CreateInt64(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(System.UInt64 v)
        {
            Handle = Dynamic_CreateUInt64(v);
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

        public Dynamic(IntVector2 v)
        {
            Handle = Dynamic_CreateIntVector2(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }


        public Dynamic(Vector3 v)
        {
            Handle = Dynamic_CreateVector3(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(IntVector3 v)
        {
            Handle = Dynamic_CreateIntVector3(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }


        public Dynamic(Rect v)
        {
            Handle = Dynamic_CreateRect(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(IntRect v)
        {
            Handle = Dynamic_CreateIntRect(v);
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

        public Dynamic(Matrix3 v)
        {
            Handle = Dynamic_CreateMatrix3(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(Matrix4 v)
        {
            Handle = Dynamic_CreateMatrix4(v);
            variant = (Variant)Marshal.PtrToStructure(Handle, typeof(Variant));
        }

        public Dynamic(Matrix3x4 v)
        {
            Handle = Dynamic_CreateMatrix3x4(v);
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

        public static implicit operator Dynamic(uint i)
        {
            return new Dynamic(i);
        }

        public static implicit operator uint(Dynamic v)
        {
            return v.variant.Value._uint;
        }


        public static implicit operator Dynamic(System.Int64 i)
        {
            return new Dynamic(i);
        }

        public static implicit operator System.Int64(Dynamic v)
        {
            return v.variant.Value.int64;
        }


        public static implicit operator Dynamic(System.UInt64 i)
        {
            return new Dynamic(i);
        }

        public static implicit operator System.UInt64(Dynamic v)
        {
            return v.variant.Value.uint64;
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

        public static implicit operator Dynamic(IntVector2 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator IntVector2(Dynamic v)
        {
            return v.variant.Value.intVector2;
        }

        public static implicit operator Dynamic(Vector3 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Vector3(Dynamic v)
        {
            return v.variant.Value.vector3;
        }

        public static implicit operator Dynamic(IntVector3 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator IntVector3(Dynamic v)
        {
            return v.variant.Value.intVector3;
        }

        public static implicit operator Dynamic(IntRect val)
        {
            return new Dynamic(val);
        }

        public static implicit operator IntRect(Dynamic v)
        {
            return v.variant.Value.intRect;
        }


        public static implicit operator Dynamic(Rect val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Rect(Dynamic v)
        {
            return v.variant.Value.rect;
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
            return Marshal.PtrToStringAnsi(Dynamic_GetString(v.Handle));
        }

        public static implicit operator Dynamic(Matrix3 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Matrix3(Dynamic v)
        {
            return Dynamic_GetMatrix3(v.Handle);
        }

        public static implicit operator Dynamic(Matrix4 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Matrix4(Dynamic v)
        {
            return Dynamic_GetMatrix4(v.Handle);
        }


        public static implicit operator Dynamic(Matrix3x4 val)
        {
            return new Dynamic(val);
        }

        public static implicit operator Matrix3x4(Dynamic v)
        {
            return Dynamic_GetMatrix3x4(v.Handle);
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
        static extern IntPtr Dynamic_CreateUInt(uint i);

        ////
        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateInt64(System.Int64 i);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateUInt64(System.UInt64 i);

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

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateIntVector2(IntVector2 v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateIntVector3(IntVector3 v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateIntRect(IntRect v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateRect(Rect v);


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateMatrix3(Matrix3 v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern Matrix3 Dynamic_GetMatrix3(IntPtr v);


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateMatrix4(Matrix4 v);


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern Matrix4 Dynamic_GetMatrix4(IntPtr v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_CreateMatrix3x4(Matrix3x4 v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern Matrix3x4 Dynamic_GetMatrix3x4(IntPtr v);

        ////////////////////////////////////////////////////////////////////


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_Dispose(IntPtr handle);


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Dynamic_GetBuffer(IntPtr v, out int count);






    }

}