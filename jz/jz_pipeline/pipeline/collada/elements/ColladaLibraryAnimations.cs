using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryAnimations : _ColladaElementWithIdAndName
    {
        public ColladaLibraryAnimations(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kAnimation);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion       
        }
    }
}
