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

#include <jz_sail/ThreePointLighting.h>

namespace jz
{
    namespace sail
    {

        const float ThreePointLighting::kNearPlaneScale = 4.38e-4f;
        const float ThreePointLighting::kCameraStep = 1.5f;

        ThreePointLighting::ThreePointLighting()
            : mKeyDiffuse(ColorRGB::kBlack),
              mKeySpecular(ColorRGB::kBlack), 
              mBackDiffuse(ColorRGB::kBlack),
              mBackSpecular(ColorRGB::kBlack),
              mCamera(Matrix4::kIdentity), 
              mInvCamera(Matrix4::kIdentity),
              mCameraYaw(Radian::kZero), 
              mCameraPitch(Radian::kZero),
              mCurrent(ThreePointSettings::kDefault),
              mDesiredCameraYaw(Radian::kZero), 
              mDesiredCameraPitch(Radian::kZero),
              mMotivation(ThreePointSettings::kDefault)
        {}

        ThreePointLighting::~ThreePointLighting()
        { }

        void ThreePointLighting::Tick(float aElapsedTime, const Matrix4& aInverseViewTransform, const BoundingSphere& aTargetBS, const vector<MotivatingLight>& aMotivatingLights, LightSettings& arSettings)
        {
            #pragma region Update
            {
                Vector3 keyWorldDir;
                Vector3 backWorldDir;
                _UpdateHelper(aElapsedTime, aInverseViewTransform, aTargetBS, aMotivatingLights, keyWorldDir, backWorldDir);

                float fillLum = 0.0f;
                mKeyDiffuse = ColorRGB::kBlack;
                mKeySpecular = ColorRGB::kBlack;
                mBackDiffuse = ColorRGB::kBlack;
                mBackSpecular = ColorRGB::kBlack;

                size_t size = aMotivatingLights.size();
                for (size_t i = 0u; i < size; i++)
                {
                    Vector3 dir = _GetDirection(aTargetBS, aMotivatingLights[i]);
                    ColorRGB diff;
                    ColorRGB spec;
                    _GetDiffuseAndSpecular(aTargetBS, aMotivatingLights[i], diff, spec);

                    float keyDot = jz::Max(Vector3::Dot(dir, keyWorldDir), 0.0f);
                    mKeyDiffuse += (keyDot * diff);
                    mKeySpecular += (keyDot * spec);

                    float floatDot = jz::Max(Vector3::Dot(dir, -keyWorldDir), 0.0f);
                    fillLum += (floatDot * jz::GetYofCIE1931sRGB(diff));

                    float backDot = jz::Max(Vector3::Dot(dir, backWorldDir), 0.0f);
                    mBackDiffuse += (backDot * diff);
                    mBackSpecular += (backDot * spec);
                }

                float keyLuminance = jz::GetYofCIE1931sRGB(mKeyDiffuse);
                float keyLuminanceFactor = (jz::GreaterThan(keyLuminance, 0.0f)) ? (1.0f / keyLuminance) : 0.0f;
                mMotivation.SetFill(fillLum * keyLuminanceFactor);
            }
            #pragma endregion

            #pragma region Step and calculate output
            mLearner.Step(mIdeal, mCurrent, mMotivation, aElapsedTime);

            arSettings.KeyDiffuse = mKeyDiffuse;
            arSettings.KeySpecular = mKeySpecular;
            arSettings.FillDiffuse = (mCurrent.GetFill() * mKeyDiffuse);
            arSettings.BackDiffuse = mBackDiffuse;
            arSettings.BackSpecular = mBackSpecular;

            Radian keyRoll;
            Radian keyYaw;
            Radian fillYaw;
            Radian backYaw;
            _Get(mCurrent, keyRoll, keyYaw, fillYaw, backYaw);

            arSettings.KeyTransform = _GetTransform(aTargetBS, keyRoll, keyYaw);
            arSettings.FillTransform = _GetTransform(aTargetBS, keyRoll, fillYaw);
            arSettings.BackTransform = _GetTransform(aTargetBS, keyRoll, backYaw);
            #pragma endregion
        }


        Radian ThreePointLighting::_GetFillYaw(Radian aKeyYaw)
        {
            return (aKeyYaw - Radian::kPiOver2);
        }

        Radian ThreePointLighting::_GetBackYaw(Radian aKeyYaw)
        {
            return (aKeyYaw + Radian::kPiOver2 + Radian::kPiOver4);
        }

