using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    // Warning: make sure if you specialize this class that you call ColladaDocument.PushSidNamespace() and
    //     ColladaDocument.PopSidNamespace() around adding any children.
    public abstract class _ColladaElementWithId : _ColladaElement
    {
        #region Protected members
        protected readonly string mId = "";
        #endregion

        public _ColladaElementWithId(XmlReader aReader)
        {
            #region Attributes
            if (_SetOptionalAttribute(aReader, Attributes.kId, ref mId))
            {
                ColladaDocument.AddElementWithId(mId, this);
            }
            #endregion
        }

        public string Id
        {
            get
            {
                return mId;
            }
        }
    }
}