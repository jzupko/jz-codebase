using Microsoft.Xna.Framework;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaBindShapeMatrix : _ColladaElement
    {
        #region Private members
        private Matrix mMatrix;
        #endregion

        public const int kMatrixTotalSize = 16;

        public ColladaBindShapeMatrix(XmlReader aReader)
        {
            #region Element values
            float[] buf = new float[kMatrixTotalSize];
            string value = string.Empty;
            _SetValue(aReader, ref value);
            Utilities.Tokenize(value, buf, XmlConvert.ToSingle);

            mMatrix = ToXnaMatrix(buf);
            #endregion
        }

        public Matrix XnaMatrix { get { return mMatrix; } }
        public Matrix OpenGLMatrix { get { return Matrix.Transpose(mMatrix); } }
    }
}
