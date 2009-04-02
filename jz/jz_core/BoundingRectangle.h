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
#ifndef _JZ_BOUNDING_RECTANGLE_H_
#define _JZ_BOUNDING_RECTANGLE_H_

#include <jz_core/Memory.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector2.h>

namespace jz
{

    struct BoundingCircle;
    struct Matrix3;

    struct BoundingRectangle
    {
		Vector2 Min;
        Vector2 Max;
        
        BoundingRectangle()
			: Min(Vector2::kZero), Max(Vector2::kZero)
        {}

		BoundingRectangle(const BoundingRectangle& b)
			: Min(b.Min), Max(b.Max)
		{}

        BoundingRectangle(const Vector2& aMin, const Vector2& aMax)
            : Min(aMin),
              Max(aMax)
        {}
        
		BoundingRectangle& operator=(const BoundingRectangle& b)
		{
			Max = b.Max;
			Min = b.Min;

			return *this;
		}

        bool operator==(const BoundingRectangle& b) const
        {
            return (Max == b.Max) && (Min == b.Min);
        }

        bool operator!=(const BoundingRectangle& b) const
        {
            return !((*this) == b);
        }

        bool Contains(const BoundingCircle& b) const;
        bool Contains(const BoundingRectangle& b) const;
        bool Contains(const Vector2& b) const { return Intersects(b); }

        bool Intersects(const BoundingCircle& b) const;
        bool Intersects(const BoundingRectangle& b) const;
        bool Intersects(const Vector2& b) const;

        template <typename T>
        bool Disjoint(const T& b) const { return !Intersects(b); }
        
        Vector2 Center() const { return 0.5f * (Max + Min); }

        float DiagonalLength() const
        {
            float ret = Vector2::Distance(Min, Max);
            
            return ret;
        }

        float EffectiveRadius(const Vector2& aDirection) const;

        Vector2 Extents() const { return (Max - Min); }
        Vector2 HalfExtents() const { return 0.5f * Extents(); }

        float InverseArea() const
        {
            float ret = (1.0f / Area());

            return ret;
        }

        float Area() const
        {
            Vector2 wh(Max - Min);
            float a = (wh.Y * wh.X);

            return a;
        }
        
        static const BoundingRectangle kZero;
		static const BoundingRectangle kInvertedMax;
		static const BoundingRectangle kMax;
        static const BoundingRectangle kNegOneToOne;

        static bool AboutEqual(const BoundingRectangle& a, const BoundingRectangle& b, float aTolerance = jz::Constants<float>::kZeroTolerance)
        {
            return (Vector2::AboutEqual(a.Min, b.Min, aTolerance) &&
                Vector2::AboutEqual(a.Max, b.Max, aTolerance));
        }

        static BoundingRectangle Clamp(const BoundingRectangle& a, const BoundingRectangle& aMinMax)
        {
            BoundingRectangle ret(
                Vector2::Min(Vector2::Max(a.Min, aMinMax.Min), aMinMax.Max),
                Vector2::Max(Vector2::Min(a.Max, aMinMax.Max), aMinMax.Min));

            return ret;
        }

        static BoundingRectangle CreateFrom(const BoundingCircle& b);

		template <typename ITR>
		static BoundingRectangle CreateFromPoints(ITR aBegin, ITR aEnd)
		{
            BoundingRectangle ret = kInvertedMax;

            for (ITR I = aBegin; I != aEnd; I++)
            {
                ret.Min = Vector2::Min(ret.Min, *I);
                ret.Max = Vector2::Max(ret.Max, *I);
            }

			return ret;
        }

		static BoundingRectangle Merge(const BoundingRectangle& a, const BoundingRectangle& b)
		{
            return BoundingRectangle(Vector2::Min(a.Min, b.Min), Vector2::Max(a.Max, b.Max));
		}

        static BoundingRectangle Transform(const Matrix3& m, const BoundingRectangle& a);
    };
    
}

#endif
