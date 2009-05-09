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
#include <jz_core/Matrix4.h>
#include <jz_core/OrientedBoundingBox.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector4.h>

namespace jz
{

	PlaneIntersection::Type Plane::Intersects(const BoundingBox& b) const
	{
		float radius = b.EffectiveRadius(*this);
		float negRadius = -radius;

		float d = DotCoordinate(b.Center(), *this);

		if (d < negRadius) { return PlaneIntersection::kBack; }
		else if (d <= radius) { return PlaneIntersection::kIntersecting; }
		else { return PlaneIntersection::kFront; }
	}

	PlaneIntersection::Type Plane::Intersects(const BoundingSphere& s) const
	{
		float radius = s.Radius;
		float negRadius = -radius;

		float d = DotCoordinate(s.Center, *this);

		if (d < negRadius) { return PlaneIntersection::kBack; }
		else if (d <= radius) { return PlaneIntersection::kIntersecting; }
		else { return PlaneIntersection::kFront; }
	}

    PlaneIntersection::Type Plane::Intersects(const OrientedBoundingBox& b) const
    {
        float radius = b.EffectiveRadius(*this);
        float negRadius = -radius;

        float d = DotCoordinate(b.Center, *this);

        if (d < negRadius) { return PlaneIntersection::kBack; }
        else if (d <= radius) { return PlaneIntersection::kIntersecting; }
        else { return PlaneIntersection::kFront; }
    }

	PlaneIntersection::Type Plane::Intersects(const Vector3& v) const
	{
		float d = DotCoordinate(v, *this);

		if (d < -Constants<float>::kLooseTolerance) { return PlaneIntersection::kBack; }
		else if (d > Constants<float>::kLooseTolerance) { return PlaneIntersection::kFront; }
		else { return PlaneIntersection::kIntersecting; }
	}

    void Plane::Set(float a, float b, float c, float d)
    {
        mNormal.Set(a, b, c);
        mAbsNormal = Vector3::Abs(mNormal);
        mD = d;
    }
    
    void Plane::Set(const Vector3& aNormal, const Vector3& aPosition)
    {
        mNormal = aNormal;
        mAbsNormal = Vector3::Abs(mNormal);
        mD = Vector3::Dot(-mNormal, aPosition);
    }
    
    void Plane::Set(const Vector3& p0, const Vector3& p1, const Vector3& p2)
    {
        const Vector3 a(p0 - p1);
        const Vector3 b(p2 - p1);
        
        mNormal = Vector3::UnitCross(b, a);
        mAbsNormal = Vector3::Abs(mNormal);
        mD = Vector3::Dot(-mNormal, p1);
    }     

    Vector3 Plane::ProjectOnto(const Vector3& v, const Plane& p)
    {
        float d = DotCoordinate(v, p);
        Vector3 ret = (v - (d * p.mNormal));

        return ret;
    }

    Vector3 Plane::ReflectDirection(const Vector3& v, const Plane& p)
    {
        float d = DotNormal(v, p);
        Vector3 ret = (v - (2.0f * d * p.mNormal));

        return ret;
    }

    Vector3 Plane::ReflectPosition(const Vector3& v, const Plane& p)
    {
        float d = DotCoordinate(v, p);
        Vector3 ret = (v - (2.0f * d * p.mNormal));

        return ret;
    }

    // TODO: verify.
    Plane Plane::Transform(const Matrix4& m, const Plane& aPlane)
    {
        Matrix4 itm = Matrix4::CreateNormalTransform(m);
        Vector4 p = Vector4::Transform(itm, Vector4(aPlane.mNormal, aPlane.mD));
        Plane ret = Plane(p.X, p.Y, p.Z, p.W);
        ret.Normalize();

        return ret;
    }

}
