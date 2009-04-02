using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInstanceNode : _ColladaInstance<ColladaNode>
    {
        public ColladaInstanceNode(XmlReader aReader)
            : base(aReader)
        { }
    }
}
