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
#ifndef _JZ_PHYSICS_BODY_H_
#define _JZ_PHYSICS_BODY_H_

#include <jz_core/Auto.h>
#include <jz_core/BoundingBox.h>
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Event.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Vector3.h>
#include <jz_physics/narrowphase/WorldContactPoint.h>

namespace jz
{
    namespace physics
    {

        class ICollisionShape3D; typedef AutoPtr<ICollisionShape3D> ICollisionShape3DPtr;
        class World3D;
        class Body3D sealed
        {
        public:
            enum Flags
            {
                kNone = 0,
                kDynamic = (1 << 0),
                kStatic = (1 << 1),
                kNonAngular = (1 << 2),
                kNotAffectedByGravity = (1 << 3),
                kSleeping = (1 << 4)
            };

            ~Body3D();

            bool IsAffectedByGravity() const { return ((mType & kNotAffectedByGravity) == 0 && !IsStatic()); }
            bool IsAngular() const { return ((mType & kNonAngular) == 0 && !IsStatic()); }
            bool IsDynamic() const { return ((mType & kDynamic) != 0 && !IsStatic()); }
            bool IsSleeping() const { return ((mType & kSleeping) != 0 || IsStatic()); }
            bool IsStatic() const { return ((mType & kStatic) != 0 || (mInverseMass == 0.0f)); }

            void SetAffectedByGravity(bool b)
            {
                if (b) { mType &= ~kNotAffectedByGravity; }
                else { mType |= kNotAffectedByGravity; }
            }

            void SetAngular(bool b)
            {
                if (b) { mType &= ~kNonAngular; }
                else { mType |= kNonAngular; }
            }

            void SetSleeping(bool b)
            {
                if (b) { mType |= kSleeping; }
                else { mType &= ~kSleeping; }
            }

            ICollisionShape3D* GetCollisionShape() const { return mpShape.Get(); }
            u32 GetCollidesWith() const { return mCollidesWith; }
            Vector3 GetInertiaTensor() const;
            Vector3 GetInverseInertiaTensor() const;
            BoundingBox GetLocalBounding() const;
            BoundingBox GetWorldBounding(const CoordinateFrame3D& v) const;
            BoundingBox GetWorldBounding() const { return GetWorldBounding(mFrame); }
            u32 GetType() const { return mType; }

            const CoordinateFrame3D& GetFrame() const { return (mFrame); }
            const Vector3& GetTranslation() const { return (mFrame.Translation); }

            void SetFrame(const CoordinateFrame3D& v);
            void SetOrientation(const Matrix3& v);
            void SetTranslation(const Vector3& v);

            const Vector3& GetAngularVelocity() const { return mAngularVelocity; }
            void SetAngularVelocity(const Vector3& v) { mAngularVelocity = v; }

            const Vector3& GetLinearVelocity() const { return mLinearVelocity; }
            void SetLinearVelocity(const Vector3& v) { mLinearVelocity = v; }

            float GetFriction() const { return (mFriction); }
            void SetFriction(float v) { mFriction = Clamp(v, 0.0f, 1.0f); }

            float GetMass() const { return (mInverseMass > Constants<float>::kZeroTolerance) ? (1.0f / mInverseMass) : 0.0f; }
            void SetMass(float v) { mInverseMass = (v > Constants<float>::kZeroTolerance) ? (1.0f / v) : 0.0f; }

            void Update();
            Event<void(Body3D*)> OnUpdate;

        private:
            friend class World3D;

            friend void ::jz::__IncrementRefCount<Body3D>(Body3D*);
            friend void ::jz::__DecrementRefCount<Body3D>(Body3D*);

            size_t mReferenceCount;
            u32 mCollidesWith;
            u32 mType;
            float mFriction;
            float mInverseMass;
            World3D* mpWorld;
            ICollisionShape3DPtr mpShape;
            CoordinateFrame3D mPrevFrame;
            CoordinateFrame3D mFrame;
            Vector3 mAngularVelocity;
            Vector3 mLinearVelocity;
            ushort mHandle;

            Body3D(World3D* apWorld, ICollisionShape3D* apShape, u32 aType, u32 aCollidesWith);
        };
        typedef AutoPtr<Body3D> Body3DPtr;


    }
}

#endif
