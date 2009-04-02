using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaParam : _ColladaElementWithSid
    {
        #region Private members
        private readonly Enums.ParamName mName = Defaults.kParamName;
        private readonly Enums.Type mType;
        private readonly string mSemantic = Defaults.kParamSemantic;
        #endregion

        public ColladaParam(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kName, ref mName, Enums.GetParamName);
            _SetRequiredAttribute(aReader, Attributes.kType, out mType, Enums.GetType);
            _SetOptionalAttribute(aReader, Attributes.kSemantic, ref mSemantic);

            _NextElement(aReader);
            #endregion
        }

        public Enums.ParamName Name { get { return mName; } }
        public string Semantic { get { return mSemantic; }}
        public Enums.Type Type { get { return mType; } }
    }
}
