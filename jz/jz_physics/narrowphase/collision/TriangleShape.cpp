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

#include <jz_physics/narrowphase/collision/TriangleShape.h>

namespace jz
{
    namespace physics
    {

        Vector3 TriangleShape::GetSupport(const Vector3& n) const
        {
            if (Vector3::Dot(mTriangle.P2 - mTriangle.P0, n) > 0.0f)
            {
                if (Vector3::Dot(mTriangle.P2 - mTriangle.P1, n) > 0.0f) { return mTriangle.P2; }
                else { return mTriangle.P1; }
            }
            else
            {
                if (Vector3::Dot(mTriangle.P1 - mTriangle.P0, n) > 0.0f) { return mTriangle.P1; }
                else { return mTriangle.P0; }
            }
        }

    }
}