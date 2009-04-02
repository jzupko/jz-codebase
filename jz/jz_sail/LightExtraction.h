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
#ifndef _JZ_SAIL_LIGHT_EXTRACTION_H_
#define _JZ_SAIL_LIGHT_EXTRACTION_H_

#include <jz_core/Angle.h>
#include <jz_core/Color.h>
#include <jz_core/Memory.h>
#include <jz_sail/GaussianImageSmooth.h>
#include <jz_system/Files.h>

namespace jz
{
    namespace sail
    {

        struct ImageIlluminationMetrics
        {
            static const float kMaxIntensityMin;
            static const float kMaxIntensityMax;
            static const float kEntropyMin;
            static const float kEntropyMax;
            static const float kRollMin;
            static const float kRollMax;
            static const float kYawMin;
            static const float kYawMax;
            static const float kHalfRollMax;

            float MaxIntensity; // 0...1
            float Entropy; // 0...1
            float Roll; // 0...2, 1:1 correlation with 0...360, set to 0...1 so a separation of 180 has the same weight as a 0...1 separation for other terms.
            float Yaw; // -1...1

            ImageIlluminationMetrics()
                : MaxIntensity(0), Entropy(0), Roll(0), Yaw(0)
            {}

            ImageIlluminationMetrics operator-(const ImageIlluminationMetrics& b) const
            {
                ImageIlluminationMetrics ret;
                ret.MaxIntensity = (MaxIntensity - b.MaxIntensity);
                ret.Entropy = (Entropy - b.Entropy);
                ret.Yaw = (Yaw - b.Yaw);

                float rollA = Roll;
                float rollB = b.Roll;

                while (rollA - rollB > kHalfRollMax) { rollA -= kRollMax; }
                while (rollB - rollA > kHalfRollMax) { rollB -= kRollMax; }

                ret.Roll = (rollA - rollB);

                return ret;
            }

            static float Dot(const ImageIlluminationMetrics& a, const ImageIlluminationMetrics& b)
            {
                float ret =
                    (a.MaxIntensity * b.MaxIntensity) +
                    (a.Entropy * b.Entropy) +
                    (a.Roll * b.Roll) +
                    (a.Yaw * b.Yaw);

                return ret;
            }

            static ImageIlluminationMetrics Lerp(const ImageIlluminationMetrics& a, const ImageIlluminationMetrics& b, float aWeightOfB)
            {
                ImageIlluminationMetrics ret;
                ret.MaxIntensity = jz::Lerp(a.MaxIntensity, b.MaxIntensity, aWeightOfB);
                ret.Entropy = jz::Lerp(a.Entropy, b.Entropy, aWeightOfB);
                ret.Yaw = jz::Lerp(a.Yaw, b.Yaw, aWeightOfB);

                float rollA = a.Roll;
                float rollB = b.Roll;

                while (rollA - rollB > kHalfRollMax) { rollA -= kRollMax; }
                while (rollB - rollA > kHalfRollMax) { rollB -= kRollMax; }

                ret.Roll = jz::Lerp(rollA, rollB, aWeightOfB);

                while (ret.Roll > kRollMax) { ret.Roll -= kRollMax; }
                while (ret.Roll < kRollMin) { ret.Roll += kRollMax; }

                return ret;
            }
        };

        class ImageData
        {
        public:
            static const ColorRGBu kMaskColor;

            ImageData(int aWidth, int aHeight, GaussianImageSmooth::PixelFormat aFormat, u8c_p apData)
                : mWidth(aWidth), mHeight(aHeight), mFormat(aFormat),
                mCount(0),
                mX0(aWidth), mY0(aHeight),
                mX1(0), mY1(0)
            {
                const size_t kSize = (size_t)(aWidth * aHeight * GaussianImageSmooth::GetStride(aFormat));
                mData.resize(kSize);
                mData.CopyFrom(apData);

                _Init();
            }
            
