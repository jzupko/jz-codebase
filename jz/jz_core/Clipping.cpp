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

#include <jz_core/Clipping.h>

namespace jz
{

    Vector3 ComputeIntersection(const Vector3& v1, const Vector3& v2, const Plane& aPlane)
    {
        Vector3 diff = v2 - v1;
        float a = Plane::DotNormal(diff, aPlane);
        float t = -(Plane::DotCoordinate(v1, aPlane) / a);

        Vector3 ret = v1 + (diff * t);
        return ret;
    }

    // Sutherland-Hodgman clipping algorithm
    // http://en.wikipedia.org/wiki/Sutherland-Hodgeman
    size_t Clip(const PositionBuffer& aVertices, const Plane& aPlane, PositionBuffer& arNewVertices)
    {
        size_t positive = 0;
        size_t negative = 0;
        const size_t kVertexCount = aVertices.size();
        MemoryBuffer<PlaneIntersection::Type> intersections(kVertexCount);

        for (size_t i = 0; i < kVertexCount; i++)
        {
            intersections[i] = aPlane.Intersects(aVertices[i]);

            if (intersections[i] == PlaneIntersection::kFront) { positive++; }
            else if (intersections[i] == PlaneIntersection::kBack) { negative++; }
        }

        if (negative == 0u)
        {
            arNewVertices = aVertices;
            return kVertexCount;
        }
        else if (positive == 0u) { return 0u; }

        PositionBuffer outVertices(kVertexCount + 1u);

        size_t count = 0u;
        size_t Si = kVertexCount - 1u;

        for (size_t Pi = 0; Pi < kVertexCount; Pi++)
        {
            PlaneIntersection::Type P = intersections[Pi];
            PlaneIntersection::Type S = intersections[Si];

            if (P == PlaneIntersection::kFront)
            {
                if (S == PlaneIntersection::kFront)
                {
                    outVertices[count++] = aVertices[Pi];
                }
                else
                {
                    outVertices[count++] = ComputeIntersection(aVertices[Si], aVertices[Pi], aPlane);
                    outVertices[count++] = aVertices[Pi];
                }
            }
            else if (S == PlaneIntersection::kFront)
            {
                outVertices[count++] = ComputeIntersection(aVertices[Pi], aVertices[Si], aPlane);
            }

            Si = Pi;
        }

        arNewVertices = outVertices;

        return arNewVertices.size();
    }

    size_t Clip(const PositionBuffer& aVertices, const PlaneBuffer& aPlanes, PositionBuffer& arNewVertices)
    {
        PositionBuffer vertices(aVertices);
        const size_t kPlaneCount = aPlanes.size();

        for (size_t i = 0; i < kPlaneCount; i++)
        {
            if (Clip(vertices, aPlanes[i], vertices) == 0)
            {
                return 0;
            }
        }

        arNewVertices = vertices;
        return arNewVertices.size();
    }

}

