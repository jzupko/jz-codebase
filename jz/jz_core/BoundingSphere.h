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
#ifndef _JZ_BOUNDING_SPHERE_H_
#define _JZ_BOUNDING_SPHERE_H_

#include <jz_core/Memory.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector3.h>

namespace jz
{

    struct BoundingBox;
    struct CoordinateFrame3D;
    struct Matrix4;
    struct Ray;

    struct BoundingSphere
    {
        Vector3 Center;
        float   Radius;
        
        BoundingSphere()
			: Center(Vector3::kZero), Radius(0)
		{}

		BoundingSphere(const BoundingSphere& b)
			: Center(b.Center), Radius(b.Radius)
		{}

        BoundingSphere(const Vector3& aCenter, float aRadius)
            : Center(aCenter), Radius(aRadius)
        {}
        
        BoundingSphere& operator=(const BoundingSphere& b)
        {
            Center = b.Center;
            Radius = b.Radius;
            
            return *this;
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

        float RadiusSquared() const
        {
            return (Radius * Radius);
        }
        
        static BoundingSphere CreateFrom(const BoundingBox& b);

        ///
        /// <summary>Bounding sphere construction from points.</summary>
        /// <remarks>
        /// From Lengyel, E. 2004. "Mathematics for 3D Game Programming and COmputer Graphics".
		template <typename ITR>
		static BoundingSphere CreateFromPoints(ITR aBegin, ITR aEnd)
		{
			Vector3 r, s, t;
			Vector3 halfExtents;

			CalculatePrincipalComponentAxes(aBegin, aEnd, r, s, t);

			BoundingSphere ret;
			CalculateCenterAndHalfExtents(aBegin, aEnd, r, s, t, ret.Center, halfExtents);
            ret.Radius = halfExtents.Length();

			for (ITR I = aBegin; I != aEnd; I++)
			{
				Vector3 a = ((*I) - ret.Center);
				float n = a.LengthSquared();

                if (jz::GreaterThan(n, ret.RadiusSquared(), Constants<float>::kLooseTolerance))
				{
					Vector3 b = (ret.Center - (a * (1.0f / Sqrt(n)) * ret.Radius));

					ret.Center = 0.5f * (b + (*I));
                    ret.Radius = Vector3::Distance(b, ret.Center);
				}
			}

			return ret;
		}

		static BoundingSphere Merge(const BoundingSphere& a, const BoundingSphere& b);
        static BoundingSphere Transform(const CoordinateFrame3D& cf, const BoundingSphere& a);
		static BoundingSphere Transform(const Matrix4& m, const BoundingSphere& a);

        static const BoundingSphere kZero;
    };
    
}

#endif