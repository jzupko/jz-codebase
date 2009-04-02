using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaMatrix : _ColladaTransformElement
    {
        #region Private members
        private Matrix mMatrix;
        #endregion

        public const int kMatrixTotalSize = 16;

        public ColladaMatrix(XmlReader aReader)
            : base(aReader)
        {
            #region Element values
            float[] buf = new float[kMatrixTotalSize];
            string value = string.Empty;
            _SetValue(aReader, ref value);
            Utilities.Tokenize(value, buf, XmlConvert.ToSingle);

            mMatrix = ToXnaMatrix(buf);
            #endregion
        }

        public override Matrix XnaMatrix { get { return mMatrix; } }
        public Matrix OpenGLMatrix { get { return Matrix.Transpose(mMatrix); } }

        public override AnimationKeyFrame[] GetKeyFrames(int index)
        {
            if (index >= TargetOf.Length)
            {
                return new AnimationKeyFrame[0];
            }
            else
            {
                List<AnimationKeyFrame> ret = new List<AnimationKeyFrame>();
                ColladaChannel channel = TargetOf[index];

                _ColladaArray<float> times = channel.Inputs;
                _ColladaArray<float> data = channel.Outputs;
                uint dataStride = data.Stride;
                Enums.ParamName[] prms = data.Params;
                uint paramStride = (uint)prms.Length;

                if (times.Stride != 1 || (data.Count / dataStride) != times.Count)
                {
                    throw new Exception("Invalid arrays for matrix key frame.");
                }

                uint count = times.Count;
                for (uint i = 0; i < count; i++)
                {
                    uint dataIndex = i * dataStride;
                    Matrix transform = mMatrix;
                    float time = times[i];

                    for (uint j = dataIndex; j < dataIndex + paramStride; j++)
                    {
                        switch (prms[j - dataIndex])
                        {
                            case Enums.ParamName.kTransform: transform = ToXnaMatrix(data.Array, j); break;
                            default:
                                throw new Exception("Invalid <param> name \"" + prms[j - dataIndex].ToString() + "\" for matrix key frame.");
                        }
                    }

                    ret.Add(new AnimationKeyFrame(time, transform));
                }

                return ret.ToArray();
            }
        }
    }
}
