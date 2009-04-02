using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaOptics : _ColladaElement
    {
        public ColladaOptics(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddRequiredChild(aReader, Elements.kTechniqueCommonOfOptics);
            _AddZeroToManyChildren(aReader, Elements.kTechnique);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
