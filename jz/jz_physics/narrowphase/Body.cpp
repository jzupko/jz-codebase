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

        Body3D::Body3D(World3D* apWorld, ICollisionShape3D* apShape, u32 aType, u32 aCollidesWith)
            : mReferenceCount(0u),
            mCollidesWith(aCollidesWith),
            mType(aType),
            mFriction(1.0f),
            mInverseMass(0.0f),
            mpWorld(apWorld),
            mpShape(apShape),
            mPrevFrame(CoordinateFrame3D::kIdentity),
            mFrame(CoordinateFrame3D::kIdentity),
            mAngularVelocity(Vector3::kZero),
            mLinearVelocity(Vector3::kZero),
            mHandle(Constants<u16>::kMax)
        {}

        Body3D::~Body3D()
        {
            if (mpWorld)
            {
                mpWorld->_Remove(this);
            }
        }

        Vector3 Body3D::GetInertiaTensor() const
        {
            if (mpShape.IsValid())
            {
                return (mpShape->GetInertiaTensor(mInverseMass));
            }
            else
            {
                return (Vector3::kZero);
            }
        }

        Vector3 Body3D::GetInverseInertiaTensor() const
        {
            if (mpShape.IsValid())
            {
                return (mpShape->GetInverseInertiaTensor(mInverseMass));
            }
            else
            {
                return (Vector3::kZero);
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

        BoundingBox Body3D::GetWorldBounding(const CoordinateFrame3D& v) const
        {
            if (mpShape.IsValid())
            {
                if (mpShape->bRotationallyInvariant())
                {
                    BoundingBox ret = (mpShape->GetBounding());
                    ret.Min += v.Translation;
                    ret.Max += v.Translation;

                    return ret;
                }
                else
                {
                    BoundingBox ret = BoundingBox::Transform(v, (mpShape->GetBounding()));
                    return ret;
                }
            }
            else
            {
                return (BoundingBox::kInvertedMax);
            }
        }

        void Body3D::SetFrame(const CoordinateFrame3D& v)
        {
            mPrevFrame = v;
            mFrame = v;
            Update();
        }

        void Body3D::SetOrientation(const Matrix3& v)
        {
            mPrevFrame.Orientation = v;
            mFrame.Orientation = v;
            Update();
        }

        void Body3D::SetTranslation(const Vector3& v)
        {
            mPrevFrame.Translation = v;
            mFrame.Translation = v;
            Update();
        }

        void Body3D::Update()
        {
            if (mpWorld)
            {
                mpWorld->_Update(this, GetWorldBounding());
            }
        }

    }
}
