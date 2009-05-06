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
#ifndef _JZ_PREREQS_H_
#define _JZ_PREREQS_H_

#include <limits>
#include <type_traits>

// uses the following standards:
// - row-major storage for matrices
// - vectors-as-rows (left-to-right matrix multiplication)
// - right-handed coordinate system
// - x to the right, y up, -z into the screen
// - clockwise winding
// - screen and texture origin from the upper left.
namespace jz
{

    template <bool> struct __StaticAssert__;
    template <>     struct __StaticAssert__<true> { enum { v = 1 }; };
    template <int>  struct __StaticAssertHelper__ {};

}

#define JZ_CAT_IMPLC(a) a
#define JZ_CAT_IMPLB(a,b) JZ_CAT_IMPLC(a##b)
#define JZ_CAT_IMPL(a) JZ_CAT_IMPLB##a
#define JZ_CAT(a,b) JZ_CAT_IMPL((a,b))

#define JZ_STATIC_ASSERT( a )                       \
    typedef ::jz::__StaticAssertHelper__<            \
        sizeof(::jz::__StaticAssert__<(bool)( a )>)> \
        __StaticAssertTypedef__

namespace jz
{

    #pragma region EnableIf
    template <bool B, typename T = void> struct EnableIf { typedef T type; };
    template <typename T> struct EnableIf<false, T> {};
    #pragma endregion

	using namespace std;

//  basic platform defines.
#   if WIN32 || _WIN32
#       define JZ_PLATFORM_MACOS   0
#       define JZ_PLATFORM_WINDOWS 1
#       define JZ_LITTLE_ENDIAN    1
#       define JZ_BIG_ENDIAN       0

#       define JZ_PLATFORM_32      1
#       define JZ_PLATFORM_64      0
#   else
#       error "Platform not yet supported."
#   endif

// define JZ_STATICLIB if you want to link as a static library.
#   ifndef JZ_STATICLIB
#       ifdef JZ_BUILD
#           define JZ_EXPORT __declspec(dllexport)
#       else
#           define JZ_EXPORT __declspec(dllimport)
#       endif
#   else
#       define JZ_EXPORT
#   endif

#if !NDEBUG
#   if JZ_PLATFORM_WINDOWS
        inline void jz_win32_assert(bool b)
        {
            if (!b)
            {
                _asm int 3;
            }
        }
#       define JZ_ASSERT(b) jz::jz_win32_assert(b);
#   else
#       define JZ_ASSERT(b) assert(b);
#   endif
#else
#   define JZ_ASSERT(b)
#endif

    typedef unsigned char  uchar;
    typedef unsigned int   uint;
    typedef unsigned short ushort;
    typedef unsigned long  ulong;

#   if JZ_PLATFORM_WINDOWS && JZ_PLATFORM_32
        typedef long  natural;
        typedef ulong unatural;
#   endif

    typedef void* void_p;
    typedef void const* voidc_p;
    typedef char* char_p;

    typedef unsigned char u8;
    JZ_STATIC_ASSERT(std::numeric_limits<u8>::digits == 8);
    JZ_STATIC_ASSERT(!std::numeric_limits<u8>::is_signed);
    JZ_STATIC_ASSERT(std::numeric_limits<u8>::is_integer);

    typedef u8* u8_p;
    typedef u8 const* u8c_p;

    typedef char s8;
    JZ_STATIC_ASSERT(std::numeric_limits<s8>::digits == 7);
    JZ_STATIC_ASSERT(std::numeric_limits<s8>::is_signed);
    JZ_STATIC_ASSERT(std::numeric_limits<s8>::is_integer);

    typedef unsigned short u16;
    JZ_STATIC_ASSERT(std::numeric_limits<u16>::digits == 16);
    JZ_STATIC_ASSERT(!std::numeric_limits<u16>::is_signed);
    JZ_STATIC_ASSERT(std::numeric_limits<u16>::is_integer);

    typedef short s16;
    JZ_STATIC_ASSERT(std::numeric_limits<s16>::digits == 15);
    JZ_STATIC_ASSERT(std::numeric_limits<s16>::is_signed);
    JZ_STATIC_ASSERT(std::numeric_limits<s16>::is_integer);

    typedef unsigned int u32;
    JZ_STATIC_ASSERT(std::numeric_limits<u32>::digits == 32);
    JZ_STATIC_ASSERT(!std::numeric_limits<u32>::is_signed);
    JZ_STATIC_ASSERT(std::numeric_limits<u32>::is_integer);

