using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaArray<T> : _ColladaElementWithIdAndName
    {
        #region Protected members
        protected readonly T[] mArray;
        protected readonly uint mCount = 0;
        #endregion

        public _ColladaArray(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kCount, out mCount);
            mArray = new T[mCount];
            #endregion
        }

        public T[] Array { get { return mArray; }}
        public uint Count { get { return mCount; } }
        public T this[uint i] { get { return mArray[i]; } }

        public uint ElementCount
        {
            get
            {
                ColladaAccessor accessor = mParent.GetFirst<ColladaTechniqueCommonOfSource>()
                    .GetFirst<ColladaAccessor>();

                return accessor.Count;
            }
        }

        public Enums.ParamName[] Params
        {
            get
            {
                ColladaAccessor accessor = mParent.GetFirst<ColladaTechniqueCommonOfSource>()
                    .GetFirst<ColladaAccessor>();

                Enums.ParamName[] ret = new Enums.ParamName[accessor.GetChildCount<ColladaParam>()];

                int index = 0;
                foreach (ColladaParam p in accessor.GetEnumerable<ColladaParam>())
                {
                    ret[index++] = p.Name;
                }

                // Putting the check here because the relationship fo array/accessor elements
                // doesn't give me a better choice at the moment.
                if (ret.Length > Stride)
                {
                    throw new Exception("<*_array> has a stride that is lower than the number of <param> elements describing it.");
                }

                return ret;
            }
        }

        public uint Stride
        {
            get
            {
                uint ret = mParent.GetFirst<ColladaTechniqueCommonOfSource>()
                    .GetFirst<ColladaAccessor>().Stride;

                if (Count % ret != 0)
                {
                    throw new Exception("<*_array> has a count that is not evenly divisible by its stride.");
                }

                return ret;
            }
        }
                


    }
}
