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

#define JZ_ENABLE_CLEANUP 1

#include <jz_sail/LightLearner.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>
#include <jz_system/WriteHelpers.h>

namespace jz
{
    namespace sail
    {

        const ThreePointSettings ThreePointSettings::kDefault = ThreePointSettings(Degree(0), 0.0f, Degree(0));
        const float ThreePointSettings::kMinFill = 0.0f;
        const float ThreePointSettings::kMaxFill = 0.5f;
        const Degree ThreePointSettings::kMinYaw = Degree(0);
        const Degree ThreePointSettings::kMaxYaw = Degree(135);

        const float LightLearner::kStepSize = 3.0f;
        const float LightLearner::kMotWeight = 0.7f;
        const float LightLearner::kDesiredWeight = (1.0f - kMotWeight);

        const int LightLearner::kSegments = 12;
        const int LightLearner::kSegmentsPlus1 = kSegments + 1;
        const int LightLearner::kSegmentsPlus1Squared = kSegmentsPlus1 * kSegmentsPlus1;
        const float LightLearner::kSegmentsF = kSegments;

        const int LightLearner::kStorage = kSegments * kSegmentsPlus1Squared;

        const float LightLearner::kJitterFactor = (kSegmentsF / 24.0f) * 0.80f;

        const float LightLearner::kFillFactor = ThreePointSettings::kMaxFill / kSegmentsF;
        const float LightLearner::kFillFactorHalf = 0.5f * kFillFactor;
        const float LightLearner::kFillJitterScale = kFillFactor * kJitterFactor;
        const float LightLearner::kFillJitterScaleAdjust = 0.5f * kFillJitterScale;

        const float LightLearner::kRollFactor = (360.0f / kSegmentsF);
        const float LightLearner::kRollFactorHalf = 0.5f * kRollFactor;
        const float LightLearner::kRollJitterScale = kRollFactor * kJitterFactor;
        const float LightLearner::kRollJitterScaleAdjust = 0.5f * kRollJitterScale;

        const float LightLearner::kYawFactor = ThreePointSettings::kMaxYaw.GetValue() / kSegmentsF;
        const float LightLearner::kYawFactorHalf = 0.5f * kYawFactor;
        const float LightLearner::kYawJitterScale = kYawFactor * kJitterFactor;
        const float LightLearner::kYawJitterScaleAdjust = 0.5f * kYawJitterScale;

        const float LightLearner::kRollTolerance = 15.0f;
        const float LightLearner::kYawMinimum = 30.0f;

