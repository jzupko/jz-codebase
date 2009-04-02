using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaAnimation : _ColladaElementWithIdAndName
    {
        public ColladaAnimation(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            int animationCount = 0;
            int samplerCount = 0;
            int channelCount = 0;

            _AddOptionalChild(aReader, Elements.kAsset);
            animationCount += _AddZeroToManyChildren(aReader, Elements.kAnimation);
            _AddZeroToManyChildren(aReader, Elements.kSource);
            samplerCount += _AddZeroToManyChildren(aReader, Elements.kSampler);
            channelCount += _AddZeroToManyChildren(aReader, Elements.kChannel);
            _AddZeroToManyChildren(aReader, Elements.kExtra);

            if (samplerCount != channelCount)
            {
                throw new Exception("# of <sampler> elements != # of <channel> elements");
            }

            if (samplerCount + animationCount == 0)
            {
                throw new Exception("element <animation> does not have the necessary children.");
            }

            #endregion
        }
    }
}
