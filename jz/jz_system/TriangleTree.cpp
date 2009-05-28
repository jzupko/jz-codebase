//
// Copyright (c) 2009 Joseph A. Zupko
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>
#include <jz_system/TriangleTree.h>
#include <algorithm>

namespace jz
{
    namespace system
    {

        void TriangleTree::Node::Read(IReadFilePtr& p)
        {
            mFlags = ReadUInt32(p);
            Sibling = ReadUInt32(p);
        }

        Vector3 TriangleTree::CalculateMean(const vector<u32>& e)
        {
            Vector3 ret = Vector3::kZero;
            size_t size = e.size();

            if (size > 0u)
            {
                for (size_t i = 0u; i < size; i++)
                {
                    Triangle3D t = GetTriangle(e[i]);
                    ret += t.GetCenter();
                }
                ret *= (1.0f / (float)size);
            }

            return ret;
        }

        Vector3 TriangleTree::CalculateVariance(const Vector3& mean, const vector<u32>& e)
        {
            Vector3 ret = Vector3::kZero;

            size_t size = e.size();
            if (size > 1)
            {
                for (size_t i = 0; i < size; i++)
                {
                    Triangle3D t = GetTriangle(e[i]);

                    Vector3 a = (t.GetCenter() - mean);
                    ret += (a * a);
                }

                ret = (ret / ((float)(size - 1u)));
            }

            return ret;
        }

        BoundingBox TriangleTree::CalculateTotalAABB(const vector<u32>& aTriangles)
        {
            Vector3 min = Vector3::kMax;
            Vector3 max = Vector3::kMin;

            size_t size = aTriangles.size();
            for (size_t i = 0u; i < size; i++)
            {
                Triangle3D t = GetTriangle(aTriangles[i]);

                min = Vector3::Min(min, Vector3::Min(t.P0, Vector3::Min(t.P1, t.P2)));
                max = Vector3::Max(max, Vector3::Max(t.P0, Vector3::Max(t.P1, t.P2)));
            }

            return BoundingBox(min, max);
        }

        Triangle3D TriangleTree::GetTriangle(u32 i)
        {
            const u32 kIndex = (i * 3u);

            JZ_ASSERT(kIndex < mTriangles.size());

            Triangle3D ret = Triangle3D(
                mVertices[mTriangles[(kIndex + 0u)]],
                mVertices[mTriangles[(kIndex + 1u)]],
                mVertices[mTriangles[(kIndex + 2u)]]);

            return ret;
        }

        void TriangleTree::Split(float pos, Axis::Id axis,
            vector<u32>& e, vector<u32>& front, vector<u32>& back,
            float& frontPos, float& backPos)
        {
            frontPos = pos;
            backPos = pos;

            size_t size = e.size();
            for (size_t i = 0u; i < size; i++)
            {
                Triangle3D t = GetTriangle(e[i]);
                float center = t.GetCenter()[axis];

                if (pos <= center)
                {
                    frontPos = jz::Min(frontPos, t.GetMin()[axis]);
                    front.push_back(e[i]);
                    e[i] = e[size - 1u];
                    size--; i--;
                }
                else if (pos > center)
                {
                    backPos = jz::Max(backPos, t.GetMax()[axis]);
                    back.push_back(e[i]);
                    e[i] = e[size - 1u];
                    size--; i--;
                }
            }

            JZ_ASSERT(front.size() + back.size() == e.size());
            JZ_ASSERT(e.size() == 0u);
            e.clear();
        }

        void TriangleTree::Split(size_t aIndex, const BoundingBox& aParentAABB, vector<u32>& e)
        {
            JZ_ASSERT(e.size() > 0u);

            if (e.size() == 1u)
            {
                mNodes[aIndex].SetLeaf();
                mNodes[aIndex].TriangleIndex = e[0];
                e.clear();
                return;
            }

            Vector3 mean = CalculateMean(e);
            Vector3 variance = CalculateVariance(mean, e);
            float max = jz::Max(variance.X, variance.Y, variance.Z);

            Axis::Id axis = (max == variance.Z) ? Axis::kZ : ((max == variance.X) ? Axis::kX : Axis::kY);
            float pos = aParentAABB.Center()[axis];

            vector<u32> front;
            vector<u32> back;
            float frontPos = 0.0f;
            float backPos = 0.0f;
            Split(pos, axis, e, front, back, frontPos, backPos);
            JZ_ASSERT(e.size() == 0u);

            // It's possible that splitting at the box center places all the objects on one side of the split.
            if ((front.size() == 0u && back.size() > 1u) || (back.size() == 0u && front.size() > 1u))
            {
                copy(front.begin(), front.end(), e.end());
                front.clear();
                copy(back.begin(), back.end(), e.end());
                back.clear();

                pos = mean[axis];
                Split(pos, axis, e, front, back, frontPos, backPos);
                JZ_ASSERT(!((front.size() == 0u && back.size() > 1u) || (back.size() == 0u && front.size() > 1u)));
            }

            #pragma region Create front node
            if (front.size() > 0u)
            {
                mNodes.push_back(Node());
                mNodes[mNodes.size() - 1u].SetAxis(axis);
                mNodes[mNodes.size() - 1u].SetSplitPos(frontPos);
                mNodes[mNodes.size() - 1u].SetFront();

                BoundingBox frontAABB = aParentAABB;
                frontAABB.Min[axis] = frontPos;

                Split(mNodes.size() - 1u, frontAABB, front);
            }
            #pragma endregion

            #pragma region Create back node
            if (back.size() > 0u)
            {
                mNodes.push_back(Node());
                mNodes[mNodes.size() - 1u].SetAxis(axis);
                mNodes[mNodes.size() - 1u].SetSplitPos(backPos);
                mNodes[mNodes.size() - 1u].SetBack();

                BoundingBox backAABB = aParentAABB;
                backAABB.Max[axis] = backPos;

                Split(mNodes.size() - 1u, backAABB, back);
            }
            #pragma endregion

            mNodes[aIndex].Sibling = mNodes.size();
        }

        void TriangleTree::Build(const MemoryBuffer<u16>& aTriangles, const MemoryBuffer<Vector3>& aVertices)
        {
            mTriangles.clear();
            mTriangles = aTriangles;
            mVertices.clear();
            mVertices = aVertices;

            vector<u32> t;
            size_t size = mTriangles.size();
            for (size_t i = 0u; i < size; i += 3) { t.push_back(i / 3); }

            mTotalAABB = CalculateTotalAABB(t);

            mNodes.clear();
            #pragma region Insert a root node
            mNodes.resize(1);
            mNodes[0].SetAxis(Axis::kZ);
            mNodes[0].SetFront();
            mNodes[0].SetSplitPos(mTotalAABB.Min.Z);
            #pragma endregion        
            Split(0u, mTotalAABB, t);
        }

        void TriangleTree::Read(IReadFilePtr& pFile)
        {
            mTriangles.clear();
            mVertices.clear();
            mNodes.clear();

            ReadBuffer(pFile, mTriangles);
            ReadBuffer(pFile, mVertices);

            size_t size = ReadSizeT(pFile);
            for (size_t i = 0u; i < size; i++)
            {
                Node node;
                node.Read(pFile);

                mNodes.push_back(node);
            }

            mTotalAABB = ReadBoundingBox(pFile);
        }

    }
}
