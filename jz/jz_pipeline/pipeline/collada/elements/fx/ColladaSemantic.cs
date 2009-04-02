using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaSemantic : _ColladaStringElement
    {
        public ColladaSemantic(XmlReader aReader)
            : base(aReader)
        { }
    }
}
