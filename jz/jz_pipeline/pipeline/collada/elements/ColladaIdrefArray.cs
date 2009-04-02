using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaIdrefArray : _ColladaArray<object>
    {
        #region Private members
        private void _ParseIdsHelper(int aIndex, string aId)
        {
            ColladaDocument.QueueIdForResolution(Settings.kFragmentDelimiter + aId, delegate(_ColladaElement a) { mArray[aIndex] = a; });
        }

        private void _ParseIds(string[] aIds)
        {
            for (int i = 0; i < mCount; i++)
            {
                string id = aIds[i];
                _ParseIdsHelper(i, id);
            }
        }
        #endregion

        public ColladaIdrefArray(XmlReader aReader)
            : base(aReader)
        {
            #region Element value
            string value = string.Empty;
            string[] ids = new string[mCount];

            _SetValue(aReader, ref value);
            Utilities.Tokenize(value, ids);
            _ParseIds(ids);
            #endregion
        }
    }
}
