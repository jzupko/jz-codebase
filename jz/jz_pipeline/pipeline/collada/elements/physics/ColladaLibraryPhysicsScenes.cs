using System;
using System.Xml;

namespace jz.pipeline.collada.elements.physics
{
    public sealed class ColladaLibraryPhysicsScenes : _ColladaElementWithIdAndName
    {
        public ColladaLibraryPhysicsScenes(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.Physics.kPhysicsScene);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
