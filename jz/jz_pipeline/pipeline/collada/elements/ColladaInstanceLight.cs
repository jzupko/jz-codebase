using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInstanceLight : _ColladaInstance<ColladaLight>
    {
        public ColladaInstanceLight(XmlReader aReader)
            : base(aReader)
        { }
    }
}
