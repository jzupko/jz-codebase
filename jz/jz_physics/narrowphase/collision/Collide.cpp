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
#include <jz_core/CoordinateFrame2D.h>
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Segment.h>
#include <jz_physics/TriangleTree.h>
#include <jz_physics/World.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/WorldContactPoint.h>
#include <jz_physics/narrowphase/collision/ICollisionShape.h>
#include <jz_physics/narrowphase/collision/Collide.h>
#include <jz_physics/narrowphase/collision/ConvexShape.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>
#include <jz_physics/narrowphase/collision/TriangleShape.h>
#include <jz_physics/narrowphase/collision/TriangleTreeShape.h>

namespace jz
{
    namespace physics
    {

        namespace Collide
        {

            #pragma region Collision2D
            bool Collide(
                ICollisionShape2D* a, const CoordinateFrame2D& acf,
                ICollisionShape2D* b, const CoordinateFrame2D& bcf,
                WorldContactPoint2D& cp)
            {
                if (!a || !b) { return false; }

                if (a->GetType() == ICollisionShape2D::kCircle &&
                    b->GetType() == ICollisionShape2D::kCircle)
                {
                    CircleShape* ca = (CircleShape*)a;
                    CircleShape* cb = (CircleShape*)b;

                    return Collide(ca, acf, cb, bcf, cp);
                }
                else if (a->GetType() == ICollisionShape2D::kConvex &&
                         b->GetType() == ICollisionShape2D::kConvex)
                {
                    ConvexShape2D* ca = (ConvexShape2D*)a;
                    ConvexShape2D* cb = (ConvexShape2D*)b;

                    return Collide(ca, acf, cb, bcf, cp);
                }
                else
                {
                    return false;
                }
            }

            bool Collide(
                CircleShape* a, const CoordinateFrame2D& acf,
                CircleShape* b, const CoordinateFrame2D& bcf,
                WorldContactPoint2D& cp)
            {
                Vector2 wa = (acf.Translation);
                Vector2 wb = (bcf.Translation);
                float d2 = Vector2::DistanceSquared(wa, wb);
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
            #pragma endregion

            #pragma region Collision3D
            bool XenoCollide(
                ICollisionShape3D* a, const CoordinateFrame3D& acf,
                ICollisionShape3D* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            bool Collide(
                ICollisionShape3D* a, const CoordinateFrame3D& acf,
                ICollisionShape3D* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                if (!a || !b) { return false; }

                if (a->GetType() == ICollisionShape3D::kSphere &&
                    b->GetType() == ICollisionShape3D::kSphere)
                {
                    SphereShape* ca = (SphereShape*)a;
                    SphereShape* cb = (SphereShape*)b;

                    return Collide(ca, acf, cb, bcf, cp);
                }
                else if (a->GetType() == ICollisionShape3D::kConvex &&
                         b->GetType() == ICollisionShape3D::kConvex)
                {
                    ConvexShape3D* ca = (ConvexShape3D*)a;
                    ConvexShape3D* cb = (ConvexShape3D*)b;

                    return Collide(ca, acf, cb, bcf, cp);
                }
                else if (a->GetType() == ICollisionShape3D::kSphere &&
                        b->GetType() == ICollisionShape3D::kTriangle)
                {
                    SphereShape* ca = (SphereShape*)a;
                    TriangleShape* cb = (TriangleShape*)b;

                    return Collide(ca, acf, cb, bcf, cp);
                }
                else if (a->GetType() == ICollisionShape3D::kTriangle &&
                        b->GetType() == ICollisionShape3D::kSphere)
                {
                    TriangleShape* ca = (TriangleShape*)a;
                    SphereShape* cb = (SphereShape*)b;

                    bool bReturn = Collide(cb, bcf, ca, acf, cp);
                    cp = WorldContactPoint3D::Flip(cp);
                    return bReturn;
                }
                else if (a->bConvex() && b->bConvex())
                {
                    return (XenoCollide(a, acf, b, bcf, cp));
                }
                else
                {
                    return false;
                }
            }

            bool Collide(
                SphereShape* a, const CoordinateFrame3D& acf,
                SphereShape* b, const CoordinateFrame3D& bcf,
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
                SphereShape* a, const CoordinateFrame3D& acf,
                TriangleShape* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                static const float kFactor = (float)(1.0 / 3.0);

                Vector3 sc = Vector3::TransformPosition(acf, a->GetBounding().Center());
                
                Triangle3D t;
                t.P0 = Vector3::TransformPosition(bcf, b->mTriangle.P0);
                t.P1 = Vector3::TransformPosition(bcf, b->mTriangle.P1);
                t.P2 = Vector3::TransformPosition(bcf, b->mTriangle.P2);
                Vector3 p = ClosestPointOnTriangleToP(sc, t);
                
                Vector3 v = (p - sc);
                float lenSq = (v.LengthSquared());
                bool bReturn = (lenSq <= (a->Radius * a->Radius));

                if (bReturn)
                {
                    float l = Sqrt(lenSq);
                    float d = (l - a->Radius);
                    Vector3 n = (l > Constants<float>::kZeroTolerance) ? -(v / l) : Vector3::TransformDirection(bcf, b->mTriangle.GetNormal());

                    cp.WorldNormal = n;
                    cp.WorldPointB = p;
                    cp.WorldPointA = (p + (d * n));
                }

                return bReturn;
            }
            #pragma endregion

            /// From: Ericson, C. 2005. "Real-Time Collision Detection",
            ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 139
            Vector3 ClosestPointOnTriangleToP(const Vector3& p, const Triangle3D& t)
            {
                const Vector3& a = t.P0;
                const Vector3& b = t.P1;
                const Vector3& c = t.P2;

                const Vector3 ab = (b - a);
                const Vector3 ac = (c - a);
                const Vector3 bc = (c - b);

                float snom = Vector3::Dot(p - a, ab);
                float sdenom = Vector3::Dot(p - b, a - b);

                float tnom = Vector3::Dot(p - a, ac);
                float tdenom = Vector3::Dot(p - c, a - c);

                if (snom <= 0.0f && tnom <= 0.0f) { return a; }

                float unom = Vector3::Dot(p - b, bc);
                float udenom = Vector3::Dot(p - c, b - c);

                if (sdenom <= 0.0f && unom <= 0.0f) { return b; }
                if (tdenom <= 0.0f && udenom <= 0.0f) { return c; }

                Vector3 n = Vector3::Cross(b - a, c - a);
                float vc = Vector3::Dot(n, Vector3::Cross(a - p, b - p));

                if (vc <= 0.0f && snom >= 0.0f && sdenom >= 0.0f)
                {
                    return (a + ((snom / (snom + sdenom)) * ab));
                }

                float va = Vector3::Dot(n, Vector3::Cross(b - p, c - p));

                if (va <= 0.0f && unom >= 0.0f && udenom >= 0.0f)
                {
                    return (b + ((unom / (unom + udenom)) * bc));
                }

                float vb = Vector3::Dot(n, Vector3::Cross(c - p, a - p));

                if (vb <= 0.0f && tnom >= 0.0f && tdenom >= 0.0f)
                {
                    return (a + ((tnom / (tnom + tdenom)) * ac));
                }

                float u = (va / (va + vb + vc));
                float v = (vb / (va + vb + vc));
                float w = (1.0f - u - v);
                
                return ((u * a) + (v * b) + (w * c));
            }


            
        }
    }
}

