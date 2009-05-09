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
#ifndef _JZ_COLOR_H_
#define _JZ_COLOR_H_

#include <jz_core/Angle.h>
#include <jz_core/Math.h>
#include <jz_core/Prereqs.h>
#include <jz_core/Vector3.h>
#include <jz_core/Vector4.h>

namespace jz
{
    
    static const double gskYfromRgbAlpha = 0.055;
    static const double gskYfromRgbPhi   = 12.92;
    static const double gskYfromRgbTau   = 2.4;
    
    inline double GetYfromRgbHelper(double v)
    {
        static const double k = 1.0 + gskYfromRgbAlpha;
        
        if (v > 0.04045)
        {
            return pow((v + gskYfromRgbAlpha) / k, gskYfromRgbTau);    
        }
        else
        {
            return (v  / gskYfromRgbPhi);
        }
    }
    
    inline float GetYfromRgbHelper(float v)
    {
        return float(GetYfromRgbHelper(double(v)));
    }

    inline double GetYofCIE1931sRGB(double r, double g, double b)
    {
        return ((0.2126 * GetYfromRgbHelper(r)) + (0.7152 * GetYfromRgbHelper(g)) + (0.0722 * GetYfromRgbHelper(b)));
    }

    inline float GetYofCIE1931sRGB(float r, float g, float b)
    {
        return ((0.2126f * GetYfromRgbHelper(r)) + (0.7152f * GetYfromRgbHelper(g)) + (0.0722f * GetYfromRgbHelper(b)));
    }

    inline u8 GetYofCIE1931sRGB(u8 r, u8 g, u8 b)
    {
        static const float skToFactor   = float(1.0 / 255.0);
        static const float skFromFactor = float(255.0);
        
        return u8(GetYofCIE1931sRGB(float(r) * skToFactor, float(g) * skToFactor, float(b) * skToFactor) * skFromFactor);
    }
    
    inline float GetYofCIE1931sRGB(const Vector3& v)
    {
        return GetYofCIE1931sRGB(v.X, v.Y, v.Z);
    }

    inline float GetYofCIE1931sRGB(const Vector4& v)
    {
        return GetYofCIE1931sRGB(v.X, v.Y, v.Z);
    }

    struct ColorHSV;

    struct ColorRGB
    {
        union
        {
            struct
            {
                float R, G, B;
            };
            float pData[3];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorRGB
#       define JZ_NUMBER_ARRAY_DIM  3
#       include <jz_core/_NumberArray.h>

        explicit ColorRGB(const ColorHSV& c)
        {
            SetFromHSV(c);
        }

        void SetFromHSV(const ColorHSV& c);
        
        Vector3 ToVector3() const { return Vector3(R, G, B); }

        static const ColorRGB kBlack;
        static const ColorRGB kWhite;
        static const ColorRGB kHdrWhite;
    };

    inline float GetValue(const ColorRGB& c)
    {
        return Max(c.R, c.G, c.B);
    }

    inline ColorRGB operator*(float s, const ColorRGB& c)
    {
        return ColorRGB(s * c.R, s * c.G, s * c.B);
    }

    inline float GetYofCIE1931sRGB(const ColorRGB& c)
    {
        return GetYofCIE1931sRGB(c.R, c.G, c.B);
    }

    struct ColorRGBu
    {
        union
        {
            struct
            {
                u8 R, G, B;
            };
            u8 pData[3];
        };

        ColorRGBu()
            : R(0), G(0), B(0)
        {}

        ColorRGBu(u8 r, u8 g, u8 b)
            : R(r), G(g), B(b)
        {}

        ColorRGBu(const ColorRGBu& b)
            : R(b.R), G(b.G), B(b.B)
        {}

        ColorRGBu& operator=(const ColorRGBu& b)
        {
            R = b.R;
            G = b.G;
            B = b.B;
        }

        static const ColorRGBu kBlack;
        static const ColorRGBu kMagenta;
        static const ColorRGBu kWhite;
    };

    inline bool operator==(const ColorRGBu& a, const ColorRGBu& b)
    {
        return (a.R == b.R) &&
            (a.G == b.G) &&
            (a.B == b.B);
    }

    inline bool operator!=(const ColorRGBu& a, const ColorRGBu& b)
    {
        return (a.R != b.R) ||
            (a.G != b.G) ||
            (a.B != b.B);
    }

    inline u8 GetYofCIE1931sRGB(const ColorRGBu& c)
    {
        return GetYofCIE1931sRGB(c.R, c.G, c.B);
    }

    struct ColorRGBAu
    {
        union
        {
            struct
            {
                u8 R, G, B, A;
            };
            u8 pData[4];
        };

        static const ColorRGBAu kBlack;
        static const ColorRGBAu kMagenta;
        static const ColorRGBAu kWhite;
    };

    inline bool operator<(const ColorRGBAu& a, const ColorRGBAu& b)
    {
        return (a.R < b.R) ||
            (a.G < b.G) ||
            (a.B < b.B) ||
            (a.A < b.A);
    }

    inline bool operator==(const ColorRGBAu& a, const ColorRGBAu& b)
    {
        return (a.R == b.R) &&
            (a.G == b.G) &&
            (a.B == b.B) &&
            (a.A == b.A);
    }

