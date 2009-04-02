using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTargets : _ColladaElement
    {
        #region Private members
        private void _VerifyInputChildren()
        {
            bool bA = false;
            bool bB = false;

            for (_ColladaElement e = mFirstChild; e != null; e = e.NextSibling)
            {
                ColladaInputGroupA input = e as ColladaInputGroupA;

                if (input != null)
                {
                    if (input.Semantic == Enums.InputSemantic.kMorphTarget)
                    {
                        bA = true;
                    }
                    else if (input.Semantic == Enums.InputSemantic.kMorphWeight)
                    {
                        bB = true;
                    }

                    if (bA && bB)
                    {
                        return;
                    }
                }
            }

            throw new Exception("missing input semantics for <targets>.");
        }
        #endregion

        public ColladaTargets(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddRequiredChild(aReader, Elements.kInputGroupA);
            _AddOneToManyChildren(aReader, Elements.kInputGroupA);
            _VerifyInputChildren();
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion   
        }
    }
}
