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
#ifndef _JZ_VECTOR2_H_
#define _JZ_VECTOR2_H_

#include <jz_core/Angle.h>
#include <type_traits>

namespace jz
{

    struct CoordinateFrame2D;
    struct Matrix2;
    struct Matrix3;
    struct Vector2
    {
        union
        {
            struct
            {
                float X, Y;
            };
            float pData[2];
        };

#       define JZ_NUMBER_ARRAY_NAME Vector2
#       define JZ_NUMBER_ARRAY_DIM  2
#       include <jz_core/_NumberArray.h>

        static float Cross(const Vector2& a, const Vector2& b)
        {
            float ret = ((a.X * b.Y) - (a.Y * b.X));

            return ret;
        }

        static Vector2 Transform(const Matrix2& m, const Vector2& v);
        static Vector2 TransformDirection(const CoordinateFrame2D& m, const Vector2& v);
        static Vector2 TransformPosition(const CoordinateFrame2D& m, const Vector2& v);
        static Vector2 TransformDirection(const Matrix3& m, const Vector2& v);
        static Vector2 TransformPosition(const Matrix3& m, const Vector2& v);

        static const Vector2 kDown;
        static const Vector2 kLeft;
        static const Vector2 kMax;        
        static const Vector2 kMin;
        static const Vector2 kNegOne;
        static const Vector2 kNegUnitX;
        static const Vector2 kNegUnitY;
        static const Vector2 kOne;
        static const Vector2 kRight;
        static const Vector2 kUnitX;
        static const Vector2 kUnitY;
        static const Vector2 kUp;
        static const Vector2 kZero;
    };

    inline Vector2 operator*(float s, const Vector2& v)
    {
        return (v * s);
    }

}

#endif
