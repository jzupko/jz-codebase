using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaEffect : _ColladaElementWithIdAndName
    {
        #region Private members
        string mEffectHLSLFilename = string.Empty;
        #endregion

        public static readonly string[] kHlslProfileValues = new string[] { "d3dfx", "fx" };

        public ColladaEffect(XmlReader aReader)
            : base(aReader)
        {
            if (mId == string.Empty)
            {
                throw new Exception("<effect> requires an id.");
            }

            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddZeroToManyChildren(aReader, Elements.FX.kAnnotate);
            _AddZeroToManyChildren(aReader, Elements.kImage);
            _AddZeroToManyChildren(aReader, Elements.FX.kNewparamOfProfileGLESandEffect);
            
            {
                int profileCount = 0;
                profileCount += _AddZeroToManyChildren(aReader, Elements.FX.kProfileCG);
                profileCount += _AddZeroToManyChildren(aReader, Elements.FX.kProfileGLSL);
                profileCount += _AddZeroToManyChildren(aReader, Elements.FX.kProfileCOMMON);
                if (profileCount == 0)
                {
                    throw new Exception("<effect> requires at least one profile child element.");
                }
            }

            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion

            #region HLSL Effect
            foreach (ColladaExtra extra in GetEnumerable<ColladaExtra>())
            {
                foreach (ColladaTechnique technique in extra.GetEnumerable<ColladaTechnique>())
                {
                    foreach (_ColladaGenericElement generic in technique.GetEnumerable<_ColladaGenericElement>())
                    {
                        if (generic.GetContains(Attributes.kProfile))
                        {
                            if (Array.Exists(kHlslProfileValues, delegate(string s) { return (s == generic[Attributes.kProfile]); }))
                            {
                                mEffectHLSLFilename = Utilities.FromUriFileToPath(ColladaDocument.CurrentBase, generic[Attributes.kUrl]);
                                goto done;
                            }
                        }
                    }
                }
            }
            #endregion

        done:
            return;
        }

        public bool HasProfileCg { get { return (GetFirstOptional<fx.ColladaProfileCG>() != null); } }
        public bool HasProfileGLSL { get { return (GetFirstOptional<fx.ColladaProfileGLSL>() != null); } }
        public bool HasProfileCOMMON { get { return (GetFirstOptional<fx.ColladaProfileCOMMON>() != null); } }

        /// <summary>
        /// Returns the filanem of an HLSL effect file if this COLLADA effect has one.
        /// </summary>
        public string EffectHLSLFilename { get { return mEffectHLSLFilename; } }

        /// <summary>
        /// Whether or not this Effect contains an HLSL effect file.
        /// </summary>
        /// <remarks>
        /// HLSL is not one of the shader languages supported in COLLADA so vendors have added
        /// an extension using "extra" elements that allows for attaching an external HLSL effect (.fx)
        /// file to a COLLADA effect.
        /// </remarks>
        public bool HasEffectHLSL { get { return (mEffectHLSLFilename != string.Empty); } }

        public ColladaEffectOfProfileCOMMON EffectCOMMON
        {
            get
            {
                return GetFirst<ColladaProfileCOMMON>().GetFirst<ColladaTechniqueFXOfProfileCOMMON>().GetFirst<ColladaEffectOfProfileCOMMON>();
            }
        }

    }
}
