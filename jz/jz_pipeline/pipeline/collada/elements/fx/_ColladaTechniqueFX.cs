using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public abstract class _ColladaTechniqueFX : _ColladaElementWithId
    {
        #region Private members
        private readonly string mSid;
        #endregion

        public _ColladaTechniqueFX(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kSid, out mSid);
            #endregion
        }

        public string Sid { get { return mSid; } }
    }
}
