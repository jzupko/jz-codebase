using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLinestrips : _ColladaPrimitive
    {
        #region Protected members
        protected override void _ProcessPrimitives(int aInputCount, XmlReader aReader)
        {
            int primitiveCount = _AddZeroToManyChildren(aReader, Elements.kPrimitives);

            // Note: in the context of LineStrips, mCount is apparently the number of primitives,
            //   not (apparently, the spec is vague) the number of segments like in Lines.
            if (primitiveCount != mCount)
            {
                throw new Exception("count is not equal to primitive count.");
            }
        }
        #endregion

        public ColladaLinestrips(XmlReader aReader)
            : base(aReader)
        { }
    }
}
