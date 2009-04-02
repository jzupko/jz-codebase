using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInputGroupB : _ColladaInput
    {
        #region Private members
        private readonly uint mOffset;
        #endregion

        public ColladaInputGroupB(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kOffset, out mOffset);
            _SetOptionalAttribute(aReader, Attributes.kSet, ref mSet);

            _NextElement(aReader);
            #endregion
        }

        public uint Offset { get { return mOffset; } }

    }
}
