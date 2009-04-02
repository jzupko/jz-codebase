using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaExtra : _ColladaElementWithIdAndName
    {
        #region Private members
        private readonly string mType = string.Empty;
        #endregion

        public ColladaExtra(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kType, ref mType);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kTechnique);
            #endregion   
        }

        public string Type { get { return mType; } }
    }
}
