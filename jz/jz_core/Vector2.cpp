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

#include <jz_core/CoordinateFrame2D.h>
#include <jz_core/Matrix2.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Vector2.h>

namespace jz
{

    Vector2 Vector2::Transform(const Matrix2& m, const Vector2& v)
    {
        const float x = (m.M11 * v.X) + (m.M21 * v.Y);
        const float y = (m.M12 * v.X) + (m.M22 * v.Y);

        return Vector2(x, y);
    }

    Vector2 Vector2::TransformDirection(const CoordinateFrame2D& m, const Vector2& v)
    {
        const float x = (m.Orientation.M11 * v.X) + (m.Orientation.M21 * v.Y);
        const float y = (m.Orientation.M12 * v.X) + (m.Orientation.M22 * v.Y);

        return Vector2(x, y);
    }

    Vector2 Vector2::TransformPosition(const CoordinateFrame2D& m, const Vector2& v)
    {
        const float x = (m.Orientation.M11 * v.X) + (m.Orientation.M21 * v.Y) + m.Translation.X;
        const float y = (m.Orientation.M12 * v.X) + (m.Orientation.M22 * v.Y) + m.Translation.Y;

        return Vector2(x, y);
    }

    Vector2 Vector2::TransformDirection(const Matrix3& m, const Vector2& v)
    {
        const float x = (m.M11 * v.X) + (m.M21 * v.Y);
        const float y = (m.M12 * v.X) + (m.M22 * v.Y);

        return Vector2(x, y);
    }

    Vector2 Vector2::TransformPosition(const Matrix3& m, const Vector2& v)
    {
        const float x = (m.M11 * v.X) + (m.M21 * v.Y) + m.M31;
        const float y = (m.M12 * v.X) + (m.M22 * v.Y) + m.M32;

        return Vector2(x, y);
    }

    const Vector2 Vector2::kDown      = Vector2( 0, -1);
    const Vector2 Vector2::kLeft      = Vector2(-1,  0);
    const Vector2 Vector2::kMax       = Vector2(Constants<float>::kMax, Constants<float>::kMax);
    const Vector2 Vector2::kMin       = Vector2(Constants<float>::kMin, Constants<float>::kMin);
    const Vector2 Vector2::kNegOne    = Vector2(-1, -1);
    const Vector2 Vector2::kNegUnitX  = Vector2(-1,  0);
    const Vector2 Vector2::kNegUnitY  = Vector2( 0, -1);
    const Vector2 Vector2::kOne       = Vector2( 1,  1);
    const Vector2 Vector2::kRight     = Vector2( 1,  0);
    const Vector2 Vector2::kUnitX     = Vector2( 1,  0);
    const Vector2 Vector2::kUnitY     = Vector2( 0,  1);
    const Vector2 Vector2::kUp        = Vector2( 0,  1);
    const Vector2 Vector2::kZero      = Vector2( 0,  0);

}
