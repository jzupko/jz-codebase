using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaTechniqueCommonOfOptics : _ColladaElement
    {
        public ColladaTechniqueCommonOfOptics(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);

            int childCount = 0;
            childCount += _AddOptionalChild(aReader, Elements.kOrthographic);
            childCount += _AddOptionalChild(aReader, Elements.kPerspective);

            if (childCount != 1)
            {
                throw new Exception("element <technique_common> of <optics> must contain one and only one of element <orthographi> or <perspective>");
            }
            #endregion
        }
    }
}