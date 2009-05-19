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

#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Segment.h>
#include <jz_core/Triangle3D.h>
#include <jz_core/Vector3.h>
#include <jz_physics/World.h>
#include <jz_physics/narrowphase/collision/ICollisionShape.h>
#include <jz_physics/narrowphase/collision/Collide.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>

namespace jz
{
    namespace physics
    {

        namespace Collide
        {

            #pragma region Helpers
            struct Triangle
            {
                static Triangle Create(const Triangle3D& t)
                {
                    Triangle ret;
                    ret.T = t;

                    return ret;
                }

                Vector3 GetSupport(const Vector3& n) const
                {
                    if (Vector3::Dot(T.P2 - T.P0, n) > 0.0f)
                    {
                        if (Vector3::Dot(T.P2 - T.P1, n) > 0.0f) { return T.P2; }
                        else { return T.P1; }
                    }
                    else
                    {
                        if (Vector3::Dot(T.P1 - T.P0, n) > 0.0f) { return T.P1; }
                        else { return T.P0; }
                    }
                }

                Vector3 GetSupport(const CoordinateFrame3D& cf, const Vector3& n) const
                {
                    Vector3 p = GetSupport(Vector3::TransformDirection(CoordinateFrame3D::Invert(cf), n));
                    p = Vector3::TransformPosition(cf, p);

                    return p;
                }

                Vector3 GetCenter() const
                {
                    return (T.GetCenter());
                }

                Triangle3D T;
            };
            #pragma endregion

            #pragma region Discrete collision 
#           if JZ_PROFILING
                unatural AverageMprCollideIterations = 0u;
                unatural AverageMprDistanceIterations = 0u;
#           endif

#           include <jz_physics/narrowphase/collision/_Collide.h>

            bool Collide(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                if (!a || !b) { return false; }

                if (a->GetType() == ICollisionShape3D::kSphere &&
                    b->GetType() == ICollisionShape3D::kSphere)
                {
                    return Collide((SphereShape*)a, acf, (SphereShape*)b, bcf, cp);
                }
                else
                {
                    return MprCollide(a, acf, b, bcf, cp);
                }
            }

            bool Collide(
                SphereShape const* a, const CoordinateFrame3D& acf,
                SphereShape const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                Vector3 wa = (acf.Translation);
                Vector3 wb = (bcf.Translation);
                float d2 = Vector3::DistanceSquared(wa, wb);
                float r = (a->Radius + b->Radius);
                float r2 = (r * r);

                if (d2 < r2)
                {
                    float d = Sqrt(d2);
                    cp.WorldNormal = (wb - wa);
                    if (d > Constants<float>::kZeroTolerance) { cp.WorldNormal /= d; }

                    cp.WorldPointA = (wa + (cp.WorldNormal * a->Radius));
                    cp.WorldPointB = (wb - (cp.WorldNormal * b->Radius));

                    return true;
                }

                return false;
            }

            bool Collide(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                if (a->GetType() == ICollisionShape3D::kSphere)
                {
                    return Collide((SphereShape*)a, acf, b, bcf, cp);
                }
                else
                {
                    Triangle t;
                    t.T = *b;

                    return MprCollide(a, acf, &t, bcf, cp);
                }
            }

            bool Collide(
                SphereShape const* a, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                Vector3 center = Vector3::TransformPosition(CoordinateFrame3D::Invert(bcf), acf.Translation);
                Vector3 p = Collide::ClosestPointOnTriangleToP(center, *b);
                Vector3 v = (p - center);

                float lenSq = (v.LengthSquared());
                bool bReturn = (lenSq <= (a->Radius * a->Radius));

                if (bReturn)
                {
                    float l = Sqrt(lenSq);
                    float d = (l - a->Radius);
                    Vector3 n = (-v / l);

                    cp.WorldNormal = Vector3::TransformDirection(bcf, n);
                    cp.WorldPointB = Vector3::TransformPosition(bcf, p);
                    cp.WorldPointA = (cp.WorldPointB + (d * cp.WorldNormal));
                }

                return bReturn;
            }
            #pragma endregion

            #pragma region Continuous collision
            // Temp: TODO, FIX
            template <typename T1, typename T2>
            bool ConservativeAdvancementCollide(
                T1 const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                T2 const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t)
            {
                t = 0.0f;
                Segment segment = Segment::Create(pacf.Translation - pbcf.Translation, acf.Translation - bcf.Translation);
                float maxDistance = segment.Distance();

                if (AboutZero(maxDistance))
                {
                    t = 1.0f;
                    return Collide(a, acf, b, bcf, cp);
                }

                while (t >= 0.0f && t <= 1.0f)
                {
                    CoordinateFrame3D ca = CoordinateFrame3D::Lerp(pacf, acf, t);
                    CoordinateFrame3D cb = CoordinateFrame3D::Lerp(pbcf, bcf, t);

                    float distance = ConservativeDistanceEstimate(a, ca, b, cb, cp);
                    
                    // cp has valid data if distance == 0.0f
                    if (distance == 0.0f) { return true; }
                    else
                    {
                        distance = Max(distance, Constants<float>::kLooseTolerance);
                        t += (distance / maxDistance);
                    }
                }

                t = Min(t, 1.0f);

                return false;
            }

