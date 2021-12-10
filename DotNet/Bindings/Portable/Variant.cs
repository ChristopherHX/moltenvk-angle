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


    [StructLayout(LayoutKind.Sequential)]
    public struct VariantStorage
    {
        public UIntPtr Storage0;
        public UIntPtr Storage1;
        public UIntPtr Storage2;
        public UIntPtr Storage3;
    }

    [StructLayout(LayoutKind.Explicit)]
    public unsafe struct VariantValue
    {
        [FieldOffset(0)] public bool _bool;
        [FieldOffset(0)] public int _int;
        [FieldOffset(0)] public uint _uint;
        [FieldOffset(0)] public System.Int64 int64;
        [FieldOffset(0)] public System.UInt64 uint64;
        [FieldOffset(0)] public float _float;
        [FieldOffset(0)] public double _double;
        [FieldOffset(0)] public Vector2 vector2;
        [FieldOffset(0)] public Vector3 vector3;
        [FieldOffset(0)] public Vector4 vector4;
        [FieldOffset(0)] public Color color;
        [FieldOffset(0)] public Rect rect;
        [FieldOffset(0)] public IntVector2 intVector2;
        [FieldOffset(0)] public IntVector3 intVector3;
        [FieldOffset(0)] public IntRect intRect;
        [FieldOffset(0)] public Matrix3 matrix3;
        [FieldOffset(0)] public Matrix4 matrix4;
        [FieldOffset(0)] public Matrix3x4 matrix3x4;
        [FieldOffset(0)] public Quaternion quaternion;
        [FieldOffset(0)] public IntPtr intPtr;
        [FieldOffset(0)] public VariantStorage variantStorage;
    }



    [StructLayout(LayoutKind.Explicit)]
    public unsafe struct Variant
    {
        [FieldOffset(0)]
        public VariantType Type;
        [FieldOffset(8)]
        public VariantValue Value;


        public static implicit operator int(Variant v)
        {
            return v.Value._int;
        }


        public static implicit operator bool(Variant v)
        {
            return v.Value._bool;
        }


        public static implicit operator float(Variant v)
        {
            return v.Value._float;
        }

        public static implicit operator Vector2(Variant v)
        {
            return v.Value.vector2;
        }

        public static implicit operator Vector3(Variant v)
        {
            return v.Value.vector3;
        }

        public static implicit operator Vector4(Variant v)
        {
            return v.Value.vector4;
        }

        public static implicit operator Quaternion(Variant v)
        {
            return v.Value.quaternion;
        }


        public static implicit operator Color(Variant v)
        {
            return v.Value.color;
        }

        public static implicit operator double(Variant v)
        {
            return v.Value._double;
        }

        public static implicit operator IntPtr(Variant v)
        {
            return v.Value.intPtr;
        }

        public static implicit operator string(Variant v)
        {
            IntPtr nativeCString = Variant_GetString(ref v);
            string result = Marshal.PtrToStringAnsi(nativeCString);
            return result;
        }


        public static implicit operator byte[](Variant v)
        {
            int size;
            var bytesPtr = Variant_GetBuffer(ref v, out size);
            if (bytesPtr == IntPtr.Zero)
                return new byte[0];
            byte[] result = new byte[size];
            Marshal.Copy(bytesPtr, result, 0, size);
            return result;
        }


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Variant_GetString(ref Variant v);

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Variant_GetBuffer(ref Variant v, out int count);
    }

}