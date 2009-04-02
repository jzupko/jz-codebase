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
#ifndef _JZ_PHYSICS_CONVEX_SHAPE_H_
#define _JZ_PHYSICS_CONVEX_SHAPE_H_

#include <jz_physics/narrowphase/collision/ICollisionShape.h>

namespace jz
{
    namespace physics
    {

        class ConvexShape2D : public ICollisionShape2D
        {
        public:
            typedef MemoryBuffer<Vector2> Container;

            ConvexShape2D(const MemoryBuffer<Vector2>& aPoints = Container())
                : ICollisionShape2D(ICollisionShape2D::kConvex), mPoints(aPoints)
            {
                _Update();
            }
            virtual ~ConvexShape2D() {}

            const Container& GetNormals() const { return mNormals; }
            const Container& GetPoints() const { return mPoints; }
            virtual BoundingRectangle GetBounding() const override
            {
                return mBounding;
            }

        protected:
            friend void ::jz::__IncrementRefCount<physics::ConvexShape2D>(physics::ConvexShape2D*);
            friend void ::jz::__DecrementRefCount<physics::ConvexShape2D>(physics::ConvexShape2D*);

        private:
            BoundingRectangle mBounding;
            Container mNormals;
            Container mPoints;

            void _Update();
        };
        typedef AutoPtr<ConvexShape2D> ConvexShape2DPtr;

        class ConvexShape3D : public ICollisionShape3D
        {
        public:
            typedef MemoryBuffer<Vector3> Container;

            ConvexShape3D(const MemoryBuffer<Vector3>& aPoints = Container())
                : ICollisionShape3D(ICollisionShape3D::kConvex), mPoints(aPoints)
            {
                _Update();
            }
            virtual ~ConvexShape3D() {}

            const Container& GetPoints() const { return mPoints; }

            virtual BoundingBox GetBounding() const override
            {
                return mBounding;
            }

            virtual Vector3 GetSupport(const Vector3& n) const override;

        protected:
            friend void ::jz::__IncrementRefCount<physics::ConvexShape3D>(physics::ConvexShape3D*);
            friend void ::jz::__DecrementRefCount<physics::ConvexShape3D>(physics::ConvexShape3D*);

        private:
            BoundingBox mBounding;
            Container mPoints;

            void _Update();
        };
        typedef AutoPtr<ConvexShape3D> ConvexShape3DPtr;

    }
}

#endif
