using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaLibraryMaterials : _ColladaElementWithIdAndName
    {
        public ColladaLibraryMaterials(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddZeroToManyChildren(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.FX.kMaterial);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
