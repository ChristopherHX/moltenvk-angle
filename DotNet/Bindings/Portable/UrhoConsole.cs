using System;
using System.Runtime.InteropServices;

namespace Urho
{
	partial class UrhoConsole
	{
		[DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
		internal static extern void Console_OpenConsoleWindow();
		
		[DllImport(Consts.NativeImport, CallingConvention = CallingConvention.Cdecl)]
		internal static extern IntPtr Console_GetConsoleInput();

		public static void OpenConsoleWindow()
		{
			Runtime.Validate(typeof(UrhoConsole));
			Console_OpenConsoleWindow();
		}

		public static string GetConsoleInput()
		{
			Runtime.Validate(typeof(UrhoConsole));
			IntPtr native_string = Console_GetConsoleInput();
			string managed_string = Marshal.PtrToStringAnsi (native_string);
			NativeString.Free(native_string);
			return managed_string;

			// Memory leak return Marshal.PtrToStringAnsi(Console_GetConsoleInput());
		}
	}
}
