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
#ifndef _JZ_SEGMENT_H_
#define _JZ_SEGMENT_H_

#include <jz_core/Vector3.h>

namespace jz
{

    struct BoundingSphere;
    struct Cylinder;
    struct Plane;
    struct Segment
    {
        Vector3 P0;
        Vector3 P1;

        static Segment Create(const Vector3& p0, const Vector3& p1)
        {
            Segment ret;
            ret.P0 = p0;
            ret.P1 = p1;

            return ret;
        }

        static bool AboutEqual(const Segment& a, const Segment& b, float aTolerance = Constants<float>::kZeroTolerance)
        {
            return Vector3::AboutEqual(a.P0, b.P0, aTolerance) && 
                Vector3::AboutEqual(a.P1, b.P1, aTolerance);
        }

        Vector3 DirectedMagnitude() const
        {
            return (P1 - P0);
        }

        Vector3 HalfDirectedMagnitude() const
        {
            return (0.5f * DirectedMagnitude());
        }

        Vector3 Direction() const
        {
            return Vector3::Normalize(DirectedMagnitude());
        }

        float Distance() const
        {
            return (Vector3::Distance(P0, P1));
        }

        // Computes a point based on interval factor t.
        // t is on [0,1] and represents a point between P0 and P1 of this segment.
        Vector3 Evaluate(float t) const
        {
            return (P0 + (t * DirectedMagnitude()));
        }

        Vector3 Midpoint() const
        {
            return (0.5f * (P0 + P1));
        }

        // Calculates the closest point between segments a an b.
        // Point on each line can be computed with:
        //    P(a) = (a.P0 + (s * (a.P1 - a.P0)))
        //    P(b) = (b.P0 + (t * (b.P1 - b.P0)))
        static void ClosestPoint(const Segment& sa, const Segment& sb, float& s, float& t);

        bool Intersects(const BoundingSphere& bs, float& t) const;
        bool Intersects(const Cylinder& cyl, float& t) const;
        bool Intersects(const Plane& aPlane, float& t) const;
    };

    inline bool operator==(const Segment& a, const Segment& b)
    {
        return (a.P0 == b.P0 && a.P1 == b.P1);
    }

    inline bool operator!=(const Segment& a, const Segment& b)
    {
        return (a.P0 != b.P0 || a.P1 != b.P1);
    }
}

#endif