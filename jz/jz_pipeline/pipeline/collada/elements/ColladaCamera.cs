using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaCamera : _ColladaElementWithIdAndName
    {
        public ColladaCamera(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddRequiredChild(aReader, Elements.kOptics);
            _AddOptionalChild(aReader, Elements.kImager);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
