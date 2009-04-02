using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaMorph : _ColladaElement
    {
        #region Private members
        // Todo: determine what the type of this should be - may need to create a common subclass for mesh, convexmesh, and spline
        private _ColladaElement mSource;
        private readonly Enums.MorphMethodType mType = Defaults.kMorphMethodAttribute;
        #endregion

        public ColladaMorph(XmlReader aReader)
        {
            #region Attributes
            string source;
            _SetRequiredAttribute(aReader, Attributes.kSource, out source);
            ColladaDocument.QueueIdForResolution(source, delegate(_ColladaElement aResolvedSource) { mSource = aResolvedSource; });

            string method = default(string);
            if (_SetOptionalAttribute(aReader, Attributes.kMethod, ref method))
            {
                mType = Enums.GetMorphMethodType(method);
            }
            #endregion

            #region Children
            _NextElement(aReader);
            _AddRequiredChild(aReader, Elements.kSource);
            _AddOneToManyChildren(aReader, Elements.kSource);
            _AddRequiredChild(aReader, Elements.kTargets);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public _ColladaElement Source { get { return mSource; } }
        public Enums.MorphMethodType Type { get { return mType; } }
    }
}
