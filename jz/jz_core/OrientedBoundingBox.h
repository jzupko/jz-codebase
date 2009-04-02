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
#ifndef _JZ_ORIENTED_BOUNDING_BOX_H_
#define _JZ_ORIENTED_BOUNDING_BOX_H_

#include <jz_core/Matrix3.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector3.h>

namespace jz
{

    struct BoundingBox;
    struct BoundingSphere;
    struct Ray;

    struct OrientedBoundingBox
    {
        const Vector3 Center;
        const Vector3 Rmag;
        const Vector3 Smag;
        const Vector3 Tmag;
        const Vector3 AbsXYZ;

        OrientedBoundingBox(const Vector3& aCenter, const Vector3& aHalfExtents, const Vector3& aR, const Vector3& aS, const Vector3& aT)
            : Center(aCenter), 
            Rmag(aHalfExtents.X * aR),
            Smag(aHalfExtents.Y * aS),
            Tmag(aHalfExtents.Z * aT),
            AbsXYZ(jz::Abs(Rmag.X) + jz::Abs(Smag.X) + jz::Abs(Tmag.X),
                   jz::Abs(Rmag.Y) + jz::Abs(Smag.Y) + jz::Abs(Tmag.Y),
                   jz::Abs(Rmag.Z) + jz::Abs(Smag.Z) + jz::Abs(Tmag.Z))
        { }

        float EffectiveRadius(const Vector3& aDirection) const;
        float EffectiveRadius(const Plane& aPlane) const
        {
            return EffectiveRadius(aPlane.GetNormal());
        }

        bool Contains(const BoundingBox& b) const;
        bool Contains(const BoundingSphere& b) const;
        bool Contains(const Vector3& b) const { return Intersects(b); }

        bool Intersects(const BoundingBox& b) const;
        bool Intersects(const BoundingSphere& b) const;
        bool Intersects(const Plane& b) const { return (b.Intersects(*this) == PlaneIntersection::kIntersecting); }
        bool Intersects(const Ray& b) const;
        bool Intersects(const Vector3& b) const;

        template <typename T>
        bool Disjoint(const T& b) const { return !Intersects(b); }

        JZ_EXPORT static OrientedBoundingBox CreateFromAABB(const BoundingBox& aAABB, const CoordinateFrame3D& aFrame);

		template <typename ITR>
		static OrientedBoundingBox CreateFromPoints(ITR aBegin, ITR aEnd)
        {
            Vector3 center;
            Vector3 r, s, t;
            Vector3 halfExtents;

            CalculatePrincipalComponentAxes(aBegin, aEnd, r, s, t);
            CalculateCenterAndHalfExtents(aBegin, aEnd, r, s, t, center, halfExtents);

            OrientedBoundingBox ret = OrientedBoundingBox(center, halfExtents, r, s, t);

            return ret;
        }
    };

}

#endif
