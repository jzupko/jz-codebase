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
#ifndef _JZ_STRING_UTILITY_H_
#define _JZ_STRING_UTILITY_H_

#include <jz_core/Memory.h>
#include <string>

namespace jz
{

    struct ColorRGB;
    struct ColorRGBA;
    struct Rectangle;
    struct Vector3;

    struct StringUtility
    {
        static void Crop(string& ar, string::size_type aLeftPos, string::size_type aRightPos);
        static void MakeLowercase(string& ar);
        static void MakeUppercase(string& ar);
        static void TrimWhitespace(string& ar);

        static string ToString(int n, bool bHex = false);
        static string ToString(ulong n, bool bHex = false) { return ToString((size_t)n, bHex); }
        static string ToString(void_p p) { return ToString((size_t)p, true); }
        static string ToString(size_t n, bool bHex = false);
        static string ToString(u64 n, bool bHex = false);
        static string ToString(float v);
        static string ToString(const ColorRGB& c);
        static string ToString(const ColorRGBA& c);
        static string ToString(const Rectangle& r);
        static string ToString(const Vector3& v);
        
        static wstring Convert(const string& s);
    };
   
}

#endif