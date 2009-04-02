using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaSamplerFX : _ColladaElement
    {
        #region Private members
        private Enums.SamplerType mType;
        private _ColladaElement mSource;
        private readonly Enums.SamplerWrap mWrapS = Defaults.kWrapS;
        private readonly Enums.SamplerWrap mWrapT = Defaults.kWrapT;
        private readonly Enums.SamplerWrap mWrapP = Defaults.kWrapP;
        private readonly Enums.SamplerFilter mMinfilter = Defaults.kMinfilter;
        private readonly Enums.SamplerFilter mMagfilter = Defaults.kMagfilter;
        private readonly Enums.SamplerFilter mMipfilter = Defaults.kMipfilter;
        private readonly float mBorderColor = Defaults.kBorderColor;
        private readonly uint mMipmapMaxlevel = Defaults.kMipmapMaxlevel;
        private readonly float mMipmapBias = Defaults.kMipmapBias;
        #endregion

        public ColladaSamplerFX(XmlReader aReader, Enums.SamplerType aType)
        {
            mType = aType;

            #region Element values
            _NextElement(aReader);

            string source;
            _SetValueRequired(aReader, Elements.kSource.Name, out source);
            ColladaDocument.QueueSidForResolution(source, delegate(_ColladaElement a) { mSource = a; });

            _SetValueOptional<Enums.SamplerWrap>(aReader, Elements.FX.kWrapS.Name, ref mWrapS, _ColladaElement.Enums.SamplerWrapFromString);
            _SetValueOptional<Enums.SamplerWrap>(aReader, Elements.FX.kWrapT.Name, ref mWrapT, _ColladaElement.Enums.SamplerWrapFromString);
            _SetValueOptional<Enums.SamplerWrap>(aReader, Elements.FX.kWrapP.Name, ref mWrapP, _ColladaElement.Enums.SamplerWrapFromString);
            _SetValueOptional<Enums.SamplerFilter>(aReader, Elements.FX.kMinfilter.Name, ref mMinfilter, _ColladaElement.Enums.SamplerFilterFromString);
            _SetValueOptional<Enums.SamplerFilter>(aReader, Elements.FX.kMagfilter.Name, ref mMagfilter, _ColladaElement.Enums.SamplerFilterFromString);
            _SetValueOptional<Enums.SamplerFilter>(aReader, Elements.FX.kMipfilter.Name, ref mMipfilter, _ColladaElement.Enums.SamplerFilterFromString);
            _SetValueOptional(aReader, Elements.FX.kBorderColor.Name, ref mBorderColor);
            _SetValueOptional(aReader, Elements.FX.kMipmapMaxlevel.Name, ref mMipmapMaxlevel);
            _SetValueOptional(aReader, Elements.FX.kMipmapBias.Name, ref mMipmapBias);
            #endregion

            #region Children
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public _ColladaElement Source { get { return mSource; } }
        public Enums.SamplerWrap WrapS { get { return mWrapS; } }
        public Enums.SamplerWrap WrapT { get { return mWrapT; } }
        public Enums.SamplerWrap WrapP { get { return mWrapP; } }
        public Enums.SamplerFilter Minfilter { get { return mMinfilter; } }
        public Enums.SamplerFilter Magfilter { get { return mMagfilter; } }
        public Enums.SamplerFilter Mipfilter { get { return mMipfilter; } }
        public float BorderColor { get { return mBorderColor; } }
        public uint MipmapMaxlevel { get { return mMipmapMaxlevel; } }
        public float MipmapBias { get { return mMipmapBias; } }
        public Enums.SamplerType Type { get { return mType; } }
    }
}
