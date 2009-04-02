using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaSkeleton : _ColladaElement
    {
        #region Private members
        private _ColladaElement mSkeleton;
        #endregion

        public ColladaSkeleton(XmlReader aReader)
        {
            #region Element values
            string skeleton = "";
            _SetValue(aReader, ref skeleton);
            ColladaDocument.QueueIdForResolution(skeleton, delegate(_ColladaElement a) { mSkeleton = a; });
            #endregion
        }

        public _ColladaElement Instance { get { return mSkeleton; } }
    }
}
