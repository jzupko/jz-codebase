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
#ifndef _JZ_SAIL_GAUSSIAN_IMAGE_SMOOTH_H_
#define _JZ_SAIL_GAUSSIAN_IMAGE_SMOOTH_H_

#include <jz_core/Color.h>

namespace jz
{
    namespace sail
    {

        struct GaussianCoefficients
        {
            static const int kBdim = 4;

            GaussianCoefficients()
                : B(0.0f)
            {
                for (int i = 0; i < kBdim; i++) { b[i] = 0; }
            }

            float b[kBdim];
            float B;
        };

        /// <summary>
        /// Smooths an image with a gaussian kernel.
        /// </summary>
        /// <remarks>
        /// Uses the algorithm from: Young, I. T., Van Vliet, L. J., 
        ///     "Recursive implementation of the Gaussian filter", Signal Processing, 44, 139-151.
        /// </remarks>
        class GaussianImageSmooth
        {
        public:
            static const int kGaussianSmoothPadding;
            static const double kRetinexKernelRadius;
            static const float kRetinexStdDev;

            enum PixelFormat
            {
                kA8R8G8B8 = 0, // expected little-endian, ordered BGRA in memory.
                kR8G8B8 = 1, // expected little-endian, ordered BGR in memory.
                kA8B8G8R8 = 2, // expected little-endian, ordered RGBA in memory.
                kBGR = 3, // expected little-endian, ordered RGB in memory.
                kGrayscale = 4
            };

            static void Calculate(int aX0, int aY0, int aX1, int aY1, int aWidth, int aHeight, PixelFormat aFormat, u8_p apImage)
            {
                _GaussianSmooth(kRetinexStdDev, aX0, aY0, aX1, aY1, aWidth, aHeight, aFormat, apImage);
            }

            static size_t InPlaceConvertToGrayscale(u8_p p, size_t aDataSize, PixelFormat aFormat)
            {
                if (aFormat == kGrayscale) { return (aDataSize); }

                const int kSize = (int)aDataSize;
                const int kInStride = GetStride(aFormat);
                const int kOutStride = 1;

                for (int in = 0, out = 0; in < kSize; in += kInStride, out += kOutStride)
                {
                    ColorRGBu c = GetPixel(aFormat, p, in);
                    p[out] = GetYofCIE1931sRGB(c);
                }

                return (size_t)(aDataSize / kInStride);
            }

            static int GetStride(PixelFormat aFormat)
            {
                switch (aFormat)
                {
                case kA8R8G8B8: return 4; break;
                case kR8G8B8: return 3; break;
                case kA8B8G8R8: return 4; break;
                case kBGR: return 3; break;
                case kGrayscale: return 1; break;
                default:
                    JZ_ASSERT(false);
                    return 0;
                    break;
                }
            }

            static ColorRGBu GetPixel(PixelFormat aFormat, u8c_p p, size_t aIndex)
            {
                const int kStride = GetStride(aFormat);

                if (kStride >= 3)
                {
                    ColorRGBu ret;
                    ret.R = (aFormat == kA8B8G8R8 || aFormat == kBGR) ? p[aIndex + 0u] : p[aIndex + 2u];
                    ret.G = p[aIndex + 1u];
                    ret.B = (aFormat == kA8B8G8R8 || aFormat == kBGR) ? p[aIndex + 2u] : p[aIndex + 0u];

                    return ret;
                }
                else if (kStride == 1)
                {
                    ColorRGBu ret(p[aIndex], p[aIndex], p[aIndex]);
                    return ret;
                }
                else
                {
                    JZ_ASSERT(false);
                }
            }

        private:
            static GaussianCoefficients _CalculateGaussianCoefficients(float aStdDev);
            static void _GaussianSmooth(float aStdDev, int aX0, int aY0, int aX1, int aY1, int aWidth, int aHeight, PixelFormat aFormat, u8_p arImage);
        };

    }
}

#endif
