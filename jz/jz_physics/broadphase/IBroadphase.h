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
#ifndef _JZ_PHYSICS_IBROADPHASE_H_
#define _JZ_PHYSICS_IBROADPHASE_H_

#include <jz_core/Auto.h>
#include <jz_core/BoundingBox.h>
#include <jz_core/BoundingRectangle.h>

namespace jz
{
    namespace physics
    {

        class IBroadphase2D
        {
        public:
            IBroadphase2D()
                : mReferenceCount(0u)
            {}

            virtual ~IBroadphase2D() {}

            virtual u16 Add(void_p apCollideable, u32 aType, u32 aCollidesWith, const BoundingRectangle& aBounding) = 0;
            virtual void Remove(u16 aHandle) = 0;
            virtual float GetUnitMeter() const = 0;
            virtual void SetUnitMeter(float v) = 0;
            virtual void Tick() = 0;
            virtual void Update(u16 aHandle, const BoundingRectangle& aNewBounding) = 0;

        protected:
            size_t mReferenceCount;
            friend void ::jz::__IncrementRefCount<IBroadphase2D>(IBroadphase2D*);
            friend void ::jz::__DecrementRefCount<IBroadphase2D>(IBroadphase2D*);
        };
        typedef AutoPtr<IBroadphase2D> IBroadphase2DPtr;

        class IBroadphase3D
        {
        public:
            IBroadphase3D()
                : mReferenceCount(0u)
            {}

            virtual ~IBroadphase3D() {}

            virtual u16 Add(void_p apCollideable, u32 aType, u32 aCollidesWith, const BoundingBox& aBounding) = 0;
            virtual void Remove(u16 aHandle) = 0;
            virtual float GetUnitMeter() const = 0;
            virtual void SetUnitMeter(float v) = 0;
            virtual void Tick() = 0;
            virtual void Update(u16 aHandle, const BoundingBox& aNewBounding) = 0;

        protected:
            size_t mReferenceCount;
            friend void ::jz::__IncrementRefCount<IBroadphase3D>(IBroadphase3D*);
            friend void ::jz::__DecrementRefCount<IBroadphase3D>(IBroadphase3D*);
        };
        typedef AutoPtr<IBroadphase3D> IBroadphase3DPtr;

    }
}

#endif
