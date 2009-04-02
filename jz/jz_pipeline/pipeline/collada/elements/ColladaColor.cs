using Microsoft.Xna.Framework;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaColor : _ColladaElement
    {
        #region Private members
        private readonly Vector4 mColor;
        #endregion

        public ColladaColor(XmlReader aReader)
        {
            #region Element value
            string color = string.Empty;
            _SetValue(aReader, ref color);
            float[] values = new float[4];
            Utilities.Tokenize(color, values, XmlConvert.ToSingle);
            mColor = new Vector4(values[0], values[1], values[2], values[3]);
            #endregion
        }

        public Vector3 ColorRGB { get { return new Vector3(mColor.X, mColor.Y, mColor.Z); } }
        public Vector4 ColorRGBA { get { return mColor; } }
    }
}
