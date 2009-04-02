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

#include <jz_core/BoundingBox.h>
#include <jz_core/BoundingSphere.h>
#include <jz_core/Ray3D.h>

namespace jz
{

    /// <remarks>
    /// From: Ericson, C. 2005. "Real-Time Collision Detection",
    ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 181
    /// </remarks>
    bool Ray3D::Intersects(const BoundingBox& aBox, float& arDistance) const
    {
        arDistance = 0.0f;
        float dmax = Constants<float>::kMax;

        for (int i = 0; i < 3; i++)
        {
            if (AboutZero(Direction[i]))
            {
                if (Position[i] < aBox.Min[i] ||
                    Position[i] > aBox.Max[i])
                {
                    return false;
                }
            }
            else
            {
                float ood = (1.0f / Direction[i]);
                float t1 = (aBox.Min[i] - Position[i]) * ood;
                float t2 = (aBox.Max[i] - Position[i]) * ood;

                if (t1 > t2) { Swap(t1, t2); }

                arDistance = Max(arDistance, t1);
                dmax = Min(dmax, t2);

                if (arDistance > dmax)
                {
                    return false;
                }
            }
        }

        return true;
    }

    /// <remarks>
    /// From: Ericson, C. 2005. "Real-Time Collision Detection",
    ///     Elsevier, Inc. ISBN: 1-55860-732-3, page 178
    /// </remarks>
    bool Ray3D::Intersects(const BoundingSphere& bs, float& t) const
    {
        Vector3 m = (Position - bs.Center);
        float b = Vector3::Dot(m, Direction);
        float c = m.LengthSquared() - (bs.RadiusSquared());
        if (c > 0.0f && b > 0.0f) { return false; }

        float discr = (b * b) - c;
        if (discr < 0.0f) { return false; }

        t = Max((-b - Sqrt(discr)), 0.0f);

        return true;
    }

    bool Ray3D::Intersects(const Plane& aPlane, float& arDistance) const
    {
        float t = -(Plane::DotCoordinate(Position, aPlane)) / Vector3::Dot(Direction, aPlane.GetNormal());

        float dc = Plane::DotCoordinate(Position, aPlane);
        float dn = Vector3::Dot(aPlane.GetNormal(), Direction);

        if (dc > Constants<float>::kLooseTolerance)
        {
            if (dn < Constants<float>::kLooseTolerance)
            {
                arDistance = (-dc / dn);
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (dc < Constants<float>::kLooseTolerance)
        {
            if (dn > Constants<float>::kLooseTolerance)
            {
                arDistance = (-dc / dn);
                return true;
            }
            else
            {
                return false;
            }
        }
        // Ray's starting position is within the plane plus a tolerance, return true.
        else
        {
            arDistance = 0.0f;
            return true;
        }
    }

}