    typedef int s32;
    JZ_STATIC_ASSERT(std::numeric_limits<s32>::digits == 31);
    JZ_STATIC_ASSERT(std::numeric_limits<s32>::is_signed);
    JZ_STATIC_ASSERT(std::numeric_limits<s32>::is_integer);

#   if JZ_PLATFORM_32
        typedef float f32;

        typedef unsigned __int64 u64;
        JZ_STATIC_ASSERT(std::numeric_limits<u64>::digits == 64);
        JZ_STATIC_ASSERT(!std::numeric_limits<u64>::is_signed);
        JZ_STATIC_ASSERT(std::numeric_limits<u64>::is_integer);

        typedef __int64 s64;
        JZ_STATIC_ASSERT(std::numeric_limits<s64>::digits == 63);
        JZ_STATIC_ASSERT(std::numeric_limits<s64>::is_signed);
        JZ_STATIC_ASSERT(std::numeric_limits<s64>::is_integer);
#   endif

    static const int null = 0;
    static const char string_terminator = 0;
    
    struct Handle
    {
        Handle()
            : P(null)
        {}

        Handle(uint i)
            : V(i)
        {}
        
        template <typename T>
        Handle(T* p)
            : P(p)
        {}

        template <typename T>
        Handle(T const* p)
            : P(const_cast<T*>(p))
        {}

        bool operator==(const Handle& h) const
        {
            return (P == h.P);
        }

        template <typename T>
        T Cast(typename EnableIf<tr1::is_pointer<T>::value && !tr1::is_const<typename tr1::remove_pointer<T>::type>::value>::type* pDummy = null) const
        {
            return static_cast<T>(P);
        }

        template <typename T>
        T Cast(typename EnableIf<tr1::is_pointer<T>::value && tr1::is_const<typename tr1::remove_pointer<T>::type>::value>::type* pDummy = null) const
        {
            return static_cast<T const>(const_cast<voidc_p>(P));
        }

        uint CastUInt() const
        {
            return (V);
        }

        voidc_p CastVoidCP() const
        {
            return (P);
        }

        void Reset()
        {
            P = null;
        }

        template <typename T>
        T* Cast(typename EnableIf<!tr1::is_pointer<T>::value && !tr1::is_const<T>::value>::type* pDummy = null) const
        {
            return static_cast<T*>(P);
        }

        template <typename T>
        T* Cast(typename EnableIf<!tr1::is_pointer<T>::value && tr1::is_const<T>::value>::type* pDummy = null) const
        {
            return static_cast<T const*>(const_cast<voidc_p>(P));
        }

        operator bool() const { return (P != null); }

        operator void_p() const { return P; }
        operator voidc_p() const { return const_cast<voidc_p>(P); }

    private:
        JZ_STATIC_ASSERT(sizeof(uint) == sizeof(void_p));

        union
        {
            void_p P;
            uint V;
        };
    };

#   define kUnreachableException "expected unreachable code reached."

    namespace Axis
    {
        enum Id
        {
            kX = 0,
            kY = 1,
            kZ = 2,
            kW = 3
        };
    }

    template <typename T>
    struct Constants
    {
        static const T kE;
        static const T kInfinity;
        static const T kJacobiTolerance;
        static const T kLooseTolerance;
        static const T kNegativeLooseTolerance;
        static const T kMax;
        static const T kMin;
        static const T kMinLuminance;
        static const T kNaN;
        static const T kPi;
        static const T kPiOver2;
        static const T kPiOver4;
        static const T kSlerpThreshold;
        static const T k3PiOver2;
        static const T kTwoPi;
        static const T kZeroTolerance;
    };

