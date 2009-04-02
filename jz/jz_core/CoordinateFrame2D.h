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
#ifndef _JZ_COORDINATE_FRAME_2D_H_
#define _JZ_COORDINATE_FRAME_2D_H_

#include <jz_core/Matrix2.h>
#include <jz_core/Vector2.h>

namespace jz
{
    struct Matrix3;

    struct CoordinateFrame2D
    {
        static const CoordinateFrame2D kIdentity;
        static const CoordinateFrame2D kZero;

        Matrix2 Orientation;
        Vector2 Translation;

        CoordinateFrame2D()
            : Orientation(Matrix2::kIdentity), Translation(Vector2::kZero)
        {}

        CoordinateFrame2D(const Matrix2& aOrientation, const Vector2& aTranslation)
            : Orientation(aOrientation), Translation(aTranslation)
        { }

        static CoordinateFrame2D Invert(const CoordinateFrame2D& aFrame)
        {
            CoordinateFrame2D ret;
            ret.Orientation = Matrix2::Transpose(aFrame.Orientation);
            ret.Translation = Vector2::Transform(ret.Orientation, -aFrame.Translation);

            return ret;
        }

        static CoordinateFrame2D Lerp(const CoordinateFrame2D& a, const CoordinateFrame2D& b, float aWeightOfB)
        {
            CoordinateFrame2D ret;
            ret.Orientation = Matrix2::Lerp(a.Orientation, b.Orientation, aWeightOfB);
            ret.Translation = Vector2::Lerp(a.Translation, b.Translation, aWeightOfB);

            return ret;
        }
    };

    inline CoordinateFrame2D operator*(const CoordinateFrame2D& a, const CoordinateFrame2D& b)
    {
        CoordinateFrame2D ret;
        ret.Orientation = (a.Orientation * b.Orientation);
        ret.Translation = Vector2::Transform(b.Orientation, a.Translation) + b.Translation;

        return ret;
    }

    void FromTransform(const Matrix3& a, CoordinateFrame2D& b);
    void ToTransform(const CoordinateFrame2D& a, Matrix3& b);

}

#endif
