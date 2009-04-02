using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    /// <summary>
    /// Encapsulates a COLLADA "lines" element.
    /// </summary>
    /// <remarks>
    /// The specification is unclear regarding the lines primitive definition. The "count" attribute
    /// is inconsistent across primitives such as "lines" or "linestrips". The validity check may
    /// fail because of this.
    /// </remarks>
    public sealed class ColladaLines : _ColladaPrimitive
    {
        #region Protected members
        protected override void _ProcessPrimitives(int aInputCount, XmlReader aReader)
        {
            if (mCount > 0)
            {
                if (aReader.Name != Elements.kPrimitives.Name || aInputCount == 0)
                {
                    throw new Exception("line count > 0 but input or primitive data is not correct for this.");
                }
                else
                {
                    // number of lines * 2 vertices per line * the number of offsets (one index entry for each offset).
                    uint size = mCount * 2u * OffsetCount;

                    Elements.Element e = new Elements.Element(Elements.kPrimitives.Name, delegate(XmlReader a) { return new ColladaPrimitives(a, size); });

                    _AddRequiredChild(aReader, e);
                }
            }
        }
        #endregion

        public ColladaLines(XmlReader aReader)
            : base(aReader)
        { }
    }
}