    inline bool operator!=(const ColorRGBAu& a, const ColorRGBAu& b)
    {
        return (a.R != b.R) ||
            (a.G != b.G) ||
            (a.B != b.B) ||
            (a.A != b.A);
    }

    inline u8 GetYofCIE1931sRGB(const ColorRGBAu& c)
    {
        return GetYofCIE1931sRGB(c.R, c.G, c.B);
    }


    struct ColorRGBA
    {
        union
        {
            struct
            {
                float R, G, B, A;
            };
            float pData[4];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorRGBA
#       define JZ_NUMBER_ARRAY_DIM  4
#       include <jz_core/_NumberArray.h>

        explicit ColorRGBA(const ColorRGBAu& c)
        {
            static const float kFactor = (float)(1.0 / 255.0);

            R = (c.R * kFactor);
            G = (c.G * kFactor);
            B = (c.B * kFactor);
            A = (c.A * kFactor);
        }

        explicit ColorRGBA(const ColorHSV& hsv)
        {
            ColorRGB c(hsv);
            
            R = c.R;
            G = c.G;
            B = c.B;
            A = 1.0f;
        }

        static const ColorRGBA kBlack;
        static const ColorRGBA kWhite;
    };

    inline ColorRGBA operator*(float s, const ColorRGBA& c)
    {
        return ColorRGBA(s * c.R, s * c.G, s * c.B, s * c.A);
    }

    inline float GetYofCIE1931sRGB(const ColorRGBA& c)
    {
        return GetYofCIE1931sRGB(c.R, c.G, c.B);
    }

    struct ColorHSL
    {
        union
        {
            struct
            {
                float H, S, L;
            };
            float pData[3];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorHSL
#       define JZ_NUMBER_ARRAY_DIM  3
#       include <jz_core/_NumberArray.h>

        explicit ColorHSL(const ColorRGB& c)
        {
            SetFromRGB(c);
        }

        void SetFromRGB(const ColorRGB& c);
        void SetFromRGBu(const ColorRGBu& c);

        static const ColorHSL kBlack;
        static const ColorHSL kWhite;
    };

    inline ColorHSL operator*(float s, const ColorHSL& c)
    {
        return ColorHSL(s * c.H, s * c.S, s * c.L);
    }

    struct ColorHSLA
    {
        union
        {
            struct
            {
                float H, S, L, A;
            };
            float pData[4];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorHSLA
#       define JZ_NUMBER_ARRAY_DIM  4
#       include <jz_core/_NumberArray.h>

        static const ColorHSLA kBlack;
        static const ColorHSLA kWhite;
    };

    inline ColorHSLA operator*(float s, const ColorHSLA& c)
    {
        return ColorHSLA(s * c.H, s * c.S, s * c.L, s * c.A);
    }

    struct ColorHSV
    {
        union
        {
            struct
            {
                float H, S, V;
            };
            float pData[3];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorHSV
#       define JZ_NUMBER_ARRAY_DIM  3
#       include <jz_core/_NumberArray.h>

        explicit ColorHSV(const ColorRGB& c)
        {
            SetFromRGB(c);
        }
        
        explicit ColorHSV(const ColorRGBA& c)
        {
            SetFromRGB(ColorRGB(c.R, c.G, c.B));
        }

        void SetFromRGB(const ColorRGB& c);

        static const ColorHSV kBlack;
        static const ColorHSV kWhite;
    };

    inline ColorHSV operator*(float s, const ColorHSV& c)
    {
        return ColorHSV(s * c.H, s * c.S, s * c.V);
    }

    struct ColorHSVA
    {
        union
        {
            struct
            {
                float H, S, V, A;
            };
            float pData[4];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorHSVA
#       define JZ_NUMBER_ARRAY_DIM  4
#       include <jz_core/_NumberArray.h>

        static const ColorHSVA kBlack;
        static const ColorHSVA kWhite;
    };

    inline ColorHSVA operator*(float s, const ColorHSVA& c)
    {
        return ColorHSVA(s * c.H, s * c.S, s * c.V, s * c.A);
    }

    struct ColorXYZ
    {
        union
        {
            struct
            {
                float X, Y, Z;
            };
            float pData[3];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorXYZ
#       define JZ_NUMBER_ARRAY_DIM  3
#       include <jz_core/_NumberArray.h>

        void SetFromRGB(const ColorRGB& c);
    };

    inline ColorXYZ operator*(float s, const ColorXYZ& c)
    {
        return ColorXYZ(s * c.X, s * c.Y, s * c.Z);
    }

    struct ColorLUV
    {
        union
        {
            struct
            {
                float L, U, V;
            };
            float pData[3];
        };

#       define JZ_NUMBER_ARRAY_NAME ColorLUV
#       define JZ_NUMBER_ARRAY_DIM  3
#       include <jz_core/_NumberArray.h>

        void SetFromXYZ(const ColorXYZ& c);
        void SetFromRGB(const ColorRGB& c);
    };

    inline ColorLUV operator*(float s, const ColorLUV& c)
    {
        return ColorLUV(s * c.L, s * c.U, s * c.V);
    }

}

#endif
