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

#pragma once
#ifndef _JZ_AABB_TREE_H_
#define _JZ_AABB_TREE_H_

#include <jz_core/BoundingBox.h>
#include <jz_core/Memory.h>
#include <vector>

namespace jz
{

    struct AABBTreeNode
    {
        AABBTreeNode()
            : Sibling(0u), mFlags(0u)
        {
            SetLeaf();
        }

        static const u32 kAxisMask = (1 << 0) | (1 << 1);
        static const u32 kFbMask = (1 << 2);
        static const u32 kLeafMask = (1 << 3);

        Axis::Id GetAxis() const
        {
            Axis::Id ret = (Axis::Id)(mFlags & kAxisMask);
            return ret;
        }

        void SetAxis(Axis::Id v)
        {
            mFlags &= ~kAxisMask;
            mFlags |= (((u32)v) & kAxisMask);
        }

        float GetSplitPos() const { return mSplitPos; }

        void SetSplitPos(float v)
        {
            u32 flags = (mFlags & (kAxisMask | kFbMask | kLeafMask));
            mSplitPos = v;
            mFlags &= ~(kAxisMask | kFbMask | kLeafMask);
            mFlags |= flags;
        }

        bool IsLeaf() const { return ((mFlags & kLeafMask) != 0); }
        bool IsBack() const { return ((mFlags & kFbMask) == 0); }
        bool IsFront() const { return ((mFlags & kFbMask) != 0); }

        void SetBack() { mFlags &= ~kFbMask; }
        void SetFront() { mFlags |= kFbMask; }
        void SetLeaf() { mFlags |= kLeafMask; }

        union
        {
            u32 Sibling;
            u32 ObjectIndex;
        };

    private:
        union
        {
            f32 mSplitPos;
            u32 mFlags;
        };
    };

    namespace AABBTreeHelpers
    {
        Vector3 CalculateMean(const vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs);
        Vector3 CalculateVariance(const Vector3& mean, const vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs);
        BoundingBox CalculateTotalAABB(const vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs);
        void Split(size_t aIndex, const BoundingBox& aParentAABB, vector<u32>& e, const MemoryBuffer<BoundingBox>& aAABBs, vector<AABBTreeNode>& arNodes);
        void Split(float pos, Axis::Id axis, vector<u32>& e, vector<u32>& front, vector<u32>& back, float& frontPos, float& backPos, const MemoryBuffer<BoundingBox>& aAABBs);
    }

    template <typename T>
    class AABBTree
    {
    public:
        virtual ~AABBTree() {}

        virtual void Build(const MemoryBuffer<BoundingBox>& aAABBs, const vector<T>& aObjects)
        {
            JZ_ASSERT(aAABBs.size() == aObjects.size());

            mObjects.clear();
            mObjects = aObjects;

            vector<u32> t;
            size_t size = mObjects.size();
            for (size_t i = 0u; i < size; i++) { t.push_back(i); }

            BoundingBox aabb = CalculateTotalAABB(t, aAABBs);

            mNodes.clear();
            #pragma region Insert a root node
            mNodes.resize(1);
            mNodes[0].SetAxis(Axis::kZ);
            mNodes[0].SetFront();
            mNodes[0].SetSplitPos(aabb.Min.Z);
            #pragma endregion        
            AABBTreeHelpers::Split(0u, aabb, t, aAABBs, mNodes);
        }

    protected:
        vector<AABBTreeNode> mNodes;
        vector<T> mObjects;

    };

}

#endif
