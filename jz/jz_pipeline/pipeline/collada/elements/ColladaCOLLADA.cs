using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaCOLLADA : _ColladaElement
    {
        #region Private members
        private readonly string mSchema;
        private readonly string mSourceFile = "";
        #endregion

        public const string kLibrary = "library_";

        public ColladaCOLLADA(XmlReader aReader, string aSourceFile)
        {
            mSourceFile = aSourceFile;

            #region Attributes

            #region Version
            string colladaVersion;
            _SetRequiredAttribute(aReader, Attributes.kVersion, out colladaVersion);
            if (Array.IndexOf<string>(kColladaSupportedVersions, colladaVersion) < 0)
            {
                throw new Exception("Unsupported COLLADA version.");
            }
            #endregion

            _SetRequiredAttribute(aReader, Attributes.kSchema, out mSchema);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddRequiredChild(aReader, Elements.kAsset);

            while (aReader.Name.StartsWith(kLibrary))
            {
                _AddOptionalChild(aReader, Elements.kLibraryAnimations);
                _AddOptionalChild(aReader, Elements.kLibraryAnimationClips);
                _AddOptionalChild(aReader, Elements.Physics.kLibraryPhysicsMaterials);
                _AddOptionalChild(aReader, Elements.Physics.kLibraryPhysicsModels);
                _AddOptionalChild(aReader, Elements.Physics.kLibraryPhysicsScenes);
                _AddOptionalChild(aReader, Elements.Physics.kLibraryForceFields);
                _AddOptionalChild(aReader, Elements.kLibraryCameras);
                _AddOptionalChild(aReader, Elements.kLibraryLights);
                _AddOptionalChild(aReader, Elements.kLibraryImages);
                _AddOptionalChild(aReader, Elements.FX.kLibraryMaterials);
                _AddOptionalChild(aReader, Elements.FX.kLibraryEffects);
                _AddOptionalChild(aReader, Elements.kLibraryGeometries);
                _AddOptionalChild(aReader, Elements.kLibraryControllers);
                _AddOptionalChild(aReader, Elements.kLibraryVisualScenes);
                _AddOptionalChild(aReader, Elements.kLibraryNodes);
            }
            _AddOptionalChild(aReader, Elements.kScene);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public string SourceFile { get { return mSourceFile; } }
    }
}
