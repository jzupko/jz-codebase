using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInstanceAnimation : _ColladaInstance<ColladaAnimation>
    {
        public ColladaInstanceAnimation(XmlReader aReader)
            : base(aReader)
        { }
    }
}
