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
#ifndef _JZ_VECTOR4_H_
#define _JZ_VECTOR4_H_

#include <jz_core/Angle.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <type_traits>

namespace jz
{

    struct Matrix4;
    struct Vector4
    {
        union
        {
            struct
            {
                float X, Y, Z, W;
            };
            float pData[4];
        };

#       define JZ_NUMBER_ARRAY_NAME Vector4
#       define JZ_NUMBER_ARRAY_DIM  4
#       include <jz_core/_NumberArray.h>

        Vector4(const Vector3& v, float w)
            : X(v.X), Y(v.Y), Z(v.Z), W(w)
        {}

        Vector4(const Vector2& u, const Vector2& v)
            : X(u.X), Y(u.Y), Z(v.X), W(v.Y)
        {}

        static Vector4 Transform(const Matrix4& m, const Vector4& v);

        static const Vector4 kMax;        
        static const Vector4 kMin;
        static const Vector4 kNegOne;
        static const Vector4 kNegUnitX;
        static const Vector4 kNegUnitY;
        static const Vector4 kNegUnitZ;
        static const Vector4 kNegUnitW;
        static const Vector4 kOne;
        static const Vector4 kUnitX;
        static const Vector4 kUnitY;
        static const Vector4 kUnitZ;
        static const Vector4 kUnitW;
        static const Vector4 kZero;
    };

    inline Vector4 operator*(float s, const Vector4& v)
    {
        return (v * s);
    }

}

#endif
