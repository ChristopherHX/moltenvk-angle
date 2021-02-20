// 
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
	/// <summary>
	/// Base class for objects with type identification, subsystem access and event sending/receiving capability.
	/// </summary>
	public unsafe partial class UrhoObject
	{
        public string ToString(bool v)
		{
			return v.ToString();
		}

		public string ToString(byte v)
		{
			return v.ToString();
		}

		public string ToString(sbyte v)
		{
			return v.ToString();
		}

		public string ToString(short v)
		{
			return v.ToString();
		}

		public string ToString(ushort v)
		{
			return v.ToString();
		}

		public string ToString(int v)
		{
			return v.ToString();
		}

		public string ToString(uint v)
		{
			return v.ToString();
		}

		public string ToString(long v)
		{
			return v.ToString();
		}

		public string ToString(ulong v)
		{
			return v.ToString();
		}

		public string ToString(decimal v)
		{
			return v.ToString();
		}


		public string ToString(float f)
		{
			return MathHelper.ToString(f);
		}

		public string ToString(double d)
		{
			return MathHelper.ToString(d);
		}
    }

}