        ImageIlluminationMetrics LightLearner::Get(const ThreePointSettings& arSettings)
        {
            float r = (arSettings.GetKeyRoll().GetValue() / kRollFactor);
            float f = (jz::Clamp(arSettings.GetFill(), ThreePointSettings::kMinFill, ThreePointSettings::kMaxFill) / kFillFactor);
            float y = (jz::Clamp(arSettings.GetKeyYaw(), ThreePointSettings::kMinYaw, ThreePointSettings::kMaxYaw).GetValue() / kYawFactor);

            int r0 = (int)jz::Floor(r);
            int f0 = (int)jz::Floor(f);
            int y0 = (int)jz::Floor(y);

            int r1 = (int)jz::Ceil(r);
            int f1 = (int)jz::Ceil(f);
            int y1 = (int)jz::Ceil(y);

            while (r0 >= kSegments)
            {
                r0 -= kSegments;
            }
            while (r1 >= kSegments)
            {
                r1 -= kSegments;
            }

            float rd = r - (float)r0;
            float fd = f - (float)f0;
            float yd = y - (float)y0;

            ImageIlluminationMetrics i0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f0, y0)], mSamples[_I(r0, f0, y1)], yd);
            ImageIlluminationMetrics i1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f1, y0)], mSamples[_I(r0, f1, y1)], yd);
            ImageIlluminationMetrics j0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f0, y0)], mSamples[_I(r1, f0, y1)], yd);
            ImageIlluminationMetrics j1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f1, y0)], mSamples[_I(r1, f1, y1)], yd);

            ImageIlluminationMetrics k0 = ImageIlluminationMetrics::Lerp(i0, i1, fd);
            ImageIlluminationMetrics k1 = ImageIlluminationMetrics::Lerp(j0, j1, fd);

            ImageIlluminationMetrics ret = ImageIlluminationMetrics::Lerp(k0, k1, rd);

            return ret;
        }

        void LightLearner::Step(const ImageIlluminationMetrics& aTarget, ThreePointSettings& arCurrent, const ThreePointSettings& aMotivation, float aTimeStep)
        {
            ThreePointSettings orig = arCurrent;
            ImageIlluminationMetrics targetMetrics = aTarget;
            ImageIlluminationMetrics curMetrics = Get(arCurrent);
            ImageIlluminationMetrics motMetrics = Get(aMotivation);

            float rollDelta = kStepSize * aTimeStep * ((kDesiredWeight * _GetErrorGradientRoll(targetMetrics, arCurrent)) + (kMotWeight * _GetErrorGradientRoll(motMetrics, arCurrent)));
            float fillDelta = kStepSize * aTimeStep * ((kDesiredWeight * _GetErrorGradientFill(targetMetrics, arCurrent)) + (kMotWeight * _GetErrorGradientFill(motMetrics, arCurrent)));
            float yawDelta = kStepSize * aTimeStep * ((kDesiredWeight * _GetErrorGradientYaw(targetMetrics, arCurrent)) + kMotWeight * (_GetErrorGradientYaw(motMetrics, arCurrent)));

            arCurrent.SetKeyRoll(arCurrent.GetKeyRoll() - (rollDelta * Degree::k180));
            arCurrent.SetFill(jz::Max(arCurrent.GetFill() - (fillDelta * ThreePointSettings::kMaxFill), ThreePointSettings::kMinFill));
            arCurrent.SetKeyYaw(jz::Clamp(arCurrent.GetKeyYaw() - (ThreePointSettings::kMaxYaw * yawDelta), ThreePointSettings::kMinYaw, Degree::k180));
        }

        bool LightLearner::Tick(const ImageData& aImageData, ThreePointSettings& arSettings)
        {
            int kIndex = _Index();

            ImageIlluminationMetrics sample;
            LightingExtractor::ExtractLighting(aImageData, sample);

            // Cleanup handles cases where:
            // - change of key yaw no longer has an effect on the image pixels because it is too big
            //   and the key light is effectively behind the object relative to the camera.
            // - change of key roll no longer has an effect on the image pixels because key yaw is
            //   very close to zero, effectively the key light is a camera mounted front light.
            // - change of fill intensity no longer has an effect on the image pixels because the yaw is
            //   too small (very close to 0) and the fill is effectively behind the object.
    #           if JZ_ENABLE_CLEANUP
                    sample.Roll = (arSettings.GetKeyRoll() / Degree::k360).GetValue() * ImageIlluminationMetrics::kRollMax;

                    if (arSettings.GetKeyYaw().GetValue() - (2.0f * kYawFactor) > ThreePointSettings::kMinYaw.GetValue())
                    {
                        ThreePointSettings set0 = ThreePointSettings(arSettings.GetKeyRoll(), arSettings.GetFill(), arSettings.GetKeyYaw() - Degree(2.0f * kYawFactor));
                        ThreePointSettings set1 = ThreePointSettings(arSettings.GetKeyRoll(), arSettings.GetFill(), arSettings.GetKeyYaw() - Degree(1.0f * kYawFactor));

                        ImageIlluminationMetrics s0 = Get(set0);
                        ImageIlluminationMetrics s1 = Get(set1);

                        if (sample.Yaw < s1.Yaw)
                        {
                            sample.Yaw = (s0.Yaw + (2.0f * (s1.Yaw - s0.Yaw)));
                        }
                    }

                    if (arSettings.GetFill() - (2.0f * kFillFactor) > ThreePointSettings::kMinFill)
                    {
                        ThreePointSettings set0 = ThreePointSettings(arSettings.GetKeyRoll(), arSettings.GetFill() - (2.0f * kFillFactor), arSettings.GetKeyYaw());
                        ThreePointSettings set1 = ThreePointSettings(arSettings.GetKeyRoll(), arSettings.GetFill() - (1.0f * kFillFactor), arSettings.GetKeyYaw());

                        ImageIlluminationMetrics s0 = Get(set0);
                        ImageIlluminationMetrics s1 = Get(set1);

                        if (sample.Entropy < s1.Entropy)
                        {
                            sample.Entropy = (s0.Entropy + (2.0f * (s1.Entropy - s0.Entropy))); 
                        }
                    }
    #           endif

            mSamples[kIndex] = sample;

            if (_IncrementIndex())
            {
                _GetNextSettings(arSettings);
                return true;
            }
            else { return false; }
        }

	    void LightLearner::Save(const string& aFilename)
	    {
            if (!aFilename.empty())
            {
                system::IWriteFilePtr pFile = system::Files::GetSingleton().OpenWriteable(aFilename.c_str());

                system::WriteInt32(pFile, kStorage);
                system::WriteBuffer(pFile, mSamples);
            }
	    }

	    void LightLearner::Load(const string& aFilename)
	    {
            if (!aFilename.empty())
            {
                system::IReadFilePtr pFile = system::Files::GetSingleton().Open(aFilename.c_str());

                _Init();
                int fileStorage = system::ReadInt32(pFile);
                JZ_E_ON_FAIL(fileStorage == kStorage, "file data is not compatible with this light learner.");

                system::ReadBuffer(pFile, mSamples);
            }
	    }

        void LightLearner::_GetNextSettings(ThreePointSettings& arSettings)
        {
            float roll = (mR * kRollFactor) + (UniformRandomf() * kRollJitterScale - kRollJitterScaleAdjust);
            float fill = (mF * kFillFactor) + (UniformRandomf() * kFillJitterScale - kFillJitterScaleAdjust);
            float yaw = (mY * kYawFactor) + (UniformRandomf() * kYawJitterScale - kYawJitterScaleAdjust);

            arSettings.SetKeyRoll(Degree(roll));
            arSettings.SetFill(jz::Max(fill, ThreePointSettings::kMinFill));
            arSettings.SetKeyYaw(jz::Clamp(Degree(yaw), ThreePointSettings::kMinYaw, ThreePointSettings::kMaxYaw));
        }

        float LightLearner::_GetError(
            const ImageIlluminationMetrics& arTarget, 
            const ImageIlluminationMetrics& arCurrent)
        {
            ImageIlluminationMetrics a = (arTarget - arCurrent);

            float ret = ImageIlluminationMetrics::Dot(a, a);

            return ret;
        }

        float LightLearner::_GetErrorGradient(
            const ImageIlluminationMetrics& arTarget,
            const ImageIlluminationMetrics& ar0, 
            const ImageIlluminationMetrics& ar1)
        {
            float e0 = _GetError(arTarget, ar0);
            float e1 = _GetError(arTarget, ar1);
            
            float ret = 0.5f * (e1 - e0);

            return ret;
        }

        float LightLearner::_GetErrorGradientRoll(
            const ImageIlluminationMetrics& arTarget,
            const ThreePointSettings& arCurrent)
        {
            float r = (arCurrent.GetKeyRoll().GetValue() / kRollFactor);
            float f = (jz::Clamp(arCurrent.GetFill(), ThreePointSettings::kMinFill, ThreePointSettings::kMaxFill) / kFillFactor);
            float y = (jz::Clamp(arCurrent.GetKeyYaw(), ThreePointSettings::kMinYaw, ThreePointSettings::kMaxYaw).GetValue() / kYawFactor);

            int r0 = (int)jz::Floor(r);
            int f0 = (int)jz::Floor(f);
            int y0 = (int)jz::Floor(y);

            int r1 = (int)jz::Ceil(r);
            int f1 = (int)jz::Ceil(f);
            int y1 = (int)jz::Ceil(y);

            if (r0 == r1) { r0--; r1++; }
            while (r0 < 0) { r0 += kSegments; }

            while (r0 >= kSegments) { r0 -= kSegments; }
            while (r1 >= kSegments) { r1 -= kSegments; }

            float fd = f - (float)f0;
            float yd = y - (float)y0;

            ImageIlluminationMetrics i0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f0, y0)], mSamples[_I(r0, f0, y1)], yd);
            ImageIlluminationMetrics i1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f1, y0)], mSamples[_I(r0, f1, y1)], yd);
            ImageIlluminationMetrics j0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f0, y0)], mSamples[_I(r1, f0, y1)], yd);
            ImageIlluminationMetrics j1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f1, y0)], mSamples[_I(r1, f1, y1)], yd);

            ImageIlluminationMetrics k0 = ImageIlluminationMetrics::Lerp(i0, i1, fd);
            ImageIlluminationMetrics k1 = ImageIlluminationMetrics::Lerp(j0, j1, fd);

            float ret = _GetErrorGradient(arTarget, k0, k1);

            return ret;
        }

        float LightLearner::_GetErrorGradientFill(
            const ImageIlluminationMetrics& arTarget,
            const ThreePointSettings& arCurrent)
        {
            float r = (arCurrent.GetKeyRoll().GetValue() / kRollFactor);
            float f = (jz::Clamp(arCurrent.GetFill(), ThreePointSettings::kMinFill, ThreePointSettings::kMaxFill) / kFillFactor);
            float y = (jz::Clamp(arCurrent.GetKeyYaw(), ThreePointSettings::kMinYaw, ThreePointSettings::kMaxYaw).GetValue() / kYawFactor);

            int r0 = (int)jz::Floor(r);
            int f0 = (int)jz::Floor(f);
            int y0 = (int)jz::Floor(y);

            int r1 = (int)jz::Ceil(r);
            int f1 = (int)jz::Ceil(f);
            int y1 = (int)jz::Ceil(y);

            while (r0 >= kSegments) { r0 -= kSegments; }
            while (r1 >= kSegments) { r1 -= kSegments; }

            if (f0 == f1) { f0--; f1++; }
            if (f0 < 0) { f0 = 0; }
            if (f1 > kSegments) { f1 = kSegments; }

            float rd = r - (float)r0;
            float yd = y - (float)y0;

            ImageIlluminationMetrics i0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f0, y0)], mSamples[_I(r0, f0, y1)], yd);
            ImageIlluminationMetrics i1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f0, y0)], mSamples[_I(r1, f0, y1)], yd);
            ImageIlluminationMetrics j0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f1, y0)], mSamples[_I(r0, f1, y1)], yd);
            ImageIlluminationMetrics j1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f1, y0)], mSamples[_I(r1, f1, y1)], yd);

            ImageIlluminationMetrics k0 = ImageIlluminationMetrics::Lerp(i0, i1, rd);
            ImageIlluminationMetrics k1 = ImageIlluminationMetrics::Lerp(j0, j1, rd);

            float ret = _GetErrorGradient(arTarget, k0, k1);

            return ret;
        }

        float LightLearner::_GetErrorGradientYaw(
            const ImageIlluminationMetrics& arTarget,
            const ThreePointSettings& arCurrent)
        {
            float r = (arCurrent.GetKeyRoll().GetValue() / kRollFactor);
            float f = (jz::Clamp(arCurrent.GetFill(), ThreePointSettings::kMinFill, ThreePointSettings::kMaxFill) / kFillFactor);
            float y = (jz::Clamp(arCurrent.GetKeyYaw(), ThreePointSettings::kMinYaw, ThreePointSettings::kMaxYaw).GetValue() / kYawFactor);

            int r0 = (int)jz::Floor(r);
            int f0 = (int)jz::Floor(f);
            int y0 = (int)jz::Floor(y);

            int r1 = (int)jz::Ceil(r);
            int f1 = (int)jz::Ceil(f);
            int y1 = (int)jz::Ceil(y);

            while (r0 >= kSegments) { r0 -= kSegments; }
            while (r1 >= kSegments) { r1 -= kSegments; }

            if (y0 == y1) { y0--; y1++; }
            if (y0 < 0) { y0 = 0; }
            if (y1 > kSegments) { y1 = kSegments; }

            float rd = r - (float)r0;
            float fd = f - (float)f0;

            ImageIlluminationMetrics i0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f0, y0)], mSamples[_I(r0, f1, y0)], fd);
            ImageIlluminationMetrics i1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f0, y0)], mSamples[_I(r1, f1, y0)], fd);
            ImageIlluminationMetrics j0 = ImageIlluminationMetrics::Lerp(mSamples[_I(r0, f0, y1)], mSamples[_I(r0, f1, y1)], fd);
            ImageIlluminationMetrics j1 = ImageIlluminationMetrics::Lerp(mSamples[_I(r1, f0, y1)], mSamples[_I(r1, f1, y1)], fd);

            ImageIlluminationMetrics k0 = ImageIlluminationMetrics::Lerp(i0, i1, rd);
            ImageIlluminationMetrics k1 = ImageIlluminationMetrics::Lerp(j0, j1, rd);

            float ret = _GetErrorGradient(arTarget, k0, k1);

            return ret;
        }

    }
}

