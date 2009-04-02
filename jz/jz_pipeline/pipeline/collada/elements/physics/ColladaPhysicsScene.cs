using System;
using System.Xml;

namespace jz.pipeline.collada.elements.physics
{
    public sealed class ColladaPhysicsScene : _ColladaElementWithIdAndName
    {
        public ColladaPhysicsScene(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddZeroToManyChildren(aReader, Elements.Physics.kInstanceForceField);
            _AddZeroToManyChildren(aReader, Elements.Physics.kInstancePhysicsModel);
            _AddRequiredChild(aReader, Elements.Physics.kTechniqueCommonOfPhysicsScene);
            _AddZeroToManyChildren(aReader, Elements.kTechnique);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
