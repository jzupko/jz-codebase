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
#ifndef _JZ_RECTANGLE_H_
#define _JZ_RECTANGLE_H_

#include <jz_core/Prereqs.h>
#include <jz_core/Vector2.h>

namespace jz
{

    struct Rectangle sealed
    {
        Rectangle()
            : BottomLeft(Vector2::kZero), TopRight(Vector2::kOne)
        {}

        Rectangle(float x0, float y0, float x1, float y1)
            : BottomLeft(x0, y0), TopRight(x1, y1)
        {}

        Rectangle(const Vector2& bl, const Vector2& tr)
            : BottomLeft(bl), TopRight(tr)
        {}

        Vector2 BottomLeft;
        Vector2 TopRight;

        static const Rectangle kZeroToOne;
        static const Rectangle kNegOneToOne;
        static const Rectangle kNegOneToOneFlippedY;

        Rectangle operator*(const Rectangle& b) const
        {
            Rectangle ret;
            ret.BottomLeft = (BottomLeft * b.BottomLeft);
            ret.TopRight = (TopRight * b.TopRight);

            return ret;
        }
    };

    struct RectangleU
    {
        natural Left;
        natural Top;
        natural Right;
        natural Bottom;
    };

}

#endif
