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

#include <jz_core/Triangle3D.h>

namespace jz
{

    /// <remarks>
    /// From: Ericson, C. 2005. "Real-Time Collision Detection",
    ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 204
    /// </remarks>
    bool Triangle3D::Intersects(const Vector3& av) const
    {
        Vector3 a = (P0 - av);
        Vector3 b = (P1 - av);
        Vector3 c = (P2 - av);

        Vector3 u = Vector3::Cross(b, c);
        Vector3 v = Vector3::Cross(c, a);
        if (Vector3::Dot(u, v) < 0.0f) { return false; }

        Vector3 w = Vector3::Cross(a, b);
        if (Vector3::Dot(u, w) < 0.0f) { return false; }

        return true;
    }

}