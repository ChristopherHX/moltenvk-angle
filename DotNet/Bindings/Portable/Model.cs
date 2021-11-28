using System;
using System.Runtime.InteropServices;

namespace Urho {
	public partial class Model {
		[DllImport (Consts.NativeImport, CallingConvention=CallingConvention.Cdecl)]
		extern static IntPtr Model_Clone_EmptyName (IntPtr handle);
		
		public Model Clone ()
		{
			Runtime.ValidateRefCounted(this);
			return Runtime.LookupObject<Model> (Model_Clone_EmptyName (handle));
		}
	}
}
