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

#include <jz_core/Color.h>
#include <jz_core/Math.h>

namespace jz
{

    const ColorRGB ColorRGB::kBlack    = ColorRGB(0, 0, 0);
    const ColorRGB ColorRGB::kWhite    = ColorRGB(1, 1, 1);

    const ColorRGBu ColorRGBu::kBlack = ColorRGBu(0, 0, 0);
    const ColorRGBu ColorRGBu::kMagenta = ColorRGBu(255, 0, 255);
    const ColorRGBu ColorRGBu::kWhite = ColorRGBu(255, 255, 255);

    const ColorRGBAu ColorRGBAu::kBlack = { 0, 0, 0, 255 };
    const ColorRGBAu ColorRGBAu::kMagenta = { 255, 0, 255, 255 };
    const ColorRGBAu ColorRGBAu::kWhite = { 255, 255, 255, 255 };
    
    const ColorRGBA ColorRGBA::kBlack  = ColorRGBA(0, 0, 0, 1);
    const ColorRGBA ColorRGBA::kWhite  = ColorRGBA(1, 1, 1, 1);

    const ColorHSL ColorHSL::kBlack    = ColorHSL(0, 0, 0);
    const ColorHSL ColorHSL::kWhite    = ColorHSL(1, 1, 1);

    const ColorHSLA ColorHSLA::kBlack  = ColorHSLA(0, 0, 0, 1);
    const ColorHSLA ColorHSLA::kWhite  = ColorHSLA(1, 1, 1, 1);

    const ColorHSV ColorHSV::kBlack    = ColorHSV(0, 0, 0);
    const ColorHSV ColorHSV::kWhite    = ColorHSV(1, 1, 1);

    const ColorHSVA ColorHSVA::kBlack  = ColorHSVA(0, 0, 0, 1);
    const ColorHSVA ColorHSVA::kWhite  = ColorHSVA(1, 1, 1, 1);

    double GetYfromRgbHelper(double v)
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

    void ColorRGB::SetFromHSV(const ColorHSV& c)
    {
        if (jz::AboutZero(c.S))
        {
            R = c.V;
            G = c.V;
            B = c.V;
        }
        else
        {
            float tempH = c.H * 6.0f;

            if (jz::AboutEqual(tempH, 6.0f))
            {
                tempH = 0;
            }

            int tempHint = (int)tempH;
            
            float temp1 = c.V * (1.0f - c.S);
            float temp2 = c.V * (1.0f - (c.S * (tempH - float(tempHint))));
            float temp3 = c.V * (1.0f - (c.S * (1.0f - (tempH - float(tempHint)))));
            
            if (tempHint == 0)
            {
                R = c.V;
                G = temp3;
                B = temp1;
            }
            else if (tempHint == 1)
            {
                R = temp2;
                G = c.V;
                B = temp1;
            }
            else if (tempHint == 2)
            {
                R = temp1;
                G = c.V;
                B = temp3;
            }
            else if (tempHint == 3)
            { 
                R = temp1;
                G = temp2;
                B = c.V;
            }
            else if (tempHint == 4)
            {
                R = temp3;
                G = temp1;
                B = c.V;
            }
            else
            {
                R = c.V;
                G = temp1;
                B = temp2;
            }
        }
    }

    void ColorHSL::SetFromRGBu(const ColorRGBu& c)
    {
        static const float kFactor = (float)(1.0 / 255.0);
        
        ColorRGB fC = ColorRGB(kFactor * c.R, kFactor * c.G, kFactor * c.B);

        SetFromRGB(fC);
    }

    void ColorHSL::SetFromRGB(const ColorRGB& c)
    {
	    static const float k1 = (float)(1.0 / 6.0);
	    static const float k2 = (float)(1.0 / 3.0);
	    static const float k3 = (float)(2.0 / 3.0);

        float vmin = jz::Min(c.R, c.G, c.B);
	    float vmax = jz::Max(c.R, c.G, c.B);
	    float diff = (vmax - vmin);
	    float sum  = (vmax + vmin);

	    L = (sum * 0.5f);

        if (jz::AboutZero(diff))
	    {
		    H = 0.0;
		    S = 0.0;
	    }
	    else
	    {
		    if (L < 0.5f) { S = (diff / sum); }
		    else          { S = (diff / (2.0f - sum)); }

			     if (vmax == c.R) { H = ((c.G - c.B) / diff); }
		    else if (vmax == c.G) { H = (2.0f + ((c.B - c.R) / diff)); }
		    else                  { H = (4.0f + ((c.R - c.G) / diff)); }
    		
		    H *= k1;
	    }
    }

