using System;
using System.Collections.Generic;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class _ColladaGenericElement : _ColladaElement
    {
        #region Private members
        private readonly Dictionary<string, string> mAttributes = new Dictionary<string, string>();
        private readonly string mName;
        private readonly string mValue = "";
        #endregion

        public _ColladaGenericElement(XmlReader aReader)
        {
            mName = aReader.Name;

            #region Attributes
            if (aReader.HasAttributes)
            {
                while (aReader.MoveToNextAttribute())
                {
                    mAttributes.Add(aReader.Name, aReader.Value);
                }

                aReader.MoveToElement();
            }
            #endregion

            #region Element value
            _NextElementOrText(aReader);
            if (aReader.HasValue)
            {
                mValue = aReader.Value;
                _NextElement(aReader);
            }
            #endregion

            #region Children
            while (aReader.NodeType != XmlNodeType.None)
            {
                Elements.Element e = new Elements.Element(aReader.Name, delegate(XmlReader a) { return new _ColladaGenericElement(a); });
                _AddRequiredChild(aReader, e);
            }
            #endregion
        }

        public bool GetContains(string aAttribute) { return mAttributes.ContainsKey(aAttribute); }

        public string Name { get { return mName; } }
        public string Value { get { return mValue; } }

        public string this[string s]
        {
            get
            {
                if (!mAttributes.ContainsKey(s))
                {
                    throw new Exception("Collada generic element \"" + mName + "\" does not contain " +
                        "requested attribute \"" + s + "\".");
                }

                return mAttributes[s];
            }
        }
    }
}
