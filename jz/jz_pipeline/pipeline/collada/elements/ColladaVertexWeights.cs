using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaVertexWeights : _ColladaElement
    {
        #region Private members
        private readonly uint mCount;

        private void _VerifyInputChildren()
        {
            for (_ColladaElement e = mFirstChild; e != null; e = e.NextSibling)
            {
                ColladaInputGroupB input = e as ColladaInputGroupB;

                if (input != null)
                {
                    if (input.Semantic == Enums.InputSemantic.kJoint)
                    {
                        return;
                    }
                }
            }

            throw new Exception("<vertex_weights> requires an <input> child with " +
                "semantic \"" + Enums.InputSemantic.kJoint + "\".");
        }
        #endregion

        public ColladaVertexWeights(XmlReader aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kCount, out mCount);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddRequiredChild(aReader, Elements.kInputGroupB);
            _AddOneToManyChildren(aReader, Elements.kInputGroupB);
            _VerifyInputChildren();

            #region <vcount> and <v>
            {
                Elements.Element e = new Elements.Element(Elements.kVcount.Name, delegate(XmlReader a) { return new ColladaVcount(a, mCount); });
                if (_AddOptionalChild(aReader, e) > 0)
                {
                    // Note: two entries per bone.
                    uint expectedVsize = ((ColladaVcount)mLastChild).ExpectPrimitivesCount * 2u;
                    e = new Elements.Element(Elements.kV.Name, delegate(XmlReader a) { return new ColladaPrimitives(a, expectedVsize); });
                    _AddRequiredChild(aReader, e);
                }
                else
                {
                    if (aReader.Name == Elements.kV.Name)
                    {
                        throw new Exception("<v> of <vertex_weights> must exist with <vcount>");
                    }
                }
            }
            #endregion

            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public uint Count { get { return mCount; } }
    }
}
