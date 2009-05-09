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
#ifndef _JZ_PLANE_H_
#define _JZ_PLANE_H_

#include <jz_core/Prereqs.h>
#include <jz_core/Vector3.h>
#include <jz_core/Vector4.h>
#include <type_traits>

namespace jz
{

    namespace PlaneIntersection
    {
        enum Type
        {
            kFront,
            kBack,
            kIntersecting
        };
    };

	struct BoundingBox;
	struct BoundingSphere;
    struct OrientedBoundingBox;

    struct Plane
    {
        Plane() 
            : mNormal(Vector3::kZero),
            mAbsNormal(Vector3::kZero),
            mD(0.0f)
        {}
		Plane(const Vector3& aNormal, float aD) { Set(aNormal, aD); }
        Plane(float a, float b, float c, float d) { Set(a, b, c, d); }
        Plane(const Vector3& aNormal, const Vector3& aPosition) { Set(aNormal, aPosition); }
        Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2) { Set(p0, p1, p2); }

        Plane Flip() const
        {
            return Plane(-mNormal, -mD);
        }

		const Vector3& GetAbsNormal() const { return mAbsNormal; }
		const Vector3& GetNormal() const { return mNormal; }
        void SetNormal(const Vector3& v)
        {
            mNormal = v;
            mAbsNormal = Vector3::Abs(v);
        }

		float GetD() const { return mD; }
		void SetD(float v) { mD = v; }

		PlaneIntersection::Type Intersects(const BoundingBox& b) const;
		PlaneIntersection::Type Intersects(const BoundingSphere& s) const;
        PlaneIntersection::Type Intersects(const OrientedBoundingBox& b) const;
		PlaneIntersection::Type Intersects(const Vector3& v) const;

        void Set(const Vector3& aNormal, float aD)
        {
            mNormal = aNormal;
            mAbsNormal = aNormal;
            mD = aD;
        }

        void Set(const Vector4& v)
        {
            Set(v.X, v.Y, v.Z, v.W);
        }

        // coefficients ax + by + cz + d = 0        
        void Set(float a, float b, float c, float d);
        // a normal and a position on the plane.
        // aNormal will be normalized.
        void Set(const Vector3& aNormal, const Vector3& aPosition);
        // three vertices on the plane, used to calculate the plane normal:
        // as (p2 - p1) cross (p0 - p1), handle winding accordingly.
        void Set(const Vector3& p0, const Vector3& p1, const Vector3& p2);

        static float DotCoordinate(const Vector3& v, const Plane& p)
        {
            return (p.mD + Vector3::Dot(p.mNormal, v));
        }

        static float DotNormal(const Vector3& v, const Plane& p)
        {
            return (Vector3::Dot(v, p.mNormal));
        }

        void Normalize()
        {
            float l = mNormal.Length();
            if (l > Constants<float>::kLooseTolerance)
            {
                float invL = (1.0f / l);

                mNormal *= invL;
                mD *= invL;
                mAbsNormal = Vector3::Abs(mNormal);
            }
        }

        static Vector3 ReflectDirection(const Vector3& v, const Plane& p);
        static Vector3 ReflectPosition(const Vector3& v, const Plane& p);

        static Vector3 ProjectOnto(const Vector3& v, const Plane& p);
        static Plane Transform(const Matrix4& aTransform, const Plane& aPlane);

		private:
			Vector3 mAbsNormal;
			float mD;
			Vector3 mNormal;
    };

}

#endif
