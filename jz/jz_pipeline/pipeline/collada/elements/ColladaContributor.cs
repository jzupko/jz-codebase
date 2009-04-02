using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaContributor : _ColladaElement
    {
        #region Private members
        private readonly string mAuthor = "";
        private readonly string mAuthoringTool = "";
        private readonly string mComments = "";
        private readonly string mCopyright = "";
        private readonly string mSourceData = "";
        #endregion

        public ColladaContributor(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _SetValueOptional(aReader, Elements.kAuthor.Name, ref mAuthor);
            _SetValueOptional(aReader, Elements.kAuthoringTool.Name, ref mAuthoringTool);
            _SetValueOptional(aReader, Elements.kComments.Name, ref mComments);
            _SetValueOptional(aReader, Elements.kCopyright.Name, ref mCopyright);
            _SetValueOptional(aReader, Elements.kSourceData.Name, ref mSourceData);
            #endregion                
        }

        public string Author { get { return mAuthor; } }
        public string AuthoringTool { get { return mAuthoringTool; } }
        public string Comments { get { return mComments; } }
        public string Copyright { get { return mCopyright; } }
        public string SourceData { get { return mSourceData; } }
    }
}
