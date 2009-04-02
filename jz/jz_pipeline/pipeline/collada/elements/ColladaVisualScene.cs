using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaVisualScene : _ColladaElementWithIdAndName
    {
        public ColladaVisualScene(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddOneToManyChildren(aReader, Elements.kNode);
            _AddZeroToManyChildren(aReader, Elements.kEvaluateScene);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion   
        }
    }
}
