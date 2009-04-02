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

#include <jz_core/AABBTree.h>

namespace jz
{
    namespace AABBTreeHelpers
    {

        Vector3 CalculateMean(const vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs)
        {
            Vector3 ret = Vector3::kZero;
            size_t size = e.size();

            if (size > 0u)
            {
                for (size_t i = 0u; i < size; i++)
                {
                    ret += aAABBs[e[i]].Center();
                }
                ret *= (1.0f / (float)size);
            }

            return ret;
        }

        Vector3 CalculateVariance(const Vector3& mean, const vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs)
        {
            Vector3 ret = Vector3::kZero;
            size_t size = e.size();

            if (size > 1u)
            {
                for (size_t i = 0u; i < size; i++)
                {
                    Vector3 a = (aAABBs[e[i]].Center() - mean);
                    ret += (a * a);
                }

                ret = (ret / ((float)(size - 1u)));
            }

            return ret;
        }

        BoundingBox CalculateTotalAABB(const vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs)
        {
            Vector3 min = Vector3::kMax;
            Vector3 max = Vector3::kMin;

            size_t size = e.size();
            for (size_t i = 0u; i < size; i++)
            {
                const BoundingBox& aabb = aAABBs[e[i]];

                min = Vector3::Min(min, aabb.Min);
                max = Vector3::Max(max, aabb.Max);
            }

            return (BoundingBox(min, max));
        }


        void Split(size_t aIndex, const BoundingBox& aParentAABB, vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs, vector<AABBTreeNode>& arNodes)
        {
            JZ_ASSERT(e.size() > 0u);

            if (e.size() == 1u)
            {
                arNodes[aIndex].SetLeaf();
                arNodes[aIndex].ObjectIndex = e[0];
                e.clear();
                return;
            }

            Vector3 mean = CalculateMean(e, aAABBs);
            Vector3 variance = CalculateVariance(mean, e, aAABBs);
            float max = jz::Max(variance.X, variance.Y, variance.Z);

            Axis::Id axis = (max == variance.Z) ? Axis::kZ : ((max == variance.X) ? Axis::kX : Axis::kY);
            float pos = aParentAABB.Center()[axis];

            vector<u32> front;
            vector<u32> back;
            float frontPos = 0.0f;
            float backPos = 0.0f;
            Split(pos, axis, e, front, back, frontPos, backPos, aAABBs);
            JZ_ASSERT(e.size() == 0u);

            // It's possible that splitting at the box center places all the objects on one side of the split.
            if ((front.size() == 0u && back.size() > 1u) || (back.size() == 0u && front.size() > 1u))
            {
                copy(front.begin(), front.end(), e.end());
                front.clear();
                copy(back.begin(), back.end(), e.end());
                back.clear();

                pos = mean[axis];
                Split(pos, axis, e, front, back, frontPos, backPos, aAABBs);
                JZ_ASSERT(!((front.size() == 0u && back.size() > 1u) || (back.size() == 0u && front.size() > 1u)));
            }

            #pragma region Create front node
            if (front.size() > 0u)
            {
                arNodes.push_back(AABBTreeNode());
                arNodes[arNodes.size() - 1u].SetAxis(axis);
                arNodes[arNodes.size() - 1u].SetSplitPos(frontPos);
                arNodes[arNodes.size() - 1u].SetFront();

                BoundingBox frontAABB = aParentAABB;
                frontAABB.Min[axis] = frontPos;

                Split(arNodes.size() - 1u, frontAABB, front, aAABBs, arNodes);
            }
            #pragma endregion

            #pragma region Create back node
            if (back.size() > 0u)
            {
                arNodes.push_back(AABBTreeNode());
                arNodes[arNodes.size() - 1u].SetAxis(axis);
                arNodes[arNodes.size() - 1u].SetSplitPos(backPos);
                arNodes[arNodes.size() - 1u].SetBack();

                BoundingBox backAABB = aParentAABB;
                backAABB.Max[axis] = backPos;

                Split(arNodes.size() - 1u, backAABB, back, aAABBs, arNodes);
            }
            #pragma endregion

            arNodes[aIndex].Sibling = arNodes.size();
        }

        void Split(float pos, Axis::Id axis, vector<u32>& e, vector<u32>& front, vector<u32>& back, float& frontPos, float& backPos, const MemoryBuffer<BoundingBox>& aAABBs)
        {
            frontPos = pos;
            backPos = pos;

            size_t size = e.size();
            for (size_t i = 0u; i < size; i++)
            {
                const BoundingBox& aabb = aAABBs[e[i]];
                float center = aabb.Center()[axis];

                if (pos <= center)
                {
                    frontPos = jz::Min(frontPos, aabb.Min[axis]);
                    front.push_back(e[i]);
                    e[i] = e[size - 1u];
                    size--; i--;
                }
                else if (pos > center)
                {
                    backPos = jz::Max(backPos, aabb.Max[axis]);
                    back.push_back(e[i]);
                    e[i] = e[size - 1u];
                    size--; i--;
                }
            }

            JZ_ASSERT(front.size() + back.size() == e.size());
            JZ_ASSERT(e.size() == 0u);
            e.clear();
        }

    }
}