    void ColorHSV::SetFromRGB(const ColorRGB& c)
    {
        const float k1 = float(1.0 / 6.0);
        const float k2 = float(1.0 / 3.0);
        const float k3 = float(2.0 / 3.0);

        float min = jz::Min(c.R, c.G, c.B);
        float max = jz::Max(c.R, c.G, c.B);
        float dif = max - min;

        V = max;

        if (jz::AboutZero(max))
        {
            S = 0.0f;
        }
        else
        {
            S = 1.0f - (min / max);
        }

        if (jz::AboutEqual(max, min))
        {
            H = 0.0f;
        }
        else if (max == c.R)
        {
            H = k1 * ((c.G - c.B) / dif);

            if (c.G < c.B)
            {
                H += 1.0f;
            }
        }
        else if (max == c.G)
        {
            H = (k1 * ((c.B - c.R) / dif)) + k2;
        }
        else
        {
            H = (k1 * ((c.R - c.G) / dif)) + k3;
        }
    }

    void ColorXYZ::SetFromRGB(const ColorRGB& c)
    {
        float r = c.R;
        float g = c.G;
        float b = c.B;

        if (GreaterThan(r, 0.04045f))
        {
            r = pow((r + 0.055f) / 1.055f, 2.4f);
        }
        else
        {
            r /= 12.92f;
        }

        if (GreaterThan(g, 0.04045f))
        {
            g = pow((g + 0.055f) / 1.055f, 2.4f);
        }
        else
        {
            g /= 12.92f;
        }

        if (GreaterThan(b, 0.04045f))
        {
            b = pow((b + 0.055f) / 1.055f, 2.4f);
        }
        else
        {
            b /= 12.92f;
        }

        r *= 100.0f;
        g *= 100.0f;
        b *= 100.0f;

        // Observer at 2 degrees, Illuminant = D65
        X = (r * 0.4124f)
          + (g * 0.3576f)
          + (b * 0.1805f);
        Y = (r * 0.2126f)
          + (g * 0.7152f)
          + (b * 0.0722f);
        Z = (r * 0.0193f)
          + (g * 0.1192f)
          + (b * 0.9505f);
    }

    void ColorLUV::SetFromXYZ(const ColorXYZ& c)
    {
        float denom =         c.X 
                   + (15.0f * c.Y) 
                   + (3.0f  * c.Z);

        float nom_u = 4.0f * c.X;
        float nom_v = 9.0f * c.Y;
        float u     = 0;
        float v     = 0;

        if (jz::AboutZero(denom))
        {
            JZ_ASSERT(jz::AboutZero(nom_u));
            JZ_ASSERT(jz::AboutZero(nom_v));
        }
        else
        {
            u = nom_u / denom;
            v = nom_v / denom;
        }

        float y = c.Y / 100.0f;

        if (GreaterThan(y, 0.008856f))
        {
            y = pow(y, float(1.0 / 3.0));
        }
        else
        {
            y = (7.787f * y) + float(16.0/116.0);
        }
        
        // Observer at 2 degrees, Illuminant = D65
        const float refX = 95.047f;
        const float refY = 100.000f;
        const float refZ = 108.883f;
        
        float refU = (4.0f * refX) 
                   / (refX + (15.0f * refY) 
                           + (3.0f  * refZ));
        float refV = (9.0f * refY) 
                   / (refX + (15.0f * refY) 
                           + (3.0f  * refZ));

        L = (116.0f * y) - 16.0f;
        U =  13.0f * L * (u - refU);
        V =  13.0f * L * (v - refV);
    }

    void ColorLUV::SetFromRGB(const ColorRGB& c)
    {
        ColorXYZ xyz;

        xyz.SetFromRGB(c);

        SetFromXYZ(xyz);
    }

}
