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
#ifndef _JZ_RESET_POOL_H_
#define _JZ_RESET_POOL_H_

#include <jz_core/Memory.h>
#include <new>

namespace jz
{

    template <typename T>
    class ResetPool
    {
    public:
        static const int kInitialPoolSize = 4096;
        static const int kGrowthMultiplier = 2;

        static T* Grab()
        {
            static Helper sPool;

            if (msCount == msSize)
            {
                JZ_ASSERT(msSize > 0u);
                JZ_ASSERT(mspOldData == null);

                msSize *= kGrowthMultiplier;
                mspOldData = mspData;
                mspData = (T*)Malloc(msSize * sizeof(T));

                for (size_t i = 0u; i < msSize; i++)
                {
                    new (mspData + i) T();
                }
            }

            size_t index = msCount++;
            T* p = (mspData + index);

            return p;
        }

        static void Reset()
        {
            msCount = 0u;
            if (mspOldData)
            {
                Free(mspOldData);
                mspOldData = null;
            }
        }

    private:
        ResetPool();
        ResetPool(const ResetPool&);
        ResetPool& operator=(const ResetPool&);

        static size_t msCount;
        static T* mspData;
        static T* mspOldData;
        static size_t msSize;

        struct Helper
        {
            Helper()
            {
                ResetPool<T>::msSize = kInitialPoolSize;
                ResetPool<T>::msCount = 0u;
                ResetPool<T>::mspData = (T*)Malloc(ResetPool<T>::msSize * sizeof(T));
                ResetPool<T>::mspOldData = null;

                for (size_t i = 0; i < ResetPool<T>::msSize; i++)
                {
                    new (ResetPool<T>::mspData + i) T();
                }
            }

            ~Helper()
            {
                for (size_t i =0; i < ResetPool<T>::msSize; i++)
                {
                    ResetPool<T>::mspData[i].~T();
                }

                Free(ResetPool<T>::mspData);
                ResetPool<T>::mspOldData = null;
                ResetPool<T>::mspData = null;
                ResetPool<T>::msCount = 0u;
                ResetPool<T>::msSize = 0u;
            }
        };
    };
}

#endif
