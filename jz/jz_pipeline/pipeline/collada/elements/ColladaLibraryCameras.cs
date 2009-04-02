using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryCameras : _ColladaElementWithIdAndName
    {
        public ColladaLibraryCameras(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kCamera);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion       
        }
    }
}
