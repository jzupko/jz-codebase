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

#include <jz_core/Matrix4.h>
#include <jz_core/Vector4.h>

namespace jz
{

    Vector4 Vector4::Transform(const Matrix4& m, const Vector4& v)
    {
        const float x = (m.M11 * v.X) + (m.M21 * v.Y) + (m.M31 * v.Z) + (m.M41 * v.W);
        const float y = (m.M12 * v.X) + (m.M22 * v.Y) + (m.M32 * v.Z) + (m.M42 * v.W);
        const float z = (m.M13 * v.X) + (m.M23 * v.Y) + (m.M33 * v.Z) + (m.M43 * v.W);
        const float w = (m.M14 * v.X) + (m.M24 * v.Y) + (m.M34 * v.Z) + (m.M44 * v.W);
        
        return Vector4(x, y, z, w);        
    }

    const Vector4 Vector4::kMax       = Vector4(Constants<float>::kMax, Constants<float>::kMax, Constants<float>::kMax, Constants<float>::kMax);
    const Vector4 Vector4::kMin       = Vector4(Constants<float>::kMin, Constants<float>::kMin, Constants<float>::kMin, Constants<float>::kMin);
    const Vector4 Vector4::kNegOne    = Vector4(-1, -1, -1, -1);
    const Vector4 Vector4::kNegUnitX  = Vector4(-1,  0,  0,  0);
    const Vector4 Vector4::kNegUnitY  = Vector4( 0, -1,  0,  0);
    const Vector4 Vector4::kNegUnitZ  = Vector4( 0,  0, -1,  0);
    const Vector4 Vector4::kNegUnitW  = Vector4( 0,  0,  0, -1);
    const Vector4 Vector4::kOne       = Vector4( 1,  1,  1,  1);
    const Vector4 Vector4::kUnitX     = Vector4( 1,  0,  0,  0);
    const Vector4 Vector4::kUnitY     = Vector4( 0,  1,  0,  0);
    const Vector4 Vector4::kUnitZ     = Vector4( 0,  0,  1,  0);
    const Vector4 Vector4::kUnitW     = Vector4( 0,  0,  0,  1);
    const Vector4 Vector4::kZero      = Vector4( 0,  0,  0,  0);

}
