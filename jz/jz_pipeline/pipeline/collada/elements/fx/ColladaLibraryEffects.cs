using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaLibraryEffects : _ColladaElementWithIdAndName
    {
        public ColladaLibraryEffects(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddZeroToManyChildren(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.FX.kEffect);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
