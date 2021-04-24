using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using Urho.Urho2D;
using Urho.Gui;
using Urho.Resources;
using Urho.IO;
using Urho.Navigation;
using Urho.Network;
using System.Reflection;

namespace Urho
{
    /// <summary>
    /// Base class for objects with automatic serialization through attributes.
    /// </summary>
    public unsafe partial class Serializable : UrhoObject
    {
        private object GetObjectValueFromXmlElement(XmlElement xmlElement, Type type)
        {
            string key = "value";

            if (type == typeof(string))
                return xmlElement.GetAttribute(key);
            else if (type == typeof(Vector2))
                return (object)xmlElement.GetVector2(key);
            else if (type == typeof(BoundingBox))
                return (object)xmlElement.GetBoundingBox(key);
            else if (type == typeof(Vector3))
                return (object)xmlElement.GetVector3(key);
            else if (type == typeof(Vector4))
                return (object)xmlElement.GetVector4(key);
            else if (type == typeof(IntRect))
                return (object)xmlElement.GetIntRect(key);
            else if (type == typeof(Quaternion))
                return (object)xmlElement.GetQuaternion(key);
            else if (type == typeof(Color))
                return (object)xmlElement.GetColor(key);
            else if (type == typeof(float))
                return (object)xmlElement.GetFloat(key);
            else if (type == typeof(int))
                return (object)xmlElement.GetInt(key);
            else if (type == typeof(uint))
                return (object)xmlElement.GetUInt(key);
            else if (type == typeof(bool))
                return (object)xmlElement.GetBool(key);
            else if (type == typeof(double))
                return (object)xmlElement.GetDouble(key);
            else if (type == typeof(long))
                return (object)xmlElement.GetInt64(key);
            else if (type == typeof(IntVector2))
                return (object)xmlElement.GetIntVector2(key);
            else if (type == typeof(Matrix3))
                return (object)xmlElement.GetMatrix3(key);
            else if (type == typeof(Matrix3x4))
                return (object)xmlElement.GetMatrix3x4(key);
            else if (type == typeof(Matrix4))
                return (object)xmlElement.GetMatrix4(key);
            else if (type == typeof(ulong))
                return (object)xmlElement.GetUInt64(key);

            return null;
        }

        private void setAttributeValue(XmlElement xmlElement, string name)
        {
            Type CompnentType = this.GetType();
            FieldInfo mInfo = CompnentType.GetField(name);
            if (mInfo != null)
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
                if ((mInfo.IsPublic || isSerializable))
                    if ((fieldAttributes & FieldAttributes.Literal) != FieldAttributes.Literal)
                    {
                        Type type = mInfo.FieldType;
                        object value = GetObjectValueFromXmlElement(xmlElement, type);
                        mInfo.SetValue(this, value);
                    }
            }
        }


        public virtual void OnDeserialize(XmlElement source)
        {
            if (!source.NotNull()) return;

            var xmlElement = source.GetChild("attribute");
            while (xmlElement.NotNull())
            {
                string name = xmlElement.GetAttribute("name");
                if (name != string.Empty)
                    setAttributeValue(xmlElement, name);
                xmlElement = xmlElement.GetNext();
            }

            if (source.NotNull())
                OnDeserialize(source.GetNext());
        }
    }

}