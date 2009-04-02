using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;

namespace jz
{
    public class TriangleTree
    {
        #region Protected members
        [StructLayout(LayoutKind.Explicit)]
        protected struct Node
        {
            public static Node Create()
            {
                Node ret = default(Node);
                ret.Sibling = 0u;
                ret.mFlags = 0u;

                return ret;
            }

            const UInt32 kAxisMask = (1 << 0) | (1 << 1);
            const UInt32 kFbMask = (1 << 2);
            const UInt32 kLeafMask = (1 << 3);

            public Axis Axis
            {
                get
                {
                    Axis ret = (Axis)(mFlags & kAxisMask);
                    return ret;
                }

                set
                {
                    mFlags &= ~kAxisMask;
                    mFlags |= (((UInt32)value) & kAxisMask);
                }
            }

            public float SplitPos
            {
                get
                {
                    return mSplitPos;
                }

                set
                {
                    UInt32 flags = (mFlags & (kAxisMask | kFbMask | kLeafMask));
                    mSplitPos = value;
                    mFlags &= ~(kAxisMask | kFbMask | kLeafMask);
                    mFlags |= flags;
                }
            }

            public bool IsLeaf { get { return ((mFlags & kLeafMask) != 0); } }
            public bool IsBack { get { return ((mFlags & kFbMask) == 0); } }
            public bool IsFront { get { return ((mFlags & kFbMask) != 0); } }

            public void SetBack() { mFlags &= ~kFbMask; }
            public void SetFront() { mFlags |= kFbMask; }
            public void SetLeaf() { mFlags |= kLeafMask; }

            public void Write(BinaryWriter w)
            {
                w.Write(mFlags);
                w.Write(Sibling);
            }

            [FieldOffset(0)]
            private float mSplitPos;
            [FieldOffset(0)]
            private UInt32 mFlags;

            [FieldOffset(4)]
            public UInt32 Sibling;
            [FieldOffset(4)]
            public UInt32 TriangleIndex;
        };

        protected List<Node> mNodes = new List<Node>();
        protected List<UInt16> mTriangles = new List<UInt16>();
        protected List<Vector3> mVertices = new List<Vector3>();
        #endregion

        #region Private members
        private BoundingBox mTotalAABB = Utilities.kZeroBox;
        private Vector3 CalculateMean(List<UInt32> e)
        {
            Vector3 ret = Vector3.Zero;
            int count = e.Count;

            if (count > 0)
            {
                for (int i = 0; i < count; i++)
                {
                    Triangle t = GetTriangle(e[i]);
                    ret += t.Center;
                }
                ret *= (1.0f / (float)count);
            }

            return ret;
        }

        private Vector3 CalculateVariance(Vector3 mean, List<UInt32> e)
        {
            Vector3 ret = Vector3.Zero;

            int count = e.Count;
            if (count > 1)
            {
                for (int i = 0; i < count; i++)
                {
                    Triangle t = GetTriangle(e[i]);

                    Vector3 a = (t.Center - mean);
                    ret += (a * a);
                }

                ret = (ret / ((float)(count - 1)));
            }

            return ret;
        }

        private BoundingBox CalculateTotalAABB(List<UInt32> e)
        {
            Vector3 min = Utilities.kMaxVector3;
            Vector3 max = Utilities.kMinVector3;

            int size = e.Count;
            for (int i = 0; i < size; i++)
            {
                Triangle t = GetTriangle(e[i]);

                min = Vector3.Min(min, Vector3.Min(t.P0, Vector3.Min(t.P1, t.P2)));
                max = Vector3.Max(max, Vector3.Max(t.P0, Vector3.Max(t.P1, t.P2)));
            }

            return new BoundingBox(min, max);
        }

        private Triangle GetTriangle(UInt32 i)
        {
            UInt32 kIndex = (i * 3u);

            Triangle ret = new Triangle(
                mVertices[(int)mTriangles[(int)(kIndex + 0u)]],
                mVertices[(int)mTriangles[(int)(kIndex + 1u)]],
                mVertices[(int)mTriangles[(int)(kIndex + 2u)]]);

            return ret;
        }

        private void Split(float pos, Axis axis,
            List<UInt32> e, List<UInt32> front, List<UInt32> back,
            out float frontPos, out float backPos)
        {
            frontPos = pos;
            backPos = pos;

            int size = e.Count;
            for (int i = 0; i < size; i++)
            {
                Triangle t = GetTriangle(e[i]);
                float center = Utilities.GetElement(t.Center, (int)axis);

                if (pos <= center)
                {
                    frontPos = Utilities.Min(frontPos, Utilities.GetElement(t.Min, (int)axis));
                    front.Add(e[i]);
                    e[i] = e[size - 1];
                    size--; i--;
                }
                else if (pos > center)
                {
                    backPos = Utilities.Max(backPos, Utilities.GetElement(t.Max, (int)axis));
                    back.Add(e[i]);
                    e[i] = e[size - 1];
                    size--; i--;
                }
            }

            Debug.Assert(front.Count + back.Count == e.Count);
            Debug.Assert(size == 0);
            e.Clear();
        }

