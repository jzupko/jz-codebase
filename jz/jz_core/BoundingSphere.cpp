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

#include <jz_core/BoundingBox.h>
#include <jz_core/BoundingSphere.h>
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Matrix4.h>

namespace jz
{

    const BoundingSphere BoundingSphere::kZero = BoundingSphere(Vector3(0), 0.0f);

    bool BoundingSphere::Contains(const BoundingSphere& b) const
    {
        return ((Vector3::DistanceSquared(Center, b.Center) + b.RadiusSquared()) < RadiusSquared());
    }

    bool BoundingSphere::Intersects(const BoundingBox& b) const
    {
        return b.Intersects(*this);
    }

    bool BoundingSphere::Intersects(const BoundingSphere& b) const
    {
        return (Vector3::DistanceSquared(Center, b.Center) < (RadiusSquared() + b.RadiusSquared()));
    }

    bool BoundingSphere::Intersects(const Vector3& b) const
    {
        return (Vector3::DistanceSquared(Center, b) < RadiusSquared());
    }

	BoundingSphere BoundingSphere::Merge(const BoundingSphere& a, const BoundingSphere& b)
    {
		BoundingSphere ret;

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
        Vector3 v = b.Center - a.Center;
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

	BoundingSphere BoundingSphere::Transform(const CoordinateFrame3D& cf, const BoundingSphere& a)
	{
        BoundingSphere ret;
        ret.Center = Vector3::TransformPosition(cf, a.Center);
        ret.Radius = a.Radius;

		return ret;
    }

	BoundingSphere BoundingSphere::Transform(const Matrix4& m, const BoundingSphere& a)
	{
		Vector3 x = Vector3::TransformDirection(m, Vector3::kUnitX);
		Vector3 y = Vector3::TransformDirection(m, Vector3::kUnitY);
		Vector3 z = Vector3::TransformDirection(m, Vector3::kUnitZ);

		BoundingSphere ret;
		ret.Center = Vector3::TransformPosition(m, a.Center);
        ret.Radius = (a.Radius * jz::Max(x.Length(), y.Length(), z.Length()));

		return ret;
	}

}
