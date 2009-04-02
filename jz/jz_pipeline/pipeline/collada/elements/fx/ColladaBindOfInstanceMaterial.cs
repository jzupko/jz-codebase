using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaBindOfInstanceMaterial : _ColladaElement
    {
        #region Private members
        private readonly string mSemantic;
        private _ColladaTarget mTarget;
        #endregion

        public ColladaBindOfInstanceMaterial(XmlReader aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kSemantic, out mSemantic);
            #region target
            {
                string target;
                _SetRequiredAttribute(aReader, Attributes.kTarget, out target);
                {
                    string targetElement;
                    string accessors;
                    _ParseTargetToSidReference(target, out targetElement, out accessors);
                    ColladaDocument.QueueSidForResolution(targetElement, delegate(_ColladaElement a) { mTarget = new _ColladaTarget(a, accessors); });
                }
            }
            #endregion
            #endregion

            _NextElement(aReader);
        }

        public string Semantic { get { return mSemantic; } }
        public _ColladaTarget Target { get { return mTarget; } }
    }
}
