using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryImages : _ColladaElementWithIdAndName
    {
        public ColladaLibraryImages(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kImage);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion       
        }
    }
}
