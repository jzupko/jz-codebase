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

#include <jz_core/Math.h>
#include <jz_core/Vector2.h>
#include <jz_sail/LightExtraction.h>

namespace jz
{
    namespace sail
    {
        const float ImageIlluminationMetrics::kMaxIntensityMin = 0.0f;
        const float ImageIlluminationMetrics::kMaxIntensityMax = 1.0f;
        const float ImageIlluminationMetrics::kEntropyMin = 0.0f;
        const float ImageIlluminationMetrics::kEntropyMax = 1.0f;
        const float ImageIlluminationMetrics::kRollMin = 0.0f;
        const float ImageIlluminationMetrics::kRollMax = 2.0f;
        const float ImageIlluminationMetrics::kYawMin = -1.0f;
        const float ImageIlluminationMetrics::kYawMax = 1.0f;

        const float ImageIlluminationMetrics::kHalfRollMax = 0.5f * kRollMax;

        const ColorRGBu ImageData::kMaskColor = ColorRGBu::kMagenta;

        const size_t LightingExtractor::kMinimumDataSize = 9;
        const int LightingExtractor::kEntropyBinCount = 30;
        const float LightingExtractor::kBinCountFactor = (float)(kEntropyBinCount - 1);
        const int LightingExtractor::kWindowSize = 3;
        const int LightingExtractor::kWindowHalfSize = kWindowSize / 2;
        const float LightingExtractor::kPixelFactor = (float)(1.0 / 255.0);
        const float LightingExtractor::kMaxRoll = 360.0f;
        const float LightingExtractor::kHalfMaxRoll = kMaxRoll * 0.5f;
        const float LightingExtractor::kBestProbPerBin = (float)(1.0 / ((double)kEntropyBinCount));

        const float LightingExtractor::kMaxEntropy = -(float)(((float)kEntropyBinCount) * (kBestProbPerBin * jz::Log(kBestProbPerBin)));

        float LightingExtractor::_CalculateEntropy(const ImageData& aData)
        {
            const float binFactor = 1.0f / ((float)aData.GetCount());
            u8c_p p = aData.GetImageData();
            int width = aData.GetWidth();
            int height = aData.GetHeight();
            int size = (width * height);
            int x0 = aData.GetX0();
            int y0 = aData.GetY0();
            int x1 = aData.GetX1();
            int y1 = aData.GetY1();
                    
            MemoryBuffer<int> bins = MemoryBuffer<int>(kEntropyBinCount);
            bins.Initialize();
            
            for (int y = y0; y <= y1; y++)
            {
                for (int x = x0; x <= x1; x++)
                {
                    if (aData.IsValid(x, y))
                    {
                        int index = (y * width) + x;
                        u8 value = p[index];
                        int binIndex = (int)((((float)value) / 255.0f) * ((float)(kEntropyBinCount - 1)));
                        bins[binIndex]++;
                    }
                }
            }

            float entropy = 0.0f;
            for (int i = 0; i < kEntropyBinCount; i++)
            {
                float prob = ((float)bins[i]) * binFactor;
                
                if (!jz::AboutZero(prob))
                {
                    entropy += prob * jz::Log(prob);
                }
            }
            
            return (-entropy) / kMaxEntropy;
        }

        float LightingExtractor::_CalculateRoll(const ImageData& aData)
        {
            u8c_p p = aData.GetImageData();
            int width = aData.GetWidth();
            int height = aData.GetHeight();
            int size = (width * height);
            int x0 = aData.GetX0();
            int y0 = aData.GetY0();
            int x1 = aData.GetX1();
            int y1 = aData.GetY1();
            
            int count = 0;
            float dirX  = 0.0f;
            float dirY  = 0.0f;
            
            for (int y = y0 + kWindowHalfSize; y <= y1 - kWindowHalfSize; y++)
            {
                for (int x = x0 + kWindowHalfSize; x <= x1 - kWindowHalfSize; x++)
                {
                    int centerMass  = 0;
                    int centerX     = 0;
                    int centerY     = 0;
                    int brightMass  = 0;
                    int brightX     = 0;
                    int brightY     = 0;
                    
                    for (int j = y - kWindowHalfSize; j <= y + kWindowHalfSize; j++)
                    {
                        for (int i = x - kWindowHalfSize; i <= x + kWindowHalfSize; i++)
                        {
                            if (aData.IsValid(i, j))
                            {
                                int kIndex = (j * width) + i;
                                
                                int val = p[kIndex];
                                    
                                centerMass += 255 - val;
                                centerX    += i * (255 - val);
                                centerY    += j * (255 - val);
                                 
                                brightMass += val;
                                brightX    += i * val;
                                brightY    += j * val;
                            }
                        }
                    }

                    if (centerMass > 0 && brightMass > 0)
                    {
                        float fX = ((float)brightX / (float)brightMass) - ((float)centerX / (float)centerMass);
                        float fY = ((float)brightY / (float)brightMass) - ((float)centerY / (float)centerMass);
                     
                        dirX  = dirX + jz::Sign(fX);
                        dirY  = dirY + jz::Sign(fY);
                        count = count + 1;
                    }
                 }
            }

            if (count > 0)
            {
                dirX = dirX / (float)count;
                dirY = dirY / (float)count;
                
                if (jz::AboutZero(dirX)) { dirX = 0.0f; }
                if (jz::AboutZero(dirY)) { dirY = 0.0f; }
            }

            Vector2 v = Vector2::Normalize(Vector2(dirX, dirY));
            Degree roll = jz::ACos(jz::Clamp(v.X, -1.0f, 1.0f));

            if (jz::LessThan(v.Y, 0.0f)) { roll = Degree(kMaxRoll) - roll; }
            
            float ret = (roll.GetValue() / kHalfMaxRoll);

            return ret;
        }

