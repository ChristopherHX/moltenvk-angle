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
	/// Urho3D execution context. Provides access to subsystems, object factories and attributes, and event receivers.
	/// </summary>
	public unsafe partial class Context : RefCounted
	{
        public List<string> Categories
        {
            get{
                 List<string> categories = new List<string>();
                 int size = GetCetegoriesSize ();
                 for(int i = 0 ; i < size;i++)
                 {
                     categories.Add(GetCategory (i));
                 }
                 return categories;
            }
        }

        public List<string>  GetObjectsByCategory( string category)
        {
            List<string> objects = new List<string>();
            PopulateByCategory (category);
            int size = GetObjectCountInLastPopulatedCetegory();
            for(int i = 0 ; i < size;i++)
            {
                
                     objects.Add(GetObjectInLastPopulatedCetegory (i));
            }
            ClearLastPopulatedCategory ();
            return objects;
        }
    }

}