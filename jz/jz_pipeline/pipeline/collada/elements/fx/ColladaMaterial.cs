using System;
using System.Xml;
using System.Collections.Generic;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaMaterial : _ColladaElementWithIdAndName
    {
        public ColladaMaterial(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);
            _AddRequiredChild(aReader, Elements.FX.kInstanceEffect);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public ColladaEffect Effect { get { return GetFirst<ColladaInstanceEffect>().Instance; } }

        public ColladaSetparamOfInstanceEffect[] Params
        {
            get
            {
                List<ColladaSetparamOfInstanceEffect> list = new List<ColladaSetparamOfInstanceEffect>();

                GetFirst<ColladaInstanceEffect>().GetAll<ColladaSetparamOfInstanceEffect>(list);

                return list.ToArray();
            }
        }
    }
}
