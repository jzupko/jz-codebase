using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaProfileCOMMON : _ColladaElementWithId
    {
        public ColladaProfileCOMMON(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddZeroToManyChildren(aReader, Elements.kImage);
            _AddZeroToManyChildren(aReader, Elements.FX.kNewparamOfProfileCOMMON);
            _AddRequiredChild(aReader, Elements.FX.kTechniqueOfProfileCOMMON);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
