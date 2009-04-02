using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaAnimationClip : _ColladaElementWithIdAndName
    {
        #region Private members
        private readonly double mStart = Defaults.kStartAttribute;
        private readonly double mEnd = 0.0;
        #endregion

        public ColladaAnimationClip(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kStart, ref mStart);
            _SetOptionalAttribute(aReader, Attributes.kEnd, ref mEnd);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kInstanceAnimation);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
