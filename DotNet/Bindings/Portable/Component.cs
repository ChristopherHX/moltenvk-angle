//
// Component C# sugar
//
// Authors:
//   Miguel de Icaza (miguel@xamarin.com)
//
// Copyrigh 2015 Xamarin INc
//

using System;
using System.Linq;
using System.Reflection;
using Urho.Resources;
using Urho.IO;

namespace Urho
{
    public partial class Component : Animatable
    {
        bool subscribedToSceneUpdate = false;

        bool isDisposed = false;

        public bool ReceiveSceneUpdates
        {
            get
            {
                return subscribedToSceneUpdate;
            }
            set
            {
                if (value == true)
                {
                    if (!subscribedToSceneUpdate)
                    {
                        subscribedToSceneUpdate = true;
                        Application.Update += HandleUpdate;
                    }
                }
                else
                {
                    if (subscribedToSceneUpdate)
                    {
                        subscribedToSceneUpdate = false;
                        Application.Update -= HandleUpdate;
                    }
                }
            }
        }

        public T GetComponent<T>() where T : Component
        {
            Runtime.ValidateRefCounted(this);
            return (T)Node.Components.FirstOrDefault(c => c is T);
        }

        protected override void Dispose(bool disposing)
        {
            isDisposed = true;
            ReceiveSceneUpdates = false;
            base.Dispose(disposing);
        }

        public Application Application => Application.Current;

        public virtual void OnSerialize(IComponentSerializer serializer) { }

        public virtual void OnDeserialize(IComponentDeserializer deserializer) { }

        public void SerializeFields(IComponentSerializer serializer)
        {
            Type type = GetType();

            BindingFlags bindingFlags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static;

            foreach (FieldInfo mInfo in type.GetFields(bindingFlags))
            {
                FieldAttributes fieldAttributes = mInfo.Attributes;
                bool isSerializable = false;

                foreach (Attribute attr in
                          Attribute.GetCustomAttributes(mInfo))
                {
                    if (attr.GetType() == typeof(SerializeFieldAttribute))
                    {
                        isSerializable = true;
                    }
                }

                // save only public or serializable fields
                if (!(mInfo.IsPublic || isSerializable)) continue;
                // don't save constants
                if ((fieldAttributes & FieldAttributes.Literal) == FieldAttributes.Literal) continue;


                Type field_type = mInfo.FieldType;
                string key = mInfo.Name;
                object value = mInfo.GetValue(this);

                serializer.SetObjectValueToXmlElement(key, value);
            }
        }

        public void DeserializeFields(IComponentDeserializer deserializer)
        {
            Type CompnentType = this.GetType();

            BindingFlags bindingFlags = BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.Static;

            foreach (FieldInfo mInfo in CompnentType.GetFields(bindingFlags))
            {
                FieldAttributes fieldAttributes = mInfo.Attributes;
                bool isSerializable = false;

                foreach (Attribute attr in Attribute.GetCustomAttributes(mInfo))
                {
                    if (attr.GetType() == typeof(SerializeFieldAttribute))
                    {
                        isSerializable = true;
                    }
                }

                // load only public or serializable fields
                if (!(mInfo.IsPublic || isSerializable)) continue;
                // don't load constants
                if ((fieldAttributes & FieldAttributes.Literal) == FieldAttributes.Literal) continue;

                Type type = mInfo.FieldType;
                string key = mInfo.Name;

                object value = deserializer.GetObjectValueFromXmlElement(type, key);
                if (value != null)
                {
                    mInfo.SetValue(this, value);
                }

            }
        }

        public virtual void OnAttachedToNode(Node node) { }

        public virtual void OnSceneSet(Scene scene) { }

        public virtual void OnNodeSetEnabled() { }

        public virtual void OnCloned(Scene scene, Component originalComponent) { }

        protected override void OnDeleted()
        {
            isDisposed = true;
            if (subscribedToSceneUpdate)
            {
                ReceiveSceneUpdates = false;
            }
            base.OnDeleted();
        }

        internal void AttachedToNode(Node node)
        {
            if (node != null && !subscribedToSceneUpdate)
            {
                ReceiveSceneUpdates = true;
            }
            else if (node == null && subscribedToSceneUpdate)
            {
                ReceiveSceneUpdates = false;
            }
            OnAttachedToNode(node);
        }

        /// <summary>
        /// Make sure you set SubscribeToSceneUpdate property to true in order to receive Update events
        /// </summary>
        protected virtual void OnUpdate(float timeStep) { }

        internal static bool IsDefinedInManagedCode<T>() => typeof(T).GetRuntimeProperty("TypeStatic") == null;

        void HandleUpdate(UpdateEventArgs args)
        {
            if (isDisposed) return;

            if (Enabled == false) return;
#if __EDITOR__
            if (Application.HasCurrent)
            {
                if (Application.Current.EditorMode == true && Application.Current.EditorUpdate == false)
                {
                    return;
                }
            }
#endif
            try
            {
                OnUpdate(args.TimeStep);
            }
            catch (Exception ex)
            {
                Urho.Application.ThrowUnhandledException(
                     new Exception(ex.ToString() + " . You can omit this exception by subscribing to Urho.Application.UnhandledException event and set Handled property to True.\nApplicationOptions: " + Application.CurrentOptions));
            }
        }
    }
}