    const float  Constants<float>::kE                = 2.71828182845904523536f;
    const double Constants<double>::kE               = 2.71828182845904523536;
    const float  Constants<float>::kInfinity         = std::numeric_limits<float>::infinity();
    const double Constants<double>::kInfinity        = std::numeric_limits<double>::infinity();
    const float  Constants<float>::kJacobiTolerance  = 1e-8f;
    const double Constants<double>::kJacobiTolerance = 1e-10;
    const float  Constants<float>::kLooseTolerance   = 1e-3f;
    const double Constants<double>::kLooseTolerance  = 1e-5;
    const float  Constants<float>::kNegativeLooseTolerance = -Constants<float>::kLooseTolerance; 
    const double Constants<double>::kNegativeLooseTolerance = -Constants<double>::kLooseTolerance; 
    const float  Constants<float>::kMax              = FLT_MAX;
    const double Constants<double>::kMax             = DBL_MAX;
    // Note: numeric_limits<float>::min() actually returns the smallest positive number that can
    //       be represented, not the smallest signed number.
    const float  Constants<float>::kMin              = -FLT_MAX; 
    const double Constants<double>::kMin             = -DBL_MAX;
    const float  Constants<float>::kMinLuminance     = 1e-6f;
    const double Constants<double>::kMinLuminance    = 1e-6;
    const float  Constants<float>::kNaN              = std::numeric_limits<float>::signaling_NaN();
    const double Constants<double>::kNaN             = std::numeric_limits<double>::signaling_NaN();
    const float  Constants<float>::kPi               = 3.14159265359f;
    const double Constants<double>::kPi              = 3.14159265359;
    const float  Constants<float>::kPiOver2          = 1.57079632679f;
    const double Constants<double>::kPiOver2         = 1.57079632679;
    const float  Constants<float>::kPiOver4          = 0.7853981634f;
    const double Constants<double>::kPiOver4         = 0.7853981634;
    const float  Constants<float>::kSlerpThreshold   = 5e-4f;
    const double Constants<double>::kSlerpThreshold  = 5e-4;
    const float  Constants<float>::k3PiOver2         = 4.71238898f;
    const double Constants<double>::k3PiOver2        = 4.71238898;
    const float  Constants<float>::kTwoPi            = 6.28318530718f;
    const double Constants<double>::kTwoPi           = 6.28318530718;
    const float  Constants<float>::kZeroTolerance    = 1e-06f;
    const double Constants<double>::kZeroTolerance   = 1e-08;

    const int Constants<int>::kMin = INT_MIN;
    const int Constants<int>::kMax = INT_MAX;

    const u8 Constants<u8>::kMin = 0u;
    const u8 Constants<u8>::kMax = 0xFF;

    const u16 Constants<u16>::kMin = 0u;
    const u16 Constants<u16>::kMax = 0xFFFF;

    const size_t Constants<size_t>::kMin = 0u;
    const size_t Constants<size_t>::kMax = UINT_MAX;

    template <typename T>
    inline T Clamp(T aValue, T aMin, T aMax)
    {
        JZ_ASSERT(!IsNan(aValue));

        if (aValue < aMin) { return aMin; }
        else if (aValue > aMax) { return aMax; }
        else { return aValue; }
    }

    template <typename T>
    inline bool IsNan(T a)
    {
        return (a != a);
    }

    template <typename T>
    inline T Max(T a, T b)
    {
        return ((a > b) ? a : b);
    }

    template <typename T>
    inline T Max(T a, T b, T c)
    {
        return Max(a, Max(b, c));
    }

    template <typename T>
    inline T Max(T a, T b, T c, T d)
    {
        return Max(a, b, Max(c, d));
    }

    template <typename T>
    inline T Min(T a, T b)
    {
        return ((a < b) ? a : b);
    }

    template <typename T>
    inline T Min(T a, T b, T c)
    {
        return Min(a, Min(b, c));
    }

    template <typename T>
    inline T Min(T a, T b, T c, T d)
    {
        return Min(a, b, Min(c, d));
    }

    template <typename T>
    inline void Swap(T& a, T& b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }

#   define JZ_E_ON_FAIL(a, msg) if (!(a)) { throw std::exception(__FUNCTION__ ": " msg); }

    template <typename T> struct _stride_helper { T a; char b; };
    
    #define JZ_STRIDE_OF(T) \
        ((sizeof(::jz::_stride_helper<T>) > sizeof(T)) ?  \
        sizeof(::jz::_stride_helper<T>) - sizeof(T) : sizeof(T))

    template <typename T>
    void SafeDelete(T*& aP)
    {
        T* p = aP;
        aP = null;

        if (p)
        {
            delete p;
        }
    }

    template <typename T>
    uint SafeAcquire(Handle& ap)
    {
        T* p = ap.Cast<T>();

        if (p)
        {
            return p->AddRef();
        }

        return 0u;
    }

    template <typename T>
    ulong SafeRelease(Handle& ap)
    {
        T* p = ap.Cast<T>();
        ap = Handle();

        if (p)
        {
            ulong ret = p->Release();
            return ret;
        }

        return 0u;
    }

    template <typename T>
    uint SafeAcquire(T* ap)
    {
        if (ap)
        {
            return ap->AddRef();
        }

        return 0u;
    }

    template <typename T>
    void SafeRelease(T*& ap)
    {
        T* p = ap;
        ap = null;

        if (p)
        {
            p->Release();
        }
    }

    template <typename T, typename ITR>
    void append(T& c, ITR begin, ITR end)
    {
        for (ITR I = begin; I != end; I++)
        {
            c.push_back(*I);
        }
    }

    template <typename T> void __IncrementRefCount(T* p);
    template <typename T> size_t __GetRefCount(T* p);
    template <typename T> void __DecrementRefCount(T* p);
    
}

#endif
