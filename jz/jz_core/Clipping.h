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
#ifndef _JZ_CLIPPING_H_
#define _JZ_CLIPPING_H_

#include <jz_core/Memory.h>
#include <jz_core/Plane.h>
#include <jz_core/Vector3.h>

namespace jz
{

    typedef MemoryBuffer<Vector3> PositionBuffer;
    typedef MemoryBuffer<Plane> PlaneBuffer;

    Vector3 ComputeIntersection(const Vector3& v1, const Vector3& v2, const Plane& aPlane);
    size_t Clip(const PositionBuffer& aVertices, const Plane& aPlane, PositionBuffer& arNewVertices);
    size_t Clip(const PositionBuffer& aVertices, const PlaneBuffer& aPlanes, PositionBuffer& arNewVertices);

}

#endif
