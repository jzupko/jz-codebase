using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaFloatArray : _ColladaArray<float>
    {
        #region Protected members
        private readonly short mDigits;
        private readonly short mMagnitude;
        #endregion

        public ColladaFloatArray(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kDigits, ref mDigits);
            _SetOptionalAttribute(aReader, Attributes.kMagnitude, ref mMagnitude);
            #endregion        

            #region Element value
            string value = string.Empty;
            _SetValue(aReader, ref value);
            Utilities.Tokenize<float>(value, mArray, XmlConvert.ToSingle);
            #endregion
        }

        public short Digits { get { return mDigits; } }
        public short Magnitude { get { return mMagnitude; } }
    }
}
