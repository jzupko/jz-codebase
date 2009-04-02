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

#include <jz_core/StringUtility.h>
#include <jz_filesystem/FileTag.h>

#if JZ_LITTLE_ENDIAN
#   include <jz_core/Hash.h>
#   define JZ_HASH hash64
#else
#   include <hash.h>
#   define JZ_HASH hash
#endif

namespace jz
{

    namespace FileHash
    {
        enum Levels
        {
            kHashTableIndex = 0,
            kHashId = 1
        };
    }

    FileTag::FileTag(const char* apFilename)
        : mIndex(0u), mHash(0u)
    {
        string filename = apFilename;
        StringUtility::TrimWhitespace(filename);
        StringUtility::MakeLowercase(filename);
        const size_t kFilenameSize = filename.length();

        mIndex = (u32)(JZ_HASH((u8*)filename.c_str(), kFilenameSize, FileHash::kHashTableIndex) & hashmask(32));
        mHash = JZ_HASH((u8*)filename.c_str(), kFilenameSize, FileHash::kHashId);
        
#       ifndef NDEBUG
            mFilename = filename;
#       endif        
    }

}

#undef JZ_HASH
