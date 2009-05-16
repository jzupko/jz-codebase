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
#ifndef _JZ_RAY_3D_H_
#define _JZ_RAY_3D_H_

#include <jz_core/Vector3.h>

namespace jz
{

    struct BoundingBox;
    struct BoundingSphere;
    struct Plane;
    struct Region;

    struct Ray3D
    {
        Vector3 Direction;
        Vector3 Position;

        Ray3D(const Vector3& aPosition, const Vector3& aDirection)
            : Position(aPosition), Direction(aDirection)
        {}

        bool Intersects(const BoundingBox& aBox, float& arDistance) const;
        bool Intersects(const BoundingSphere& aSphere, float& arDistance) const;
        bool Intersects(const Plane& aPlane, float& arDistance) const;
        bool Intersects(const Region& aRegion, float& arDistance) const;

        static bool AboutEqual(const Ray3D& a, const Ray3D& b, float aTolerance = Constants<float>::kZeroTolerance)
        {
            return Vector3::AboutEqual(a.Direction, b.Direction, aTolerance) && 
                Vector3::AboutEqual(a.Position, b.Position, aTolerance);
        }
    };

    __inline bool operator==(const Ray3D& a, const Ray3D& b)
    {
        return (a.Direction == b.Direction && a.Position == b.Position);
    }

    __inline bool operator!=(const Ray3D& a, const Ray3D& b)
    {
        return (a.Direction != b.Direction || a.Position != b.Position);
    }
}

#endif