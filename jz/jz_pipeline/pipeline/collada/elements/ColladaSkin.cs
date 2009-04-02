using Microsoft.Xna.Framework;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaSkin : _ColladaElement
    {
        #region Private members
        // Todo: determine what type this should be.
        private _ColladaElement mSource = null;
        #endregion

        public ColladaSkin(XmlReader aReader)
        {
            #region Attributes
            string source;
            _SetRequiredAttribute(aReader, Attributes.kSource, out source);
            ColladaDocument.QueueIdForResolution(source, delegate(_ColladaElement aResolvedElement) { mSource = (_ColladaElement)aResolvedElement; });
            #endregion

            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kBindShapeMatrix);
            _AddRequiredChild(aReader, Elements.kSource);
            _AddRequiredChild(aReader, Elements.kSource);
            _AddOneToManyChildren(aReader, Elements.kSource);
            _AddRequiredChild(aReader, Elements.kJoints);
            _AddRequiredChild(aReader, Elements.kVertexWeights);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion

        }

        public Matrix XnaBindShapeTransform { get { return GetFirst<ColladaBindShapeMatrix>().XnaMatrix; } }
        public _ColladaElement Source { get { return mSource; } }

        public _ColladaArray<object> Joints
        {
            get
            {
                return GetFirst<ColladaJoints>().GetFirst<ColladaInputGroupA>(delegate(ColladaInputGroupA e)
                {
                    if (e.Semantic == Enums.InputSemantic.kJoint)
                    {
                        return true;
                    }

                    return false;
                }).GetArray<object>();
            }
        }

        public Matrix[] InverseBindTransforms
        {
            get
            {
                _ColladaArray<float> a = GetFirst<ColladaJoints>().GetFirst<ColladaInputGroupA>(
                    delegate(ColladaInputGroupA e)
                    {
                        if (e.Semantic == Enums.InputSemantic.kInverseBindMatrix)
                        {
                            return true;
                        }

                        return false;
                    }).GetArray<float>();

                uint elementCount = a.ElementCount;
                uint stride = a.Stride;

                if (stride != 16 || elementCount != Joints.ElementCount)
                {
                    throw new Exception("Invalid inverse bind transforms.");
                }

                Matrix[] ret = new Matrix[elementCount];
                for (uint i = 0; i < elementCount; i++)
                {
                    uint index = (i * stride);
                    ret[i] = ToXnaMatrix(a.Array, index);
                }

                return ret;
            }
        }
    }
}
