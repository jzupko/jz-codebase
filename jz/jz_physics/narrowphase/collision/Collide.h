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

namespace jz
{
    struct CoordinateFrame2D;
    struct CoordinateFrame3D;
    struct BoundingSphere;
    struct Plane;
    struct Triangle3D;
    struct Vector3;
    namespace physics
    {

        class ICollisionShape2D;
        class ICollisionShape3D;
        class Body2D;
        class Body3D;
        class CircleShape;
        class ConvexShape2D;
        class ConvexShape3D;
        class Box2DShape;
        class Box3DShape;
        class SphereShape;
        class TriangleShape;
        class TriangleTreeShape;
        struct WorldContactPoint2D;
        struct WorldContactPoint3D;
        namespace Collide
        {

            #pragma region Collision2D
            bool Collide(
                ICollisionShape2D* a, const CoordinateFrame2D& acf,
                ICollisionShape2D* b, const CoordinateFrame2D& bcf,
                WorldContactPoint2D& cp);

            bool Collide(
                CircleShape* a, const CoordinateFrame2D& acf,
                CircleShape* b, const CoordinateFrame2D& bcf,
                WorldContactPoint2D& cp);

            bool Collide(
                ConvexShape2D* a, const CoordinateFrame2D& acf,
                ConvexShape2D* b, const CoordinateFrame2D& bcf,
                WorldContactPoint2D& cp);
            #pragma endregion

            #pragma region Collision3D
            bool Collide(
                ICollisionShape3D* a, const CoordinateFrame3D& acf,
                ICollisionShape3D* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            bool Collide(
                SphereShape* a, const CoordinateFrame3D& acf,
                SphereShape* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            bool Collide(
                ConvexShape3D* a, const CoordinateFrame3D& acf,
                ConvexShape3D* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);

            bool Collide(
                SphereShape* a, const CoordinateFrame3D& acf,
                TriangleShape* b, const CoordinateFrame3D& bcf,
                WorldContactPoint3D& cp);
            #pragma endregion

            Vector3 ClosestPointOnTriangleToP(const Vector3& p, const Triangle3D& t);

        }

    }
}

#endif
