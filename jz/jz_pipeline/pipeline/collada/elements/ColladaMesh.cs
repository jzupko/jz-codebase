using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaMesh : _ColladaElement
    {
        public ColladaMesh(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOneToManyChildren(aReader, Elements.kSource);
            _AddRequiredChild(aReader, Elements.kVertices);
            _AddZeroToManyChildren(aReader, Elements.kLines);
            _AddZeroToManyChildren(aReader, Elements.kLineStrips);
            _AddZeroToManyChildren(aReader, Elements.kPolygons);
            _AddZeroToManyChildren(aReader, Elements.kPolylist);
            _AddZeroToManyChildren(aReader, Elements.kTriangles);
            _AddZeroToManyChildren(aReader, Elements.kTrifans);
            _AddZeroToManyChildren(aReader, Elements.kTristrips);
            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }
    }
}
