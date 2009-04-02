using EffectsBySymbol = System.Collections.Generic.Dictionary<string, jz.pipeline.JzEffectContent>;
using MaterialsBySymbol = System.Collections.Generic.Dictionary<string, jz.pipeline.JzMaterialContent>;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using Microsoft.Xna.Framework.Content.Pipeline.Serialization.Compiler;
using Microsoft.Xna.Framework.Graphics;

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Xml;

using jz.pipeline.collada.elements;
using jz.pipeline.collada.elements.fx;


namespace jz.pipeline.collada
{

    public sealed class ColladaProcessorMaterial
    {
        #region Constants
        public const string kDxt5Flag = "bDxt5";
        public const string kColorKeyColorParameter = "ColorKeyColor";
        public const string kColorKeyEnabledParameter = "ColorKeyEnabled";
        public const string kGenerateMipmapsParameter = "GenerateMipmaps";
        public const string kResizeToPowerOfTwoParameter = "ResizeToPowerOfTwo";
        public const string kTextureFormatParameter = "TextureFormat";

        public const string kStandardEffectFile = "..\\..\\media\\material.fxh";

        public const string kColorPostfix = "_COLOR";
        public const string kTexturePostfix = "_TEXTURE";
        public const string kTexcoordsPostfix = "_TEXCOORDS";
        public const string kAddressUPostfix = "_ADDRESSU";
        public const string kAddressVPostfix = "_ADDRESSV";
        public const string kAddressWPostfix = "_ADDRESSW";
        public const string kMinFilterPostfix = "_MIN_FILTER";
        public const string kMagFilterPostfix = "_MAG_FILTER";
        public const string kMipFilterPostfix = "_MIP_FILTER";
        public const string kBorderColorPostfix = "_BORDER_COLOR";
        public const string kMaxMipLevelPostfix = "_MAX_MIP_LEVEL";
        public const string kMipMapLodBias = "_MIP_MAP_LOD_BIAS";

        public const string kAmbientPrefix = "AMBIENT";
        public const string kBumpPrefix = "BUMP";
        public const string kDiffusePrefix = "DIFFUSE";
        public const string kEmissionPrefix = "EMISSION";
        public const string kReflectivePrefix = "REFLECTIVE";
        public const string kReflectivity = "REFLECTIVITY";
        public const string kShininess = "SHININESS";
        public const string kSpecularPrefix = "SPECULAR";
        public const string kTransparency = "TRANSPARENCY";
        public const string kTransparentPrefix = "TRANSPARENT";

        public const string kAlphaOne = "ALPHA_ONE";
        public const string kRgbZero = "RGB_ZERO";

        public const string kAnimated = "ANIMATED";
        public const string kBlinn = "BLINN";
        public const string kPhong = "PHONG";

        public const string kTexcoordsInput = "Texcoords";
        public const string kTexcoordsChannelCount = "TEXCOORDS_COUNT";

        public const string kAmbientSemanticPrefix = "jz_Ambient";
        public const string kBumpSemanticPrefix = "jz_Bump";
        public const string kColorSemanticPostfix = "Color";
        public const string kDiffuseSemanticPrefix = "jz_Diffuse";
        public const string kEmissionSemanticPrefix = "jz_Emission";
        public const string kReflectiveSemanticPrefix = "jz_Reflective";
        public const string kReflectivitySemantic = "jz_Reflectivity";
        public const string kShininessSemantic = "jz_Shininess";
        public const string kSpecularSemanticPrefix = "jz_Specular";
        public const string kTextureSemanticPostfix = "Texture";
        public const string kTransparentSemanticPrefix = "jz_Transparent";
        public const string kTransparencySemantic = "jz_Transparency";

        public const float kZeroColorTolerance = 0.05f;
        #endregion

        #region Private members
        private static readonly OpaqueDataDictionary mskTextureBuildParameters = new OpaqueDataDictionary();

