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
#ifndef _JZ_TRIANGLE_3D_H_
#define _JZ_TRIANGLE_3D_H_

#include <jz_core/BoundingBox.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector3.h>

namespace jz
{

    struct Triangle3D sealed
    {
    public:
        Vector3 P0;
        Vector3 P1;
        Vector3 P2;

        Triangle3D()
        {}

        Triangle3D(const Vector3& p0, const Vector3& p1, const Vector3& p2)
            : P0(p0), P1(p1), P2(p2)
        { }

        bool IsDegenerate() const
        {
            return (jz::AboutZero(GetArea()));
        }

        BoundingBox GetAABB() const
        {
            BoundingBox ret;
            ret.Min = Vector3::Min(P0, Vector3::Min(P1, P2));
            ret.Max = Vector3::Max(P0, Vector3::Max(P1, P2));

            return ret;
        }

        float GetArea() const
        {
            float ret = (0.5f * Vector3::Cross((P2 - P0), (P1 - P0)).Length());

            return ret;
        }

        Vector3 GetCenter() const
        {
            static const float kFactor = (float)(1.0 / 3.0);

            Vector3 ret = (P0 + P1 + P2) * kFactor;

            return ret;
        }

        int GetFaceCount() const { return 1; }

        Vector3 GetMin() const
        {
            Vector3 ret = Vector3::Min(P0, P1, P2);
            return ret;
        }

        Vector3 GetMax() const
        {
            Vector3 ret = Vector3::Max(P0, P1, P2);
            return ret;
        }

        Vector3 GetNormal() const
        {
            Vector3 ret = Vector3::Normalize(Vector3::Cross(P2 - P0, P1 - P0));

            return ret;
        }

        Plane GetPlane() const
        {
            return Plane(P0, P2, P1);
        }
        
        static bool AboutEqual(const Triangle3D& a, const Triangle3D& b, float aTolerance = Constants<float>::kZeroTolerance)
        {
            return Vector3::AboutEqual(a.P0, b.P0, aTolerance) &&
                Vector3::AboutEqual(a.P1, b.P1, aTolerance) &&
                Vector3::AboutEqual(a.P2, b.P2, aTolerance);
        }

        bool Intersects(const Vector3& av) const;
    };

    __inline bool operator==(const Triangle3D& a, const Triangle3D& b)
    {
        return (a.P0 == b.P0) &&
            (a.P1 == b.P1) &&
            (a.P2 == b.P2);
    }

    __inline bool operator!=(const Triangle3D& a, const Triangle3D& b)
    {
        return (a.P0 != b.P0) ||
            (a.P1 != b.P1) ||
            (a.P2 != b.P2);
    }
}

#endif