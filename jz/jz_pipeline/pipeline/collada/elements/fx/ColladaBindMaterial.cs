using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaBindMaterial : _ColladaElement
    {
        public ColladaBindMaterial(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddZeroToManyChildren(aReader, Elements.kParam);
            _AddRequiredChild(aReader, Elements.FX.kTechniqueCommonOfBindMaterial);
            _AddZeroToManyChildren(aReader, Elements.FX.kTechniqueOfProfileCOMMON);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
