using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaPolylist : _ColladaPrimitive
    {
        #region Protected members
        protected override void _ProcessPrimitives(int aInputCount, XmlReader aReader)
        {
            if (mCount > 0)
            {
                if (aReader.Name != Elements.kVcount.Name || aInputCount == 0)
                {
                    throw new Exception("polylist count > 0 but input or vcount data is not correct for this.");
                }
                else
                {
                    Elements.Element e = new Elements.Element(Elements.kVcount.Name, delegate(XmlReader a) { return new ColladaVcount(a, mCount); });
                    _AddRequiredChild(aReader, e);

                    uint expectedPsize = ((ColladaVcount)mLastChild).ExpectPrimitivesCount * (uint)aInputCount;
                    e = new Elements.Element(Elements.kPrimitives.Name, delegate(XmlReader a) { return new ColladaPrimitives(a, expectedPsize); });
                    _AddRequiredChild(aReader, e);
                }
            }
        }
        #endregion

        public ColladaPolylist(XmlReader aReader)
            : base(aReader)
        { }
    }
}
