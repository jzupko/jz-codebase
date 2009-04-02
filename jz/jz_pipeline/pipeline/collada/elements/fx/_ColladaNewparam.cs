using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public abstract class _ColladaNewparam : _ColladaElementWithSid
    {
        public _ColladaNewparam(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            if (mSid == "")
            {
                throw new Exception("<newparam> requires a sid attribute.");
            }
            #endregion
        }
    }
}
