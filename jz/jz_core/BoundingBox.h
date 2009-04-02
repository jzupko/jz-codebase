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
#ifndef _JZ_BOUNDING_BOX_H_
#define _JZ_BOUNDING_BOX_H_

#include <jz_core/Memory.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector3.h>

namespace jz
{

    struct BoundingSphere;
    struct CoordinateFrame3D;
    struct Matrix4;
    struct Ray;

    struct BoundingBox
    {
		Vector3 Min;
        Vector3 Max;
        
        BoundingBox()
			: Min(Vector3::kZero), Max(Vector3::kZero)
        {}

		BoundingBox(const BoundingBox& b)
			: Min(b.Min), Max(b.Max)
		{}

        BoundingBox(const Vector3& aMin, const Vector3& aMax)
            : Min(aMin),
              Max(aMax)
        {}
        
		BoundingBox& operator=(const BoundingBox& b)
		{
			Max = b.Max;
			Min = b.Min;

			return *this;
		}

        bool operator==(const BoundingBox& b) const
        {
            return (Max == b.Max) && (Min == b.Min);
        }
        bool operator!=(const BoundingBox& b) const
        {
            return !((*this) == b);
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
        
        Vector3 Center() const
        {
            return 0.5f * (Max + Min);
        }

        float DiagonalLength() const
        {
            float ret = Vector3::Distance(Min, Max);
            
            return ret;
        }

        float EffectiveRadius(const Vector3& aDirection) const;
        float EffectiveRadius(const Plane& aPlane) const;

        Vector3 Extents() const
        {
            return (Max - Min);
        }

        Vector3 HalfExtents() const
        {
            return 0.5f * Extents();
        }

        float InverseSurfaceArea() const
        {
            Vector3 whd(Max - Min);
            float invSa = 0.5f / ((whd.Z * whd.X) + (whd.Z * whd.Y) + (whd.X * whd.Y));

            return invSa;
        }

        float SurfaceArea() const
        {
            Vector3 whd(Max - Min);
            float sa = 2.0f * ((whd.Z * whd.X) + (whd.Z * whd.Y) + (whd.X * whd.Y));

            return sa;
        }
        
        static const BoundingBox kZero;
		static const BoundingBox kInvertedMax;
		static const BoundingBox kMax;

        static bool AboutEqual(const BoundingBox& a, const BoundingBox& b, float aTolerance = jz::Constants<float>::kZeroTolerance)
        {
            return (Vector3::AboutEqual(a.Min, b.Min, aTolerance) &&
                Vector3::AboutEqual(a.Max, b.Max, aTolerance));
        }

        static BoundingBox Clamp(const BoundingBox& a, const BoundingBox& aMinMax)
        {
            BoundingBox ret(
                Vector3::Min(Vector3::Max(a.Min, aMinMax.Min), aMinMax.Max),
                Vector3::Max(Vector3::Min(a.Max, aMinMax.Max), aMinMax.Min));

            return ret;
        }

        static BoundingBox CreateFrom(const BoundingSphere& b);

		template <typename ITR>
		static BoundingBox CreateFromPoints(ITR aBegin, ITR aEnd)
		{
            Vector3 min = Vector3::kMax;
            Vector3 max = Vector3::kMin;

            for (ITR I = aBegin; I != aEnd; I++)
            {
                min = Vector3::Min(min, *I);
                max = Vector3::Max(max, *I);
            }

            BoundingBox ret(min, max);

            return ret;
        }

		static BoundingBox Merge(const BoundingBox& a, const BoundingBox& b)
		{
            return BoundingBox(Vector3::Min(a.Min, b.Min), Vector3::Max(a.Max, b.Max));
		}

        static BoundingBox Transform(const CoordinateFrame3D& cf, const BoundingBox& a);
        static BoundingBox Transform(const Matrix4& m, const BoundingBox& a);
    };
    
}

#endif
