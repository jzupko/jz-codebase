using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaInstanceMaterial : _ColladaElementWithSidAndName
    {
        #region Private members
        private ColladaMaterial mInstance;
        private readonly string mSymbol;
        #endregion

        public ColladaInstanceMaterial(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            string target;
            _SetRequiredAttribute(aReader, Attributes.kTarget, out target);
            ColladaDocument.QueueIdForResolution(target, delegate(_ColladaElement aResolvedElement) { mInstance = (ColladaMaterial)aResolvedElement; });
            _SetRequiredAttribute(aReader, Attributes.kSymbol, out mSymbol);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddZeroToManyChildren(aReader, Elements.FX.kBindOfInstanceMaterial);
            _AddZeroToManyChildren(aReader, Elements.FX.kBindVertexInput);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public ColladaMaterial Instance { get { return mInstance; } }
        public string Symbol { get { return mSymbol; } }
    }
}
