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
#include <jz_core/Vector3.h>
#include <vector>

namespace jz
{
    namespace physics
    {

        class IBroadphase3D; typedef AutoPtr<IBroadphase3D> IBroadphase3DPtr;
        class ICollisionShape3D;
        class Body3D; typedef AutoPtr<Body3D> Body3DPtr;
        struct WorldContactPoint3D;
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

#           if JZ_PROFILING
                unatural AverageCollisionPairs;
#           endif

        private:
#           if JZ_PROFILING
                unatural mAverageCollisionPairs;
#           endif

            typedef vector<Body3D*> Bodies;
            IBroadphase3DPtr mpBroadphase;
            Bodies mBodies;
            Vector3 mGravity;
            float mTimePool;
            float mUnitMeter;

        protected:
            World3D(const World3D&);
            World3D& operator=(const World3D&);

            friend class Body3D;

            void _StartStopCollisionHandler(void_p a, void_p b);
            void _UpdateCollisionHandler(void_p a, void_p b);
            void _ResolveConvexCollision(Body3D* pa, Body3D* pb, const WorldContactPoint3D& cp);
            void _ResolveConcaveCollision(Body3D* pa, Body3D* pb);

            void _Add(Body3D* apBody, u32 aType, u32 aCollidesWith);
            void _Remove(Body3D* apBody);
            void _Update(Body3D* apBody, const BoundingBox& aBoundingBox);
        };

    }
}

#endif
