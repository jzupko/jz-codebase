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

#include <jz_core/BoundingSphere.h>
#include <jz_core/Cylinder.h>
#include <jz_core/Plane.h>
#include <jz_core/Ray3D.h>
#include <jz_core/Segment.h>
#include <jz_core/Triangle3D.h>

namespace jz
{

    /// From: Ericson, C. 2005. "Real-Time Collision Detection",
    ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 149
    void Segment::ClosestPoint(const Segment& sa, const Segment& sb, float& s, float& t)
    {
        Vector3 d1 = sa.DirectedMagnitude();
        Vector3 d2 = sb.DirectedMagnitude();
        Vector3 r = (sa.P0 - sb.P0);

        float a = d1.LengthSquared();
        float e = d2.LengthSquared();

        if (a < Constants<float>::kZeroTolerance)
        {
            if (e < Constants<float>::kZeroTolerance)
            {
                s = 0.0f;
                t = 0.0f;
            }
            else
            {
                s = 0.0f;
                float f = Vector3::Dot(d2, r);
                t = Clamp((f / e), 0.0f, 1.0f);
            }
        }
        else if (e < Constants<float>::kZeroTolerance)
        {
            if (a < Constants<float>::kZeroTolerance)
            {
                s = 0.0f;
                t = 0.0f;
            }
            else
            {
                t = 0.0f;
                float c = Vector3::Dot(d1, r);
                s = Clamp(-(c / a), 0.0f, 1.0f);
            }
        }
        else
        {
            float b = Vector3::Dot(d1, d2);
            float denom = ((a * e) - (b * b));

            float f = Vector3::Dot(d2, r);
            float c = Vector3::Dot(d1, r);

            if (!AboutZero(denom))
            {
                s = Clamp(((b * f) - (c * e)) / denom, 0.0f, 1.0f);
            }
            else
            {
                s = 0.0f;
            }

            t = ((b * s) + f) / e;

            if (t < 0.0f)
            {
                t = 0.0f;
                s = Clamp((-c / a), 0.0f, 1.0f);
            }
            else if (t > 1.0f)
            {
                t = 1.0f;
                s = Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    bool Segment::Intersects(const BoundingSphere& bs, float& t) const
    {
        Vector3 ds = DirectedMagnitude();
        float len = ds.Length();

        if (len > Constants<float>::kZeroTolerance)
        {
            Vector3 direction = (ds / len);
            Ray3D r(P0, direction);
            
            if (r.Intersects(bs, t) && (t <= len))
            {
                t = (t / len);
                return true;
            }
        }
        else
        {
            t = 0.0f;
            return (bs.Intersects(P0));
        }

        return false;
    }

    /// From: Ericson, C. 2005. "Real-Time Collision Detection",
    ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 196
    bool Segment::Intersects(const Cylinder& cyl, float& t) const
    {
        Vector3 d = cyl.Axis.DirectedMagnitude();
        Vector3 m = (P0 - cyl.Axis.P0);
        Vector3 n = DirectedMagnitude();

        float md = Vector3::Dot(m, d);
        float nd = Vector3::Dot(n, d);
        float dd = Vector3::Dot(d, d);

        if (md < 0.0f && (md + nd) < 0.0f) { return false; }
        if (md > dd && (md + nd) > dd) { return false; }

        float nn = Vector3::Dot(n, n);
        float mn = Vector3::Dot(m, n);
        
        float a = (dd * nn) - (nd * nd);
        float k = Vector3::Dot(m, m) - (cyl.Radius * cyl.Radius);
        float c = (dd * k) - (md * md);

        if (Abs(a) < Constants<float>::kLooseTolerance)
        {
            if (c > 0.0f) { return false; }
            if (md < 0.0f) { t = (-mn / nn); }
            else if (md > dd) { t = (nd - mn) / nn; }
            else { t = 0.0f; }

            return true;
        }

        float b = (dd * mn) - (nd * md);
        float discr = (b * b) - (a * c);
        if (discr < 0.0f) { return false; }

        t = (-b - Sqrt(discr)) / a;
        if (t < 0.0f || t > 1.0f) { return false; }

        if (md + (t * nd) < 0.0f)
        {
            if (nd <= 0.0f) { return false; }
            t = (-md / nd);

            return ((k + (2.0f * t * (mn + (t * nn)))) <= 0.0f);
        }
        else if (md + (t * nd) > dd)
        {
            if (nd >= 0.0f) { return false; }
            t = (dd - md) / nd;

            return ((k + dd - (2.0f * md) + (t * (2.0f * (mn - nd) + (t * nn)))) <= 0.0f);
        }

        return true;
    }

    /// From: Ericson, C. 2005. "Real-Time Collision Detection",
    ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 176

    // See the reference for why an explicit divide-by-zero check is not 
    // necessary in this case.
    bool Segment::Intersects(const Plane& aPlane, float& t) const
    {
        Vector3 ab = DirectedMagnitude();
        t = (aPlane.GetD() - Vector3::Dot(aPlane.GetNormal(), P0)) / Vector3::Dot(aPlane.GetNormal(), ab);

        return (t >= 0.0f && t <= 1.0f);
    }

    /// From: Ericson, C. 2005. "Real-Time Collision Detection",
    ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 191
    bool Segment::Intersects(const Triangle3D& aTriangle, float& t) const
    {
        Vector3 ab = (aTriangle.P1 - aTriangle.P0);
        Vector3 ac = (aTriangle.P2 - aTriangle.P0);
        Vector3 qp = (P0 - P1);

        Vector3 n = Vector3::Cross(ab, ac);
        float d = Vector3::Dot(qp, n);
        if (d <= 0.0f) { return false; }

        Vector3 ap = (P0 - aTriangle.P0);
        t = Vector3::Dot(ap, n);
        if (t < 0.0f) { return false; }
        if (t > d) { return false; }

        Vector3 e = Vector3::Cross(qp, ap);
        float v = Vector3::Dot(ac, e);
        if (v < 0.0f || v > d) { return false; }

        float w = -Vector3::Dot(ab, e);
        if (w < 0.0f || (v + w) > d) { return false; }

        float ood = (1.0f / d);
        t *= ood;

        return true;
    }

}