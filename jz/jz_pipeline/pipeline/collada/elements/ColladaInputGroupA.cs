using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInputGroupA : _ColladaInput
    {
        public ColladaInputGroupA(XmlReader aReader)
            : base(aReader)
        {
            _NextElement(aReader);
        }
    }
}
