using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaTechniqueCommonOfBindMaterial : _ColladaElement
    {
        public ColladaTechniqueCommonOfBindMaterial(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOneToManyChildren(aReader, Elements.FX.kInstanceMaterial);
            #endregion
        }
    }
}