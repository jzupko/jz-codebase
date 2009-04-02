using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaBindVertexInput : _ColladaElement
    {
        #region Private members
        private readonly string mSemantic;
        private readonly string mInputSemantic;
        private readonly uint mInputSet = 0;
        #endregion

        public ColladaBindVertexInput(XmlReader aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kSemantic, out mSemantic);
            _SetRequiredAttribute(aReader, Attributes.kInputSemantic, out mInputSemantic);
            _SetOptionalAttribute(aReader, Attributes.kInputSet, ref mInputSet);
            #endregion
            _NextElement(aReader);
        }

        public string Semantic { get { return mSemantic; } }
        public string InputSemantic { get { return mInputSemantic; } }
        public uint InputSet { get { return mInputSet; } }
    }
}
