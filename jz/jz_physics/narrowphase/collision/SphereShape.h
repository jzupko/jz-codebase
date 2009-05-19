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
#ifndef _JZ_PHYSICS_SPHERE_SHAPE_H_
#define _JZ_PHYSICS_SPHERE_SHAPE_H_

#include <jz_physics/narrowphase/collision/ICollisionShape.h>

namespace jz
{
    namespace physics
    {

        class SphereShape : public ICollisionShape3D
        {
        public:
            SphereShape(float aRadius = 0.0f)
                : ICollisionShape3D(ICollisionShape3D::kSphere), Radius(aRadius)
            {}
            virtual ~SphereShape() {}

            float Radius;

            virtual bool bRotationallyInvariant() const override { return true; }

            virtual Vector3 GetInertiaTensor(float aInverseMass) const;
            virtual Vector3 GetInverseInertiaTensor(float aInverseMass) const;

            virtual BoundingBox GetBounding() const override
            {
                return (BoundingBox(Vector3(-Radius), Vector3(Radius)));
            }

            virtual Vector3 GetSupport(const Vector3& n) const override
            {
                Vector3 ret = (Vector3::Normalize(n) * Radius);

                return ret;
            }

        protected:
            friend void ::jz::__IncrementRefCount<physics::SphereShape>(physics::SphereShape*);
            friend void ::jz::__DecrementRefCount<physics::SphereShape>(physics::SphereShape*);
        };
        typedef AutoPtr<SphereShape> SphereShapePtr;

    }
}

#endif