        void ThreePointLighting::_Get(const ThreePointSettings& aSettings, Radian& arKeyRoll, Radian& arKeyYaw, Radian& arFillYaw, Radian& arBackYaw)
        {
            arKeyRoll = aSettings.GetKeyRoll();
            arKeyYaw = aSettings.GetKeyYaw();

            arFillYaw = _GetFillYaw(arKeyYaw);
            arBackYaw = _GetBackYaw(arKeyYaw);
        }

        Matrix4 ThreePointLighting::_GetLocalTransform(Radian aRoll, Radian aYaw)
        {
            Matrix4 ret = Matrix4::CreateRotationY(aYaw) * Matrix4::CreateRotationZ(aRoll);

            return ret;
        }

        Matrix4 ThreePointLighting::_GetCameraTransform(Radian aYaw, Radian aPitch)
        {
            Matrix4 ret = Matrix4::CreateRotationX(aPitch) * Matrix4::CreateRotationY(aYaw);

            return ret;
        }

        Matrix4 ThreePointLighting::_GetInvCameraTransform(Radian aYaw, Radian aPitch)
        {
            Matrix4 ret = Matrix4::CreateRotationY(-aYaw) * Matrix4::CreateRotationX(-aPitch);

            return ret;
        }

        static float _CalculateRigDistance(const BoundingSphere& bs)
        {
            static const float kFactor = (jz::Sqrt(2.0f) / jz::Tan(Degree(60) * 0.5f));
            
            float radius = bs.Radius;
            float twoRadius = (2.0f * radius);
            float nearPlane = (ThreePointLighting::kNearPlaneScale * twoRadius);
            float farPlane = (2.0f * twoRadius);
            
            float ret = (nearPlane + (kFactor * radius));

            return ret;
        }

        Matrix4 ThreePointLighting::_GetTransform(const BoundingSphere& aTargetBS, Radian aRoll, Radian aYaw)
        {
            Matrix4 trans = Matrix4::CreateTranslation(Vector3::kBackward * _CalculateRigDistance(aTargetBS));
            Matrix4 postTrans = Matrix4::CreateTranslation(aTargetBS.Center);

            Matrix4 ret = trans * _GetLocalTransform(aRoll, aYaw) * mCamera * postTrans;

            return ret;
        }

        float ThreePointLighting::_GetAttenuation(const BoundingSphere& aTargetSphere, const MotivatingLight& l)
        {
            MotivatingLight::LightType type = l.Type;

            if (type == MotivatingLight::kDirectional) { return 1.0f; }
            else
            {
                float d = Vector3::Distance(aTargetSphere.Center, l.Position);

                float att = 1.0f;
                if (!AboutZero(d))
                {
                    att = 1.0f /
                        (l.Attenuation.X +
                        (l.Attenuation.Y * d) +
                        (l.Attenuation.Z * (d * d)));
                }

                if (type == MotivatingLight::kSpot)
                {
                    Vector3 nlv = Vector3::Normalize(aTargetSphere.Center - l.Position);
                    if (Vector3::AboutZero(nlv)) { nlv = Vector3::kBackward; }

                    float dot = Vector3::Dot(nlv, l.Direction);
                    float spot = (float)jz::Pow(jz::Max(dot, 0.0f), l.SpotExponent);
                    if (dot < l.SpotCosHalfAngle) { spot = 0.0f; }

                    att *= spot;
                }

                return att;
            }
        }

        ColorRGB ThreePointLighting::_GetDiffuseColor(const BoundingSphere& aTargetSphere, const MotivatingLight& l)
        {
            return (l.Diffuse * _GetAttenuation(aTargetSphere, l));
        }

        void ThreePointLighting::_GetDiffuseAndSpecular(const BoundingSphere& aTargetSphere, const MotivatingLight& l, ColorRGB& diff, ColorRGB& spec)
        {
            float att = _GetAttenuation(aTargetSphere, l);
            diff = (l.Diffuse * att);
            spec = (l.Specular * att);
        }

        Vector3 ThreePointLighting::_GetDirection(const BoundingSphere& aTargetSphere, const MotivatingLight& l)
        {
            Vector3 dir;
            MotivatingLight::LightType type = l.Type;

            if (type == MotivatingLight::kDirectional) { dir = (-l.Direction); }
            else
            {
                dir = Vector3::Normalize(l.Position - aTargetSphere.Center);
                if (Vector3::AboutZero(dir)) { dir = Vector3::kBackward; }
            }

            return dir;
        }

