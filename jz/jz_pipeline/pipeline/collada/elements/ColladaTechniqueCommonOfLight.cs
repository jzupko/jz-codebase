using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTechniqueCommonOfLight : _ColladaElement
    {
        public ColladaTechniqueCommonOfLight(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);

            _AddRequiredChild(aReader, new Elements.Element(aReader.Name, delegate(XmlReader r) { return new ColladaLightData(r, Enums.GetLightType(aReader.Name)); }));
            #endregion
        }
    }
}