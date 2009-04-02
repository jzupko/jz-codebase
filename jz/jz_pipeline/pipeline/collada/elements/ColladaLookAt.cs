using Microsoft.Xna.Framework;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLookAt : _ColladaTransformElement
    {
        #region Private members
        private Vector3 mEyePosition;
        private Vector3 mTargetPosition;
        private Vector3 mUpAxis;
        #endregion

        public const int kLookAtParts = 9;

        public ColladaLookAt(XmlReader aReader)
            : base(aReader)
        {
            #region Element values
            float[] buf = new float[kLookAtParts];
            string value = string.Empty;
            _SetValue(aReader, ref value);
            Utilities.Tokenize(value, buf, XmlConvert.ToSingle);

            mEyePosition = new Vector3(buf[0], buf[1], buf[2]);
            mTargetPosition = new Vector3(buf[3], buf[4], buf[5]);
            mUpAxis = new Vector3(buf[6], buf[7], buf[8]);
            #endregion
        }

        public override Matrix XnaMatrix { get { return Matrix.CreateLookAt(EyePosition, TargetPosition, UpAxis); } }
        public Vector3 EyePosition { get { return mEyePosition; } }
        public Vector3 TargetPosition { get { return mTargetPosition; } }
        public Vector3 UpAxis { get { return mUpAxis; } }
    }
}
