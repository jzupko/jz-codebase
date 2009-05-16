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
#ifndef _JZ_PHYSICS_BOX_SHAPE_H_
#define _JZ_PHYSICS_BOX_SHAPE_H_

#include <jz_physics/narrowphase/collision/ConvexShape.h>

namespace jz
{
    namespace physics
    {

        __inline ConvexShape2D::Container GetPoints(const BoundingRectangle& abr)
        {
            ConvexShape2D::Container ret(4);
            ret[0] = Vector2(abr.Min.X, abr.Min.Y);
            ret[1] = Vector2(abr.Min.X, abr.Max.Y);
            ret[2] = Vector2(abr.Max.X, abr.Max.Y);
            ret[3] = Vector2(abr.Max.X, abr.Min.Y);

            return ret;
        }

        class Box2DShape : public ConvexShape2D
        {
        public:
            Box2DShape(const BoundingRectangle& abr = BoundingRectangle::kZero)
                : ConvexShape2D(physics::GetPoints(abr))
            {}

            virtual ~Box2DShape() {}

        protected:
            friend void ::jz::__IncrementRefCount<physics::Box2DShape>(physics::Box2DShape*);
            friend void ::jz::__DecrementRefCount<physics::Box2DShape>(physics::Box2DShape*);

        };
        typedef AutoPtr<physics::Box2DShape> Box2DShapePtr;

        __inline ConvexShape3D::Container GetPoints(const BoundingBox& bb)
        {
            ConvexShape3D::Container ret(8);
            ret[0] = Vector3(bb.Min.X, bb.Min.Y, bb.Min.Z);
            ret[1] = Vector3(bb.Min.X, bb.Max.Y, bb.Min.Z);
            ret[2] = Vector3(bb.Max.X, bb.Max.Y, bb.Min.Z);
            ret[3] = Vector3(bb.Max.X, bb.Min.Y, bb.Min.Z);
            ret[4] = Vector3(bb.Min.X, bb.Min.Y, bb.Max.Z);
            ret[5] = Vector3(bb.Min.X, bb.Max.Y, bb.Max.Z);
            ret[6] = Vector3(bb.Max.X, bb.Max.Y, bb.Max.Z);
            ret[7] = Vector3(bb.Max.X, bb.Min.Y, bb.Max.Z);

            return ret;
        }

        class Box3DShape : public ConvexShape3D
        {
        public:
            Box3DShape(const BoundingBox& bb = BoundingBox::kZero)
                : ConvexShape3D(physics::GetPoints(bb))
            {}

            virtual ~Box3DShape() {}

        protected:
            friend void ::jz::__IncrementRefCount<physics::Box3DShape>(physics::Box3DShape*);
            friend void ::jz::__DecrementRefCount<physics::Box3DShape>(physics::Box3DShape*);

        };
        typedef AutoPtr<physics::Box3DShape> Box3DShapePtr;

    }
}

#endif