            bool IsValid(int x, int y) const
            {
                const int kIndex = (y * mWidth) + x;
                
                return mMask[kIndex];
            }

            int GetCount() const { return (mCount); }
            int GetHeight() const { return (mHeight); }
            u8c_p GetImageData() const { return (mData.Get()); }
            int GetX0() const { return (mX0); }
            int GetY0() const { return (mY0); }
            int GetX1() const { return (mX1); }
            int GetY1() const { return (mY1); }
            int GetWidth() const { return (mWidth); }

        private:
            void _Init()
            {
                GaussianImageSmooth::PixelFormat format = mFormat;
                int stride = GaussianImageSmooth::GetStride(format);
                int pitch = (mWidth * stride);
                int width = mWidth;
                int height = mHeight;
                u8c_p p = (mData.Get());

                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        int maskIndex = (y * width) + x;
                        int imageIndex = (y * pitch) + (x * stride);

                        if (_PixelIsValid(format, p, imageIndex))
                        {
                            mMask[maskIndex] = true;
                            mX0 = jz::Min(mX0, x);
                            mY0 = jz::Min(mY0, y);
                            mX1 = jz::Max(mX1, x);
                            mY1 = jz::Max(mY1, y);
                            mCount++;
                        }
                        else { mMask[maskIndex] = false; }
                    }
                }

                GaussianImageSmooth::Calculate(mX0, mY0, mX1, mY1, mWidth, mHeight, mFormat, mData.Get());
                
                mData.resize(GaussianImageSmooth::InPlaceConvertToGrayscale(mData.Get(), mData.GetSizeInBytes(), mFormat));
                mFormat = GaussianImageSmooth::kGrayscale;
            }

            int mWidth;
            int mHeight;
            GaussianImageSmooth::PixelFormat mFormat;
            ByteBuffer mData;
            MemoryBuffer<bool> mMask;
            int mCount;
            
            int mX0;
            int mY0;
            int mX1;
            int mY1;

            static bool _PixelIsValid(GaussianImageSmooth::PixelFormat aFormat, u8c_p p, int index)
            {
                ColorRGBu color = GaussianImageSmooth::GetPixel(aFormat, p, index);

                return (color != kMaskColor);
            }
        };

        class LightingExtractor
        {
        public:
            static const size_t kMinimumDataSize;
            static const int kEntropyBinCount;
            static const float kBinCountFactor;
            static const int kWindowSize;
            static const int kWindowHalfSize;
            static const float kPixelFactor;
            static const float kMaxRoll;
            static const float kHalfMaxRoll;
            static const float kBestProbPerBin;
            static const float kMaxEntropy;

            static void ExtractLighting(const ImageData& aImageData, ImageIlluminationMetrics& arOut)
            {
                _CalculateLighting(aImageData, arOut);
            }

        private:
            static float _CalculateEntropy(const ImageData& aData);
            static float _CalculateRoll(const ImageData& aData);
            static void _CalculateYawHelperHelper(const vector<float>& aData, float& arTotalCom, float& arTotalCount);
            static void _CalculateYawHelper(const ImageData& aData, float xSlope, float ySlope, float fX, float fY, float& arTotalCom, float& arTotalCount);
            static float _CalculateYaw(const ImageData& aData, Degree aAngle);
            static float _CalculateMaximumIntensity(const ImageData& aData);

            static void _CalculateLighting(const ImageData& aData, ImageIlluminationMetrics& arMetrics)
            {
                arMetrics.MaxIntensity = _CalculateMaximumIntensity(aData);
                arMetrics.Entropy = _CalculateEntropy(aData);
                arMetrics.Roll = _CalculateRoll(aData);
                arMetrics.Yaw = _CalculateYaw(aData, Degree((arMetrics.Roll / ImageIlluminationMetrics::kRollMax) * kMaxRoll));
            }
        };

    }
}

#endif
