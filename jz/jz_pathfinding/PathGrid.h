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
#ifndef _JZ_PATHFINDING_PATH_GRID_H_
#define _JZ_PATHFINDING_PATH_GRID_H_

#include <jz_core/Memory.h>
#include <jz_core/Prereqs.h>
#include <jz_core/BoundingRectangle.h>
#include <vector>

namespace jz
{
    namespace pathfinding
    {

        // Dynamic 2D pathfinding.
        // TODO:
        // - rename this class. It's named PathGrid because its original implementation used
        //   a tile-based grid implementation. It now uses a continuous representation.
        // - write unit tests and create visualization debugging tools for checking path generation.
        // - caching - should allow caching of paths based on the radius of the object that
        //             is pathfinding. All data is currently generated and thrown out on a call
        //             to FindPath.
        // - allow for objects to be specified by arbitrary convex polygons instead of only
        //   bounding rectangles. 
        //
        // Changes to point generation
        // - may want to split long distance adjacencies - in cases where points are connected
        //   that are very far apart, pathfinding has a tendency to zig-zag between distant points
        // - alternative - instead of using the points on expanded rectangle edges directly, find
        //                 adjacencies, then generate a new point set that consists of the midpoints
        //                 of all of the adjacencies, and then redo the adjacency finding with this
        //                 new point set.
        //                 - the points themselves should not collid with anything but collision will
        //                   still need to be checked between points.
        //   - this is probably only practical once caching is in-place and updates can partially
        //     modify the point set instead of forcing a full rebuild
        //     - this will also make partial updates harder, as the pathfinding is implied from the
        //       corners of colliding objects instead of derived directly from them.
        //
        // Temporality
        // - once caching occurs, the ideal would be to update pathing partially based on what has
        //   changed vs. rebuilding the entire pathing.
        class PathGrid sealed
        {
        public:
            PathGrid();
            ~PathGrid();

            size_t Add(const BoundingRectangle& v);
            bool FindPath(size_t aObject, const Vector2& v1, vector<Vector2>& arPath) const;
            void Remove(size_t v);
            void Update(size_t aObject, const BoundingRectangle& v);

        private:
            vector<BoundingRectangle> mObjects;
            vector<size_t> mFreeList;
        };

    }
}

#endif