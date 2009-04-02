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
#include <jz_core/BoundingRectangle.h>
#include <jz_core/Event.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <jz_physics/narrowphase/WorldContactPoint.h>

namespace jz
{
    namespace physics
    {

        class ICollisionShape2D; typedef AutoPtr<ICollisionShape2D> ICollisionShape2DPtr;
        class ICollisionShape3D; typedef AutoPtr<ICollisionShape3D> ICollisionShape3DPtr;
        class World2D;
        class World3D;
        class Body2D sealed
        {
        public:
            enum Flags
            {
                kNone = 0,
                kDynamic = (1 << 0),
                kStatic = (1 << 1)
            };

            ~Body2D();

            ICollisionShape2D* GetCollisionShape() const { return mpShape.Get(); }
            u32 GetCollidesWith() const { return mCollidesWith; }
            BoundingRectangle GetLocalBounding() const;
            u32 GetType() const { return mType; }

            const Vector2& GetPrevTranslation() const { return (mPrevTranslation); }
            const Vector2& GetTranslation() const { return (mTranslation); }
            void SetTranslation(const Vector2& v);

            const Vector2& GetVelocity() const { return mVelocity; }
            void SetVelocity(const Vector2& v) { mVelocity = v; }

            float GetFriction() const { return (mFriction); }
            void SetFriction(float v) { mFriction = Clamp(v, 0.0f, 1.0f); }

            float GetMass() const { return (mInverseMass > Constants<float>::kZeroTolerance) ? (1.0f / mInverseMass) : 0.0f; }
            void SetMass(float v) { mInverseMass = (v > Constants<float>::kZeroTolerance) ? (1.0f / v) : 0.0f; }

            void Update();

        private:
            size_t mReferenceCount;

            u32 mCollidesWith;
            u32 mType;

            friend void ::jz::__IncrementRefCount<Body2D>(Body2D*);
            friend void ::jz::__DecrementRefCount<Body2D>(Body2D*);

            friend class World2D;

            ushort mHandle;
            float mFriction;
            float mInverseMass;
            ICollisionShape2DPtr mpShape;
            Vector2 mPrevTranslation;
            Vector2 mTranslation;
            Vector2 mVelocity;
            World2D* mpWorld;

            Body2D(World2D* apWorld, ICollisionShape2D* apShape, u32 aType, u32 aCollidesWith);
        };
        typedef AutoPtr<Body2D> Body2DPtr;

        class Body3D sealed
        {
        public:
            enum Flags
            {
                kNone = 0,
                kDynamic = (1 << 0),
                kStatic = (1 << 1)
            };

            ~Body3D();

            ICollisionShape3D* GetCollisionShape() const { return mpShape.Get(); }
            u32 GetCollidesWith() const { return mCollidesWith; }
            BoundingBox GetLocalBounding() const;
            u32 GetType() const { return mType; }

            const Vector3& GetPrevTranslation() const { return (mPrevTranslation); }
            const Vector3& GetTranslation() const { return (mTranslation); }
            void SetTranslation(const Vector3& v);

            const Vector3& GetVelocity() const { return mVelocity; }
            void SetVelocity(const Vector3& v) { mVelocity = v; }

            float GetFriction() const { return (mFriction); }
            void SetFriction(float v) { mFriction = Clamp(v, 0.0f, 1.0f); }

            float GetMass() const { return (mInverseMass > Constants<float>::kZeroTolerance) ? (1.0f / mInverseMass) : 0.0f; }
            void SetMass(float v) { mInverseMass = (v > Constants<float>::kZeroTolerance) ? (1.0f / v) : 0.0f; }

            void Update();

        private:
            size_t mReferenceCount;

            u32 mCollidesWith;
            u32 mType;

            friend void ::jz::__IncrementRefCount<Body3D>(Body3D*);
            friend void ::jz::__DecrementRefCount<Body3D>(Body3D*);

            friend class World3D;

            ushort mHandle;
            float mFriction;
            float mInverseMass;
            ICollisionShape3DPtr mpShape;
            Vector3 mPrevTranslation;
            Vector3 mTranslation;
            Vector3 mVelocity;
            World3D* mpWorld;

            Body3D(World3D* apWorld, ICollisionShape3D* apShape, u32 aType, u32 aCollidesWith);
        };
        typedef AutoPtr<Body3D> Body3DPtr;


    }
}

#endif
