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
#ifndef _JZ_MATH_H_
#define _JZ_MATH_H_

#include <jz_core/Prereqs.h>

namespace jz
{

    #pragma region Basic math functions
    float  NormalRandomf(float  aMean = 0.0f, float  aStdDev = 1.0f);
    double NormalRandomd(double aMean = 0.0f, double aStdDev = 1.0f);
    
    // between 0...1
    float  UniformRandomf();
    double UniformRandomd();

    template <typename T>
    __inline bool AboutEqual(T a, T b, T aEpsilon = Constants<T>::kZeroTolerance)
    {
        JZ_STATIC_ASSERT(std::numeric_limits<T>::is_specialized);

        return (Abs(a - b) < aEpsilon);
    }

    template <typename T>
    __inline bool AboutZero(T a, T aEpsilon = Constants<T>::kZeroTolerance)
    {
        return (Abs(a) < aEpsilon);
    }

    template <typename T>
    __inline T Abs(T a)
    {
        JZ_STATIC_ASSERT(std::numeric_limits<T>::is_specialized);
        JZ_STATIC_ASSERT(std::numeric_limits<T>::is_signed);

        return (a < T(0)) ? -a : a;
    }

    template <typename T>
    __inline T Ceil(T v)
    {
        return (T)std::ceil(v);
    }

    template <typename T>
    __inline T Exp(T v)
    {
        return (T)std::exp(v);
    }

    template <typename T>
    __inline T Floor(T v)
    {
        return (T)std::floor(v);
    }

    template <typename T>
    __inline bool GreaterThan(T a, T b, T aEpsilon = Constants<T>::kZeroTolerance)
    {
        return (a >= (b + aEpsilon));
    }

    __inline float Lerp(float v1, float v2, float aWeightOfV2)
    {
        // (v1 * (1.0 - aWeightOfV2) + v2 * aWeightOfV2)
        // (v1 - v1 * aWeightOfV2 + v2 * aWeightOfV2
        // (v1 + ((v2 - v1) * aWeightOfV2))
        return v1 + ((v2 - v1) * aWeightOfV2);
    }

    __inline double Lerp(double v1, double v2, double aWeightOfV2)
    {
        return v1 + ((v2 - v1) * aWeightOfV2);
    }

    template <typename T>
    __inline bool LessThan(T a, T b, T aEpsilon = Constants<T>::kZeroTolerance)
    {
        return (a <= (b - aEpsilon));
    }

    template <typename T>
    __inline T Log(T v)
    {
        return (T)std::log(v);
    }

    template <typename T>
    __inline T Pow(T v, T p)
    {
        return (T)std::pow(v, p);
    }

    template <typename T>
    __inline T Sign(T v)
    {
        JZ_STATIC_ASSERT(std::numeric_limits<T>::is_specialized);
        JZ_STATIC_ASSERT(std::numeric_limits<T>::is_signed);

        return (v == T(0)) ? T(0) : ((v < T(0)) ? T(-1) : T(1));
    }

    template <>
    __inline float Sign(float v)
    {
        return AboutZero(v) ? 0.0f : ((v < 0.0f) ? -1.0f : 1.0f);
    }

    template <>
    __inline double Sign(double v)
    {
        return AboutZero(v) ? 0.0 : ((v < 0.0) ? -1.0 : 1.0);
    }
   
    template <typename T>
    __inline T RSqrt(T v)
    {
        return (T)std::rsqrt(v);
    }

    template <typename T>
    __inline T Sqrt(T v)
    {
        return (T)std::sqrt(v);
    }

    float Gaussian1D(float dx, float aStdDev);
    #pragma endregion

    #pragma region Stat functions
    template <typename T>
    typename T::value_type Max(const T& a)
    {
        typedef typename T::value_type     V;
        typedef typename T::const_iterator ConstItr;

        V max(V(0));
        
        for (ConstItr I = a.begin(); I != a.end(); I++)
        {
            max = jz::Max(max, *I);
        }
        
        return max;
    }

    template <typename T>
    typename T::value_type Min(const T& a)
    {
        typedef typename T::value_type     V;
        typedef typename T::const_iterator ConstItr;

        V min(V(0));
        
        for (ConstItr I = a.begin(); I != a.end(); I++)
        {
            min = jz::Min(min, *I);
        }
        
        return min;
    }

    template <typename T>
    typename T::value_type Mean(const T& a)
    {
        typedef typename T::value_type     V;
        typedef typename T::const_iterator ConstItr;

        V mean = V(0);

        if (a.size() == 0)
        {
            return 0;
        }

        for (ConstItr I = a.begin(); I != a.end(); I++)
        {
            mean += *I;
        }

        mean /= V(a.size());

        return mean;
    }

    template <typename T>
    typename T::value_type Kurtosis(const T& a, const typename T::value_type& aMean, const typename T::value_type& aStdDev)
    {
        typedef typename T::value_type     V;
        typedef typename T::const_iterator ConstItr;

        V n = V(0);
        
        if (a.size() < 2)
        {
            return Constants<V>::kNaN;
        }
        
        for (ConstItr I = a.begin(); I != a.end(); I++)
        {
            V t = (*I - aMean);
            
            float t2 = t * t;
            
            n += (t2 * t2);
        }
        
        float stdDev2 = aStdDev * aStdDev;
        
        return (n / (V(a.size() - 1) * stdDev2 * stdDev2)) - 3.0f;
    }

    float Round(float a);    
    double Round(double a);

    template <typename T>
    typename T::value_type Skew(const T& a, const typename T::value_type& aMean, const typename T::value_type& aStdDev)
    {
        typedef typename T::value_type     V;
        typedef typename T::const_iterator ConstItr;

        V n = V(0);
        
        if (a.size() < 2)
        {
            return Constants<V>::kNaN;
        }
        
        for (ConstItr I = a.begin(); I != a.end(); I++)
        {
            V t = (*I - aMean);
            
            n += (t * t * t);
        }
        
        return (n / (V(a.size() - 1) * aStdDev * aStdDev * aStdDev));
    }

    template <typename T>
    __inline T StatNormalize(const T& aValue, const T& aMean, const T& aStdDev)
    {
        return (aValue - aMean) / aStdDev;
    }

    template <typename T>
    __inline typename T::value_type StdDev(const T& a, const typename T::value_type& aMean)
    {
        return T::value_type(Sqrt(Variance(a, aMean)));
    }

    template <typename T>
    typename T::value_type Variance(const T& a, const typename T::value_type& aMean)
    {
        typedef typename T::value_type     V;
        typedef typename T::const_iterator ConstItr;

        V ret = V(0);

        if (a.size() < 2)
        {
            return 0;
        }

        for (ConstItr I = a.begin(); I != a.end(); I++)
        {
            V temp = *I - aMean;
            
            ret += (temp * temp);
        }

        return (ret / V(a.size() - 1));
    }
    #pragma endregion

}

#endif
