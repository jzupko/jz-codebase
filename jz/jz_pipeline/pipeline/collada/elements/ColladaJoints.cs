using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaJoints : _ColladaElement
    {
        #region Private members
        // Todo: the input with JOINT semantic should reference a Name_array.
        //    need to find a way to verify this (the source may not exist at this point
        //    and the check needs to be deferred until it does).
        private void _VerifyInputChildren()
        {
            for (_ColladaElement e = mFirstChild; e != null; e = e.NextSibling)
            {
                ColladaInputGroupA input = e as ColladaInputGroupA;

                if (input != null)
                {
                    if (input.Semantic == Enums.InputSemantic.kJoint)
                    {
                        return;
                    }
                }
            }

            throw new Exception("<joints> requires an <input> child with semantic + \"" + Enums.InputSemantic.kJoint + "\".");
        }
        #endregion

        public ColladaJoints(XmlReader aReader)
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
