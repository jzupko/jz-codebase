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
#include <jz_core/Matrix4.h>

namespace jz
{

    const CoordinateFrame3D CoordinateFrame3D::kIdentity = CoordinateFrame3D(Matrix3(1, 0, 0, 0, 1, 0, 0, 0, 1), Vector3(0));
    const CoordinateFrame3D CoordinateFrame3D::kZero = CoordinateFrame3D(Matrix3(0, 0, 0, 0, 0, 0, 0, 0, 0), Vector3(0));

    void FromTransform(const Matrix4& a, CoordinateFrame3D& b)
    {
        b.Orientation = Matrix3::CreateFromUpperLeft(a);
        b.Translation = a.GetTranslation();
    }

    void ToTransform(const CoordinateFrame3D& a, Matrix4& b)
    {
        b.SetOrientation(a.Orientation);
        b.SetTranslation(a.Translation);
    }

}