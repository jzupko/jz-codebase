using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaInput : _ColladaElement
    {
        #region Protected members
        protected uint mSet = 0u;
        protected readonly string mSemantic = "";
        protected _ColladaElement mSource = null;
        #endregion

        public const string kSemanticVertex = "VERTEX";
        public const string kSemanticNormal = "NORMAL";
        public const string kSemanticTexcoord = "TEXCOORD";
        public const string kSemanticTexcoord0 = "TEXCOORD0";
        public const string kSemanticTexcoord1 = "TEXCOORD1";
        public const string kSemanticTexcoord2 = "TEXCOORD2";
        public const string kSemanticTexcoord3 = "TEXCOORD3";
        public const string kSemanticPosition = "POSITION";

        public _ColladaInput(XmlReader aReader)
        {
            #region Attributes
            string source;
            _SetRequiredAttribute(aReader, Attributes.kSemantic, out mSemantic);
            _SetRequiredAttribute(aReader, Attributes.kSource, out source);
            ColladaDocument.QueueIdForResolution(source, delegate(_ColladaElement aResolvedElement) { mSource = aResolvedElement; });
            #endregion
        }

        public ColladaAccessor GetAccessor()
        {
            return mSource.GetFirst<ColladaTechniqueCommonOfSource>().GetFirst<ColladaAccessor>();
        }

        public _ColladaArray<T> GetArray<T>()
        {
            return mSource.GetFirst<_ColladaArray<T>>();
        }

        public bool HasSource
        {
            get
            {
                return (mSource is ColladaSource);
            }
        }

        public string Semantic
        {
            get
            {
                return mSemantic;
            }
        }

        public _ColladaElement Source
        {
            get
            {
                return mSource;
            }
        }

        public uint Set
        {
            get
            {
                return mSet;
            }
        }

        public uint Stride
        {
            get
            {
                return mSource.GetFirst<ColladaTechniqueCommonOfSource>().GetFirst<ColladaAccessor>().Stride;
            }
        }

    }
}
