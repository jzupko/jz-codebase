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
#include <jz_core/Memory.h>
#include <jz_core/Prereqs.h>
#include <jz_core/Rectangle.h>
#include <jz_core/StringUtility.h>

#include <cstdio>

namespace jz
{

    static const natural gskCharacterBufferSize = 128;

    void StringUtility::Crop(string& ar, string::size_type aLeftPos, string::size_type aRightPos)
    {
        const string::size_type kSize = ar.length();
        
        ar.erase(aRightPos, kSize - aRightPos);
        ar.erase(0, aLeftPos);
    }
    
    void StringUtility::MakeLowercase(string& ar)
    {
        for (string::iterator I = ar.begin(); I != ar.end(); I++)
        {
            (*I) = tolower(*I);
        }
    }
    
    void StringUtility::MakeUppercase(string& ar)
    {
        for (string::iterator I = ar.begin(); I != ar.end(); I++)
        {
            (*I) = toupper(*I);
        }    
    }
    
    void StringUtility::TrimWhitespace(string& ar)
    {
        const int kSize = ar.length();
        int lPos = 0;
        int rPos = kSize - 1;
        
        for (; lPos < kSize; lPos++)
        {
            if (!iswspace(ar[lPos]))
            {
                break;
            }
        }
        
        for (; rPos >= 0; rPos--)
        {
            if (!iswspace(ar[rPos]))
            {
                break;
            }
        }
        
        if (lPos >= 0 && rPos >= 0 && rPos >= lPos)
        {
            if (lPos != 0 || (rPos != (kSize - 1)))
            {
                Crop(ar, lPos, rPos);    
            }
        }
    }

    wstring StringUtility::Convert(const string& s)
    {
        MemoryBuffer<wchar_t> buf(s.length() + 1);
        
#       if JZ_PLATFORM_WINDOWS
            size_t numConverted = 0;
            JZ_E_ON_FAIL(mbstowcs_s(&numConverted, buf.Get(), buf.size(), s.c_str(), s.length()) == 0, "char to wchar_t string conversion failed.");
            JZ_E_ON_FAIL(numConverted == s.length() + 1, "char to wchar_t string conversion failed.");
#       else
            JZ_E_ON_FAIL(mbstowcs(buf.Get(), s.c_str(), s.length()) == s.length() + 1, "char to wchar_t string conversion failed.");
#       endif

        return wstring(buf.Get());
    }
    
    string StringUtility::ToString(int n, bool bHex)
    {
        char buf[gskCharacterBufferSize];

        const char* pFormat = (bHex) ? "%X" : "%d";

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, pFormat, n);
#       else
            sprintf(buf, pFormat, n);
#       endif

        return string(buf);
    }

    string StringUtility::ToString(size_t n, bool bHex)
    {
        char buf[gskCharacterBufferSize];

        const char* pFormat = (bHex) ? "%X" : "%u";

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, pFormat, n);
#       else
            sprintf(buf, pFormat, n);
#       endif

        return string(buf);
    }

    string StringUtility::ToString(u64 n, bool bHex)
    {
        char buf[gskCharacterBufferSize];

        const char* pFormat = (bHex) ? "%X" : "%u";

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, pFormat, n);
#       else
            sprintf(buf, pFormat, n);
#       endif

        return string(buf);
    }
    
    string StringUtility::ToString(float v)
    {
        char buf[gskCharacterBufferSize];

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, "%g", v);
#       else
            sprintf(buf, "%g", v);
#       endif

        return string(buf);    
    }
    
    string StringUtility::ToString(const ColorRGB& c)
    {
        char buf[gskCharacterBufferSize];

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, "%#6f, %#6f, %#6f", c.R, c.G, c.B);
#       else
            sprintf(buf, "%#6f, %#6f, %#6f", c.R, c.G, c.B);
#       endif

        return string(buf);
    }
    
    string StringUtility::ToString(const ColorRGBA& c)
    {
        char buf[gskCharacterBufferSize];

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, "%#6f, %#6f, %#6f, %#6f", c.R, c.G, c.B, c.A);
#       else
            sprintf(buf, "%#6f, %#6f, %#6f, %#6f", c.R, c.G, c.B, c.A);
#       endif

        return string(buf);    
    }

    string StringUtility::ToString(const Rectangle& r)
    {
        char buf[gskCharacterBufferSize];

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, "%#6f, %#6f, %#6f, %#6f", r.BottomLeft.X, r.BottomLeft.Y, r.TopRight.X, r.TopRight.Y);
#       else
            sprintf(buf, "%#6f, %#6f, %#6f, %#6f", r.BottomLeft.X, r.BottomLeft.Y, r.TopRight.X, r.TopRight.Y);
#       endif

        return string(buf);
    }

    string StringUtility::ToString(const Vector3& v)
    {
        char buf[gskCharacterBufferSize];

#       if JZ_PLATFORM_WINDOWS        
            sprintf_s(buf, gskCharacterBufferSize, "%#6f, %#6f, %#6f", v.X, v.Y, v.Z);
#       else
            sprintf(buf, "%#6f, %#6f, %#6f", v.X, v.Y, v.Z);
#       endif

        return string(buf);   
    }

}
