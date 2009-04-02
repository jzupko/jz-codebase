using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaTechniqueHint : _ColladaElement
    {
        #region Private members
        private readonly string mPlatform = "";
        private readonly string mReference;
        private readonly Enums.Profile mProfile;
        #endregion

        public ColladaTechniqueHint(XmlReader aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kPlatform, ref mPlatform);
            _SetRequiredAttribute(aReader, Attributes.kReference, out mReference);

            #region profile
            {
                string profile = "";

                if (_SetOptionalAttribute(aReader, Attributes.kProfile, ref profile))
                {
                    switch (profile)
                    {
                        case Enums.TechniqueHintProfile.kCg: mProfile = Enums.Profile.Cg; break;
                        case Enums.TechniqueHintProfile.kCommon: mProfile = Enums.Profile.Common; break;
                        case Enums.TechniqueHintProfile.kGles: mProfile = Enums.Profile.Gles; break;
                        case Enums.TechniqueHintProfile.kGlsl: mProfile = Enums.Profile.Glsl; break;
                        default:
                            throw new Exception("invalid profile attribute value \"" + profile + "\" of <technique_hint>.");
                    }
                }
            }
            #endregion

            _NextElement(aReader);
            #endregion
        }

        public string Platform { get { return mPlatform; } }
        public string Reference { get { return mReference; } }
        public Enums.Profile Profile { get { return mProfile; } }
    }
}
