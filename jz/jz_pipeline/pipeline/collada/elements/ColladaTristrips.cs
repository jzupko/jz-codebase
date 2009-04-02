using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTristrips : _ColladaPrimitive
    {
        #region Protected members
        protected override void _ProcessPrimitives(int aInputCount, XmlReader aReader)
        {
            int primitiveCount = _AddZeroToManyChildren(aReader, Elements.kPrimitives);

            // Note: in the context of Tristrips, mCount is apparently the number of primitives,
            //   not (apparently, the spec is vague) the number of segments like in Lines or the
            //   number of triangles like in Triangles
            if (primitiveCount != mCount)
            {
                throw new Exception("count is not equal to primitive count.");
            }
        }
        #endregion

        public ColladaTristrips(XmlReader aReader)
            : base(aReader)
        { }
    }
}
