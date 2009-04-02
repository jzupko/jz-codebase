using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInstanceGeometry : _ColladaInstance<ColladaGeometry>
    {
        #region Protected members
        protected override void _PreChildrenAdd(XmlReader aReader)
        {
            #region Children
            _AddOptionalChild(aReader, Elements.FX.kBindMaterial);
            #endregion
        }
        #endregion

        public ColladaInstanceGeometry(XmlReader aReader)
            : base(aReader)
        { }
    }
}
