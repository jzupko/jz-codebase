using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTriangles : _ColladaPrimitive
    {
        #region Protected members
        protected override void _ProcessPrimitives(int aInputCount, XmlReader aReader)
        {
            if (mCount > 0)
            {
                if (aReader.Name != Elements.kPrimitives.Name || aInputCount == 0)
                {
                    throw new Exception("triangle count > 0 but input or primitive data is not correct for this.");
                }
                else
                {
                    // number of triangles * 3 vertices per triangle * the number of offsets (one index entry for each offset).
                    uint size = mCount * 3u * OffsetCount;

                    Elements.Element e = new Elements.Element(Elements.kPrimitives.Name, delegate(XmlReader a) { return new ColladaPrimitives(a, size); });
                    _AddRequiredChild(aReader, e);
                }
            }
        }
        #endregion

        public ColladaTriangles(XmlReader aReader)
            : base(aReader)
        { }
    }
}
