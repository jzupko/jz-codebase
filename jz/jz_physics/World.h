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
#ifndef _JZ_PHYSICS_WORLD_H_
#define _JZ_PHYSICS_WORLD_H_

#include <jz_core/Auto.h>
#include <jz_core/BoundingBox.h>
#include <jz_core/BoundingRectangle.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <vector>

namespace jz
{
    namespace physics
    {

        class IBroadphase2D; typedef AutoPtr<IBroadphase2D> IBroadphase2DPtr;
        class IBroadphase3D; typedef AutoPtr<IBroadphase3D> IBroadphase3DPtr;
        class ICollisionShape2D;
        class ICollisionShape3D;
        class Body2D; typedef AutoPtr<Body2D> Body2DPtr;
        class Body3D; typedef AutoPtr<Body3D> Body3DPtr;
        struct WorldContactPoint2D;
        struct WorldContactPoint3D;
        class World2D sealed
        {
        public:
            static const Vector2 kDefaultGravity;
            static const float kTimeStep;

            World2D();
            ~World2D();

            Body2D* Create(ICollisionShape2D* apShape, u32 aType, u32 aCollidesWith);

            float GetUnitMeter() const { return mUnitMeter; }
            void SetUnitMeter(float v);

            Vector2 GetGravity() const { return (mGravity / mUnitMeter); }
            void SetGravity(const Vector2& g) { mGravity = (mUnitMeter * g); }

            void Tick(float aTimeStep);

        private:
            IBroadphase2DPtr mpBroadphase;
            vector<Body2D*> mBodies;
            Vector2 mGravity;
            float mTimePool;
            float mUnitMeter;

        protected:
            World2D(const World2D&);
            World2D& operator=(const World2D&);

            friend class Body2D;

            void _StartStopCollisionHandler(void_p a, void_p b);
            void _UpdateCollisionHelper(Body2D* pa, Body2D* pb, const WorldContactPoint2D& cp);
            void _UpdateCollisionConcaveHandler(Body2D* pa, Body2D* pb);
            void _UpdateCollisionHandler(void_p a, void_p b);

            void _Add(Body2D* apBody, u32 aType, u32 aCollidesWith);
            void _Remove(Body2D* apBody);
            void _Update(Body2D* apBody, const BoundingRectangle& aBoundingRectangle);
        };

        class World3D sealed
        {
        public:
            static const Vector3 kDefaultGravity;
            static const float kTimeStep;

            World3D();
            ~World3D();

            Body3D* Create(ICollisionShape3D* apShape, u32 aType, u32 aCollidesWith);

            float GetUnitMeter() const { return mUnitMeter; }
            void SetUnitMeter(float v);

            Vector3 GetGravity() const { return (mGravity / mUnitMeter); }
            void SetGravity(const Vector3& g) { mGravity = (mUnitMeter * g); }

            void Tick(float aTimeStep);

        private:
            IBroadphase3DPtr mpBroadphase;
            vector<Body3D*> mBodies;
            Vector3 mGravity;
            float mTimePool;
            float mUnitMeter;

        protected:
            World3D(const World3D&);
            World3D& operator=(const World3D&);

            friend class Body3D;

            void _StartStopCollisionHandler(void_p a, void_p b);
            void _UpdateCollisionHelper(Body3D* pa, Body3D* pb, const WorldContactPoint3D& cp);
            void _UpdateCollisionConcaveHandler(Body3D* pa, Body3D* pb);
            void _UpdateCollisionHandler(void_p a, void_p b);

            void _Add(Body3D* apBody, u32 aType, u32 aCollidesWith);
            void _Remove(Body3D* apBody);
            void _Update(Body3D* apBody, const BoundingBox& aBoundingBox);
        };

    }
}

#endif
