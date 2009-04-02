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

#include <jz_physics/narrowphase/collision/ConvexShape.h>

namespace jz
{
    namespace physics
    {

        #pragma region ConvexShape2D
        void ConvexShape2D::_Update()
        {
            mNormals.resize(mPoints.size());
            mBounding = BoundingRectangle::kInvertedMax;
            
            const size_t kSize = mPoints.size();
            size_t prev = (kSize - 1u);
            for (size_t i = 0u; i < kSize; i++)
            {
                mBounding.Min = Vector2::Min(mPoints[i], mBounding.Min);    
                mBounding.Max = Vector2::Max(mPoints[i], mBounding.Max);

                const Vector2& p1 = mPoints[i];
                const Vector2& p0 = mPoints[prev];

                Vector2 n = Vector2::Normalize(Vector2(p0.Y - p1.Y, p1.X - p0.X));
                mNormals[i] = n;
                prev = i;
            }
        }
        #pragma endregion

        #pragma region ConvexShape3D
        Vector3 ConvexShape3D::GetSupport(const Vector3& n) const
        {
            if (mPoints.size() > 0u)
            {
                const size_t kSize = mPoints.size();

                Vector3 best = mPoints[0];
                for (size_t i = 1u; i < kSize; i++)
                {
                    if (Vector3::Dot(mPoints[i] - best, n) > 0.0f)
                    {
                        best = mPoints[i];
                    }
                }

                return best;
            }
            else
            {
                return Vector3::kZero;
            }
        }

        void ConvexShape3D::_Update()
        {
            mBounding = BoundingBox::kInvertedMax;
            
            const size_t kSize = mPoints.size();
            for (size_t i = 0u; i < kSize; i++)
            {
                mBounding.Min = Vector3::Min(mPoints[i], mBounding.Min);    
                mBounding.Max = Vector3::Max(mPoints[i], mBounding.Max);
            }
        }
        #pragma endregion

    }
}