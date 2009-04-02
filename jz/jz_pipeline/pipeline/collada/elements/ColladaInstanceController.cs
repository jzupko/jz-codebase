using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaInstanceController : _ColladaInstance<ColladaController>
    {
        #region Protected members
        protected override void _PreChildrenAdd(XmlReader aReader)
        {
            #region Children

            _AddZeroToManyChildren(aReader, Elements.kSkeleton);
            _AddOptionalChild(aReader, Elements.FX.kBindMaterial);
            #endregion
        }
        #endregion

        public ColladaInstanceController(XmlReader aReader)
            : base(aReader)
        { }

        public _ColladaElement SkeletonRoot { get { return GetFirst<ColladaSkeleton>().Instance; } }
    }
}
