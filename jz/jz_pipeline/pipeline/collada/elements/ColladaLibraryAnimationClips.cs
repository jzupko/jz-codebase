using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLibraryAnimationClips : _ColladaElementWithIdAndName
    {
        public ColladaLibraryAnimationClips(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddZeroToManyChildren(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kAnimationClip);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