        static ColladaProcessorMaterial()
        {
            mskTextureBuildParameters.Add(kColorKeyEnabledParameter, false);
            mskTextureBuildParameters.Add(kGenerateMipmapsParameter, true);
            mskTextureBuildParameters.Add(kResizeToPowerOfTwoParameter, false);
            mskTextureBuildParameters.Add(kTextureFormatParameter, TextureProcessorOutputFormat.DxtCompressed);
        }

        private uint mTotalTexcoordChannels = 0;

        private Dictionary<string, JzEffectContent> mEffects = new Dictionary<string, JzEffectContent>();
        private Dictionary<string, JzMaterialContent> mMaterials = new Dictionary<string, JzMaterialContent>();
        private Dictionary<string, ExternalReference<TextureContent>> mTextures = new Dictionary<string, ExternalReference<TextureContent>>();

        private ContentIdentity mIdentity = null;

        private _ColladaElement.Enums.SamplerFilter mMinFilterWhenNone = _ColladaElement.Enums.SamplerFilter.LinearMipmapLinear;
        private _ColladaElement.Enums.SamplerFilter mMagFilterWhenNone = _ColladaElement.Enums.SamplerFilter.Linear;
        private _ColladaElement.Enums.SamplerFilter mMipFilterWhenNone = _ColladaElement.Enums.SamplerFilter.Linear;

        private bool _ProcessColor(_ColladaElement aColor, string aPrefix, List<CompilerMacro> aMacros, string aSemanticPrefix, JzMaterialContent aMaterial)
        {
            if (aColor is ColladaColor)
            {
                ColladaColor color = (ColladaColor)aColor;
                Vector3 rgb = color.ColorRGB;
                Vector4 rgba = color.ColorRGBA;

                if (!Utilities.AboutZero(ref rgb, kZeroColorTolerance))
                {
                    aMacros.Add(Utilities.CreateMacro(aPrefix + kColorPostfix, aSemanticPrefix + kColorSemanticPostfix));
                    aMaterial.Parameters.Add(new JzMaterialContent.Parameter(aSemanticPrefix + kColorSemanticPostfix, ParameterType.kVector4, rgba));
                    return true;
                }
            }

            return false;
        }

        private void _ProcessHLSLEffect(ColladaMaterial aMaterial, BoundEffect aBoundEffect, out JzEffectContent arEffect, out JzMaterialContent arMaterial)
        {
            #region Effect
            ColladaEffect colladaEffect = aMaterial.Effect;

            CompiledEffect compiledEffect = Effect.CompileEffectFromFile(
                colladaEffect.EffectHLSLFilename, null, null,
                CompilerOptions.None, TargetPlatform.Windows);

            if (!compiledEffect.Success)
            {
                throw new Exception("Failed compiling HLSL effect \"" +
                    colladaEffect.EffectHLSLFilename + "\"." + Environment.NewLine +
                    compiledEffect.ErrorsAndWarnings);
            }
            arEffect = new JzEffectContent(compiledEffect);
            arEffect.Id = colladaEffect.EffectHLSLFilename;
            #endregion

            #region Material
            ColladaSetparamOfInstanceEffect[] prms = aMaterial.Params;
            arMaterial = new JzMaterialContent();

            foreach (ColladaSetparamOfInstanceEffect p in prms)
            {
                ParameterType type;

                switch (p.Type)
                {
                    case _ColladaElement.Enums.CoreValueType.kFloat: type = ParameterType.kSingle; break;
                    case _ColladaElement.Enums.CoreValueType.kFloat4x4: type = ParameterType.kMatrix; break;
                    case _ColladaElement.Enums.CoreValueType.kSurface: type = ParameterType.kTexture; break;
                    case _ColladaElement.Enums.CoreValueType.kFloat2: type = ParameterType.kVector2; break;
                    case _ColladaElement.Enums.CoreValueType.kFloat3: type = ParameterType.kVector3; break;
                    case _ColladaElement.Enums.CoreValueType.kFloat4: type = ParameterType.kVector4; break;
                    default:
                        throw new Exception("Material parameter type \"" +
                            Enum.GetName(typeof(_ColladaElement.Enums.CoreValueType), p.Type) +
                            "\" is not yet supported.");
                }

                if (type == ParameterType.kTexture)
                {
                    ColladaImage image = p.GetValue<ColladaSurfaceOfProfileCOMMON>().Image;
                    if (image == null || !image.IsLocation || image.Location == string.Empty)
                    {
                        throw new Exception("<image> of texture as defined is not supported.");
                    }

                    ExternalReference<TextureContent> textureReference = _BuildTexture(image, false);

                    arMaterial.Parameters.Add(new JzMaterialContent.Parameter(p.Reference,
                        type, textureReference));
                }
                else
                {
                    arMaterial.Parameters.Add(new JzMaterialContent.Parameter(p.Reference,
                        type, p.Value));
                }
            }
            #endregion
        }

