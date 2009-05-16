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
#ifndef _JZ_AUTO_H_
#define _JZ_AUTO_H_

#include <jz_core/Prereqs.h>

namespace jz
{

    template <typename T>
    class AutoPtr sealed
    {
    public:
        AutoPtr()
            : mp(null)
        {}

        AutoPtr(T* p, bool abIncrementRefCount = true)
            : mp(p)
        {
            if (mp && abIncrementRefCount) { __IncrementRefCount(mp); }
        }

        AutoPtr(const AutoPtr& b)
            : mp(b.mp)
        {
            if (mp) { __IncrementRefCount(mp); }
        }

        template <typename U>
        AutoPtr(const AutoPtr<U>& b)
            : mp(dynamic_cast<T*>(b.Get()))
        {
            if (mp) { __IncrementRefCount(mp); }
        }

        ~AutoPtr()
        {
            if (mp) { __DecrementRefCount(mp); }
        }

        AutoPtr& operator=(const AutoPtr& b)
        {
            Reset(b.Get());

            return *this;
        }

        template <typename U>
        AutoPtr& operator=(const AutoPtr<U>& b)
        {
            Reset(dynamic_cast<T*>(b.Get()));

            return *this;
        }

        void Reset(T* ap = null)
        {
            T* p = mp;
            mp = null;

            if (p) { __DecrementRefCount(p); }
            p = ap;
            if (p) { __IncrementRefCount(p); }

            mp = p;
        }

        T* Get() const 
        {
            return mp;
        }

        size_t GetReferenceCount() const
        {
            if (mp) { return __GetRefCount(mp); }
            else { return 0; }
        }

        bool IsUnique() const
        {
            if (mp) { return (__GetRefCount(mp) == 1u); }
            else { return false; }
        }

        bool IsValid() const
        {
            return (mp != null);
        }

        T& operator*() const
        {
            JZ_ASSERT(mp != null);
            return (*mp);
        }

        T* operator->() const
        {
            JZ_ASSERT(mp != null);
            return (mp);
        }

    private:
        T* mp;
    };

    template <typename T, typename U>
    __inline bool operator==(const AutoPtr<T>& a, const AutoPtr<U>& b)
    {
        return (a.Get() == b.Get());
    }

    template <typename T, typename U>
    __inline bool operator==(const AutoPtr<T>& a, U* b)
    {
        return (a.Get() == b);
    }

    template <typename T, typename U>
    __inline bool operator==(U* a, const AutoPtr<T>& b)
    {
        return (a == b.Get());
    }

    template <typename T, typename U>
    __inline bool operator!=(const AutoPtr<T>& a, const AutoPtr<U>& b)
    {
        return (a.Get() != b.Get());
    }

    template <typename T>
    void __IncrementRefCount(T* p)
    {
        p->mReferenceCount++;
    }

    template <typename T>
    size_t __GetRefCount(T* p)
    {
        return (p->mReferenceCount);
    }

    template <typename T>
    void __DecrementRefCount(T* p)
    {
        JZ_ASSERT(p->mReferenceCount > 0u);
        p->mReferenceCount--;

        if (p->mReferenceCount == 0u) { delete p; }
    }
}

#endif
