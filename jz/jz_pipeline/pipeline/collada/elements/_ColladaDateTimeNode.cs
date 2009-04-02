using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaDateTimeNode : _ColladaElement
    {
        #region Private members
        private readonly DateTime mDateTime;
        #endregion

        public _ColladaDateTimeNode(XmlReader aReader)
        {
            #region Element value
            _SetValue(aReader, ref mDateTime);
            #endregion
        }
    }
}
