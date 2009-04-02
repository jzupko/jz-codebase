using System;
using System.Collections.Generic;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaPrimitive : _ColladaElement
    {
        #region Protected members
        protected readonly string mName = "";
        protected readonly uint mCount = 0;
        protected readonly string mMaterial = "";

        protected abstract void _ProcessPrimitives(int aInputCount, XmlReader aReader);
        #endregion

        public _ColladaPrimitive(XmlReader aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kName, ref mName);
            _SetRequiredAttribute(aReader, Attributes.kCount, out mCount);
            _SetOptionalAttribute(aReader, Attributes.kMaterial, ref mMaterial);
            #endregion

            #region Children
            _NextElement(aReader);
            int inputCount = _AddZeroToManyChildren(aReader, Elements.kInputGroupB);
            _ProcessPrimitives(inputCount, aReader);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion 
  
            #region Validation
            {
                ColladaPrimitives colladaIndexBuffer = GetFirst<ColladaPrimitives>();
                uint offsetCount = OffsetCount;
                uint primitivesCount = colladaIndexBuffer.Count;
                if (offsetCount == 0u)
                {
                    throw new Exception("A COLLADA primitives element has no indices. This is not valid.");
                }

                if (primitivesCount % offsetCount != 0u)
                {
                    throw new Exception("A COLLADA primitives element has mismatched indices and data buffers.");
                }
            }
            #endregion
        }

        public uint Count
        {
            get
            {
                return mCount;
            }
        }

        public ColladaPrimitives Indices
        {
            get
            {
                return GetFirst<ColladaPrimitives>();
            }
        }

        public uint OffsetCount
        {
            get
            {
                uint ret = 0;
                foreach (ColladaInputGroupB e in GetEnumerable<ColladaInputGroupB>())
                {
                    ret = Math.Max(ret, e.Offset + 1u);
                }

                return ret;
            }
        }

        public string Material
        {
            get
            {
                return mMaterial;
            }
        }

        public _ColladaInput[] FindInputs(uint aOffset)
        {
            List<_ColladaInput> ret = new List<_ColladaInput>();

            foreach (ColladaInputGroupB b in GetEnumerable<ColladaInputGroupB>())
            {
                if (b.Semantic == Enums.InputSemantic.kVertex && b.Offset == aOffset)
                {
                    foreach (ColladaInputGroupA a in b.Source.GetEnumerable<ColladaInputGroupA>())
                    {
                        ret.Add(a);
                    }
                }
                else if (b.Offset == aOffset)
                {
                    ret.Add(b);
                }
            }

            return ret.ToArray();
        }

        public _ColladaInput FindInput(string aInputSemantic)
        {
            foreach (ColladaInputGroupB b in GetEnumerable<ColladaInputGroupB>())
            {
                if (aInputSemantic != Enums.InputSemantic.kVertex && b.Semantic == Enums.InputSemantic.kVertex)
                {
                    foreach (ColladaInputGroupA a in b.Source.GetEnumerable<ColladaInputGroupA>())
                    {
                        if (a.Semantic == aInputSemantic)
                        {
                            return a;
                        }
                    }
                }
                else if (b.Semantic == aInputSemantic)
                {
                    return b;
                }
            }

            throw new Exception("<input> with semantic \"" + aInputSemantic + "\" not found.");
        }

    }
}
