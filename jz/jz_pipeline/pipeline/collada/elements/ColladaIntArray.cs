using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaIntArray : _ColladaArray<int>
    {
        #region Protected members
        private readonly int mMin = Defaults.kIntMinAttribute;
        private readonly int mMax = Defaults.kIntMaxAttribute;
        #endregion

        public ColladaIntArray(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kMinInclusive, ref mMin);
            _SetOptionalAttribute(aReader, Attributes.kMaxInclusive, ref mMax);
            #endregion        

            #region Element value
            string value = string.Empty;
            _SetValue(aReader, ref value);
            Utilities.Tokenize<int>(value, mArray, XmlConvert.ToInt32);
            #endregion
        }
    }
}
