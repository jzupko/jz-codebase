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

/// Implementation from Gary Snethen's XenoCollide: http://xenocollide.snethen.com

#if JZ_MPR_DISTANCE_DEFINE
template <typename T1, typename T2>
float MprDistance(
    T1 const* a, const CoordinateFrame3D& acf,
    T2 const* b, const CoordinateFrame3D& bcf,
    WorldContactPoint3D& cp)
#else
template <typename T1, typename T2>
bool MprCollide(
    T1 const* a, const CoordinateFrame3D& acf,
    T2 const* b, const CoordinateFrame3D& bcf,
    WorldContactPoint3D& cp)
#endif
{
    Vector3 v01 = Vector3::TransformPosition(acf, a->GetCenter());
    Vector3 v02 = Vector3::TransformPosition(bcf, b->GetCenter());
    Vector3 v0 = (v02 - v01);
    if (Vector3::AboutZero(v0)) { v0 = Vector3::kForward * jz::Constants<float>::kLooseTolerance; }

    Vector3 normal = -v0;
    Vector3 v11 = a->GetSupport(acf, -normal);
    Vector3 v12 = b->GetSupport(bcf, normal);
    Vector3 v1 = (v12 - v11);

#   if !JZ_MPR_DISTANCE_DEFINE
        float dot = Vector3::Dot(v1, normal);
        if (dot <= 0.0f)
        {
            cp.WorldNormal = Vector3::Normalize(normal);
            cp.WorldPointA = Vector3::kZero;
            cp.WorldPointB = Vector3::kZero;
            return false;
        }
#   endif

    normal = Vector3::Cross(v1, v0);
    if (Vector3::AboutZero(normal))
    {
        normal = Vector3::Normalize(v1 - v0);

        cp.WorldNormal = normal;
        cp.WorldPointA = v11;
        cp.WorldPointB = v12;

#       if JZ_MPR_DISTANCE_DEFINE
            return (0.0f);
#       else
            return true;
#       endif
    }

    Vector3 v21 = a->GetSupport(acf, -normal);
    Vector3 v22 = b->GetSupport(bcf, normal);
    Vector3 v2 = (v22 - v21);

#   if !JZ_MPR_DISTANCE_DEFINE
        dot = Vector3::Dot(v2, normal);
        if (dot <= 0.0f)
        {
            cp.WorldNormal = normal;
            cp.WorldPointA = Vector3::kZero;
            cp.WorldPointB = Vector3::kZero;
            return false;
        }
#   endif

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
        Vector3 v31 = a->GetSupport(acf, -normal);
        Vector3 v32 = b->GetSupport(bcf, normal);
        Vector3 v3 = (v32 - v31);

#       if !JZ_MPR_DISTANCE_DEFINE
            dot = Vector3::Dot(v3, normal);

            if (dot <= 0.0f)
            {
                cp.WorldNormal = Vector3::Normalize(normal);
                cp.WorldPointA = Vector3::kZero;
                cp.WorldPointB = Vector3::kZero;
                return false;
            }
#       endif

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
#if JZ_PROFILING
        unatural iterations = 1u;
#endif

        // refine the portal.
        while (true)
        {
            normal = Vector3::Normalize(Vector3::Cross((v2 - v1), (v3 - v1)));
            float dot = Vector3::Dot(normal, v1);

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

            Vector3 v41 = a->GetSupport(acf, -normal);
            Vector3 v42 = b->GetSupport(bcf, normal);
            Vector3 v4 = (v42 - v41);

            float delta = Vector3::Dot((v4 - v3), normal);
            float separation = -Vector3::Dot(v4, normal);

            // modified to deal with nans, need to verify that my logic is still correct.
#           if !JZ_MPR_DISTANCE_DEFINE
                if (!(delta > kMprTolerance) || !(separation < 0.0f))
#           else
                if (!(delta > kMprTolerance))
#           endif
            {
                cp.WorldNormal = normal;

                if (bDone)
                {
                    const Vector3& wa = cp.WorldPointA;
                    const Vector3& wb = cp.WorldPointB;
                    const Vector3& wn = cp.WorldNormal;

                    Vector3 ppA = (Vector3::Dot((v41 - wa), wn) * wn) + wa;
                    Vector3 ppB = (Vector3::Dot((v42 - wb), wn) * wn) + wb;

                    cp.WorldPointA = ppA;
                    cp.WorldPointB = ppB;
                }

#if JZ_PROFILING
#               if JZ_MPR_DISTANCE_DEFINE
                    AverageMprDistanceIterations = (AverageMprDistanceIterations + iterations) / 2;
#               else
                    AverageMprCollideIterations = (AverageMprCollideIterations + iterations) / 2;
#               endif
#endif

#               if JZ_MPR_DISTANCE_DEFINE
                    if (bDone) { return (0.0f); }
                    else
                    {
                        Plane p(v1, v2, v3);
                        return (Abs(p.GetD()));
                    }
#               else
                    return bDone;
#               endif
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

#if JZ_PROFILING
            iterations++;
#endif
        }
    }
}

#ifdef JZ_MPR_DISTANCE_DEFINE
#   undef JZ_MPR_DISTANCE_DEFINE
#endif