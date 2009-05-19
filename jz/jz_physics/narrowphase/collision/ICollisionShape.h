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
#ifndef _JZ_PHYSICS_ICOLLISION_SHAPE_H_
#define _JZ_PHYSICS_ICOLLISION_SHAPE_H_

#include <jz_core/Auto.h>
#include <jz_core/Prereqs.h>
#include <jz_core/BoundingBox.h>
#include <jz_core/CoordinateFrame3D.h>

namespace jz
{
    namespace physics
    {

        class ICollisionShape3D abstract
        {
        public:
            enum Type
            {
                kNone = 0,
                kSphere = 1,
                kBox = 2,
                kConvex = 3,
                kTriangleTree = 4
            };

            virtual ~ICollisionShape3D() {}

            bool bConvex() const
            {
                return (mType <= kConvex);
            }

            virtual bool bRotationallyInvariant() const = 0;

            virtual Vector3 GetInertiaTensor(float aInverseMass) const = 0;
            virtual Vector3 GetInverseInertiaTensor(float aInverseMass) const = 0;
            Type GetType() const { return mType; }

            virtual BoundingBox GetBounding() const = 0;
            Vector3 GetCenter() const { return (GetBounding().Center()); }

            // Default implementation is provided to support
            // concave shapes.
            virtual Vector3 GetSupport(const Vector3& n) const
            {
                BoundingBox bb = GetBounding();
                
                Vector3 ret = (bb.Max);
                if (n.X < 0.0f) { ret.X = bb.Min.X; }
                if (n.Y < 0.0f) { ret.Y = bb.Min.Y; }
                if (n.Z < 0.0f) { ret.Z = bb.Min.Z; }

                return ret;
            }

            Vector3 GetSupport(const CoordinateFrame3D& cf, const Vector3& n) const
            {
                Vector3 p = GetSupport(Vector3::TransformDirection(CoordinateFrame3D::Invert(cf), n));
                p = Vector3::TransformPosition(cf, p);

                return p;
            }

        protected:
            size_t mReferenceCount;

            ICollisionShape3D(Type aType)
                : mType(aType), mReferenceCount(0u)
            {}

            friend void ::jz::__IncrementRefCount<physics::ICollisionShape3D>(physics::ICollisionShape3D*);
            friend void ::jz::__DecrementRefCount<physics::ICollisionShape3D>(physics::ICollisionShape3D*);

        private:
            Type mType;

            ICollisionShape3D(const ICollisionShape3D&);
            ICollisionShape3D& operator=(const ICollisionShape3D&);
        };
        typedef AutoPtr<ICollisionShape3D> ICollisionShape3DPtr;

    }
}

#endif
