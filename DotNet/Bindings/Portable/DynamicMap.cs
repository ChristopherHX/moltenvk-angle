

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
    public class DynamicMap : EventDataContainer
    {
        public DynamicMap():base(){}

        public Dynamic this[String key]
    	{
			get
			{
				Variant value;
				int hash = StringHash.urho_stringhash_from_string (key);
				urho_map_get_value(Handle, hash, out value);

				return new Dynamic(ref value);
			}
			
			set
			{
				int hash = StringHash.urho_stringhash_from_string (key);
				urho_map_set_value(Handle, hash ,ref value.variant);
			}
            
    	}

    }

}