        private void _ProcessSamplerSettings(ColladaSamplerFX aSampler, string aPrefix, List<CompilerMacro> aMacros)
        {
            aMacros.Add(Utilities.CreateMacro(aPrefix + kAddressUPostfix, ColladaDocument.ColladaSurfaceWrapToHlsl(aSampler.WrapS)));
            aMacros.Add(Utilities.CreateMacro(aPrefix + kAddressVPostfix, ColladaDocument.ColladaSurfaceWrapToHlsl(aSampler.WrapT)));
            aMacros.Add(Utilities.CreateMacro(aPrefix + kAddressWPostfix, ColladaDocument.ColladaSurfaceWrapToHlsl(aSampler.WrapP)));
            aMacros.Add(Utilities.CreateMacro(aPrefix + kMinFilterPostfix, ColladaDocument.ColladaSurfaceFilterToHlsl(aSampler.Minfilter == _ColladaElement.Enums.SamplerFilter.None ? mMinFilterWhenNone : aSampler.Minfilter)));
            aMacros.Add(Utilities.CreateMacro(aPrefix + kMagFilterPostfix, ColladaDocument.ColladaSurfaceFilterToHlsl(aSampler.Magfilter == _ColladaElement.Enums.SamplerFilter.None ? mMagFilterWhenNone : aSampler.Magfilter)));
            aMacros.Add(Utilities.CreateMacro(aPrefix + kMipFilterPostfix, ColladaDocument.ColladaSurfaceFilterToHlsl(aSampler.Mipfilter == _ColladaElement.Enums.SamplerFilter.None ? mMipFilterWhenNone : aSampler.Mipfilter)));
            int borderColor = (aSampler.BorderColor > 0.0f) ? int.MaxValue : 0;
            aMacros.Add(Utilities.CreateMacro(aPrefix + kBorderColorPostfix, borderColor.ToString()));
            aMacros.Add(Utilities.CreateMacro(aPrefix + kMaxMipLevelPostfix, aSampler.MipmapMaxlevel.ToString()));
            aMacros.Add(Utilities.CreateMacro(aPrefix + kMipMapLodBias, aSampler.MipmapBias.ToString()));
        }

        private void _ProcessProfileCOMMONEffect(ColladaMaterial aMaterial, BoundEffect aBoundEffect, out JzEffectContent arEffect, out JzMaterialContent arMaterial, bool abAnimated)
        {
            List<CompilerMacro> macros = new List<CompilerMacro>();
            mTotalTexcoordChannels = 0;
            collada.elements.fx.ColladaEffectOfProfileCOMMON effect = aMaterial.Effect.EffectCOMMON;

            JzMaterialContent retMaterial = new JzMaterialContent();

            #region Bump map
            // can only be a texture.
            _ProcessTexture(aBoundEffect, effect.Bump, kBumpPrefix, macros, kBumpSemanticPrefix, retMaterial, true);
            #endregion

            #region Emission
            if (!_ProcessTexture(aBoundEffect, effect.Emission, kEmissionPrefix, macros, kEmissionSemanticPrefix, retMaterial, false))
            {
                _ProcessColor(effect.Emission, kEmissionPrefix, macros, kEmissionSemanticPrefix, retMaterial);
            }
            #endregion

            #region Reflectivity
            if (!Utilities.AboutZero(effect.Reflectivity, kZeroColorTolerance) &&
                !Utilities.AboutEqual(effect.Reflectivity, 1.0f, kZeroColorTolerance))
            {
                bool bReflectivity = true;

                if (!_ProcessTexture(aBoundEffect, effect.Reflective, kReflectivePrefix, macros, kReflectiveSemanticPrefix, retMaterial, false))
                {
                    if (!_ProcessColor(effect.Reflective, kReflectivePrefix, macros, kReflectiveSemanticPrefix, retMaterial))
                    {
                        bReflectivity = false;
                    }
                }

                if (bReflectivity)
                {
                    macros.Add(Utilities.CreateMacro(kReflectivity, kReflectivitySemantic));
                    retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kReflectivitySemantic, ParameterType.kSingle, effect.Reflectivity));
                }
            }
            #endregion

