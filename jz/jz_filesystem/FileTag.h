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
#ifndef _JZ_FILESYSTEM_FILE_TAG_H_
#define _JZ_FILESYSTEM_FILE_TAG_H_

#include <jz_core/Memory.h>
#ifndef NDEBUG
#   include <string>
#endif

namespace jz
{

    struct FileTag
    {
        FileTag()
            : mIndex(0u), mHash(0u)
        {}   

        FileTag(const FileTag& t)
            : mIndex(t.mIndex), mHash(t.mHash)
#           ifndef NDEBUG
                , mFilename(t.mFilename)
#           endif
        {}
        
        FileTag(const char* apFilename);
        
        FileTag& operator=(const FileTag& t)
        {
            mIndex = t.mIndex;
            mHash = t.mHash;

#           ifndef NDEBUG
                mFilename = t.mFilename;
#           endif
            
            return *this;
        }
        
        u32 GetIndex() const { return mIndex; }
        u64 GetHash() const { return mHash; }
        
        bool operator<(const FileTag& b) const
        {
            return (mIndex < b.mIndex && mHash < b.mHash);
        }

        bool operator==(const FileTag& b) const
        {
            return (mIndex == b.mIndex && mHash == b.mHash);
        }

        bool operator>(const FileTag& b) const
        {
            return (mIndex > b.mIndex && mHash > b.mHash);
        }
        
        private:
            FileTag(u32 aIndex, u64 aHash)
                : mIndex(aIndex), mHash(aHash)
            {}
            
            u32 mIndex;
            u64 mHash;
            
#           ifndef NDEBUG
                string mFilename;
#           endif
    };

}

#endif

