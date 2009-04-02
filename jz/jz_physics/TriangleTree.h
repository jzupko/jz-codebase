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
#ifndef _JZ_PHYSICS_TRIANGLE_TREE_H_
#define _JZ_PHYSICS_TRIANGLE_TREE_H_

#include <jz_core/Auto.h>
#include <jz_core/Memory.h>
#include <jz_core/Triangle3D.h>
#include <vector>

namespace jz
{
    namespace system
    {
        class IReadFile; typedef AutoPtr<IReadFile> IReadFilePtr;
    }

    namespace physics
    {

        /// <summary>
        /// A kdTree of Triangles.
        /// </summary>
        /// <remarks>
        /// A space-partitioning data structure: (http://en.wikipedia.org/wiki/Kd-tree).
        /// 
        /// Splitting heuristic based on the surface area heuristic (SAH): 
        ///     Havran, V. 2000. "Heuristic Ray Shooting Algorithms", Ph.D. Thesis, 
        ///     Department of Computer Science and Engineering, Czech Technical University in Prague,
        ///     http://www.cgg.cvut.cz/~havran/phdthesis.html .
	    class TriangleTree
	    {
	    public:
		    struct Node
		    {
                Node()
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

                void Read(system::IReadFilePtr& p);

                union
                {
                    u32 Sibling;
                    u32 TriangleIndex;
                };

            private:
                union
                {
                    f32 mSplitPos;
                    u32 mFlags;
                };
            };

            virtual ~TriangleTree() {}

            // Build expects sanitized input. Positions in aVertices should be unique and index triplets
            // in aTriangles should be unique.
            virtual void Build(const MemoryBuffer<u16>& aTriangles, const MemoryBuffer<Vector3>& aVertices);
            
            const MemoryBuffer<u16>& GetIndices() const { return mTriangles; }
            const MemoryBuffer<Vector3>& GetVertices() const { return mVertices; }
            const vector<Node>& GetNodes() const { return mNodes; }

            const BoundingBox& GetTotalAABB() const { return mTotalAABB; }
            Triangle3D GetTriangle(u32 i);

            void Read(system::IReadFilePtr& pFile);

	    protected:
            MemoryBuffer<Vector3> mVertices;
            MemoryBuffer<u16> mTriangles;
            vector<Node> mNodes;

        private:
            Vector3 CalculateMean(const vector<u32>& e);
            Vector3 CalculateVariance(const Vector3& mean, const vector<u32>& e);
            BoundingBox CalculateTotalAABB(const vector<u32>& aTriangles);
            void Split(size_t aIndex, const BoundingBox& aParentAABB, vector<u32>& e);
            void Split(float pos, Axis::Id axis, vector<u32>& e, vector<u32>& front, vector<u32>& back, float& frontPos, float& backPos);

            BoundingBox mTotalAABB;
        };

    }
}

#endif
