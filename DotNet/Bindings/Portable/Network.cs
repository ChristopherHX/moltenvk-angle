using System;
using System.Runtime.InteropServices;

namespace Urho.Network {
	public partial class Network {
		[DllImport (Consts.NativeImport, CallingConvention=CallingConvention.Cdecl)]
		extern static int Network_Connect (IntPtr handle, string address, short port, IntPtr scene);

		public bool Connect (string address, short port, Scene scene)
		{
			Runtime.ValidateRefCounted(this);
			if (address == null)
				throw new ArgumentNullException ("address");
			return Network_Connect (handle, address, port, scene?.Handle ?? IntPtr.Zero) != 0;
		}
	}
	
}
