using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaStringElement : _ColladaElement
    {
        #region Private members
        private readonly string mString = "";
        #endregion

        public _ColladaStringElement(XmlReader aReader)
        {
            #region Element value
            _SetValue(aReader, ref mString);
            #endregion
        }
    }
}