            #region Transparency
            if (effect.TransparencyType == TransparencyTypes.AlphaOne)
            {
                if (!_ProcessTexture(aBoundEffect, effect.Transparent, kTransparentPrefix, macros, kTransparentSemanticPrefix, retMaterial, true))
                {
                    Vector4 color = ((ColladaColor)effect.Transparent).ColorRGBA;
                    float transparency = effect.Transparency;
                    float alpha = transparency * color.W;

                    if (!Utilities.AboutEqual(alpha, 1.0f, kZeroColorTolerance) &&
                        !Utilities.AboutZero(alpha, kZeroColorTolerance))
                    {
                        macros.Add(Utilities.CreateMacro(kTransparentPrefix + kColorPostfix, kTransparentSemanticPrefix + kColorSemanticPostfix));
                        retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kTransparentSemanticPrefix + kColorSemanticPostfix, ParameterType.kVector4, color));
                        macros.Add(Utilities.CreateMacro(kTransparency, kTransparencySemantic));
                        retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kTransparencySemantic, ParameterType.kSingle, effect.Transparency));
                        macros.Add(Utilities.CreateMacro(kAlphaOne, "1"));
                    }
                }
                else
                {
                    macros.Add(Utilities.CreateMacro(kTransparency, kTransparencySemantic));
                    retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kTransparencySemantic, ParameterType.kSingle, effect.Transparency));
                    macros.Add(Utilities.CreateMacro(kAlphaOne, "1"));
                }
            }
            else
            {
                if (!_ProcessTexture(aBoundEffect, effect.Transparent, kTransparentPrefix, macros, kTransparentSemanticPrefix, retMaterial, true))
                {
                    Vector4 color = ((ColladaColor)effect.Transparent).ColorRGBA;
                    float luminance = Utilities.Luminance(color);
                    float transparency = effect.Transparency;

                    if (!Utilities.AboutZero(luminance, kZeroColorTolerance) &&
                        !Utilities.AboutZero(transparency, kZeroColorTolerance))
                    {
                        macros.Add(Utilities.CreateMacro(kTransparentPrefix + kColorPostfix, kTransparentSemanticPrefix + kColorSemanticPostfix));
                        retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kTransparentSemanticPrefix + kColorSemanticPostfix, ParameterType.kVector4, color));
                        macros.Add(Utilities.CreateMacro(kTransparency, kTransparencySemantic));
                        retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kTransparencySemantic, ParameterType.kSingle, effect.Transparency));
                        macros.Add(Utilities.CreateMacro(kRgbZero, "1"));
                    }
                }
                else
                {
                    macros.Add(Utilities.CreateMacro(kTransparency, kTransparencySemantic));
                    retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kTransparencySemantic, ParameterType.kSingle, effect.Transparency));
                    macros.Add(Utilities.CreateMacro(kRgbZero, "1"));
                }
            }
            #endregion

            if (effect.Type == _ColladaElement.Enums.EffectType.kLambert ||
                effect.Type == _ColladaElement.Enums.EffectType.kPhong ||
                effect.Type == _ColladaElement.Enums.EffectType.kBlinn)
            {
                #region Ambient and Diffuse
                if (!_ProcessTexture(aBoundEffect, effect.Ambient, kAmbientPrefix, macros, kAmbientSemanticPrefix, retMaterial, false))
                {
                    _ProcessColor(effect.Ambient, kAmbientPrefix, macros, kAmbientSemanticPrefix, retMaterial);
                }
                if (!_ProcessTexture(aBoundEffect, effect.Diffuse, kDiffusePrefix, macros, kDiffuseSemanticPrefix, retMaterial, false))
                {
                    _ProcessColor(effect.Diffuse, kDiffusePrefix, macros, kDiffuseSemanticPrefix, retMaterial);
                }
                #endregion

                if (effect.Type == _ColladaElement.Enums.EffectType.kPhong ||
                    effect.Type == _ColladaElement.Enums.EffectType.kBlinn)
                {
                    #region Specular
                    if (!(effect.Shininess < 1.0f))
                    {
                        bool bShininess = true;

                        if (!_ProcessTexture(aBoundEffect, effect.Specular, kSpecularPrefix, macros, kSpecularSemanticPrefix, retMaterial, false))
                        {
                            if (!_ProcessColor(effect.Specular, kSpecularPrefix, macros, kSpecularSemanticPrefix, retMaterial))
                            {
                                bShininess = false;
                            }
                        }

                        if (bShininess)
                        {
                            macros.Add(Utilities.CreateMacro(kShininess, kShininessSemantic));
                            retMaterial.Parameters.Add(new JzMaterialContent.Parameter(kShininessSemantic, ParameterType.kSingle, effect.Shininess));
                        }
                    }
                    #endregion

                    #region Identify Blinn or Phong effect for the shader.
                    if (effect.Type == _ColladaElement.Enums.EffectType.kPhong)
                    {
                        macros.Add(Utilities.CreateMacro(kPhong, "1"));
                    }
                    else
                    {
                        macros.Add(Utilities.CreateMacro(kBlinn, "1"));
                    }
                    #endregion
                }
            }
            macros.Add(Utilities.CreateMacro(kTexcoordsChannelCount, mTotalTexcoordChannels.ToString()));

            if (abAnimated)
            {
                macros.Add(Utilities.CreateMacro(kAnimated, "1"));
            }

            #region Generate hash and check for existing
            string effectId = string.Empty;
            foreach (CompilerMacro m in macros) { effectId += (m.Name + m.Definition); }

            if (mEffects.ContainsKey(effectId))
            {
                arEffect = mEffects[effectId];
            }
            else
            {
                CompiledEffect compiledEffect = Effect.CompileEffectFromFile(kStandardEffectFile, macros.ToArray(), null, CompilerOptions.None, TargetPlatform.Windows);
                if (!compiledEffect.Success)
                {
                    throw new Exception("Error: standard effect building failed, \"" + compiledEffect.ErrorsAndWarnings + "\"");
                }

                arEffect = new JzEffectContent(compiledEffect);
                arEffect.Id = Utilities.ExtractBaseFilename(kStandardEffectFile) + "_" + (effectId.GetHashCode().ToString()) + ".fx";
                arEffect.Macros = macros;
                mEffects[effectId] = arEffect;
            }
            #endregion

            string materialId = retMaterial.Id;
            if (mMaterials.ContainsKey(materialId))
            {
                arMaterial = mMaterials[materialId];
            }
            else
            {
                mMaterials[materialId] = retMaterial;
                arMaterial = retMaterial;
            }
        }

        private ExternalReference<TextureContent> _BuildTexture(ColladaImage aColladaImage, bool abDxt5)
        {
            string assetName = Utilities.RemoveExtension(aColladaImage.Location);
            string textureLocation = aColladaImage.Location;
            ExternalReference<TextureContent> textureReference = null;

            if (!mTextures.TryGetValue(textureLocation, out textureReference))
            {
                textureReference = new ExternalReference<TextureContent>(textureLocation, mIdentity);
                textureReference.OpaqueData.Add(kDxt5Flag, abDxt5);
                mTextures[textureLocation] = textureReference;
            }
            else
            {
                bool bExistingFlag = (bool)(textureReference.OpaqueData[kDxt5Flag]);
                if (!bExistingFlag && abDxt5)
                {
                    textureReference.OpaqueData.Remove(kDxt5Flag);
                    textureReference.OpaqueData.Add(kDxt5Flag, abDxt5);
                }
            }

            return textureReference;
        }

        private bool _ProcessTexture(BoundEffect aBoundEffect, _ColladaElement aElement, string aPrefix, List<CompilerMacro> aMacros, string aSemanticPrefix, JzMaterialContent aMaterial, bool abDxt5)
        {
            if (aElement is _ColladaTexture)
            {
                _ColladaTexture texture = (_ColladaTexture)aElement;
                ColladaImage image = texture.Image;
                if (!image.IsLocation || image.Location == string.Empty)
                {
                    throw new Exception("<image> of texture as defined is not supported.");
                }
                uint texCoordsIndex = 0u;
                if (texture.Texcoords != string.Empty)
                {
                    if (!aBoundEffect.FindUsageIndex(texture.Texcoords, ref texCoordsIndex))
                    {
                        throw new Exception("A usage index for texture coordinates \"" + texture.Texcoords +
                            "\" could not be found.");
                    }
                }
                mTotalTexcoordChannels = Utilities.Max(mTotalTexcoordChannels, (texCoordsIndex + 1u));

                ExternalReference<TextureContent> textureReference = _BuildTexture(image, abDxt5);

                aMacros.Add(Utilities.CreateMacro(aPrefix + kTexcoordsPostfix, kTexcoordsInput + texCoordsIndex.ToString()));
                aMacros.Add(Utilities.CreateMacro(aPrefix + kTexturePostfix, aSemanticPrefix + kTextureSemanticPostfix));
                aMaterial.Parameters.Add(new JzMaterialContent.Parameter(aSemanticPrefix + kTextureSemanticPostfix, ParameterType.kTexture, textureReference));

                _ProcessSamplerSettings(texture.Sampler, aPrefix, aMacros);

                return true;
            }

            return false;
        }
        #endregion

        public ColladaProcessorMaterial(ContentIdentity aIdentity)
        {
            mIdentity = aIdentity;
        }

        public void GetMaterials(ColladaBindMaterial aBindMaterial, ref EffectsBySymbol arEffects, ref MaterialsBySymbol arMaterials, bool abAnimated)
        {
            ColladaTechniqueCommonOfBindMaterial techniqueCommon = aBindMaterial.GetFirst<ColladaTechniqueCommonOfBindMaterial>();

            foreach (ColladaInstanceMaterial instanceMaterial in techniqueCommon.GetEnumerable<ColladaInstanceMaterial>())
            {
                BoundEffect boundEffect = new BoundEffect(instanceMaterial);

                JzEffectContent effect = null;
                JzMaterialContent material = null;

                if (instanceMaterial.Instance.Effect.HasEffectHLSL)
                {
                    _ProcessHLSLEffect(instanceMaterial.Instance, boundEffect, out effect, out material);
                }
                else
                {
                    _ProcessProfileCOMMONEffect(instanceMaterial.Instance, boundEffect, out effect, out material, abAnimated);
                }

                arEffects[instanceMaterial.Symbol] = effect;
                arMaterials[instanceMaterial.Symbol] = material;
            }
        }

        public _ColladaElement.Enums.SamplerFilter MagFilterWhenNone { get { return mMagFilterWhenNone; } set { mMagFilterWhenNone = value; } }
        public _ColladaElement.Enums.SamplerFilter MinFilterWhenNone { get { return mMinFilterWhenNone; } set { mMinFilterWhenNone = value; } }
        public _ColladaElement.Enums.SamplerFilter MipFilterWhenNone { get { return mMipFilterWhenNone; } set { mMipFilterWhenNone = value; } }
    }

}
