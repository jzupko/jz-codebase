using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaController : _ColladaElementWithIdAndName
    {
        public ColladaController(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            int childExclusiveCount = 0;
            _AddOptionalChild(aReader, Elements.kAsset);
            childExclusiveCount += _AddOptionalChild(aReader, Elements.kSkin);
            childExclusiveCount += _AddOptionalChild(aReader, Elements.kMorph);

            if (childExclusiveCount != 1)
            {
                throw new Exception("element <controller> does not have the correct child elements.");
            }

            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
