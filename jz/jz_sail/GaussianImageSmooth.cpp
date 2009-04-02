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

#include <jz_core/Memory.h>
#include <jz_sail/GaussianImageSmooth.h>

namespace jz
{
    namespace sail
    {

        const int GaussianImageSmooth::kGaussianSmoothPadding = 3;
        const double GaussianImageSmooth::kRetinexKernelRadius = 1.0;
        const float GaussianImageSmooth::kRetinexStdDev = (float)jz::Sqrt(-((kRetinexKernelRadius + 1.0) * (kRetinexKernelRadius + 1.0)) / (2.0 * jz::Log(1.0 / 255.0)));

        GaussianCoefficients GaussianImageSmooth::_CalculateGaussianCoefficients(float aStdDev)
        {
            float q = 0.0f;
            
            if (aStdDev >= 2.5f)
            {
                q = (0.98711f * aStdDev) - 0.96330f;
            }
            else if (aStdDev >= 0.5f)
            {
                q = 3.97156f - (4.14554f * jz::Sqrt(1.0f - (0.26891f * aStdDev)));
            }
            else
            {
                q = 0.1147705f;
            }

            float q2 = q * q;
            float q3 = q * q2;

            GaussianCoefficients ret;
            ret.b[0] = 1.57825f + (2.44413f * q)  + (1.4281f * q2) + (0.422205f * q3);
            ret.b[1] = (2.44413f * q) + (2.85619f  * q2) + (1.26661f  * q3);
            ret.b[2] = -((1.4281f * q2) + (1.26661f  * q3));
            ret.b[3] = (0.422205f * q3);
            
            ret.B = 1.0f - ((ret.b[1] + ret.b[2] + ret.b[3]) / ret.b[0]);

            return ret;
        }

        void GaussianImageSmooth::_GaussianSmooth(float aStdDev, int aX0, int aY0, int aX1, int aY1, int aWidth, int aHeight, PixelFormat aFormat, u8_p arImage)
        {
            GaussianCoefficients c = _CalculateGaussianCoefficients(aStdDev);

            u8_p p = arImage;
            int width = aWidth;
            int height = aHeight;
            int stride = GetStride(aFormat);
            int stride2 = stride * 2;
            int stride3 = stride * 3;
            int stride4 = stride * 4;
            int size = (width * height * stride);
            int padding = kGaussianSmoothPadding;
            int pitch = width * stride;
            int pitch2 = width * stride2;
            int pitch3 = width * stride3;
            int pitch4 = width * stride4;

            MemoryBuffer<float> a(size);
            MemoryBuffer<float> b(size);
            for (int i = 0; i < size; i++) { a[i] = p[i]; b[i] = p[i]; }

            // forward pass, rows
            for (int y = aY0; y <= aY1; y++)
            {
                for (int x = (aX0 + padding); x <= aX1; x++)
                {
                    int i = (y * pitch) + (x * stride);
                    int i0 = i;
                    int i1 = i - stride;
                    int i2 = i - stride2;
                    int i3 = i - stride3;

                    for (int j = 0; j < stride; j++)
                    {
                        float v0 = p[i0 + j];
                        float v1 = a[i1 + j];
                        float v2 = a[i2 + j];
                        float v3 = a[i3 + j];

                        a[i0 + j] = ((c.B * v0) + (((c.b[1] * v1) + (c.b[2] * v2) + (c.b[3] * v3)) / c.b[0]));
                    }
                }
            }

            // forward pass, columns
            for (int x = aX0; x <= aX1; x++)
            {
                for (int y = (aY0 + padding); y <= aY1; y++)
                {
                    int i = (y * pitch) + (x * stride);
                    int i0 = i;
                    int i1 = i - pitch;
                    int i2 = i - pitch2;
                    int i3 = i - pitch3;

                    for (int j = 0; j < stride; j++)
                    {
                        float v0 = p[i0 + j];
                        float v1 = a[i1 + j];
                        float v2 = a[i2 + j];
                        float v3 = a[i3 + j];

                        a[i0 + j] = ((c.B * v0) + (((c.b[1] * v1) + (c.b[2] * v2) + (c.b[3] * v3)) / c.b[0]));
                    }
                }
            }

            // backward pass, rows
            for (int y = aY0; y <= aY1; y++)
            {
                for (int x = (aX1 - padding); x >= aX0; x--)
                {
                    int i = (y * pitch) + (x * stride);
                    int i0 = i;
                    int i1 = i + stride;
                    int i2 = i + stride2;
                    int i3 = i + stride3;

                    for (int j = 0; j < stride; j++)
                    {
                        float v0 = a[i0 + j];
                        float v1 = b[i1 + j];
                        float v2 = b[i2 + j];
                        float v3 = b[i3 + j];

                        b[i0 + j] = ((c.B * v0) + (((c.b[1] * v1) + (c.b[2] * v2) + (c.b[3] * v3)) / c.b[0]));
                    }
                }
            }
            
            // backward pass, columns
            for (int x = aX0; x <= aX1; x++)
            {
                for (int y = (aY1 - padding); y >= aY0; y--)
                {
                    int i = (y * pitch) + (x * stride);
                    int i0 = i;
                    int i1 = i + pitch;
                    int i2 = i + pitch2;
                    int i3 = i + pitch3;

                    for (int j = 0; j < stride; j++)
                    {
                        float v0 = a[i0 + j];
                        float v1 = b[i1 + j];
                        float v2 = b[i2 + j];
                        float v3 = b[i3 + j];

                        b[i0 + j] = ((c.B * v0) + (((c.b[1] * v1) + (c.b[2] * v2) + (c.b[3] * v3)) / c.b[0]));
                    }
                }
            }

            for (int i = 0; i < size; i++) { p[i] = (u8)b[i]; }
        }

    }
}
