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
#ifndef _JZ_ADDRESS_BUFFER_H_
#define _JZ_ADDRESS_BUFFER_H_

#include <jz_core/Memory.h>

namespace jz
{

    template <typename T>
    class AddressBuffer
    {
    public:
        typedef const T* const_iterator;
        typedef const T* const_pointer;
        typedef const T& const_reference;
        typedef typename MemoryBuffer<T>::difference_type difference_type;
        typedef T* iterator;
        typedef T* pointer;
        typedef T& reference;
        typedef typename MemoryBuffer<T>::size_type size_type;
        typedef T value_type;

        static const size_type kMinSize = (1 << 4);

    private:
        MemoryBuffer<T> mData;
        MemoryBuffer<size_type> mFreeList;

        size_type mDataCount;
        size_type mFreeCount;

        void _Grow()
        {
            size_type newSize = (mData.size() < (Constants<size_type>::kMax >> 1)) ? (mData.size() << 1) : Constants<size_type>::kMax;

            mData.resize(newSize);
            mFreeList.resize(newSize >> 1);
        }

    public:
        AddressBuffer(size_type aInitialStorage = kMinSize)
            : mDataCount(0u), mFreeCount(0u)
        {
            mData.resize(jz::Clamp(aInitialStorage, kMinSize, Constants<size_type>::kMax));
            mFreeList.resize(mData.size() >> 1);
        }

        size_type Add(const T& a)
        {
            if (mDataCount == mData.size())
            {
                if (mData.size() < Constants<size_type>::kMax) { _Grow(); }
                else
                {
                    throw std::exception("AddressBuffer exceeded maximum size.");
                }
            }

            size_type index = (mFreeCount > 0) ? mFreeList[--mFreeCount] : mDataCount++;
            mData[index] = a;

            return index;
        }

        void clear()
        {
            mDataCount = 0;
            mFreeCount = 0;
        }

        void Compact()
        {
            mData.resize(jz::Max(mDataCount, kMinSize));
            mFreeList.resize(jz::Max(mDataCount >> 1, kMinSize >> 1));
        }

        operator const_pointer() const { return mData.Get(); }
        operator pointer() { return mData.Get(); }

        void remove(size_type aHandle)
        {
            if (aHandle >= mDataCount) { return; }
            if (mFreeList.size() == mFreeCount)
            {
                if (mData.size() < Constants<size_type>::kMax) { _Grow(); }
                else
                {
                    throw std::exception("AddressBuffer exceeded maximum size.");
                }
            }

            mFreeList[mFreeCount++] = aHandle;
        }
    };

}

#endif
