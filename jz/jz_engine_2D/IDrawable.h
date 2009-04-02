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
#ifndef _JZ_ENGINE_2D_IDRAWABLE_H_
#define _JZ_ENGINE_2D_IDRAWABLE_H_

#include <jz_core/Auto.h>
#include <jz_core/BoundingRectangle.h>
#include <jz_core/Matrix3.h>

namespace jz
{
    namespace engine_2D
    {

        class Cell;
        class IDrawable abstract
        {
        public:
            enum DrawableType
            {
                kCell = (1 << 0),
                kLight = (1 << 1),
                kSprite = (1 << 2)
            };

            virtual BoundingRectangle GetBounding() const = 0;
            virtual float GetZdepth() const = 0;

            virtual void Pose(IDrawable* apDrawable) = 0;

            unatural GetDrawableType() const { return mDrawableType; }

        protected:
            virtual ~IDrawable() {}

            friend void ::jz::__IncrementRefCount<IDrawable>(IDrawable*);
            friend void ::jz::__DecrementRefCount<IDrawable>(IDrawable*);

            size_t mReferenceCount;
            unatural mDrawableType;

            IDrawable(unatural aType)
                : mReferenceCount(0u), mDrawableType(aType)
            {}

        private:
            IDrawable(const IDrawable&);
            IDrawable& operator=(const IDrawable&);
        };

        typedef AutoPtr<IDrawable> IDrawablePtr;

    }
}

#endif
