using Microsoft.Xna.Framework;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public abstract class _ColladaTransformElement : _ColladaChannelTarget
    {
        public _ColladaTransformElement(XmlReader aReader)
            : base(aReader)
        { }

        /// <summary>
        /// Returns the transform element as an XNA style matrix.
        /// </summary>
        /// <remarks>
        /// An XNA transform is row-major, vectors as rows.
        /// </remarks>
        public abstract Matrix XnaMatrix { get; }
    }

}
