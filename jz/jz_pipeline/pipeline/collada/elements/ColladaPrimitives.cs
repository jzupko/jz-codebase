using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaPrimitives : _ColladaElement
    {
        #region Private members
        private readonly int[] mIndices;
        #endregion

        public ColladaPrimitives(XmlReader aReader)
        {
            #region Element value
            string value = "";
            _SetValue(aReader, ref value);
            mIndices = Utilities.Tokenize<int>(value, XmlConvert.ToInt32);
            #endregion
        }

        public ColladaPrimitives(XmlReader aReader, uint aExpectedSize)
        {
            #region Element value
            mIndices = new int[aExpectedSize];
            string value = "";
            _SetValue(aReader, ref value);
            Utilities.Tokenize<int>(value, mIndices, XmlConvert.ToInt32);
            #endregion
        }

        public int[] GetSparse(uint aOffset, uint aStride)
        {
            uint count = (uint)mIndices.Length;
            if (count % aStride != 0) { throw new Exception("Invalid stride."); }

            int[] ret = new int[count / aStride];
            
            uint index = 0;
            for (uint i = aOffset; i < count; i += aStride)
            {
                ret[index++] = mIndices[i];
            }

            return ret;
        }

        public uint Count { get { return (uint)mIndices.Length; } }
        public int this[uint i] { get { return mIndices[i]; } }
    }
}