        private void Split(int aIndex, BoundingBox aParentAABB, List<UInt32> e)
        {
            Debug.Assert(e.Count > 0);

            if (e.Count == 1)
            {
                Node n = mNodes[aIndex];
                n.SetLeaf();
                n.TriangleIndex = e[0];
                e.Clear();
                mNodes[aIndex] = n;
                return;
            }

            Vector3 mean = CalculateMean(e);
            Vector3 variance = CalculateVariance(mean, e);
            float max = Utilities.Max(variance.X, variance.Y, variance.Z);

            Axis axis = (max == variance.Z) ? Axis.Z : ((max == variance.X) ? Axis.X : Axis.Y);
            float pos = Utilities.GetElement(Utilities.GetCenter(aParentAABB), (int)axis);

            List<UInt32> front = new List<UInt32>();
            List<UInt32> back = new List<UInt32>();
            float frontPos = 0.0f;
            float backPos = 0.0f;
            Split(pos, axis, e, front, back, out frontPos, out backPos);
            Debug.Assert(e.Count == 0);

            // It's possible that splitting at the box center places all the objects on one side of the split.
            if ((front.Count == 0 && back.Count > 1) || (back.Count == 0 && front.Count > 1))
            {
                e.AddRange(front); front.Clear();
                e.AddRange(back); back.Clear();

                pos = Utilities.GetElement(mean, (int)axis);
                Split(pos, axis, e, front, back, out frontPos, out backPos);

                Debug.Assert(!((front.Count == 0 && back.Count > 1) || (back.Count == 0 && front.Count > 1)));
            }

            #region Create front node
            if (front.Count > 0)
            {
                mNodes.Add(Node.Create());
                Node n = mNodes[mNodes.Count - 1];
                n.Axis = axis;
                n.SplitPos = frontPos;
                n.SetFront();
                mNodes[mNodes.Count - 1] = n;

                BoundingBox frontAABB = aParentAABB;
                Utilities.SetElement(ref frontAABB.Min, (int)axis, frontPos);

                Split(mNodes.Count - 1, frontAABB, front);
            }
            #endregion

            #region Create back node
            if (back.Count > 0)
            {
                mNodes.Add(Node.Create());
                Node n = mNodes[mNodes.Count - 1];
                n.Axis = axis;
                n.SplitPos = backPos;
                n.SetBack();
                mNodes[mNodes.Count - 1] = n;

                BoundingBox backAABB = aParentAABB;
                Utilities.SetElement(ref backAABB.Max, (int)axis, backPos);

                Split(mNodes.Count - 1, backAABB, back);
            }
            #endregion

            #region Set node sibling reference
            {
                Node n = mNodes[aIndex];
                n.Sibling = (UInt32)mNodes.Count;
                mNodes[aIndex] = n;
            }
            #endregion
        }
        #endregion

        #region Public members
        public void Build(List<UInt16> aTriangles, List<Vector3> aVertices)
        {
            mTriangles.Clear();
            mTriangles.AddRange(aTriangles);
            mVertices.Clear();
            mVertices.AddRange(aVertices);

            List<UInt32> t = new List<UInt32>();
            UInt32 count = (UInt32)mTriangles.Count;
            for (UInt32 i = 0; i < count; i += 3) { t.Add(i / 3u); }

            mTotalAABB = CalculateTotalAABB(t);

            mNodes.Clear();

            if (t.Count > 0)
            {
                #region Insert a root node
                mNodes.Add(Node.Create());
                Node n = mNodes[0];
                n.Axis = Axis.Z;
                n.SetFront();
                n.SplitPos = mTotalAABB.Min.Z;
                mNodes[0] = n;
                #endregion
                Split(0, mTotalAABB, t);
                if (mNodes.Count == 1) { mNodes[0].SetLeaf(); }
            }
        }

        public void Write(BinaryWriter w)
        {
            w.Write((UInt32)mTriangles.Count);
            foreach (UInt16 e in mTriangles) { w.Write(e); }

            w.Write((UInt32)mVertices.Count);
            foreach (Vector3 e in mVertices) { w.Write(e.X); w.Write(e.Y); w.Write(e.Z); }

            w.Write((UInt32)mNodes.Count);
            foreach (Node e in mNodes) { e.Write(w); }

            w.Write(mTotalAABB.Min.X); w.Write(mTotalAABB.Min.Y); w.Write(mTotalAABB.Min.Z);
            w.Write(mTotalAABB.Max.X); w.Write(mTotalAABB.Max.Y); w.Write(mTotalAABB.Max.Z);
        }
        #endregion
    }
     
}
