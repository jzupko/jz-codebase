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
#include <jz_core/Matrix4.h>
#include <jz_core/Region.h>
#include <jz_core/Plane.h>

namespace jz
{

    void Region::Set(const Vector3& aCenter, const Matrix4& m)
    {
        Center = aCenter;
        Planes.resize(6);

        Vector4 c4 = m.GetCol(3);

        Planes[kLeft].Set(c4 + m.GetCol(0));
        Planes[kLeft].Normalize();
        Planes[kRight].Set(c4 - m.GetCol(0));
        Planes[kRight].Normalize();

        Planes[kBottom].Set(c4 + m.GetCol(1));
        Planes[kBottom].Normalize();
        Planes[kTop].Set(c4 - m.GetCol(1));
        Planes[kTop].Normalize();

        Planes[kNear].Set(m.GetCol(2));
        Planes[kNear].Normalize();
        Planes[kFar].Set(c4 - m.GetCol(2));
        Planes[kFar].Normalize();
    }

    Geometric::Test Region::Test(const BoundingBox& aBox) const
    {
        Geometric::Test ret = Geometric::kContains;
        Vector3 center = aBox.Center();
        const size_t kSize = Planes.size();

        const Vector3 rst = 0.5f * (aBox.Max - aBox.Min);

        for (size_t i = 0; i < kSize; i++)
        {
            const float radius = Vector3::Dot(rst, Planes[i].GetAbsNormal());
            const float negRadius = -radius;
            const float d = Plane::DotCoordinate(center, Planes[i]);

            if (d < negRadius) { return Geometric::kDisjoint; }
            else if (d <= radius) { ret = Geometric::kIntersects; }
        }

        return ret;
    }

    Geometric::Test Region::Test(const BoundingSphere& aSphere) const
    {
        Geometric::Test ret = Geometric::kContains;
        const Vector3& center = aSphere.Center;
        const float radius = aSphere.Radius;
        const float negRadius = -radius;
        const size_t kSize = Planes.size();

        for (size_t i = 0; i < kSize; i++)
        {
            const float d = Plane::DotCoordinate(center, Planes[i]);

            if (d < negRadius) { return Geometric::kDisjoint; }
            else if (d <= radius) { ret = Geometric::kIntersects; }
        }

        return ret;
    }

    Geometric::Test Region::Test(const Vector3& aPoint) const
    {
        Geometric::Test ret = Geometric::kContains;
        const size_t kSize = Planes.size();

        for (size_t i = 0; i < kSize; i++)
        {
            const float d = Plane::DotCoordinate(aPoint, Planes[i]);

            if (d < Constants<float>::kNegativeLooseTolerance) { return Geometric::kDisjoint; }
            else if (d <= Constants<float>::kLooseTolerance) { ret = Geometric::kIntersects; }
        }

        return ret;
    }

}
