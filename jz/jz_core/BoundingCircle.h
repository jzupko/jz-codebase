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
#ifndef _JZ_BOUNDING_CIRCLE_H_
#define _JZ_BOUNDING_CIRCLE_H_

#include <jz_core/Memory.h>
#include <jz_core/Vector2.h>

namespace jz
{

    struct BoundingRectangle;
    struct Matrix3;

    struct BoundingCircle
    {
        Vector2 Center;
        float   Radius;
        
        BoundingCircle()
			: Center(Vector2::kZero), Radius(0)
		{}

		BoundingCircle(const BoundingCircle& b)
			: Center(b.Center), Radius(b.Radius)
		{}

        BoundingCircle(const Vector2& aCenter, float aRadius)
            : Center(aCenter), Radius(aRadius)
        {}
        
        BoundingCircle& operator=(const BoundingCircle& b)
        {
            Center = b.Center;
            Radius = b.Radius;
            
            return *this;
		}

        bool Contains(const BoundingCircle& b) const;
        bool Contains(const BoundingRectangle& b) const;
        bool Contains(const Vector2& b) const { return Intersects(b); }

        bool Intersects(const BoundingCircle& b) const;
        bool Intersects(const BoundingRectangle& b) const;
        bool Intersects(const Vector2& b) const;

        template <typename T>
        bool Disjoint(const T& b) const { return !Intersects(b); }

        float RadiusSquared() const { return (Radius * Radius); }
        
		static BoundingCircle Merge(const BoundingCircle& a, const BoundingCircle& b);
		static BoundingCircle Transform(const Matrix3& m, const BoundingCircle& a);

        static const BoundingCircle kZero;
    };
    
}

#endif