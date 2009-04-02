using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInstanceVisualScene : _ColladaInstance<ColladaVisualScene>
    {
        public ColladaInstanceVisualScene(XmlReader aReader)
            : base(aReader)
        { }
    }
}
