using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaGeometry : _ColladaElementWithIdAndName
    {
        public ColladaGeometry(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);

            // add one and only one of (convex_mesh, mesh, or spline)
            {
                int geometryCount = 0;
                geometryCount += _AddOptionalChild(aReader, Elements.Physics.kConvexMesh);
                geometryCount += _AddOptionalChild(aReader, Elements.kMesh);
                geometryCount += _AddOptionalChild(aReader, Elements.kSpline);

                if (geometryCount != 1)
                {
                    throw new Exception("# of (convex_mesh, mesh, or spline) not equal to 1");
                }
            }

            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public bool IsConvexMesh
        {
            get
            {
                return (GetFirstOptional<physics.ColladaConvexMesh>() != null);
            }
        }

        public physics.ColladaConvexMesh ConvexMesh
        {
            get
            {
                return (GetFirst<physics.ColladaConvexMesh>());
            }
        }

        public bool IsMesh
        {
            get
            {
                return (GetFirstOptional<ColladaMesh>() != null);
            }
        }

        public ColladaMesh Mesh
        {
            get
            {
                return (GetFirst<ColladaMesh>());
            }
        }

        public bool IsSpline
        {
            get
            {
                return (GetFirstOptional<ColladaSpline>() != null);
            }
        }

        public ColladaSpline Spline
        {
            get
            {
                return GetFirst<ColladaSpline>();
            }
        }
    }
}