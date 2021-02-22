

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
        Dictionary<string, Dynamic> dynamicMap ;

        public DynamicMap():base()
        {
            dynamicMap = new Dictionary<string, Dynamic>();
        }

        public Dynamic this[String key]
    	{
			get
			{
                Dynamic dyn;
                if (dynamicMap.TryGetValue(key, out dyn))
                {
                    return dyn;
                }
                else
                {
                    Variant value;
                    int hash = StringHash.urho_stringhash_from_string (key);
                    urho_map_get_value(Handle, hash, out value);
                    dyn =  new Dynamic(value);
                    dynamicMap[key] = dyn;
                    return dyn;
                }
			}
			
			set
			{
				int hash = StringHash.urho_stringhash_from_string (key);
				urho_map_set_value_ptr(Handle, hash ,value.Handle);

                dynamicMap[key] = value;
			}
            
    	}

        ~DynamicMap()
		{
			Dispose();
		}

    }

}