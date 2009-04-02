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

#include <jz_core/BoundingCircle.h>
#include <jz_core/BoundingRectangle.h>
#include <jz_core/Matrix3.h>

namespace jz
{

    const BoundingCircle BoundingCircle::kZero = BoundingCircle(Vector2(0), 0.0f);

    bool BoundingCircle::Contains(const BoundingCircle& b) const
    {
        return ((Vector2::DistanceSquared(Center, b.Center) + b.RadiusSquared()) < RadiusSquared());
    }

    bool BoundingCircle::Intersects(const BoundingRectangle& b) const
    {
        return b.Intersects(*this);
    }

    bool BoundingCircle::Intersects(const BoundingCircle& b) const
    {
        return (Vector2::DistanceSquared(Center, b.Center) < (RadiusSquared() + b.RadiusSquared()));
    }

    bool BoundingCircle::Intersects(const Vector2& b) const
    {
        return (Vector2::DistanceSquared(Center, b) < RadiusSquared());
    }

	BoundingCircle BoundingCircle::Merge(const BoundingCircle& a, const BoundingCircle& b)
    {
		BoundingCircle ret;

        if (AboutZero(a.Radius))
        {
			ret.Center = b.Center;
			ret.Radius = b.Radius;
            return ret;
		}
        else if (AboutZero(b.Radius))
        {
			ret.Center = a.Center;
			ret.Radius = a.Radius;
            return ret;
		}
        
        float r = b.Radius;
        Vector2 v = (b.Center - a.Center);
        float d = v.LengthSquared();

        if (AboutZero(d))
        {
            ret.Radius = Max(a.Radius, r);
        }
        else
        {
            float m = 1.0f / Sqrt(d);
            float s = m * d;

            if (LessThan(a.Radius, (s + r)))
            {
                float t = s + a.Radius;

                if (LessThan(r, t))
                {
                    t = (t + r) * 0.5f;
                    ret.Center = a.Center + (v * m * (t - a.Radius));
                    ret.Radius = t;
                }
                else
                {
                    ret.Center = b.Center;
                    ret.Radius = r;
                }
            }
        }

		return ret;
    }

	BoundingCircle BoundingCircle::Transform(const Matrix3& m, const BoundingCircle& a)
	{
		Vector2 x = Vector2::TransformDirection(m, Vector2::kUnitX);
		Vector2 y = Vector2::TransformDirection(m, Vector2::kUnitY);

		BoundingCircle ret;
		ret.Center = Vector2::TransformPosition(m, a.Center);
        ret.Radius = (a.Radius * jz::Max(x.Length(), y.Length()));

		return ret;
	}

}
