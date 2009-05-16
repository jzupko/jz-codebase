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
#ifndef _JZ_ANGLE_H_
#define _JZ_ANGLE_H_

#include <jz_core/Math.h>

namespace jz
{

    class Radian;

    class Degree
    {
    public:
        Degree(const Degree& a)
            : mValue(a.mValue)
        {}
        Degree(Radian a);

        Degree& operator=(Radian a);

        static const Degree kZero;
        static const Degree k45;
        static const Degree k90;
        static const Degree k135;
        static const Degree k180;
        static const Degree k225;
        static const Degree k260;
        static const Degree k315;
        static const Degree k360;

#       define JZ_ANGLE_NAME Degree
#       include <jz_core/_Angle.h>
    };

    __inline Degree operator*(float a, Degree b)
    {
        return (b * a);
    }

    class Radian
    {
        public:
            Radian(const Radian& a)
                : mValue(a.mValue)
            {}
            Radian(Degree a);

            Radian& operator=(Degree a);

            static const Radian kPi;
            static const Radian kPiOver2;
            static const Radian kPiOver4;
            static const Radian k3PiOver2;
            static const Radian kTwoPi;
            static const Radian kZero;
                        
#           define JZ_ANGLE_NAME Radian
#           include <jz_core/_Angle.h>

    };

    __inline Radian operator*(float a, Radian b)
    {
        return (b * a);
    }

    __inline Degree::Degree(Radian a)
        : mValue((a.GetValue() / Constants<float>::kTwoPi) * 360.0f)
    {}

    __inline Degree& Degree::operator=(Radian a)
    {
        mValue = (a.GetValue() / Constants<float>::kTwoPi) * 360.0f;   
        return *this;
    }

    __inline Radian::Radian(Degree a)
        : mValue((a.GetValue() / 360.0f) * Constants<float>::kTwoPi)
    {}

    __inline Radian& Radian::operator=(Degree a)
    {
        mValue = (a.GetValue() / 360.0f) * Constants<float>::kTwoPi;
        return *this;
    }

    __inline float Sin(Radian a)
    {
        return std::sin(a.GetValue());
    }

    __inline float Cos(Radian a)
    {
        return std::cos(a.GetValue());
    }

    __inline float Tan(Radian a)
    {
        return std::tan(a.GetValue());
    }

    __inline float Cot(Radian a)
    {
        return 1.0f / std::tan(a.GetValue());
    }

    __inline Radian ASin(float a)
    {
        return Radian(std::asin(a));
    }

    __inline Radian ACos(float a)
    {
        return Radian(std::acos(a));
    }

    __inline Radian ATan(float a)
    {
        return Radian(std::atan(a));
    }
    
    __inline Radian ATan2(float y, float x)
    {
        return Radian(std::atan2(y, x));
    }

    const Degree Constants<Degree>::kZeroTolerance = Degree(Constants<float>::kZeroTolerance);
    const Radian Constants<Radian>::kZeroTolerance = Radian(Constants<float>::kZeroTolerance);

    __inline bool AboutZero(Degree a, float aEpsilon = Constants<float>::kZeroTolerance)
    {
        return jz::AboutZero(a.GetValue(), aEpsilon);
    }

    __inline bool AboutZero(Radian a, float aEpsilon = Constants<float>::kZeroTolerance)
    {
        return jz::AboutZero(a.GetValue(), aEpsilon);
    }

    __inline bool AboutEqual(Degree a, Degree b, float aEpsilon = Constants<float>::kZeroTolerance)
    {
        return jz::AboutEqual(a.GetValue(), b.GetValue(), aEpsilon);
    }

    __inline bool AboutEqual(Radian a, Radian b, float aEpsilon = Constants<float>::kZeroTolerance)
    {
        return jz::AboutEqual(a.GetValue(), b.GetValue(), aEpsilon);
    }

    Degree Clamp(Degree a, Degree aMin, Degree aMax);
    Radian Clamp(Radian a, Radian aMin, Radian aMax);
    Degree Lerp(Degree a, Degree b, float aWeightOfB);
    Radian Lerp(Radian a, Radian b, float aWeightOfB);

    __inline Degree Sign(Degree d)
    {
        return Degree(Sign(d.GetValue()));
    }

    __inline Radian Sign(Radian r)
    {
        return Radian(Sign(r.GetValue()));
    }

}

#define JZ_HELPER_TYPE     jz::Radian
#define JZ_HELPER_BASETYPE float
#include <jz_core/_StdNumericLimits.h>

#define JZ_HELPER_TYPE     jz::Degree
#define JZ_HELPER_BASETYPE float
#include <jz_core/_StdNumericLimits.h>

#endif
