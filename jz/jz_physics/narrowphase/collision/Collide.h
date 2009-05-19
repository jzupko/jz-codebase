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

#ifndef _JZ_PHYSICS_COLLIDE_H_
#define _JZ_PHYSICS_COLLIDE_H_

#include <jz_core/Prereqs.h>
#include <jz_physics/narrowphase/WorldContactPoint.h>

namespace jz
{
    struct CoordinateFrame3D;
    struct Triangle3D;
    struct Vector3;
    namespace physics
    {

        class ICollisionShape3D;
        class SphereShape;
        namespace Collide
        {

            #pragma region Discrete collision   
            static const float kMprTolerance = 1e-4f;

#if JZ_PROFILING
            extern unatural AverageMprCollideIterations;
            extern unatural AverageMprDistanceIterations;
#endif

            bool Collide(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            bool Collide(
                SphereShape const* a, const CoordinateFrame3D& acf,
                SphereShape const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            bool Collide(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            __inline bool Collide(
                Triangle3D const* a, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                bool bReturn = Collide(b, bcf, a, acf, cp);
                cp = WorldContactPoint3D::Flip(cp);

                return bReturn;
            }

            bool Collide(
                SphereShape const* a, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            __inline bool Collide(
                Triangle3D const* a, const CoordinateFrame3D& acf,
                SphereShape const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                bool bReturn = Collide(b, bcf, a, acf, cp);
                cp = WorldContactPoint3D::Flip(cp);

                return bReturn;
            }
            #pragma endregion

            #pragma region Continuous collision
            bool ContinuousCollide(
                ICollisionShape3D const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t);

            bool ContinuousCollide(
                ICollisionShape3D const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t);

            __inline bool ContinuousCollide(
                Triangle3D const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t)
            {
                bool bReturn = ContinuousCollide(b, pbcf, bcf, a, pacf, acf, cp, t);
                cp = WorldContactPoint3D::Flip(cp);

                return bReturn;
            }

            bool ContinuousCollide(
                SphereShape const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t);

            __inline bool ContinuousCollide(
                Triangle3D const* a, const CoordinateFrame3D& pacf, const CoordinateFrame3D& acf,
                SphereShape const* b, const CoordinateFrame3D& pbcf, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp, float& t)
            {
                bool bReturn = ContinuousCollide(b, pbcf, bcf, a, pacf, acf, cp, t);
                cp = WorldContactPoint3D::Flip(cp);

                return bReturn;
            }
            #pragma endregion

            #pragma region Distance estimate
            Vector3 ClosestPointOnTriangleToP(const Vector3& p, const Triangle3D& t);

            float ConservativeDistanceEstimate(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            float ConservativeDistanceEstimate(
                ICollisionShape3D const* a, const CoordinateFrame3D& acf,
                Triangle3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            __inline float ConservativeDistanceEstimate(
                Triangle3D const* a, const CoordinateFrame3D& acf,
                ICollisionShape3D const* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp)
            {
                float ret = ConservativeDistanceEstimate(b, bcf, a, acf, cp);
                cp = WorldContactPoint3D::Flip(cp);

                return ret;
            }
            #pragma endregion

        }

    }
}

#endif
