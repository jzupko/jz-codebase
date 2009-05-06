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

#pragma once
#ifndef _JZ_ENGINE_3D_LIGHT_NODE_H_
#define _JZ_ENGINE_3D_LIGHT_NODE_H_

#include <jz_core/Angle.h>
#include <jz_core/BoundingBox.h>
#include <jz_core/Region.h>
#include <jz_engine_3D/IRenderable.h>
#include <jz_engine_3D/SceneNode.h>

namespace jz
{
    namespace engine_3D
    {

        namespace LightNodeType
        {
            enum Enum
            {
                kDirectional = 0,
                kPoint = 1,
                kSpot = 2
            };
        }

        float CalculateLightRange(const Vector3& aLightAttenuation, const Vector3& aLightDiffuse);

        class LightNode : public SceneNode, public IRenderable
        {
        public:
            LightNode();
            LightNode(const string& aId);
            virtual ~LightNode();

            void ClearFixedRange()
            {
                mbFixedRange = false;
                mRange = CalculateLightRange(mAttenuation, mColor);
            }

            virtual BoundingBox GetAABB() const override { return mAABB; }
            virtual void PoseForRender() override;

            const Vector3& GetAttenuation() const { return mAttenuation; }
            void SetAttenuation(const Vector3& v)
            {
                mAttenuation = v;

                if (!mbFixedRange)
                {
                    mRange = CalculateLightRange(mAttenuation, mColor);
                }
            }

            const Vector3& GetColor() const { return mColor; }
            void SetColor(const Vector3& v)
            {
                mColor = v;

                if (!mbFixedRange)
                {
                    mRange = CalculateLightRange(mAttenuation, mColor);
                }
            }

            float GetRange() const { return mRange; }
            void SetRange(float v)
            {
                mbFixedRange = true;
                mRange = v;
                mFlags |= SceneNodeFlags::kLocalDirty;
            }

            const Region& GetWorldFrustum() const { return mWorldFrustum; }
            const Vector3 GetWorldLightDirection() const { return mWorldLightDirection; }

            Radian GetFalloffAngle() const { return mFalloffAngle; }
            void SetFalloffAngle(Radian r)
            {
                mFalloffAngle = Clamp(r, Radian::kZero, Radian::kPi);
                mFalloffCosHalfAngle = Cos(mFalloffAngle * 0.5f);
            }

            float GetFalloffCosHalfAngle() const { return mFalloffCosHalfAngle; }
            float GetFalloffExponent() const { return mFalloffExponent; }
            void SetFalloffExponent(float v)
            {
                mFalloffExponent = Clamp(v, 0.0f, 128.0f);
            }

            LightNodeType::Enum GetType() const { return mType; }
            void SetType(LightNodeType::Enum v) { mType = v; }

            int GetShadowHandle() const { return mShadowHandle; }
            const Matrix4& GetShadowTransform() const { return mShadowTransform; }
            const Matrix4& GetShadowProjection() const { return mShadowProjection; }
            const Matrix4& GetShadowView() const { return mShadowView; }

            void SetCastShadow(bool b);

        protected:
            virtual void _PopulateClone(SceneNode* apNode) override;
            virtual void _PostUpdate(bool abChanged)  override;
            virtual SceneNode* _SpawnClone(const string& aCloneId) override;

            BoundingBox mAABB;
            Vector3 mAttenuation;
            Vector3 mColor;
            float mFalloffCosHalfAngle;
            Radian mFalloffAngle;
            float mFalloffExponent;
            bool mbFixedRange;
            float mRange;
            LightNodeType::Enum mType;
            Region mWorldFrustum;
            Vector3 mWorldLightDirection;

            int mShadowHandle;
            Matrix4 mShadowProjection;
            Matrix4 mShadowTransform;
            Matrix4 mShadowView;

        private:
            friend void jz::__IncrementRefCount<engine_3D::LightNode>(engine_3D::LightNode*);
            friend void jz::__DecrementRefCount<engine_3D::LightNode>(engine_3D::LightNode*);

            LightNode(const LightNode&);
            LightNode& operator=(const LightNode&);
        };

        typedef AutoPtr<LightNode> LightNodePtr;

        float ApproximateLightAttenuation(LightNode* l, const Vector3& p);

    }
}

#endif
