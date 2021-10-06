using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace Urho.IO
{
	partial class File
	{
		[DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
		static extern uint File_GetSize(IntPtr handle);

		public uint Size => File_GetSize(Handle);

		public uint Read(byte[] buffer, uint size = 0 )
		{
			unsafe
			{
				fixed (byte* b = buffer)
				{
					if(size == 0 || size > buffer.Length)
					{
						size = (uint)buffer.Length;
					}
					return Read((IntPtr)b, size);
				}
			}
		}

		public uint Write(byte[] buffer, uint size = 0)
		{
			unsafe
			{
				fixed (byte* b = buffer)
				{
                    if (size == 0 || size > buffer.Length)
                    {
                        size = (uint)buffer.Length;
                    }
					return Write((void*)b, (uint)size);
				}
			}
		}
	}
}
