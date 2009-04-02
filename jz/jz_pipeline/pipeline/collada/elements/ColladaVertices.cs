using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaVertices : _ColladaElementWithIdAndName
    {
        #region Private members
        private void _VerifyInputChildren()
        {
            for (_ColladaElement e = mFirstChild; e != null; e = e.NextSibling)
            {
                ColladaInputGroupA input = e as ColladaInputGroupA;

                if (input != null)
                {
                    if (input.Semantic == Enums.InputSemantic.kPosition)
                    {
                        return;
                    }
                }
            }

            throw new Exception("no child input with semantic \"" +
                Enums.InputSemantic.kPosition + "\".");
        }
        #endregion

        public ColladaVertices(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOneToManyChildren(aReader, Elements.kInputGroupA);
            _VerifyInputChildren();
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion   
        }
    }
}
