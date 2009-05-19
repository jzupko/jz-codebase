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

#include <jz_core/Matrix3.h>
#include <jz_physics/narrowphase/collision/BoxShape.h>

namespace jz
{
    namespace physics
    {

        Vector3 BoxShape::GetInertiaTensor(float aInverseMass) const
        {
            if (AboutZero(aInverseMass)) { return Vector3::kZero; }
            else
            {
                static const float kFactor = (float)(1.0 / 3.0);

                float m = (1.0f / aInverseMass);

                Vector3 ret;
                ret.X = (m * (HalfExtents.Y * HalfExtents.Y + HalfExtents.Z * HalfExtents.Z) * kFactor);
                ret.Y = (m * (HalfExtents.X * HalfExtents.X + HalfExtents.Z * HalfExtents.Z) * kFactor);
                ret.Z = (m * (HalfExtents.X * HalfExtents.X + HalfExtents.Y * HalfExtents.Y) * kFactor);

                return ret;
            }
        }

        Vector3 BoxShape::GetInverseInertiaTensor(float aInverseMass) const
        {
            static const float kFactor = (3.0f);

            if (HalfExtents.LengthSquared() < (Constants<float>::kZeroTolerance * Constants<float>::kZeroTolerance))
            {
                return Vector3::kZero;
            }
            else
            {
                Vector3 ret;
                ret.X = (aInverseMass * kFactor) / (HalfExtents.Y * HalfExtents.Y + HalfExtents.Z * HalfExtents.Z);
                ret.Y = (aInverseMass * kFactor) / (HalfExtents.X * HalfExtents.X + HalfExtents.Z * HalfExtents.Z);
                ret.Z = (aInverseMass * kFactor) / (HalfExtents.X * HalfExtents.X + HalfExtents.Y * HalfExtents.Y);

               return ret;
            }
        }

    }
}