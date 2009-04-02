using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryLights : _ColladaElementWithIdAndName
    {
        public ColladaLibraryLights(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kLight);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
