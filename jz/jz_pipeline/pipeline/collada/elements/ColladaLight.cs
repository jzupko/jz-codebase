using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaLight : _ColladaElementWithIdAndName
    {
        #region Private members
        private readonly float mMayaIntensity = 1.0f;
        private readonly float mMayaDropoff = 0.0f;
        #endregion

        public const string kFColladaExtensions = "FCOLLADA";
        public const string kMayaLightIntensityAttribute = "intensity";
        public const string kMayaLightDropoffAttribute = "dropoff";

        public ColladaLight(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddRequiredChild(aReader, Elements.kTechniqueCommonOfLight);
            _AddZeroToManyChildren(aReader, Elements.kTechnique);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion

            #region Maya extra attributes
            ColladaExtra extra = GetFirstOptional<ColladaExtra>();
            if (extra != null)
            {
                ColladaTechnique technique = extra.GetFirst<ColladaTechnique>();
                if (technique.Profile == kFColladaExtensions)
                {
                    foreach (_ColladaElement e in technique.GetEnumerable<_ColladaElement>())
                    {
                        _ColladaGenericElement element = (_ColladaGenericElement)e;

                        if (element.Name == kMayaLightIntensityAttribute)
                        {
                            mMayaIntensity = float.Parse(element.Value);
                        }
                        else if (element.Name == kMayaLightDropoffAttribute)
                        {
                            mMayaDropoff = float.Parse(element.Value);
                        }
                    }
                }
            }
            #endregion
        }

        public float MayaDropoff { get { return mMayaDropoff; } }
        public float MayaIntensity { get { return mMayaIntensity; } }

        public ColladaLightData LightData
        {
            get
            {
                return GetFirst<ColladaTechniqueCommonOfLight>().GetFirst<ColladaLightData>();
            }
        }
    }
}
