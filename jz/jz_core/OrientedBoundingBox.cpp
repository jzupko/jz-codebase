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
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/OrientedBoundingBox.h>

namespace jz
{

    float OrientedBoundingBox::EffectiveRadius(const Vector3& aDirection) const
    {
        float ret = jz::Abs(Vector3::Dot(Rmag, aDirection)) 
                  + jz::Abs(Vector3::Dot(Smag, aDirection)) 
                  + jz::Abs(Vector3::Dot(Tmag, aDirection));

        return ret;
    }

    bool OrientedBoundingBox::Intersects(const BoundingBox& aAABB) const
    {
        Vector3 aabbCenter = aAABB.Center();
        Vector3 aabbHalfExtents = aAABB.HalfExtents();

        Vector3 diff = (aabbCenter - Center);

        if ((jz::Abs(diff.X) - AbsXYZ.X) > aabbHalfExtents.X) { return false; }
        if ((jz::Abs(diff.Y) - AbsXYZ.Y) > aabbHalfExtents.Y) { return false; }
        if ((jz::Abs(diff.Z) - AbsXYZ.Z) > aabbHalfExtents.Z) { return false; }

        return true;
    }

    bool OrientedBoundingBox::Intersects(const Vector3& aPoint) const
    {
        Vector3 diff = (aPoint - Center);

        if (jz::Abs(diff.X) > AbsXYZ.X) { return false; }
        if (jz::Abs(diff.Y) > AbsXYZ.Y) { return false; }
        if (jz::Abs(diff.Z) > AbsXYZ.Z) { return false; }

        return true;
    }

    OrientedBoundingBox OrientedBoundingBox::CreateFromAABB(const BoundingBox& aAABB, const CoordinateFrame3D& aFrame)
    {
        OrientedBoundingBox obb = OrientedBoundingBox(
            Vector3::TransformPosition(aFrame, aAABB.Center()),
            aAABB.HalfExtents(),
            Vector3(aFrame.Orientation.M11, aFrame.Orientation.M12, aFrame.Orientation.M13),
            Vector3(aFrame.Orientation.M21, aFrame.Orientation.M22, aFrame.Orientation.M23),
            Vector3(aFrame.Orientation.M31, aFrame.Orientation.M32, aFrame.Orientation.M33));

        return obb;
    }

}