using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryVisualScenes : _ColladaElementWithIdAndName
    {
        public ColladaLibraryVisualScenes(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kVisualScene);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
