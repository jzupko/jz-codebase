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
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, RAISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once
#ifndef _JZ_SAIL_THREE_POINT_LIGHTING_H_
#define _JZ_SAIL_THREE_POINT_LIGHTING_H_

#include <jz_core/Angle.h>
#include <jz_core/BoundingSphere.h>
#include <jz_core/Color.h>
#include <jz_core/Event.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Vector3.h>
#include <jz_sail/LightLearner.h>

#include <vector>

namespace jz
{
    namespace sail
    {

        class ThreePointLighting
        {
        public:
            static const float kCameraStep;
            static const float kNearPlaneScale;

            struct LightSettings
            {
                ColorRGB KeyDiffuse;
                ColorRGB KeySpecular;
                ColorRGB FillDiffuse;
                ColorRGB BackDiffuse;
                ColorRGB BackSpecular;

                Matrix4 KeyTransform;
                Matrix4 FillTransform;
                Matrix4 BackTransform;
            };

            struct MotivatingLight
            {
                enum LightType
                {
                    kDirectional = 0,
                    kPoint = 1,
                    kSpot = 2
                };

                LightType Type;

                ColorRGB Diffuse;
                ColorRGB Specular;

                Vector3 Direction;
                Vector3 Position;

                float SpotCosHalfAngle;
                float SpotExponent;

                Vector3 Attenuation;
            };

            ThreePointLighting();
            ~ThreePointLighting();

            const ImageIlluminationMetrics& GetIdeal() const { return mIdeal; }
            void SetIdeal(const ImageIlluminationMetrics& v) { mIdeal = v; }

            const LightLearner& GetLightLearner() const { return mLearner; }
            LightLearner& GetLightLearner() { return mLearner; }

            bool Tick(const Matrix4& aInverseViewTransform, const BoundingSphere& aTargetBS, const vector<MotivatingLight>& aMotivatingLights, LightSettings& arSettings);

        private:
            ThreePointLighting(const ThreePointLighting&);
            ThreePointLighting& operator=(const ThreePointLighting&);

            ColorRGB mKeyDiffuse;
            ColorRGB mKeySpecular;
            ColorRGB mBackDiffuse;
            ColorRGB mBackSpecular;
            
            ThreePointSettings mCurrent;
            ImageIlluminationMetrics mIdeal;
            ThreePointSettings mMotivation;

            Matrix4 mCamera;
            Matrix4 mInvCamera;
            Radian mCameraYaw;
            Radian mCameraPitch;
            Radian mDesiredCameraYaw;
            Radian mDesiredCameraPitch;
            float mLastTick;

            LightLearner mLearner;

            Radian _GetFillYaw(Radian aKeyYaw);
            Radian _GetBackYaw(Radian aKeyYaw);

            void _Get(const ThreePointSettings& aSettings, Radian& arKeyRoll, Radian& arKeyYaw, Radian& arFillYaw, Radian& arBackYaw);

            Matrix4 _GetLocalTransform(Radian aRoll, Radian aYaw);
            Matrix4 _GetCameraTransform(Radian aYaw, Radian aPitch);
            Matrix4 _GetInvCameraTransform(Radian aYaw, Radian aPitch);
            Matrix4 _GetTransform(const BoundingSphere& aTargetBS, Radian aRoll, Radian aYaw);
            float _GetAttenuation(const BoundingSphere& aTargetSphere, const MotivatingLight& l);
            ColorRGB _GetDiffuseColor(const BoundingSphere& aTargetSphere, const MotivatingLight& l);
            void _GetDiffuseAndSpecular(const BoundingSphere& aTargetSphere, const MotivatingLight& l, ColorRGB& diff, ColorRGB& spec);
            Vector3 _GetDirection(const BoundingSphere& aTargetSphere, const MotivatingLight& l);
            Vector3 _GetKeyWorldDirection(const BoundingSphere& aTargetBS, const vector<MotivatingLight>& aMotivatingLights);
            void _GetRollYaw(const Vector3& aDirection, Radian& arRoll, Radian& arYaw);
            void _GetYawPitch(const Vector3& aDirection, Radian& arYaw, Radian& arPitch);
            bool _UpdateHelper(float aElapsedTime, const Matrix4& aInverseViewTransform, const BoundingSphere& aTargetBS, const vector<MotivatingLight>& aMotivatingLights, Vector3& arKeyWorldDir, Vector3& arBackWorldDir);
        };

    }
}

#endif
