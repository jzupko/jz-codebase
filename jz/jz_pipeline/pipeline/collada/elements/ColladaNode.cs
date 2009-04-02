using Microsoft.Xna.Framework;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaNode : _ColladaElementWithIdAndName
    {
        #region Private members
        private readonly string mSid = "";
        private readonly string mType = "";
        private readonly string mLayer = "";
        private readonly Matrix mLocalTransform = Matrix.Identity;
        #endregion

        public ColladaNode(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kSid, ref mSid);
            _SetOptionalAttribute(aReader, Attributes.kType, ref mType);
            _SetOptionalAttribute(aReader, Attributes.kLayer, ref mLayer);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            #region Transform Elements
            {
                int numFound = 0;
                do
                {
                    numFound = 0;
                    numFound += _AddOptionalChild(aReader, Elements.kLookAt);
                    numFound += _AddOptionalChild(aReader, Elements.kMatrix);
                    numFound += _AddOptionalChild(aReader, Elements.kRotate);
                    numFound += _AddOptionalChild(aReader, Elements.kScale);
                    numFound += _AddOptionalChild(aReader, Elements.kSkew);
                    numFound += _AddOptionalChild(aReader, Elements.kTranslate);
                } while (numFound > 0);
            }
            #endregion
            _AddZeroToManyChildren(aReader, Elements.kInstanceCamera);
            _AddZeroToManyChildren(aReader, Elements.kInstanceController);
            _AddZeroToManyChildren(aReader, Elements.kInstanceGeometry);
            _AddZeroToManyChildren(aReader, Elements.kInstanceLight);
            _AddZeroToManyChildren(aReader, Elements.kInstanceNode);
            _AddZeroToManyChildren(aReader, Elements.kNode);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion   

            #region Calculate transforms
            foreach (_ColladaTransformElement e in GetEnumerable<_ColladaTransformElement>())
            {
                mLocalTransform = e.XnaMatrix * mLocalTransform;
            }
            #endregion
        }

        public Matrix LocalOpenGLTransform { get { return Matrix.Transpose(mLocalTransform); } }
        public Matrix LocalXnaTransform { get { return mLocalTransform; } }
        public Matrix WorldOpenGLTransform { get { return Matrix.Transpose(WorldXnaTransform); } }
        public string Sid { get { return mSid; } }
        public string Type { get { return mType; } }

        public Matrix WorldXnaTransform
        {
            get
            {
                if (mParent != null && mParent is ColladaNode)
                {
                    ColladaNode parent = (ColladaNode)mParent;
                    return mLocalTransform * parent.WorldXnaTransform;
                }
                else
                {
                    return mLocalTransform;
                }
            }
        }
    }
}
