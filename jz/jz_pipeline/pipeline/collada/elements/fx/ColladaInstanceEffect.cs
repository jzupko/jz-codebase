using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaInstanceEffect : _ColladaInstance<ColladaEffect>
    {
        public ColladaInstanceEffect(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _AddZeroToManyChildren(aReader, Elements.FX.kTechniqueHint);
            _AddZeroToManyChildren(aReader, Elements.FX.kSetparamOfInstanceEffect);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
