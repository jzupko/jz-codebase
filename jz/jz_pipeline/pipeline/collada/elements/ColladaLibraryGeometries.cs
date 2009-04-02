using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryGeometries : _ColladaElementWithIdAndName
    {
        public ColladaLibraryGeometries(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kGeometry);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion   
        }
    }
}
