using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    // Warning: make sure if you specialize this class that you call ColladaDocument.PushSidNamespace() and
    //     ColladaDocument.PopSidNamespace() around adding any children.
    public abstract class _ColladaElementWithSid : _ColladaElement
    {
        #region Protected members
        protected readonly string mSid = "";
        #endregion

        public _ColladaElementWithSid(XmlReader aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kSid, ref mSid);
            #endregion
        }

        public string Sid { get { return mSid; } }
    }
}