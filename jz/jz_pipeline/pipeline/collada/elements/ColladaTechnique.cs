using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTechnique : _ColladaTechnique
    {
        public ColladaTechnique(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);

            while (aReader.NodeType != XmlNodeType.None)
            {
                Elements.Element e = new Elements.Element(aReader.Name, delegate(XmlReader a) { return new _ColladaGenericElement(a); });
                _AddRequiredChild(aReader, e);
            }
            #endregion
        }
    }
}
