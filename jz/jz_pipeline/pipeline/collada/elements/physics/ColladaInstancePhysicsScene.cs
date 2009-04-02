using System;
using System.Xml;

namespace jz.pipeline.collada.elements.physics
{
    public sealed class ColladaInstancePhysicsScene : _ColladaInstance<ColladaPhysicsScene>
    {
        public ColladaInstancePhysicsScene(XmlReader aReader)
            : base (aReader)
        { }
    }
}