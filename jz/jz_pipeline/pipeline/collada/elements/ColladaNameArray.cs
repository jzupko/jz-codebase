using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    /// <summary>
    /// Encapsulates a COLLADA "Name_array" element.
    /// </summary>
    /// <remarks>
    /// Note that unlike most arrays, this uses type object. This is becaue a "Name_array" can either
    /// be a vanilla string array or it may be a "Name_array" of sids to resolve to elements (this is
    /// used in animation to refer to joints). The distinction is made by the enclosing context at the 
    /// source element level, so the member ColladaNameArray.ResolveToSids() is provided and is
    /// expected to be called by an enclosing member.
    /// </remarks>
    public sealed class ColladaNameArray : _ColladaArray<object>
    {
        #region Private members
        private bool mbSids = false;
        private void _ParseSidsHelper(int aIndex, string aSid)
        {
            ColladaDocument.QueueSidForResolution(aSid, delegate(_ColladaElement a) { mArray[aIndex] = a; });
        }

        private void _ParseSids()
        {
            for (int i = 0; i < mCount; i++)
            {
                _ParseSidsHelper(i, (string)mArray[i]);
            }

            mbSids = true;
        }
        #endregion

        public ColladaNameArray(XmlReader aReader)
            : base(aReader)
        {
            #region Element value
            string value = string.Empty;
            string[] sids = new string[mCount];

            _SetValue(aReader, ref value);
            Utilities.Tokenize(value, sids);

            System.Array.Copy(sids, mArray, mCount);
            #endregion
        }

        public void ResolveToSids() { if (!mbSids) { _ParseSids(); } }
    }
}
