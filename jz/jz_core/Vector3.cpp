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

#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Quaternion.h>
#include <jz_core/Vector3.h>

namespace jz
{

    Vector3 Vector3::Transform(const Matrix3& m, const Vector3& v)
    {
        const float x = (m.M11 * v.X) + (m.M21 * v.Y) + (m.M31 * v.Z);
        const float y = (m.M12 * v.X) + (m.M22 * v.Y) + (m.M32 * v.Z);
        const float z = (m.M13 * v.X) + (m.M23 * v.Y) + (m.M33 * v.Z);

        return Vector3(x, y, z);
    }

    Vector3 Vector3::TransformDirection(const CoordinateFrame3D& m, const Vector3& v)
    {
        const float x = (m.Orientation.M11 * v.X) + (m.Orientation.M21 * v.Y) + (m.Orientation.M31 * v.Z);
        const float y = (m.Orientation.M12 * v.X) + (m.Orientation.M22 * v.Y) + (m.Orientation.M32 * v.Z);
        const float z = (m.Orientation.M13 * v.X) + (m.Orientation.M23 * v.Y) + (m.Orientation.M33 * v.Z);

        return Vector3(x, y, z);
    }

    Vector3 Vector3::TransformPosition(const CoordinateFrame3D& m, const Vector3& v)
    {
        const float x = (m.Orientation.M11 * v.X) + (m.Orientation.M21 * v.Y) + (m.Orientation.M31 * v.Z) + m.Translation.X;
        const float y = (m.Orientation.M12 * v.X) + (m.Orientation.M22 * v.Y) + (m.Orientation.M32 * v.Z) + m.Translation.Y;
        const float z = (m.Orientation.M13 * v.X) + (m.Orientation.M23 * v.Y) + (m.Orientation.M33 * v.Z) + m.Translation.Z;

        return Vector3(x, y, z);
    }

    Vector3 Vector3::TransformDirection(const Matrix4& m, const Vector3& v)
    {
        const float x = (m.M11 * v.X) + (m.M21 * v.Y) + (m.M31 * v.Z);
        const float y = (m.M12 * v.X) + (m.M22 * v.Y) + (m.M32 * v.Z);
        const float z = (m.M13 * v.X) + (m.M23 * v.Y) + (m.M33 * v.Z);

        return Vector3(x, y, z);
    }

    Vector3 Vector3::TransformPosition(const Matrix4& m, const Vector3& v)
    {
        const float x = (m.M11 * v.X) + (m.M21 * v.Y) + (m.M31 * v.Z) + m.M41;
        const float y = (m.M12 * v.X) + (m.M22 * v.Y) + (m.M32 * v.Z) + m.M42;
        const float z = (m.M13 * v.X) + (m.M23 * v.Y) + (m.M33 * v.Z) + m.M43;

        return Vector3(x, y, z);
    }

    Vector3 Vector3::Transform(const Quaternion& q, const Vector3& u)
    {
        Vector3 uv, uuv;
        Vector3 qvec(q.X, q.Y, q.Z);

        uv = Vector3::Cross(qvec, u);
        uuv = Vector3::Cross(qvec, uv);

        uv  *= 2.0f * q.W;
        uuv *= 2.0f;
        
        return u + uv + uuv;
    }


    const Vector3 Vector3::kBackward  = Vector3( 0,  0,  1);
    const Vector3 Vector3::kDown      = Vector3( 0, -1,  0);
    const Vector3 Vector3::kForward   = Vector3( 0,  0, -1);
    const Vector3 Vector3::kLeft      = Vector3(-1,  0,  0);
    const Vector3 Vector3::kMax       = Vector3(Constants<float>::kMax, Constants<float>::kMax, Constants<float>::kMax);
    const Vector3 Vector3::kMin       = Vector3(Constants<float>::kMin, Constants<float>::kMin, Constants<float>::kMin);
    const Vector3 Vector3::kNegOne    = Vector3(-1, -1, -1);    
    const Vector3 Vector3::kNegUnitX  = Vector3(-1,  0,  0);
    const Vector3 Vector3::kNegUnitY  = Vector3( 0, -1,  0);
    const Vector3 Vector3::kNegUnitZ  = Vector3( 0,  0, -1);
    const Vector3 Vector3::kOne       = Vector3( 1,  1,  1);
    const Vector3 Vector3::kRight     = Vector3( 1,  0,  0);
    const Vector3 Vector3::kUnitX     = Vector3( 1,  0,  0);
    const Vector3 Vector3::kUnitY     = Vector3( 0,  1,  0);
    const Vector3 Vector3::kUnitZ     = Vector3( 0,  0,  1);
    const Vector3 Vector3::kUp        = Vector3( 0,  1,  0);
    const Vector3 Vector3::kZero      = Vector3( 0,  0,  0);

}
