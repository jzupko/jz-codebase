using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaFloat : _ColladaElement
    {
        #region Private members
        private readonly float mValue = 0.0f;
        #endregion

        public ColladaFloat(XmlReader aReader)
        {
            #region Element value
            _SetValue(aReader, ref mValue);
            #endregion
        }

        public float Value { get { return mValue; } }
    }
}
