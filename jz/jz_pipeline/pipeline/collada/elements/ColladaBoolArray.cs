using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaBoolArray : _ColladaArray<bool>
    {
        public ColladaBoolArray(XmlReader aReader)
            : base(aReader)
        {
            #region Element value
            string value = string.Empty;
            _SetValue(aReader, ref value);
            Utilities.Tokenize<bool>(value, mArray, XmlConvert.ToBoolean);
            #endregion
        }
    }
}
