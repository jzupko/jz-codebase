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
#ifndef _JZ_COORDINATE_FRAME_3D_H_
#define _JZ_COORDINATE_FRAME_3D_H_

#include <jz_core/Matrix3.h>
#include <jz_core/Vector3.h>

namespace jz
{
    struct Matrix4;

    struct CoordinateFrame3D
    {
        static const CoordinateFrame3D kIdentity;
        static const CoordinateFrame3D kZero;

        Matrix3 Orientation;
        Vector3 Translation;

        CoordinateFrame3D()
            : Orientation(Matrix3::kIdentity), Translation(Vector3::kZero)
        {}

        CoordinateFrame3D(const Matrix3& aOrientation, const Vector3& aTranslation)
            : Orientation(aOrientation), Translation(aTranslation)
        { }

        static CoordinateFrame3D Invert(const CoordinateFrame3D& aFrame)
        {
            CoordinateFrame3D ret;
            ret.Orientation = Matrix3::Transpose(aFrame.Orientation);
            ret.Translation = Vector3::Transform(ret.Orientation, -aFrame.Translation);

            return ret;
        }

        static CoordinateFrame3D Lerp(const CoordinateFrame3D& a, const CoordinateFrame3D& b, float aWeightOfB)
        {
            CoordinateFrame3D ret;
            ret.Orientation = Matrix3::Lerp(a.Orientation, b.Orientation, aWeightOfB);
            ret.Translation = Vector3::Lerp(a.Translation, b.Translation, aWeightOfB);

            return ret;
        }
    };

    __inline CoordinateFrame3D operator*(const CoordinateFrame3D& a, const CoordinateFrame3D& b)
    {
        CoordinateFrame3D ret;
        ret.Orientation = (a.Orientation * b.Orientation);
        ret.Translation = Vector3::Transform(b.Orientation, a.Translation) + b.Translation;

        return ret;
    }

    void FromTransform(const Matrix4& a, CoordinateFrame3D& b);
    void ToTransform(const CoordinateFrame3D& a, Matrix4& b);

}

#endif
