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
#include <jz_core/CoordinateFrame3D.h>
#include <jz_core/Matrix4.h>

namespace jz
{

    const BoundingBox BoundingBox::kZero = BoundingBox(Vector3(0), Vector3(0));
    const BoundingBox BoundingBox::kMax = BoundingBox(Vector3(Constants<float>::kMin), Vector3(Constants<float>::kMax));
	const BoundingBox BoundingBox::kInvertedMax = BoundingBox(Vector3(Constants<float>::kMax),Vector3(Constants<float>::kMin));

    float BoundingBox::EffectiveRadius(const Vector3& aDirection) const
    {
        Vector3 rst(Max - Min);
        Vector3 absDirection(Vector3::Abs(aDirection));

        float dot = Vector3::Dot(rst, absDirection);
        float ret = 0.5f * dot;
        
        return ret;
    }
    
    float BoundingBox::EffectiveRadius(const Plane& aPlane) const
    {
		Vector3 rst(Max - Min);
		
        float dot = Vector3::Dot(rst, aPlane.GetAbsNormal());
		float ret = (0.5f * dot);

		return ret;
    }

#   define JZ_INTERSECT_HELPER(member) \
        if (Min.member > b.Max.member) return false; \
        if (Max.member < b.Min.member) return false;
        
    bool BoundingBox::Intersects(const BoundingBox& b) const
    {
        JZ_INTERSECT_HELPER(X);
        JZ_INTERSECT_HELPER(Y);
        JZ_INTERSECT_HELPER(Z);
        
        return true;
    }
    
#   undef JZ_INTERSECT_HELPER

#   define JZ_INTERSECT_HELPER(member) \
        if (b.Center.member < Min.member) { float t = b.Center.member - Min.member; sq += (t * t); } \
        else if (b.Center.member > Max.member) { float t = b.Center.member - Max.member; sq += (t * t); }    
    
    bool BoundingBox::Intersects(const BoundingSphere& b) const
    {
        float sq = 0.0f;
        
        JZ_INTERSECT_HELPER(X);
        JZ_INTERSECT_HELPER(Y);
        JZ_INTERSECT_HELPER(Z);

        return LessThan(sq, b.RadiusSquared());
    }

#   undef JZ_INTERSECT_HELPER

#   define JZ_INTERSECT_HELPER(member) \
        if (b.member < Min.member || b.member > Max.member) { return false; }

    bool BoundingBox::Intersects(const Vector3& b) const
    {
        JZ_INTERSECT_HELPER(X);
        JZ_INTERSECT_HELPER(Y);
        JZ_INTERSECT_HELPER(Z);

        return true;
    }

#   undef JZ_INTERSECT_HELPER

#   define JZ_CONTAINS_HELPER(member) \
        bReturn = bReturn && (b.Max.member <= Max.member); \
        bReturn = bReturn && (b.Min.member >= Min.member);

    bool BoundingBox::Contains(const BoundingBox& b) const
    {
        bool bReturn = true;

        JZ_CONTAINS_HELPER(X);
        JZ_CONTAINS_HELPER(Y);
        JZ_CONTAINS_HELPER(Z);

        return bReturn;
    }

#   undef JZ_CONTAINS_HELPER

    BoundingBox BoundingBox::CreateFrom(const BoundingSphere& b)
    {
        BoundingBox ret = BoundingBox(b.Center - Vector3(b.Radius), b.Center + Vector3(b.Radius));

        return ret;
    }

	BoundingBox BoundingBox::Transform(const CoordinateFrame3D& cf, const BoundingBox& a)
	{
        Vector3 cornerBuffer[8];

        cornerBuffer[0] = Vector3::TransformPosition(cf, Vector3(a.Min.X, a.Min.Y, a.Min.Z));
        cornerBuffer[1] = Vector3::TransformPosition(cf, Vector3(a.Min.X, a.Min.Y, a.Max.Z));
        cornerBuffer[2] = Vector3::TransformPosition(cf, Vector3(a.Min.X, a.Max.Y, a.Min.Z));
        cornerBuffer[3] = Vector3::TransformPosition(cf, Vector3(a.Min.X, a.Max.Y, a.Max.Z));
        cornerBuffer[4] = Vector3::TransformPosition(cf, Vector3(a.Max.X, a.Min.Y, a.Min.Z));
        cornerBuffer[5] = Vector3::TransformPosition(cf, Vector3(a.Max.X, a.Min.Y, a.Max.Z));
        cornerBuffer[6] = Vector3::TransformPosition(cf, Vector3(a.Max.X, a.Max.Y, a.Min.Z));
        cornerBuffer[7] = Vector3::TransformPosition(cf, Vector3(a.Max.X, a.Max.Y, a.Max.Z));

        BoundingBox ret(Vector3::kMax, Vector3::kMin);
        for (int i = 0; i < 8; i++)
        {
            ret.Min = Vector3::Min(ret.Min, cornerBuffer[i]);
            ret.Max = Vector3::Max(ret.Max, cornerBuffer[i]);
        }

        return ret;
    }

	BoundingBox BoundingBox::Transform(const Matrix4& m, const BoundingBox& a)
	{
        Vector3 cornerBuffer[8];

        cornerBuffer[0] = Vector3::TransformPosition(m, Vector3(a.Min.X, a.Min.Y, a.Min.Z));
        cornerBuffer[1] = Vector3::TransformPosition(m, Vector3(a.Min.X, a.Min.Y, a.Max.Z));
        cornerBuffer[2] = Vector3::TransformPosition(m, Vector3(a.Min.X, a.Max.Y, a.Min.Z));
        cornerBuffer[3] = Vector3::TransformPosition(m, Vector3(a.Min.X, a.Max.Y, a.Max.Z));
        cornerBuffer[4] = Vector3::TransformPosition(m, Vector3(a.Max.X, a.Min.Y, a.Min.Z));
        cornerBuffer[5] = Vector3::TransformPosition(m, Vector3(a.Max.X, a.Min.Y, a.Max.Z));
        cornerBuffer[6] = Vector3::TransformPosition(m, Vector3(a.Max.X, a.Max.Y, a.Min.Z));
        cornerBuffer[7] = Vector3::TransformPosition(m, Vector3(a.Max.X, a.Max.Y, a.Max.Z));

        BoundingBox ret(Vector3::kMax, Vector3::kMin);
        for (int i = 0; i < 8; i++)
        {
            ret.Min = Vector3::Min(ret.Min, cornerBuffer[i]);
            ret.Max = Vector3::Max(ret.Max, cornerBuffer[i]);
        }

        return ret;
    }

}
