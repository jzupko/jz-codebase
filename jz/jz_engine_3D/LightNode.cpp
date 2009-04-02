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

#include <jz_core/Color.h>
#include <jz_engine_3D/Deferred.h>
#include <jz_engine_3D/LightNode.h>
#include <jz_engine_3D/ShadowMan.h>

namespace jz
{
    namespace engine_3D
    {

        static const float kNearPlaneScale = 4.38e-4f;
        static const float kFarPlaneScale = 1.0f;
        static const float kMaxLightRange = Sqrt(Constants<float>::kMax * 0.5f);
        static const float kMinLuminance = 0.004f;

        float CalculateLightRange(const Vector3& aLightAttenuation, const Vector3& aLightDiffuse)
        {
            float max = GetYofCIE1931sRGB(aLightDiffuse);

            float ret = 0.0f;
            if (!AboutZero(aLightAttenuation.Z))
            {
                // max / (x + (y * d) + (z * d * d)) = kMinLuminance
                // (max / kMinLuminance) = (x + (y * d) + (z * d * d))
                // 0 = ((x - (max / kMinLuminance)) + (y * d) + (z * d * d))
                float a = aLightAttenuation.Z;
                float b = aLightAttenuation.Y;
                float c = (aLightAttenuation.X - (max / kMinLuminance));

                ret = (-b + Sqrt((b * b) - (4 * a * c))) / (2.0f * a);
            }
            else if (!AboutZero(aLightAttenuation.Y))
            {
                ret = ((max / kMinLuminance) - aLightAttenuation.X) / aLightAttenuation.Y;
            }
            else if (!AboutZero(aLightAttenuation.X))
            {
                ret = kMaxLightRange;
            }

            return ret;
        }

        LightNode::LightNode()
            : SceneNode(),
            mType(LightNodeType::kDirectional),
            mAttenuation(Vector3::kUnitX),
            mColor(Vector3::kZero),
            mFalloffCosHalfAngle(0.707f),
            mFalloffAngle(Radian::kPiOver2),
            mFalloffExponent(1.0f),
            mRange(kMaxLightRange),
            mbFixedRange(false),
            mWorldFrustum(Vector3::kZero, 6),
            mWorldLightDirection(Vector3::kForward),
            mShadowHandle(-1),
            mShadowTransform(Matrix4::kIdentity),
            mShadowView(Matrix4::kIdentity),
            mShadowProjection(Matrix4::kIdentity)
        {}

        LightNode::LightNode(const string& aId)
            : SceneNode(aId),
            mType(LightNodeType::kDirectional),
            mAttenuation(Vector3::kUnitX),
            mColor(Vector3::kZero),
            mFalloffCosHalfAngle(0.707f),
            mFalloffAngle(Radian::kPiOver2),
            mFalloffExponent(1.0f),
            mRange(kMaxLightRange),
            mbFixedRange(false),
            mWorldFrustum(Vector3::kZero, 6),
            mWorldLightDirection(Vector3::kForward),
            mShadowHandle(-1),
            mShadowTransform(Matrix4::kIdentity),
            mShadowView(Matrix4::kIdentity),
            mShadowProjection(Matrix4::kIdentity)
        { }

        LightNode::~LightNode()
        {
        }

        void LightNode::PoseForRender()
        {
            Deferred& df = Deferred::GetSingleton();
            if (df.bActive())
            {
                if (mRange > Constants<float>::kLooseTolerance)
                {
                    df.RenderLight(this);
                }
            }
        }

        void LightNode::SetCastShadow(bool b)
        {
            if (b && mShadowHandle < 0)
            {
                mShadowHandle = ShadowMan::GetSingleton().Grab();
            }
            else if (!b && mShadowHandle >= 0)
            {
                ShadowMan::GetSingleton().Release(mShadowHandle);
            }
        }

