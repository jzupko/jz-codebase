using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaSampler : _ColladaElementWithId
    {
        #region Private members
        private void _VerifyInputChildren()
        {
            for (_ColladaElement e = mFirstChild; e != null; e = e.NextSibling)
            {
                ColladaInputGroupA input = e as ColladaInputGroupA;

                if (input != null)
                {
                    if (input.Semantic == Enums.InputSemantic.kInterpolation)
                    {
                        return;
                    }
                }
            }

            string message = "<sampler id=\"" + mId + "\"> does not have a child with expected attribute 'semantic=\"" + 
                Enums.InputSemantic.kInterpolation + "\"'. The interpolation type will need to " +
                "be application defined.";

            if (Settings.bAllowMissingInputWithInterpolationSemanticOfSamplerElement)
            {
                ColladaDocument.LogMessage(message, ColladaDocument.MessageType.Warning);
            }
            else
            {
                throw new Exception(message);
            }
        }
        #endregion

        public ColladaSampler(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);

            _AddOneToManyChildren(aReader, Elements.kInputGroupA);
            _VerifyInputChildren();
            #endregion
        }
    }
}
