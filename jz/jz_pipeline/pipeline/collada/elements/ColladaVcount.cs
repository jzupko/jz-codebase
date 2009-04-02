using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    /// <summary>
    /// Encapsulation of a COLLADA "vcount" element.
    /// </summary>
    public sealed class ColladaVcount : _ColladaElement
    {
        #region Private members
        private readonly uint[] mSides;
        private uint mExpectedPrimitivesCount = 0;

        private void _CalculateExpectedPrimitivesCount()
        {
            mExpectedPrimitivesCount = 0;
            int count = mSides.Length;

            for (int i = 0; i < count; i++)
            {
                mExpectedPrimitivesCount += mSides[i];
            }
        }
        #endregion

        public ColladaVcount(XmlReader aReader)
        {
            #region Element value
            string value = "";
            _SetValue(aReader, ref value);
            mSides = Utilities.Tokenize<uint>(value, XmlConvert.ToUInt32);
            _CalculateExpectedPrimitivesCount();
            #endregion
        }

        public ColladaVcount(XmlReader aReader, uint aExpectedSize)
        {
            #region Element value
            mSides = new uint[aExpectedSize];
            string value = "";
            _SetValue(aReader, ref value);
            Utilities.Tokenize<uint>(value, mSides, XmlConvert.ToUInt32);
            _CalculateExpectedPrimitivesCount();
            #endregion
        }

        public uint Count
        {
            get
            {
                return (uint)mSides.Length;
            }
        }

        public uint ExpectPrimitivesCount
        {
            get
            {
                return mExpectedPrimitivesCount;
            }
        }

        public uint this[uint i]
        {
            get
            {
                return mSides[i];
            }
        }
    }
}