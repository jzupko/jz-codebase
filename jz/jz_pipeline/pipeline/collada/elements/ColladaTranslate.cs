using Microsoft.Xna.Framework;
using System;
using System.Xml;
using System.Collections.Generic;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTranslate : _ColladaTransformElement
    {
        #region Private members
        private Vector3 mData;
        #endregion

        public const int kTranslateParts = 3;

        public ColladaTranslate(XmlReader aReader)
            : base(aReader)
        {
            #region Element values
            float[] buf = new float[kTranslateParts];
            string value = "";
            _SetValue(aReader, ref value);
            Utilities.Tokenize(value, buf, XmlConvert.ToSingle);

            mData = new Vector3(buf[0], buf[1], buf[2]);
            #endregion
        }

        public Vector3 Translation { get { return mData; } }
        public override Matrix XnaMatrix { get { return Matrix.CreateTranslation(mData); } }
        
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
                    throw new Exception("Invalid arrays for translation key frame.");
                }

                uint count = times.Count;
                for (uint i = 0; i < count; i++)
                {
                    uint dataIndex = i * dataStride;
                    Vector3 tra = mData;
                    float time = times[i];

                    for (uint j = dataIndex; j < dataIndex + paramStride; j++)
                    {
                        switch (prms[j - dataIndex])
                        {
                            case Enums.ParamName.kX: tra.X = data[j]; break;
                            case Enums.ParamName.kY: tra.Y = data[j]; break;
                            case Enums.ParamName.kZ: tra.Z = data[j]; break;
                            default:
                                throw new Exception("Invalid <param> name \"" + prms[j - dataIndex].ToString() + "\" for translation key frame.");
                        }
                    }

                    ret.Add(new AnimationKeyFrame(time, Matrix.CreateTranslation(tra)));
                }

                return ret.ToArray();
            }
        }
    }
}
