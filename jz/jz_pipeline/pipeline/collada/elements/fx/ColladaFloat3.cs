using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaFloat3 : _ColladaElement
    {
        #region Private members
        private readonly float mX = 0.0f;
        private readonly float mY = 0.0f;
        private readonly float mZ = 0.0f;
        #endregion

        public ColladaFloat3(XmlReader aReader)
        {
            #region Element value
            float[] buf = new float[3];
            string value = "";
            _SetValue(aReader, ref value);
            Utilities.Tokenize(value, buf, XmlConvert.ToSingle);
            mX = buf[0];
            mY = buf[1];
            mZ = buf[2];
            #endregion
        }

        public float X { get { return mX; } }
        public float Y { get { return mY; } }
        public float Z { get { return mZ; } }
    }
}
