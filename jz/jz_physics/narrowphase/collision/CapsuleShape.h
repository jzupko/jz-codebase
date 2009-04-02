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
#ifndef _JZ_PHYSICS_CAPSULE_SHAPE_H_
#define _JZ_PHYSICS_CAPSULE_SHAPE_H_

#include <jz_physics/narrowphase/collision/ICollisionShape.h>

namespace jz
{
    namespace physics
    {

        class CapsuleShape : public ICollisionShape3D
        {
        public:
            CapsuleShape(float aHalfHeight = 1.0f, float aRadius = 0.0f)
                : ICollisionShape3D(ICollisionShape3D::kCapsule), HalfHeight(aHalfHeight), Radius(aRadius)
            {}
            virtual ~CapsuleShape() {}

            float HalfHeight;
            float Radius;

            virtual BoundingBox GetBounding() const override
            {
                return (BoundingBox(
                    Vector3(-Radius, -(HalfHeight + Radius), -Radius),
                    Vector3(Radius, (HalfHeight + Radius), Radius)));
            }

            virtual Vector3 GetSupport(const Vector3& an) const override
            {
                Vector3 n = Vector3::Normalize(an);
                Vector3 p = (Radius * n);
                Vector3 p0 = Vector3(p.X, (p.Y - HalfHeight), p.Z);
                Vector3 p1 = Vector3(p.X, (p.Y + HalfHeight), p.Z);

                Vector3 ret;
                if (Vector3::Dot((p1 - p0), n) > 0.0f) { ret = p1; }
                else { ret = p0; }

                return ret;
            }

        protected:
            friend void ::jz::__IncrementRefCount<physics::CapsuleShape>(physics::CapsuleShape*);
            friend void ::jz::__DecrementRefCount<physics::CapsuleShape>(physics::CapsuleShape*);
        };
        typedef AutoPtr<CapsuleShape> CapsuleShapePtr;

    }
}

#endif