        void LightNode::_PopulateClone(SceneNode* apNode)
        {
            LightNode* p = static_cast<LightNode*>(apNode);

            p->mType = mType;
            p->mAttenuation = mAttenuation;
            p->mColor = mColor;
            p->mFalloffCosHalfAngle = mFalloffCosHalfAngle;
            p->mFalloffAngle = mFalloffAngle;
            p->mFalloffExponent = mFalloffExponent;
            p->mRange = mRange;
            p->mbFixedRange = mbFixedRange;
            p->mShadowProjection = mShadowProjection;
            p->mShadowTransform = mShadowTransform;
            p->mShadowView = mShadowView;
            p->SetCastShadow((mShadowHandle >= 0));
        }

        SceneNode* LightNode::_SpawnClone(const string& aCloneId)
        {
            return new LightNode(aCloneId);
        }

        void LightNode::_PostUpdate(bool abChanged)
        {
            if (abChanged)
            {
                #pragma region Update bounding based on light range.
                if (GreaterThan(mRange, 0.0f, Constants<float>::kLooseTolerance))
                {
                    BoundingSphere bounding(GetWorldTranslation(), mRange);

                    if (mbValidBounding)
                    {
                        mWorldBounding = BoundingSphere::Merge(mWorldBounding, bounding);
                    }
                    else
                    {
                        mWorldBounding = bounding;
                        mbValidBounding = true;
                    }

                    //Jare jare = Jare.Singleton;
                    //jare.Lights.Remove(this);
                    //mAABB = BoundingBox.CreateFromSphere(mWorldBounding);
                    //jare.Lights.Add(this);
                }
                #pragma endregion

                #pragma region Update light direction
                if (mType != LightNodeType::kPoint)
                {
                    mWorldLightDirection = Vector3::Normalize(Vector3::TransformDirection(mWit, Vector3::kForward));
                }
                else
                {
                    mWorldLightDirection = Vector3::kForward;
                }
                #pragma endregion

                #pragma region Update frustum
                if (mType == LightNodeType::kSpot && GreaterThan(mRange, 0.0f, Constants<float>::kLooseTolerance))
                {
                    float near = (kNearPlaneScale * mRange);
                    float far = (kFarPlaneScale * mRange);
                    mShadowProjection = Matrix4::CreatePerspectiveFieldOfViewDirectX(mFalloffAngle, 1.0f, near, far);
                    mShadowView = Matrix4::Invert(mWorld);
                    mShadowTransform = (mShadowView * mShadowProjection);
                    mWorldFrustum.Set(GetWorldTranslation(), mShadowTransform);
                }
                #pragma endregion

                mAABB = BoundingBox::CreateFrom(mWorldBounding);
            }

            SceneNode::_PostUpdate(abChanged);
        }

        float ApproximateLightAttenuation(LightNode* l, const Vector3& p)
        {
            if (l)
            {
                LightNodeType::Enum type = l->GetType();

                if (type == LightNodeType::kDirectional) { return 1.0f; }
                else
                {
                    float d = Vector3::Distance(p, l->GetWorldTranslation());

                    float att = 1.0f;
                    if (!AboutZero(d))
                    {
                        att = 1.0f /
                            (l->GetAttenuation().X +
                            (l->GetAttenuation().Y * d) +
                            (l->GetAttenuation().Z * (d * d)));
                    }

                    if (type == LightNodeType::kSpot)
                    {
                        Vector3 nlv = Vector3::Normalize(p - l->GetWorldTranslation());
                        if (Vector3::AboutZero(nlv)) { nlv = Vector3::kBackward; }

                        float dot = Vector3::Dot(nlv, l->GetWorldLightDirection());
                        float spot = Pow(Max(dot, 0.0f), l->GetFalloffExponent());
                        if (dot < l->GetFalloffCosHalfAngle()) { spot = 0.0f; }

                        att *= spot;
                    }

                    return att;
                }
            }

            return 0.0f;
        }

    }
}

