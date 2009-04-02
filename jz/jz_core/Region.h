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
#ifndef _JZ_REGION_H_
#define _JZ_REGION_H_

#include <jz_core/Memory.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector3.h>

namespace jz
{
    namespace Geometric
    {
        enum Test
        {
            kContains,
            kIntersects,
            kDisjoint
        };
    }

    struct Region
    {
        static const size_t kCappingPlaneCount = 2;
        static const size_t kMinimumLateralPlanes = 3;
        static const size_t kMinimumPlanes = (kCappingPlaneCount + kMinimumLateralPlanes);
        
        Vector3 Center;
        MemoryBuffer<Plane> Planes;

        enum Planes
        {
            kFar = 0,
            kNear = 1,
            kTop = 2,
            kBottom = 3,
            kRight = 4,
            kLeft = 5
        };
        
        Region()
            : Center(Vector3::kZero), Planes(kMinimumPlanes)
        { }

        Region(const Region& aRegion)
            : Center(aRegion.Center), Planes(aRegion.Planes)
        { }

        Region(const Vector3& aCenter, size_t aCount)
            : Center(aCenter), Planes(Max(aCount, kMinimumPlanes))
        { }

        Region(const Vector3& aCenter, const Matrix4& aViewProjection)
            : Center(aCenter), Planes(6)
        {
            Set(aCenter, aViewProjection);
        }
        
        Region& operator=(const Region& aRegion)
        {
            Center = aRegion.Center;
            Planes = aRegion.Planes;
            
            return *this;
        }

        void Set(const Vector3& aCenter, const Matrix4& M);

        Geometric::Test Test(const BoundingBox& aBox) const;
        Geometric::Test Test(const BoundingSphere& aSphere) const;
        Geometric::Test Test(const Vector3& aPoint) const;
    };
    
}

#endif