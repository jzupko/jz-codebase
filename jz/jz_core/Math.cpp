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
// #define JZ_SECURE_RAND 1

#ifdef JZ_SECURE_RAND
#   if JZ_PLATFORM_WINDOWS
#       define _CRT_RAND_S
#   endif
#endif

#include <cstdlib>

namespace jz
{

    float Gaussian1D(float dx, float aStdDev)
    {
        double dx2 = (double)(dx * dx);
        double stdDev2 = (double)(aStdDev * aStdDev);

        double kFactor = (1.0 / Sqrt(2.0 * Constants<double>::kPi * stdDev2));
        double ret = kFactor * Exp(-dx2 / (2.0 * stdDev2));

        return (float)ret;
    }

    float Round(float a)
    {
        float n;
        float f = modf(a, &n);
        
        if (GreaterThan(Abs(f), 0.5f) || AboutEqual(Abs(f), 0.5f))
        {
            if (n < 0.0f) { n -= 1.0f; }
            else { n += 1.0f; }
        }
        
        return n;
    }
    
    double Round(double a)
    {
        double n;
        double f = modf(a, &n);
        
        if (GreaterThan(Abs(f), 0.5))
        {
            if (n < 0.0) { n -= 1.0; }
            else { n += 1.0; }
        }
        
        return n;    
    }

    // (c) Copyright 1994, Everett F. Carter Jr.
    // Permission is granted by the author to use
    // this software for any application provided this
	// copyright notice is preserved.        
    float NormalRandomf(float aMean, float aStdDev)
    {
        float x1(0);
        float x2(0);
        float w(0);
        float y1(0);
        static float sLast(0);
        static bool sbUseLast = false;

        if (sbUseLast)
        {
            sbUseLast = false;
            return (aMean + (sLast * aStdDev));
        }
        else
        {
            do
            {
                x1 = 2.0f * UniformRandomf() - 1.0f;
                x2 = 2.0f * UniformRandomf() - 1.0f;
                w  = x1 * x1 + x2 * x2;
            } while (GreaterThan(w, 1.0f) || AboutEqual(w, 1.0f));
            
            w  = Sqrt((-2.0f * log(w)) / w);

            sLast     = x2 * w;
            sbUseLast = true;
            
            return (aMean + (x1 * w * aStdDev));
        }
    }
        
    float UniformRandomf()
    {
#       if (JZ_PLATFORM_WINDOWS && JZ_SECURE_RAND)
            uint n;
            rand_s(&n);
        
            return (float)(double(n) / double(UINT_MAX));
#       else
            return ((float)rand() / float(RAND_MAX));
#       endif
    }
        
    // (c) Copyright 1994, Everett F. Carter Jr.
    // Permission is granted by the author to use
    // this software for any application provided this
	// copyright notice is preserved.        
    double NormalRandomd(double aMean, double aStdDev)
    {
        double x1(0);
        double x2(0);
        double w(0);
        double y1(0);
        static double sLast(0);
        static bool sbUseLast = false;

        if (sbUseLast)
        {
            sbUseLast = false;
            return (aMean + (sLast * aStdDev));
        }
        else
        {
            do
            {
                x1 = 2.0 * UniformRandomd() - 1.0;
                x2 = 2.0 * UniformRandomd() - 1.0;
                w  = x1 * x1 + x2 * x2;
            } while (GreaterThan(w, 1.0) || AboutEqual(w, 1.0));
            
            w  = Sqrt((-2.0 * log(w)) / w);

            sLast     = x2 * w;
            sbUseLast = true;
            
            return (aMean + (x1 * w * aStdDev));
        }
    }
    
    double UniformRandomd()
    {
#       if (JZ_PLATFORM_WINDOWS && JZ_SECURE_RAND)
            uint n;
            rand_s(&n);
        
            return (double(n) / double(UINT_MAX));
#       else
            return ((double)rand() / double(RAND_MAX));
#       endif
    }
   
}

