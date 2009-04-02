using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using Microsoft.Xna.Framework.Content.Pipeline.Serialization.Compiler;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Xml;
using jz.pipeline.collada.elements;
using jz.pipeline.collada.elements.fx;
using System.ComponentModel;

namespace jz.pipeline.collada
{
    public sealed class ColladaProcessorPhysics
    {
        #region Private members
        private Vector3[] _Convert(float[] a)
        {
            if (a.Length % 3 != 0) { throw new Exception("Input array is incorrect length."); }
            Vector3[] ret = new Vector3[a.Length / 3];
            int count = a.Length;

            int index = 0;
            for (int i = 0; i < count; i += 3)
            {
                ret[index++] = new Vector3(a[i + 0], a[i + 1], a[i + 2]);
            }

            return ret;
        }

        private bool _ExtractPhysicsIndexPosition(_ColladaPrimitive p, out UInt16[] arIndices, out Vector3[] arPositions)
        {
            _ColladaInput input = p.FindInput(_ColladaElement.Enums.InputSemantic.kVertex);

            uint offset = ((ColladaInputGroupB)input).Offset;
            uint offsetCount = p.OffsetCount;

            input = p.FindInput(_ColladaElement.Enums.InputSemantic.kPosition);

            if (input.Stride == 3)
            {
                arIndices = Array.ConvertAll<int, UInt16>(p.Indices.GetSparse(offset, offsetCount),
                    delegate(int e)
                    {
                        if (e < 0 || e > UInt16.MaxValue) { throw new ArgumentOutOfRangeException(); }
                        return ((UInt16)e);
                    });
                arPositions = _Convert(input.GetArray<float>().Array);

                if (Utilities.kWinding == Winding.Clockwise)
                {
                    Utilities.FlipWindingOrder(ColladaDocument.ColladaPrimitiveToXnaPrimitive(p), arIndices);
                }

                return true;
            }

            arIndices = null;
            arPositions = null;
            return false;
        }

        private struct Entry
        {
            public UInt16[] Indices;
            public Vector3[] Positions;
        }
        #endregion

        public ColladaProcessorPhysics()
        {}

        // Helper used for sorting triangle indices. Note: this ignore triangle winding.
        // Two triangles are considered the same if they share the same three indices, independent

        public struct VertexHelper : IComparable<VertexHelper>
        {
            private readonly float kDistanceSqTolerance;
            public Vector3 V;
            public UInt16 I;

            public VertexHelper(Vector3 v, float aDistanceSqTolerance)
            {
                kDistanceSqTolerance = aDistanceSqTolerance;
                V = v;
                I = 0;
            }

            public int CompareTo(VertexHelper b)
            {
                if (Vector3.DistanceSquared(V, b.V) < kDistanceSqTolerance) { return 0; }
                else
                {
                    int cX = V.X.CompareTo(b.V.X);
                    int cY = V.Y.CompareTo(b.V.Y);
                    int cZ = V.Z.CompareTo(b.V.Z);

                    if (cX == 0)
                    {
                        if (cY == 0)
                        {
                            if (cZ == 0) { return 0; }
                            else { return cZ; }
                        }
                        else { return cY; }
                    }
                    else { return cX; }
                }
            }
        }

        public UInt16 Insert(Vector3 v, List<Vector3> p, List<List<int>> backRef, List<VertexHelper> phelper)
        {
            VertexHelper vh = new VertexHelper(v, Utilities.kZeroToleranceFloat);
            int index = phelper.BinarySearch(vh);

            if (index >= 0) { return phelper[index].I; }
            else
            {
                Debug.Assert(p.Count <= UInt16.MaxValue);
                vh.I = (UInt16)p.Count;
                p.Add(v);
                phelper.Insert(~index, vh);
                backRef.Add(new List<int>());

                return vh.I;
            }
        }

        private static readonly int[][] kOffsets = new int[3][]
        {
            new int[] { 1, 2 },
            new int[] { 1, -1},
            new int[] { -2, -1}
        };

        public TriangleTree ProcessPhysics(ColladaCOLLADA aRoot)
        {
            Dictionary<_ColladaPrimitive, Entry> cache = new Dictionary<_ColladaPrimitive, Entry>();

            List<UInt16> indices = new List<ushort>();
            List<Vector3> vertices = new List<Vector3>();
            List<List<int>> backRef = new List<List<int>>();
            List<Utilities.IndexComparable> ihelper = new List<Utilities.IndexComparable>();
            List<VertexHelper> vhelper = new List<VertexHelper>();

            aRoot.Apply<ColladaInstanceGeometry>(_ColladaElement.ApplyType.RecurseDown,
                _ColladaElement.ApplyStop.Delegate, delegate(ColladaInstanceGeometry instance)
                {
                    ColladaNode parent = (ColladaNode)instance.Parent;
                    ColladaGeometry geom = instance.Instance;
                    if (!geom.IsMesh) { return false; }
                    ColladaMesh mesh = geom.Mesh;

                    foreach (_ColladaPrimitive e in mesh.GetEnumerable<_ColladaPrimitive>())
                    {
                        Entry entry;

                        if (cache.ContainsKey(e)) { entry = cache[e]; }
                        else
                        {
                            if (_ExtractPhysicsIndexPosition(e, out entry.Indices, out entry.Positions)) { cache.Add(e, entry); }
                            else { continue; }
                        }

                        Matrix m = parent.WorldXnaTransform;

                        int count = entry.Indices.Length;
                        for (int i = 0; i < count; i += 3)
                        {
                            int i0 = entry.Indices[i + 0];
                            int i1 = entry.Indices[i + 1];
                            int i2 = entry.Indices[i + 2];

                            Vector3 p0 = Vector3.Transform(entry.Positions[i0], m);
                            Vector3 p1 = Vector3.Transform(entry.Positions[i1], m);
                            Vector3 p2 = Vector3.Transform(entry.Positions[i2], m);

                            #region Skip this triangle if it is degenerate
                            Triangle t = new Triangle(p0, p1, p2);
                            if (t.IsDegenerate) { continue; }
                            #endregion

                            #region Insert indices, use ih to determine if we have a duplicate triangle.
                            Utilities.IndexComparable ihe = new Utilities.IndexComparable(true);
                            ihe.I0 = Insert(p0, vertices, backRef, vhelper);
                            ihe.I1 = Insert(p1, vertices, backRef, vhelper);
                            ihe.I2 = Insert(p2, vertices, backRef, vhelper);

                            int index = ihelper.BinarySearch(ihe);
                            if (index < 0)
                            {
                                ihelper.Insert(~index, ihe);
                                indices.Add(ihe.I0);
                                backRef[ihe.I0].Add(indices.Count - 1);
                                indices.Add(ihe.I1);
                                backRef[ihe.I1].Add(indices.Count - 1);
                                indices.Add(ihe.I2);
                                backRef[ihe.I2].Add(indices.Count - 1);
                            }
                            #endregion
                        }
                    }

                    return false;
                });

            TriangleTree ret = new TriangleTree();
            ret.Build(indices, vertices);

            return ret;
        }
    }
}
