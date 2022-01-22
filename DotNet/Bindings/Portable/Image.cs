using System;
using System.Runtime.InteropServices;

namespace Urho.Resources
{
    partial class Image
    {

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Image_GetDataBytes(IntPtr handle, out int len);

        public byte[] DataBytes
        {
            get
            {
                int len;
                IntPtr ptr = Image_GetDataBytes(Handle, out len);
                byte[] data = new byte[len];
                Marshal.Copy(ptr, data, 0, data.Length);
                return data;
            }
        }


        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr Image_SavePNG2(IntPtr handle, out int len);

        public byte[] SavePNG()
        {
            int len;
            var ptr = Image_SavePNG2(Handle, out len);
            byte[] data = new byte[len];
            Marshal.Copy(ptr, data, 0, data.Length);
            UrhoObject.FreeBuffer(ptr);
            return data;
        }

        [DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
        internal static extern void Image_SetData2(IntPtr handle, byte* pixelData, int dataSize);

        /// <summary>
        /// Set new image data unsafe.
        /// </summary>
        public void SetData(byte* pixelData, int dataSize)
        {
            Runtime.ValidateRefCounted(this);
            Image_SetData2(handle, pixelData, dataSize);
        }

        /// <summary>
        /// Set new image data safe.
        /// </summary>
        public void SetData(byte[] pixelData)
        {
            Runtime.ValidateRefCounted(this);
            fixed (byte* ptr = pixelData)
            {
                Image_SetData2(handle, ptr, pixelData.Length);
            }
        }

    }
}
