using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryControllers : _ColladaElementWithIdAndName
    {
        public ColladaLibraryControllers(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kController);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion       
        }
    }
}
