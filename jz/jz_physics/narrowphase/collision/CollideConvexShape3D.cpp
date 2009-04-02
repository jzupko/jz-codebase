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

#include <jz_physics/World.h>
#include <jz_physics/narrowphase/WorldContactPoint.h>
#include <jz_physics/narrowphase/collision/Collide.h>
#include <jz_physics/narrowphase/collision/ConvexShape.h>

namespace jz
{
    namespace physics
    {
        namespace Collide
        {
            // Implementation of Gary Snethen's XenoCollide: http://xenocollide.snethen.com
            static bool _Collide(
                ICollisionShape3D* a, const CoordinateFrame3D& acf,
                ICollisionShape3D* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                cp = WorldContactPoint3D();

                Vector3 v01 = Vector3::TransformPosition(acf, a->GetBounding().Center());
                Vector3 v02 = Vector3::TransformPosition(bcf, b->GetBounding().Center());
                Vector3 v0 = (v02 - v01);

                if (Vector3::AboutZero(v0)) { v0 = Vector3::kForward * jz::Constants<float>::kLooseTolerance; }

                Vector3 normal = -v0;
                Vector3 v11 = Vector3::TransformPosition(acf, a->GetSupport(-normal));
                Vector3 v12 = Vector3::TransformPosition(bcf, b->GetSupport(normal));
                Vector3 v1 = (v12 - v11);
                float dot = Vector3::Dot(v1, normal);

                if (dot <= 0.0f)
                {
                    cp.WorldNormal = Vector3::Normalize(normal);
                    cp.WorldPointA = Vector3::kZero;
                    cp.WorldPointB = Vector3::kZero;
                    return false;
                }

                normal = Vector3::Cross(v1, v0);

                if (Vector3::AboutZero(normal))
                {
                    normal = Vector3::Normalize(v1 - v0);

                    cp.WorldNormal = normal;
                    cp.WorldPointA = v11;
                    cp.WorldPointB = v12;
                    return true;
                }

                Vector3 v21 = Vector3::TransformPosition(acf, a->GetSupport(-normal));
                Vector3 v22 = Vector3::TransformPosition(bcf, b->GetSupport(normal));
                Vector3 v2 = (v22 - v21);
                dot = Vector3::Dot(v2, normal);

                if (dot <= 0.0f)
                {
                    cp.WorldNormal = normal;
                    cp.WorldPointA = Vector3::kZero;
                    cp.WorldPointB = Vector3::kZero;
                    return false;
                }

                normal = Vector3::Cross((v1 - v0), (v2 - v0));
                float distance = Vector3::Dot(normal, v0);

                if (distance > 0.0f)
                {
                    jz::Swap(v1, v2);
                    jz::Swap(v11, v21);
                    jz::Swap(v12, v22);
                    normal = -normal;
                }

                // identify a portal
                while (true)
                {
                    Vector3 v31 = Vector3::TransformPosition(acf, a->GetSupport(-normal));
                    Vector3 v32 = Vector3::TransformPosition(bcf, b->GetSupport(normal));
                    Vector3 v3 = (v32 - v31);
                    dot = Vector3::Dot(v3, normal);

                    if (dot <= 0.0f)
                    {
                        cp.WorldNormal = Vector3::Normalize(normal);
                        cp.WorldPointA = Vector3::kZero;
                        cp.WorldPointB = Vector3::kZero;
                        return false;
                    }

                    // origin is outside (v1, v0, v3), eliminate v2 and loop
                    if (Vector3::Dot(Vector3::Cross(v1, v3), v0) < 0.0f)
                    {
                        v2 = v3;
                        v21 = v31;
                        v22 = v32;
                        normal = Vector3::Cross((v1 - v0), (v3 - v0));
                        continue;
                    }

                    // origin is outside (v3, v0, v2), eliminate v1 and loop
                    if (Vector3::Dot(Vector3::Cross(v3, v2), v0) < 0.0f)
                    {
                        v1 = v3;
                        v11 = v31;
                        v12 = v32;
                        normal = Vector3::Cross((v3 - v0), (v2 - v0));
                        continue;
                    }

                    bool bDone = false;

                    // refine the portal.
                    while (true)
                    {
                        normal = Vector3::Normalize(Vector3::Cross((v2 - v1), (v3 - v1)));
                        dot = Vector3::Dot(normal, v1);

                        if (dot >= 0.0f && !bDone)
                        {
                            float b0 = Vector3::Dot(Vector3::Cross(v1, v2), v3);
                            float b1 = Vector3::Dot(Vector3::Cross(v3, v2), v0);
                            float b2 = Vector3::Dot(Vector3::Cross(v0, v1), v3);
                            float b3 = Vector3::Dot(Vector3::Cross(v2, v1), v0);

                            float sum = (b0 + b1 + b2 + b3);

                            if (sum <= 0.0f)
                            {
                                b0 = 0.0f;
                                b1 = Vector3::Dot(Vector3::Cross(v2, v3), normal);
                                b2 = Vector3::Dot(Vector3::Cross(v3, v1), normal);
                                b3 = Vector3::Dot(Vector3::Cross(v1, v2), normal);

                                sum = (b1 + b2 + b3);
                            }

                            float inv = (1.0f / sum);

                            Vector3 wa = ((b0 * v01) + (b1 * v11) + (b2 * v21) + (b3 * v31)) * inv;
                            Vector3 wb = ((b0 * v02) + (b1 * v12) + (b2 * v22) + (b3 * v32)) * inv;

                            cp.WorldNormal = normal;
                            cp.WorldPointA = wa;
                            cp.WorldPointB = wb;
                            bDone = true;
                        }

                        Vector3 v41 = Vector3::TransformPosition(acf, a->GetSupport(-normal));
                        Vector3 v42 = Vector3::TransformPosition(bcf, b->GetSupport(normal));
                        Vector3 v4 = (v42 - v41);

                        float delta = Vector3::Dot((v4 - v3), normal);
                        float separation = -Vector3::Dot(v4, normal);

                        // modified to deal with nans, need to verify that my logic is still correct.
                        if (!(delta > jz::Constants<float>::kLooseTolerance) || !(separation < 0.0f))
                        {
                            cp.WorldNormal = normal;
                            return bDone;
                        }

                        float d1 = Vector3::Dot(Vector3::Cross(v4, v1), v0);
                        float d2 = Vector3::Dot(Vector3::Cross(v4, v2), v0);
                        float d3 = Vector3::Dot(Vector3::Cross(v4, v3), v0);

                        if (d1 < 0.0f)
                        {
                            if (d2 < 0.0f)
                            {
                                v1 = v4;
                                v11 = v41;
                                v12 = v42;
                            }
                            else
                            {
                                v3 = v4;
                                v31 = v41;
                                v32 = v42;
                            }
                        }
                        else
                        {
                            if (d3 < 0.0f)
                            {
                                v2 = v4;
                                v21 = v41;
                                v22 = v42;
                            }
                            else
                            {
                                v1 = v4;
                                v11 = v41;
                                v12 = v42;
                            }
                        }
                    }
                }
            }

            bool XenoCollide(
                ICollisionShape3D* a, const CoordinateFrame3D& acf,
                ICollisionShape3D* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                if (_Collide(a, acf, b, bcf, cp))
                {
                    const Vector3& wa = cp.WorldPointA;
                    const Vector3& wb = cp.WorldPointB;
                    const Vector3& wn = cp.WorldNormal;

                    Vector3 s1 = Vector3::TransformPosition(acf, a->GetSupport(-wn));
                    Vector3 s2 = Vector3::TransformPosition(bcf, b->GetSupport(wn));

                    Vector3 ppA = (Vector3::Dot((s1 - wa), wn) * wn) + wa;
                    Vector3 ppB = (Vector3::Dot((s2 - wb), wn) * wn) + wb;

                    cp.WorldPointA = ppA;
                    cp.WorldPointB = ppB;

                    return true;
                }

                return false;
            }

            bool Collide(
                ConvexShape3D* a, const CoordinateFrame3D& acf,
                ConvexShape3D* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                return (XenoCollide(a, acf, b, bcf, cp));
            }
        }
    }
}
