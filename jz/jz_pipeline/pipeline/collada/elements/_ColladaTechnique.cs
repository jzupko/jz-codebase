using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaTechnique : _ColladaElement
    {
        #region Protected members
        protected readonly string mProfile;
        protected readonly string mSchema = "";
        #endregion

        public _ColladaTechnique(XmlReader aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kProfile, out mProfile);
            _SetOptionalAttribute(aReader, Attributes.kSchema, ref mSchema);
            #endregion
        }

        public string Profile
        {
            get
            {
                return mProfile;
            }
        }
    }
}