            bool ContinuousCollide(
                ICollisionShape3D const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t)
            {
                return ConservativeAdvancementCollide(a, pacf, acf, b, pbcf, bcf, cp, t);
            }

            bool ContinuousCollide(
                ICollisionShape3D const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t)
            {
                return ConservativeAdvancementCollide(a, pacf, acf, b, pbcf, bcf, cp, t);
            }

            // Temp: TODO, FIX
            // Todo: this assumes no orientation changes in the CoordinateFrame. FIX.
            bool ContinuousCollide(
                SphereShape const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t)
            {
                t = 0.0f;
                Segment segment = Segment::Create(pacf.Translation - pbcf.Translation, acf.Translation - bcf.Translation);
                float maxDistance = segment.Distance();

                if (AboutZero(maxDistance))
                {
                    t = 1.0f;
                    return Collide(a, acf, b, bcf, cp);
                }

                Segment referenceSegment = Segment::Create(pbcf.Translation, bcf.Translation);

                float r2 = (a->Radius * a->Radius);
                while (t >= 0.0f && t <= 1.0f)
                {
                    Vector3 center = segment.Evaluate(t);
                    Vector3 p = Collide::ClosestPointOnTriangleToP(center, *b);
                    Vector3 v = (p - center);

                    float lenSq = (v.LengthSquared());
                    if (lenSq <= r2)
                    {
                        float l = Sqrt(lenSq);
                        float d = (l - a->Radius);
                        Vector3 n = (-v / l);

                        cp.WorldNormal = n;
                        cp.WorldPointB = (p + referenceSegment.Evaluate(t));
                        cp.WorldPointA = (cp.WorldPointB + (d * cp.WorldNormal));
                        return true;
                    }
                    else
                    {
                        float distance = Max(Sqrt(lenSq) - a->Radius, Constants<float>::kLooseTolerance);
                        t += (distance / maxDistance);
                    }
                }

                t = Min(t, 1.0f);

                return false;
            }
            #pragma endregion

            #pragma region Distance estimate
#           define JZ_MPR_DISTANCE_DEFINE 1
#           include <jz_physics/narrowphase/collision/_Collide.h>

            /// From: Ericson, C. 2005. "Real-Time Collision Detection",
            ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 141
            Vector3 ClosestPointOnTriangleToP(const Vector3& p, const Triangle3D& t)
            {
                const Vector3& a = t.P0;
                const Vector3& b = t.P1;
                const Vector3& c = t.P2;

                const Vector3 ab = (b - a);
                const Vector3 ac = (c - a);
                const Vector3 ap = (p - a);

                float d1 = Vector3::Dot(ab, ap);
                float d2 = Vector3::Dot(ac, ap);
                if (d1 <= 0.0f && d2 <= 0.0f) { return a; }

                Vector3 bp = (p - b);
                float d3 = Vector3::Dot(ab, bp);
                float d4 = Vector3::Dot(ac, bp);
                if (d3 >= 0.0f && d4 <= d3) { return b; }

                float vc = (d1 * d4) - (d3 * d2);
                if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
                {
                    float v = (d1 / (d1 - d3));
                    return (a + (v * ab));
                }

                Vector3 cp = (p - c);
                float d5 = Vector3::Dot(ab, cp);
                float d6 = Vector3::Dot(ac, cp);
                if (d6 >= 0.0f && d5 <= d6) { return c; }

                float vb = (d5 * d2) - (d1 * d6);
                if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
                {
                    float w = (d2 / (d2 - d6));
                    return (a + (w * ac));
                }

                float va = (d3 * d6) - (d5 * d4);
                if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
                {
                    float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
                    return (b + (w * (c - b)));
                }

                float denom = (1.0f / (va + vb + vc));
                float v = (vb * denom);
                float w = (vc * denom);

                return (a + (ab * v) + (ac * w));
            }

            float ConservativeDistanceEstimate(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                return MprDistance(a, acf, b, bcf, cp);
            }

            float ConservativeDistanceEstimate(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                Triangle t;
                t.T = *b;

                return MprDistance(a, acf, &t, bcf, cp);
            }
            #pragma endregion

        }
    }
}

