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
#ifndef _JZ_MEMORY_H_
#define _JZ_MEMORY_H_

#include <jz_core/Prereqs.h>
#include <type_traits>

namespace jz
{

    // Low-level memory management. Note that unlike C malloc, realloc, these will 
    // throw exceptions on failed allocation.
    void Free(void_p p);
    void_p Malloc(size_t aSize);
    void_p Realloc(void_p p, size_t aSize);

    template <typename T>
    class MemoryBuffer
    {
    public:
        typedef const T* const_iterator;
        typedef const T* const_pointer;
        typedef const T& const_reference;
        typedef ptrdiff_t difference_type;
        typedef T* iterator;
        typedef T* pointer;
        typedef T& reference;
        typedef size_t size_type;
        typedef T value_type;

        MemoryBuffer()
            : mpData(null), mSize(0)
        {}

        MemoryBuffer(size_type aSize)
            : mpData((pointer)Malloc(aSize * sizeof(T))), mSize(aSize)
        {}

        MemoryBuffer(const MemoryBuffer& aBuffer)
            : mpData(null), mSize(0)
        {
            if (aBuffer.mpData)
            {
                mpData = (pointer)Malloc(aBuffer.mSize * sizeof(T));
                memcpy(mpData, aBuffer.mpData, aBuffer.mSize * sizeof(T));
                mSize  = aBuffer.mSize;                    
            }
        }

        MemoryBuffer& operator=(const MemoryBuffer& aBuffer)
        {
            if (!aBuffer.mpData) { clear(); }
            else
            {
                mpData = (mpData) ? (pointer)Realloc(mpData, aBuffer.mSize * sizeof(T)) : (pointer)Malloc(aBuffer.mSize * sizeof(T));
                memcpy(mpData, aBuffer.mpData, aBuffer.mSize * sizeof(T));
                mSize  = aBuffer.mSize;                    
            }

            return *this;
        }

        ~MemoryBuffer()
        {
            clear();
        }

        void CopyFrom(voidc_p apData, size_type aSize)
        {
            if (!mpData || mSize < aSize) { throw std::out_of_range(__FUNCTION__); }

            memcpy(mpData, apData, aSize * sizeof(T));
        }

        void CopyTo(void_p apData, size_type aSize) const
        {
            if (!apData || mSize < aSize) { throw std::out_of_range(__FUNCTION__); }

            memcpy(apData, mpData, aSize * sizeof(T));
        }

        void CopyFrom(voidc_p apData) { CopyFrom(apData, mSize); }
        void CopyTo(void_p apData) const { CopyTo(apData, mSize); }

        const_iterator begin() const { return mpData; }
        iterator begin() { return mpData; }
        const_iterator end() const { return mpData + mSize; }
        iterator end() { return mpData + mSize; }

        const_pointer Get() const { return mpData; }
        pointer Get() { return mpData; }

        operator const_pointer() const { return mpData; }
        operator pointer() { return mpData; }

        void clear()
        {
            if (mpData)
            {
                Free(mpData);
                mpData = null;
            }

            mSize = 0;
        }

        size_type size() const { return mSize; }
        size_type GetSizeInBytes() const { return (mSize * sizeof(T)); }

        void Initialize() { memset(mpData, 0, mSize * sizeof(T)); }

        void resize(size_type aSize)
        {
            if (aSize == 0) { clear(); }
            else if (aSize != mSize)
            {
                mpData = (mpData) ? (pointer)Realloc(mpData, aSize * sizeof(T)) : (pointer)Malloc(aSize * sizeof(T));
                mSize  = aSize;
            }
        }

    private:
        pointer mpData;
        size_type mSize;
    };

    typedef MemoryBuffer<u8> ByteBuffer;

}

#endif
