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
#include <jz_core/Portal.h>

namespace jz
{

    bool Portal::Clip(const Region& aRegion, Region& arReducedRegion) const
    {
        if (PortalPlane.Intersects(aRegion.Center) == PlaneIntersection::kFront)
        {
            PositionBuffer newPoints;
            if (jz::Clip(Points, aRegion.Planes, newPoints) > 0)
            {
                size_t count = (newPoints.size());
                arReducedRegion.Center = aRegion.Center;
                arReducedRegion.Planes.resize(count + 2);

                arReducedRegion.Planes[Region::kNear] = PortalPlane.Flip();
                arReducedRegion.Planes[Region::kFar] = aRegion.Planes[Region::kFar];

                size_t prev = (count - 1);
                for (size_t i = 0; i < count; i++)
                {
                    // assumes clockwise winding
                    arReducedRegion.Planes[i + 2] = Plane(arReducedRegion.Center, newPoints[prev], newPoints[i]);

                    prev = i;
                }

                return true;
            }
        }

        return false;
    }

}