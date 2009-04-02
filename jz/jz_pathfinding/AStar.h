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
#ifndef _JZ_PATHFINDING_A_STAR_H_
#define _JZ_PATHFINDING_A_STAR_H_

#include <jz_core/Memory.h>
#include <jz_core/Prereqs.h>
#include <algorithm>
#include <set>
#include <vector>

namespace jz
{
    namespace pathfinding
    {

        ///
        /// <summary>A* graph traversal</summary>
        /// <remarks>
        /// i0 - starting node.
        /// i1 - goal node.
        /// aAdjacency - array of adjacency information. Should be initialized to kNullNode.
        ///              each node can have a maximum of ADJACENCY adjacent nodes.
        /// estimatedDist - function that should return an estimate of the distance between any two nodes.
        ///                 This function is the heuristic of the A* heuristic search. It is used to
        ///                 to weight certain paths over other paths.
        /// actualAdjacentDist - function that should return the true distance between two adjacent nodes.
        /// arOut - resulting path if true is returned, empty if false is returned.
        template <typename T, uint ADJACENCY>
        class AStar
        {
        public:
            JZ_STATIC_ASSERT(!numeric_limits<T>::is_signed);

            static const size_t kMaxNodes = (Constants<T>::kMax - 1u);
            static const size_t kNullNode = (Constants<T>::kMax);

            typedef float (*DistanceFunc)(T x, T y, void_p apUserData);

            static bool FindPath(T i0, T i1,
                vector<T> aAdjacency,
                DistanceFunc estimatedDist,
                DistanceFunc actualAdjacentDist,
                vector<T>& arOut,
                void_p apUserData = null)
            {
                JZ_ASSERT(i0 != kNullNode);
                JZ_ASSERT(i1 != kNullNode);

                arOut.clear();
                JZ_ASSERT(aAdjacency.size() % ADJACENCY == 0u);
                const size_t kSize = (aAdjacency.size() / ADJACENCY); 
                JZ_ASSERT(kSize <= kMaxNodes);
     
                set<PathHelper> open;
                vector<bool> closed(kSize); closed.assign(kSize, false);
                vector<float> g(kSize); g.assign(kSize, 0.0f);
                vector<T> cameFrom(kSize); cameFrom.assign(kSize, kNullNode);

                open.insert(PathHelper(i0));
                while (open.begin() != open.end())
                {
                    const T x = open.begin()->I;
                    open.erase(open.begin());

                    if (x == i1)
                    {
                        ReconstructPath(cameFrom, i1, arOut);
                        return true;
                    }

                    closed[x] = true;
                    
                    const size_t kAdjIndex = (x * ADJACENCY);
                    for (size_t i = 0u; i < ADJACENCY; i++)
                    {
                        const T y = (aAdjacency[kAdjIndex + i]);
                        if (y == kNullNode) { continue; }
                        if (closed[y]) { continue; }

                        float dis = actualAdjacentDist(x, y, apUserData);
                        if (dis < 0.0f)
                        {
                            closed[y] = true;
                            continue;
                        }

                        float curG = (g[x] + dis);

                        // Find node in open set using linear search.
                        set<PathHelper>::iterator I = open.end();
                        for (I = open.begin(); I != open.end(); I++) { if (I->I == y) { break; } }

                        if (I == open.end())
                        {
                            cameFrom[y] = x;
                            g[y] = curG;
                            open.insert(PathHelper(y, (g[y] + estimatedDist(y, i1, apUserData))));
                        }
                        else if (curG < g[y])
                        {
                            open.erase(I);
                            cameFrom[y] = x;
                            g[y] = curG;
                            open.insert(PathHelper(y, (g[y] + estimatedDist(y, i1, apUserData))));
                        }
                    }
                }

                return false;
            }

        private:
            static void ReconstructPath(vector<T> cameFrom, T currentNode, vector<T>& arOut)
            {
                if (cameFrom[currentNode] != kNullNode)
                {
                    ReconstructPath(cameFrom, cameFrom[currentNode], arOut);
                    arOut.push_back(currentNode);
                }
            }

            struct PathHelper
            {
                float F;
                T I;

                PathHelper(T i, float f = 0.0f)
                    : F(f), I(i)
                {}

                PathHelper(const PathHelper& b)
                    : F(b.F), I(b.I)
                {}

                PathHelper& operator=(const PathHelper& b)
                {
                    F = b.F;
                    I = b.I;
                    return (*this);
                }

                bool operator<(const PathHelper& b) const { return (F < b.F); }
                bool operator==(const PathHelper& b) const { return (F == b.F); }
                bool operator>(const PathHelper& b) const { return (F > b.F); }
            };
        };
    }
}

#endif