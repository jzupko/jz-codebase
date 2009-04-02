using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaSpline : _ColladaElement
    {
        #region Private members
        private readonly bool mbClosed = Defaults.kbClosed;
        #endregion
                
        public ColladaSpline(XmlReader aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kClosed, ref mbClosed);
            #endregion

            #region Children
            _NextElement(aReader);

            _AddOneToManyChildren(aReader, Elements.kSource);
            _AddRequiredChild(aReader, Elements.kControlVertices);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion   
        }
    }
}
