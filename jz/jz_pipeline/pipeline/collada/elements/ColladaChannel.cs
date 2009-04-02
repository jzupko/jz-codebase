using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaChannel : _ColladaElement
    {
        #region Private members
        private _ColladaElement mSource;
        private _ColladaTarget mTarget;

        private _ColladaArray<T> _GetArrayHelper<T>(string aSemantic)
        {
            return mSource.GetFirst<ColladaInputGroupA>(
                delegate(ColladaInputGroupA e)
                {
                    if (e.Semantic == aSemantic)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }).GetArray<T>();
        }
        #endregion

        public ColladaChannel(XmlReader aReader)
        {
            #region Attributes
            string source;
            _SetRequiredAttribute(aReader, Attributes.kSource, out source);
            ColladaDocument.QueueIdForResolution(source, delegate(_ColladaElement aResolved) { mSource = aResolved; });
            #region target
            {
                string target;
                _SetRequiredAttribute(aReader, Attributes.kTarget, out target);
                {
                    string targetElement;
                    string accessors;
                    _ParseTargetToSidReference(target, out targetElement, out accessors);
                    ColladaDocument.QueueSidForResolution(targetElement,
                        delegate(_ColladaElement a)
                        {
                            mTarget = new _ColladaTarget(a, accessors);
                            if (!(mTarget.Target is _ColladaChannelTarget))
                            {
                                throw new Exception("target of <channel> element is not valid.");
                            }
                            else
                            {
                                ((_ColladaChannelTarget)mTarget.Target).AddTargetOf(this);
                            }
                        });
                }
            }
            #endregion
            #endregion

            _NextElement(aReader);
        }

        public _ColladaElement Source { get { return mSource; } }
        public _ColladaTarget Target { get { return mTarget; } }
        
        public _ColladaArray<float> Inputs { get { return _GetArrayHelper<float>(Enums.InputSemantic.kInput); } }
        public _ColladaArray<float> Outputs { get { return _GetArrayHelper<float>(Enums.InputSemantic.kOutput); } }
        public _ColladaArray<float> InTangents { get { return _GetArrayHelper<float>(Enums.InputSemantic.kInTangent); } }
        public _ColladaArray<float> OutTangents { get { return _GetArrayHelper<float>(Enums.InputSemantic.kOutTangent); } }
        public _ColladaArray<string> Interpolations { get { return _GetArrayHelper<string>(Enums.InputSemantic.kInterpolation); } }

    }
}
