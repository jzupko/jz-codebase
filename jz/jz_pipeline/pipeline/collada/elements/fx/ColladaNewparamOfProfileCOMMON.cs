using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaNewparamOfProfileCOMMON : _ColladaNewparam
    {
        public ColladaNewparamOfProfileCOMMON(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.FX.kSemantic);

            {
                int count = 0;
                count += _AddOptionalChild(aReader, Elements.FX.kFloat);
                count += _AddOptionalChild(aReader, Elements.FX.kFloat2);
                count += _AddOptionalChild(aReader, Elements.FX.kFloat3);
                count += _AddOptionalChild(aReader, Elements.FX.kSurfaceOfProfileCOMMON);
                if (aReader.NodeType != XmlNodeType.None)
                {
                    count += _AddOptionalChild(aReader, new Elements.Element(aReader.Name, delegate(XmlReader r) { return new ColladaSamplerFX(r, Enums.GetSamplerType(aReader.Name)); }));
                }

                if (count != 1)
                {
                    throw new Exception("<new_param> expects one and only one of <float>, <float2>, <float3>, <surface>, or <sampler*> as a child element.");
                }
            }
            #endregion
        }
    }
}
