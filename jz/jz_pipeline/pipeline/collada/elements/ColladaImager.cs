using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaImager : _ColladaElement
    {
        public ColladaImager(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOneToManyChildren(aReader, Elements.kTechnique);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
