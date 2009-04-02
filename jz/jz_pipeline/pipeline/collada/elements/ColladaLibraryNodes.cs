using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryNodes : _ColladaElementWithIdAndName
    {
        public ColladaLibraryNodes(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kNode);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
