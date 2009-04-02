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

#include <jz_core/BoundingCircle.h>
#include <jz_core/BoundingRectangle.h>
#include <jz_core/Matrix3.h>

namespace jz
{

    const BoundingRectangle BoundingRectangle::kZero = BoundingRectangle(Vector2(0), Vector2(0));
    const BoundingRectangle BoundingRectangle::kMax = BoundingRectangle(Vector2(Constants<float>::kMin), Vector2(Constants<float>::kMax));
	const BoundingRectangle BoundingRectangle::kInvertedMax = BoundingRectangle(Vector2(Constants<float>::kMax), Vector2(Constants<float>::kMin));
    const BoundingRectangle BoundingRectangle::kNegOneToOne = BoundingRectangle(Vector2(-1), Vector2(1));

    float BoundingRectangle::EffectiveRadius(const Vector2& aDirection) const
    {
        Vector2 rst(Max - Min);
        Vector2 absDirection(Vector2::Abs(aDirection));

        float dot = Vector2::Dot(rst, absDirection);
        float ret = 0.5f * dot;
        
        return ret;
    }
    
#   define JZ_INTERSECT_HELPER(member) \
        if (Min.member > b.Max.member) return false; \
        if (Max.member < b.Min.member) return false;
        
    bool BoundingRectangle::Intersects(const BoundingRectangle& b) const
    {
        JZ_INTERSECT_HELPER(X);
        JZ_INTERSECT_HELPER(Y);
        
        return true;
    }
    
#   undef JZ_INTERSECT_HELPER

#   define JZ_INTERSECT_HELPER(member) \
        if (b.Center.member < Min.member) { float t = b.Center.member - Min.member; sq += (t * t); } \
        else if (b.Center.member > Max.member) { float t = b.Center.member - Max.member; sq += (t * t); }    
    
    bool BoundingRectangle::Intersects(const BoundingCircle& b) const
    {
        float sq = 0.0f;
        
        JZ_INTERSECT_HELPER(X);
        JZ_INTERSECT_HELPER(Y);

        return LessThan(sq, b.RadiusSquared());
    }

#   undef JZ_INTERSECT_HELPER

#   define JZ_INTERSECT_HELPER(member) \
        if (b.member < Min.member || b.member > Max.member) { return false; }

    bool BoundingRectangle::Intersects(const Vector2& b) const
    {
        JZ_INTERSECT_HELPER(X);
        JZ_INTERSECT_HELPER(Y);

        return true;
    }

#   undef JZ_INTERSECT_HELPER

#   define JZ_CONTAINS_HELPER(member) \
        bReturn = bReturn && (b.Max.member <= Max.member); \
        bReturn = bReturn && (b.Min.member >= Min.member);

    bool BoundingRectangle::Contains(const BoundingRectangle& b) const
    {
        bool bReturn = true;

        JZ_CONTAINS_HELPER(X);
        JZ_CONTAINS_HELPER(Y);

        return bReturn;
    }

#   undef JZ_CONTAINS_HELPER

    BoundingRectangle BoundingRectangle::CreateFrom(const BoundingCircle& b)
    {
        BoundingRectangle ret = BoundingRectangle(b.Center - Vector2(b.Radius), b.Center + Vector2(b.Radius));

        return ret;
    }

	BoundingRectangle BoundingRectangle::Transform(const Matrix3& m, const BoundingRectangle& a)
	{
        Vector2 cornerBuffer[4];

        cornerBuffer[0] = Vector2::TransformPosition(m, Vector2(a.Min.X, a.Min.Y));
        cornerBuffer[1] = Vector2::TransformPosition(m, Vector2(a.Min.X, a.Max.Y));
        cornerBuffer[2] = Vector2::TransformPosition(m, Vector2(a.Max.X, a.Min.Y));
        cornerBuffer[3] = Vector2::TransformPosition(m, Vector2(a.Max.X, a.Max.Y));

        BoundingRectangle ret(BoundingRectangle::kInvertedMax);
        for (int i = 0; i < 4; i++)
        {
            ret.Min = Vector2::Min(ret.Min, cornerBuffer[i]);
            ret.Max = Vector2::Max(ret.Max, cornerBuffer[i]);
        }

        return ret;
    }

}
