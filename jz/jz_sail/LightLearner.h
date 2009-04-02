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
#ifndef _JZ_SAIL_LIGHT_LEARNER_H_
#define _JZ_SAIL_LIGHT_LEARNER_H_

#include <jz_core/Angle.h>
#include <jz_core/Math.h>
#include <jz_core/Memory.h>
#include <jz_sail/LightExtraction.h>

namespace jz
{
    namespace sail
    {
        struct ThreePointSettings
        {
        private:
            float mFill;
            Degree mKeyRoll;
            Degree mKeyYaw;

        public:
            static const ThreePointSettings kDefault;
            static const float kMinFill;
            static const float kMaxFill;
            static const Degree kMinYaw;
            static const Degree kMaxYaw;

            ThreePointSettings()
                : mKeyRoll(0), mFill(0), mKeyYaw(0)
            {}

            ThreePointSettings(Degree aKeyRoll, float aFill, Degree aKeyYaw)
            {
                mKeyRoll = aKeyRoll;
                while (mKeyRoll > Degree::k360) { mKeyRoll -= Degree::k360; }
                while (mKeyRoll < Degree::kZero) { mKeyRoll += Degree::k360; }
                mFill = aFill;
                mKeyYaw = aKeyYaw;
                JZ_ASSERT(mFill >= 0.0f);
                JZ_ASSERT(mKeyYaw >= Degree::kZero && mKeyYaw <= Degree::k180);
            }

            float GetFill() const { return mFill; }
            void SetFill(float value)
            {
                mFill = value;
                JZ_ASSERT(mFill >= 0.0f);
            }

            Degree GetKeyRoll() const { return mKeyRoll; }
            void SetKeyRoll(Degree value)
            {
                mKeyRoll = value;
                while (mKeyRoll > Degree::k360) { mKeyRoll -= Degree::k360; }
                while (mKeyRoll < Degree::kZero) { mKeyRoll += Degree::k360; }
            }

            Degree GetKeyYaw() const { return mKeyYaw; }
            void SetKeyYaw(Degree value)
            {
                mKeyYaw = value;
                JZ_ASSERT(mKeyYaw >= Degree::kZero && mKeyYaw <= Degree::k180);
            }
        };

        class LightLearner
        {
        public:
            static const float kStepSize;
            static const float kMotWeight;
            static const float kDesiredWeight;

            static const int kSegments;
            static const int kSegmentsPlus1;
            static const int kSegmentsPlus1Squared;
            static const float kSegmentsF;

            static const int kStorage;

            static const float kJitterFactor;

            static const float kFillFactor;
            static const float kFillFactorHalf;
            static const float kFillJitterScale;
            static const float kFillJitterScaleAdjust;

            static const float kRollFactor;
            static const float kRollFactorHalf;
            static const float kRollJitterScale;
            static const float kRollJitterScaleAdjust;

            static const float kYawFactor;
            static const float kYawFactorHalf;
            static const float kYawJitterScale;
            static const float kYawJitterScaleAdjust;

            static const float kRollTolerance;
            static const float kYawMinimum;

            LightLearner()
            {
                _Init();
            }

            void Init(ThreePointSettings& arSettings)
            {
                _Init();
                _GetNextSettings(arSettings);
            }

            ImageIlluminationMetrics Get(const ThreePointSettings& arSettings);

            void Step(const ImageIlluminationMetrics& aTarget, ThreePointSettings& arCurrent, const ThreePointSettings& aMotivation, float aTimeStep);

            bool Tick(const ImageData& aImageData, ThreePointSettings& arSettings);

            void Save(const string& aFilename);
            void Load(const string& aFilename);

        private:
            LightLearner(const LightLearner&);
            LightLearner& operator=(const LightLearner&);

            int mR;
            int mF;
            int mY;
            MemoryBuffer<ImageIlluminationMetrics> mSamples;

            void _GetNextSettings(ThreePointSettings& arSettings);
            float _GetError(const ImageIlluminationMetrics& arTarget,  const ImageIlluminationMetrics& arCurrent);
            float _GetErrorGradient(const ImageIlluminationMetrics& arTarget, const ImageIlluminationMetrics& ar0, const ImageIlluminationMetrics& ar1);
            float _GetErrorGradientRoll(const ImageIlluminationMetrics& arTarget, const ThreePointSettings& arCurrent);
            float _GetErrorGradientFill(const ImageIlluminationMetrics& arTarget, const ThreePointSettings& arCurrent);
            float _GetErrorGradientYaw(const ImageIlluminationMetrics& arTarget, const ThreePointSettings& arCurrent);

            int _I(int aRoll, int aFill, int aYaw)
            {
                int ret = (aRoll * kSegmentsPlus1Squared) + (aFill * kSegmentsPlus1) + aYaw;

                return ret;
            }

            int _Index() { return _I(mR, mF, mY); }

            bool _IncrementIndex()
            {
                mY++;
                if (mY >= kSegmentsPlus1) { mY = 0; mF++; }
                if (mF >= kSegmentsPlus1) { mF = 0; mR++; }
                if (mR >= kSegments) { return false; }
                else { return true; }
            }

            void _Init()
            {
                mR = 0;
                mF = 0;
                mY = 0;

                mSamples.resize(kStorage);
                mSamples.Initialize();
            }
        };

    }
}

#endif
