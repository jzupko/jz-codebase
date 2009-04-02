using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaInstance<T> : _ColladaElementWithSidAndName where T : _ColladaElement
    {
        #region Protected members
        protected T mInstance = null;

        protected virtual void _PreChildrenAdd(XmlReader aReader)
        {
        }
        #endregion

        public _ColladaInstance(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            string url;
            _SetRequiredAttribute(aReader, Attributes.kUrl, out url);
            ColladaDocument.QueueIdForResolution(url, delegate(_ColladaElement aResolvedNode) { mInstance = (T)aResolvedNode; });
            #endregion

            #region Children
            _NextElement(aReader);
            _PreChildrenAdd(aReader);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public T Instance
        {
            get
            {
                return mInstance;
            }
        }
    }
}
