using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    // Warning: make sure if you specialize this class that you call ColladaDocument.PushSidNamespace() and
    //     ColladaDocument.PopSidNamespace() around adding any children.
    public abstract class _ColladaElementWithIdAndName : _ColladaElementWithId
    {
        #region Protected members
        protected readonly string mName = "";
        #endregion

        public _ColladaElementWithIdAndName(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kName, ref mName);
            #endregion
        }

        public string Name { get { return mName; } }
    }
}