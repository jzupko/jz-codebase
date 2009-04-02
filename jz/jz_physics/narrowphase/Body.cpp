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

#include <jz_physics/World.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/ICollisionShape.h>

namespace jz
{
    namespace physics
    {

        #pragma region Body2D
        Body2D::Body2D(World2D* apWorld, ICollisionShape2D* apShape, u32 aType, u32 aCollidesWith)
            : mReferenceCount(0u),
            mHandle(Constants<u16>::kMax),
            mFriction(1.0f),
            mInverseMass(0.0f),
            mpShape(apShape),
            mPrevTranslation(Vector2::kZero),
            mTranslation(Vector2::kZero),
            mType(aType), mCollidesWith(aCollidesWith),
            mVelocity(Vector2::kZero),
            mpWorld(apWorld)
        {}

        Body2D::~Body2D()
        {
            if (mpWorld)
            {
                mpWorld->_Remove(this);
            }
        }

        BoundingRectangle Body2D::GetLocalBounding() const
        {
            if (mpShape.IsValid())
            {
                return (mpShape->GetBounding());
            }
            else
            {
                return (BoundingRectangle::kInvertedMax);
            }
        }

        void Body2D::SetTranslation(const Vector2& v)
        {
            mPrevTranslation = v;
            mTranslation = v;
            Update();
        }

        void Body2D::Update()
        {
            if (mpWorld)
            {
                BoundingRectangle prevRect = mpShape->GetBounding();
                BoundingRectangle nextRect = prevRect;

                prevRect.Max += mPrevTranslation;
                prevRect.Min += mPrevTranslation;
                nextRect.Max += mTranslation;
                nextRect.Min += mTranslation;

                BoundingRectangle rect = BoundingRectangle::Merge(prevRect, nextRect);

                mpWorld->_Update(this, rect);
            }
        }
        #pragma endregion

        #pragma region Body3D
        Body3D::Body3D(World3D* apWorld, ICollisionShape3D* apShape, u32 aType, u32 aCollidesWith)
            : mReferenceCount(0u),
            mHandle(Constants<u16>::kMax),
            mFriction(1.0f),
            mInverseMass(0.0f),
            mpShape(apShape),
            mPrevTranslation(Vector3::kZero),
            mTranslation(Vector3::kZero),
            mType(aType), mCollidesWith(aCollidesWith),
            mVelocity(Vector3::kZero),
            mpWorld(apWorld)
        {}

        Body3D::~Body3D()
        {
            if (mpWorld)
            {
                mpWorld->_Remove(this);
            }
        }

        BoundingBox Body3D::GetLocalBounding() const
        {
            if (mpShape.IsValid())
            {
                return (mpShape->GetBounding());
            }
            else
            {
                return (BoundingBox::kInvertedMax);
            }
        }

        void Body3D::SetTranslation(const Vector3& v)
        {
            mPrevTranslation = v;
            mTranslation = v;
            Update();
        }

        void Body3D::Update()
        {
            if (mpWorld)
            {
                BoundingBox prevRect = mpShape->GetBounding();
                BoundingBox nextRect = prevRect;

                prevRect.Max += mPrevTranslation;
                prevRect.Min += mPrevTranslation;
                nextRect.Max += mTranslation;
                nextRect.Min += mTranslation;

                BoundingBox rect = BoundingBox::Merge(prevRect, nextRect);

                mpWorld->_Update(this, rect);
            }
        }
        #pragma endregion

    }
}
