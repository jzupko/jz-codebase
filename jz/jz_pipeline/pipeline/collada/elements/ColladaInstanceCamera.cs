using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInstanceCamera : _ColladaInstance<ColladaCamera>
    {
        public ColladaInstanceCamera(XmlReader aReader)
            : base(aReader)
        { }
    }
}