        Vector3 ThreePointLighting::_GetKeyWorldDirection(const BoundingSphere& aTargetBS, const vector<MotivatingLight>& aMotivatingLights)
        {
            size_t size = aMotivatingLights.size();
            Vector3 key = Vector3::kZero;

            for (size_t i = 0u; i < size; i++)
            {
                Vector3 dir = _GetDirection(aTargetBS, aMotivatingLights[i]);
                ColorRGB color = _GetDiffuseColor(aTargetBS, aMotivatingLights[i]);

                float d = GetYofCIE1931sRGB(color);

                key += (d * dir);
            }

            if (Vector3::AboutZero(key)) { key = Vector3::kBackward; }
            else { key = Vector3::Normalize(key); }

            return key;
        }

        void ThreePointLighting::_GetRollYaw(const Vector3& aDirection, Radian& arRoll, Radian& arYaw)
        {
            #pragma region Roll
            {
                Vector2 n = Vector2(aDirection.X, aDirection.Y);
                float len = n.Length();

                if (len > Constants<float>::kZeroTolerance)
                {
                    n /= len;
                    arRoll = jz::ATan2(n.Y, n.X);
                    if (arRoll < Radian::kZero) { arRoll += Radian::kTwoPi; }
                }
                else { arRoll = Radian::kZero; }
            }
            #pragma endregion

            #pragma region Yaw
            {
                Vector3 n = !jz::AboutZero(arRoll) ? Vector3::TransformDirection(Matrix4::CreateRotationZ(-arRoll), aDirection) : aDirection;
                Vector2 n2 = Vector2(n.Z, n.X);
                float len = n2.Length();
                if (len > Constants<float>::kZeroTolerance)
                {
                    n2 /= len;
                    arYaw = jz::ACos(n2.X);
                }
                else { arYaw = Radian::kZero; }

                JZ_ASSERT(arYaw >= Radian::kZero && arYaw <= Radian::kPi);
            }
            #pragma endregion
        }

        void ThreePointLighting::_GetYawPitch(const Vector3& aDirection, Radian& arYaw, Radian& arPitch)
        {
            arYaw = jz::ATan2(aDirection.X, aDirection.Z);
            arPitch = -jz::ASin(aDirection.Y);
        }

        void ThreePointLighting::_UpdateHelper(float aElapsedTime, const Matrix4& aInverseViewTransform, const BoundingSphere& aTargetBS, const vector<MotivatingLight>& aMotivatingLights, Vector3& arKeyWorldDir, Vector3& arBackWorldDir)
        {
            float time = aElapsedTime;

            #pragma region Calculate new camera vector
            {
                Vector3 eye = Vector3::Normalize(Vector3::TransformDirection(aInverseViewTransform, Vector3::kBackward));
                JZ_ASSERT(eye.Length() > Constants<float>::kLooseTolerance);

                _GetYawPitch(eye, mDesiredCameraYaw, mDesiredCameraPitch);

                float t = jz::Clamp(time * kCameraStep, 0.0f, 1.0f);
                mCameraYaw = jz::Lerp(mCameraYaw, mDesiredCameraYaw, t);
                mCameraPitch = jz::Lerp(mCameraPitch, mDesiredCameraPitch, t);

                mCamera = _GetCameraTransform(mCameraYaw, mCameraPitch);
                mInvCamera = _GetInvCameraTransform(mCameraYaw, mCameraPitch);
            }
            #pragma endregion

            Radian keyRoll;
            Radian keyYaw;

            #pragma region Key direction
            {
                arKeyWorldDir = _GetKeyWorldDirection(aTargetBS, aMotivatingLights);
                Vector3 localKeyDir = Vector3::Normalize(Vector3::TransformDirection(mInvCamera, arKeyWorldDir));

                _GetRollYaw(localKeyDir, keyRoll, keyYaw);
            }
            #pragma endregion

            mMotivation.SetKeyRoll(keyRoll);
            mMotivation.SetKeyYaw(keyYaw);

            #pragma region Fill and back
            {
                Radian backYaw = _GetBackYaw(keyYaw);
                Matrix4 backM = _GetTransform(aTargetBS, keyRoll, backYaw);

                arBackWorldDir = Vector3::TransformDirection(backM, Vector3::kBackward);
            }
            #pragma endregion
        }
        #pragma endregion

    }
}