        void LightingExtractor::_CalculateYawHelperHelper(const vector<float>& aData, float& arTotalCom, float& arTotalCount)
        {
            int count = (int)aData.size();
            int window = count;
            int windowCenter = window / 2;
            float windowScale = (float)(1.0 / (double)windowCenter);
            
            if (count >= window)
            {
                for (int i = 0; i + window <= count; i++)
                {
                    float com  = 0.0f;
                    float mass = 0.0f;
                    
                    for (int j = i; j < i + window; j++)
                    {
                        com  += (float)(j - i - windowCenter) * aData[j];
                        mass += aData[j];
                    }
                    
                    if (jz::GreaterThan(mass, 0.0f))
                    {
                        com /= mass;
                        com *= windowScale;
                        
                        arTotalCom += com;
                        arTotalCount += 1.0f;
                    }
                }
            }    
        }

        void LightingExtractor::_CalculateYawHelper(const ImageData& aData, float xSlope, float ySlope, float fX, float fY, float& arTotalCom, float& arTotalCount)
        {
            static const float skFactor = (float)(1.0 / 255.0);

            u8c_p p = aData.GetImageData();
            int width = aData.GetWidth();
            int height = aData.GetHeight();
            
            int x = (int)fX;
            int y = (int)fY;

            vector<float> data;
            
            while (fX >= 0.0f && fY >= 0.0f && x < width && y < height)
            {
                int kIndex = (y * width) + x;
                
                if (aData.IsValid(x, y))
                {
                    float v = (skFactor * p[kIndex]);
                    data.push_back(v);
                }
                else if (data.size() >= kMinimumDataSize)
                {
                    _CalculateYawHelperHelper(data, arTotalCom, arTotalCount);
                    data.clear();
                }
                
                fX += xSlope;
                fY += ySlope;
                
                x = (int)fX;
                y = (int)fY;
            }
            
            if (data.size() >= kMinimumDataSize)
            {
                _CalculateYawHelperHelper(data, arTotalCom, arTotalCount);
            }
        }

        float LightingExtractor::_CalculateYaw(const ImageData& aData, Degree aAngle)
        {
            static const float kYawAdjustment = (float)(3.0 / 2.0);

            u8c_p p = aData.GetImageData();
            int width = aData.GetWidth();
            int height = aData.GetHeight();
            int size = (width * height);
            int x0 = aData.GetX0();
            int y0 = aData.GetY0();
            int x1 = aData.GetX1();
            int y1 = aData.GetY1();

            float xSlope = jz::Cos(aAngle);
            float ySlope = jz::Sin(aAngle);
            float x = jz::LessThan(xSlope, 0.0f) ? (float)x1 : (float)x0;
            float y = jz::LessThan(ySlope, 0.0f) ? (float)y1 : (float)y0;
            
            float totalCom = 0.0f;
            float totalCount = 0.0f;

            for (int xStart = x0; xStart <= x1; xStart++)
            {
                _CalculateYawHelper(aData, xSlope, ySlope, (float)xStart, y, totalCom, totalCount);
            }     
            
            for (int yStart = y0; yStart <= y1; yStart++)
            {
                _CalculateYawHelper(aData, xSlope, ySlope, x, (float)yStart, totalCom, totalCount);
            }

            float ret = 0.0f;
            if (totalCount > Constants<float>::kZeroTolerance) { ret = jz::Clamp((totalCom / totalCount) * kYawAdjustment, -1.0f, 1.0f); }

            return ret;
        }

        float LightingExtractor::_CalculateMaximumIntensity(const ImageData& aData)
        {        
            static const float factor = (float)(1.0 / 255.0);

            u8c_p p = aData.GetImageData();
            int width = aData.GetWidth();
            int height = aData.GetHeight();
            int size = (width * height);
            int x0 = aData.GetX0();
            int y0 = aData.GetY0();
            int x1 = aData.GetX1();
            int y1 = aData.GetY1();

            u8 max = 0;

            for (int y = y0; y <= y1; y++)
            {
                for (int x = x0; x <= x1; x++)
                {
                    if (aData.IsValid(x, y))
                    {
                        int kIndex = (y * width) + x;
                        u8 kValue = p[kIndex];
                        
                        max = jz::Max(max, kValue);
                    }
                }
            }

            float ret = (max * factor);

            return ret;
        }

    }
}
