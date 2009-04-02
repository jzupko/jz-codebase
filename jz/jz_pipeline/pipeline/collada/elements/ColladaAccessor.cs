using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaAccessor : _ColladaElement
    {
        #region Private members
        private readonly uint mCount = 0;
        private readonly uint mOffset = Defaults.kOffsetDefault;
        private _ColladaElement mSource = null;
        private readonly uint mStride = Defaults.kStrideDefault;
        #endregion

        public ColladaAccessor(XmlReader aReader)
        {
            #region Attributes
            _SetRequiredAttribute(aReader, Attributes.kCount, out mCount);
            _SetOptionalAttribute(aReader, Attributes.kOffset, ref mOffset);
            #region source
            {
                string source;
                _SetRequiredAttribute(aReader, Attributes.kSource, out source);
                ColladaDocument.QueueIdForResolution(source, delegate(_ColladaElement a) { mSource = a; });
            }
            #endregion
            _SetOptionalAttribute(aReader, Attributes.kStride, ref mStride);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddZeroToManyChildren(aReader, Elements.kParam);
            #endregion
        }

        public uint Count { get { return mCount; } }
        public uint Offset { get { return mOffset; } }
        public _ColladaElement Source { get { return mSource; } }
        public uint Stride { get { return mStride; } }
    }
}
