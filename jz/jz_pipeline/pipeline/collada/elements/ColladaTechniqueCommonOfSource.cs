using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTechniqueCommonOfSource : _ColladaElement
    {
        public ColladaTechniqueCommonOfSource(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddRequiredChild(aReader, Elements.kAccessor);
            #endregion
        }
    }
}