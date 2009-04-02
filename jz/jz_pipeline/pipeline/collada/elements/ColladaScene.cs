using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaScene : _ColladaElement
    {
        public ColladaScene(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddZeroToManyChildren(aReader, Elements.Physics.kInstancePhysicsScene);
            _AddOptionalChild(aReader, Elements.kInstanceVisualScene);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion            
        }
    }
}
