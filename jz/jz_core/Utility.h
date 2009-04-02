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
#ifndef _JZ_UTILITY_H_
#define _JZ_UTILITY_H_

#include <jz_core/Prereqs.h>
#include <functional>

namespace jz
{

    class Scoped sealed
    {
    public:
        template <typename I, typename D>
        Scoped(const I& i, const D& d)
            : mI(i), mD(d)
        {
            mI();
        }

        ~Scoped()
        {
            mD();
        }

    private:
        tr1::function<void()> mI;
        tr1::function<void()> mD;
    };

    template <typename T> 
    class Singleton
    {
        public:
            static T& GetSingleton()
            {
                JZ_ASSERT(mspSingleton != null);
                return *mspSingleton;
            }
            
            static bool GetSingletonExists()
            {
                return (mspSingleton != null);
            }

        protected:
            Singleton()
            {
                JZ_ASSERT(mspSingleton == null);
                mspSingleton = static_cast<T*>(this);
            }

            ~Singleton()
            {
                JZ_ASSERT(mspSingleton != null);
                mspSingleton = null;
            }

        private:
            static T* mspSingleton;
    };

    template <typename T>
    class InternalListIterator
    {
        public:
            typedef T* pointer;
            typedef T& reference;
            
            InternalListIterator()
                : mpPtr(null)
            {}
            
            InternalListIterator(pointer apPtr)
                : mpPtr(apPtr)
            {}
            
            InternalListIterator(const InternalListIterator& i)
                : mpPtr(i.mpPtr)
            {}
            
            InternalListIterator& operator=(const InternalListIterator& i)
            {
                mpPtr = i.mpPtr;
            }
            
            bool operator==(const InternalListIterator& i) const
            {
                return mpPtr == i.mpPtr;
            }
            
            bool operator!=(const InternalListIterator& i) const
            {
                return mpPtr != i.mpPtr;
            }

            pointer operator->() const
            {
                return mpPtr;
            }

            reference operator*() const
            {
                return *mpPtr;
            }
            
            pointer Get() const
            {
                return mpPtr;
            }

            InternalListIterator& operator++()
            {
                mpPtr = mpPtr->GetNext();

                return *this;
            }

            InternalListIterator operator++(int)
            {
                InternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetNext();

                return ret;
            }

            InternalListIterator& operator--()
            {
                mpPtr = mpPtr->GetPrevious();

                return *this;
            }

            InternalListIterator operator--(int)
            {
                InternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetPrevious();

                return ret;
            }

        private:
            pointer mpPtr;
    };
    
    template <typename T>
    class ConstInternalListIterator
    {
        public:
            typedef const T* const_pointer;
            typedef const T& Constreference;
                    
            ConstInternalListIterator()
                : mpPtr(null)
            {}
            
            ConstInternalListIterator(const_pointer apPtr)
                : mpPtr(apPtr)
            {}
            
            ConstInternalListIterator(const ConstInternalListIterator& i)
                : mpPtr(i.mpPtr)
            {}
            
            ConstInternalListIterator& operator=(const ConstInternalListIterator& i)
            {
                mpPtr = i.mpPtr;
            }
            
            bool operator==(const ConstInternalListIterator& i) const
            {
                return mpPtr == i.mpPtr;
            }
            
            bool operator!=(const ConstInternalListIterator& i) const
            {
                return mpPtr != i.mpPtr;
            }

            const_pointer operator->() const
            {
                return mpPtr;
            }

            Constreference operator*() const
            {
                return *mpPtr;
            }

            const_pointer Get() const
            {
                return mpPtr;
            }

            ConstInternalListIterator& operator++()
            {
                mpPtr = mpPtr->GetNext();
            }

            ConstInternalListIterator operator++(int)
            {
                ConstInternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetNext();

                return ret;
            }

            ConstInternalListIterator& operator--()
            {
                mpPtr = mpPtr->GetPrevious();
            }

            ConstInternalListIterator operator--(int)
            {
                ConstInternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetPrevious();

                return ret;
            }

        private:
            const_pointer mpPtr;
    };


    template <typename T>
    class ReverseInternalListIterator
    {
        public:
            typedef T* pointer;
            typedef T& reference;
                    
            ReverseInternalListIterator()
                : mpPtr(null)
            {}
            
            ReverseInternalListIterator(pointer apPtr)
                : mpPtr(apPtr)
            {}
            
            ReverseInternalListIterator(const ReverseInternalListIterator& i)
                : mpPtr(i.mpPtr)
            {}
            
            ReverseInternalListIterator& operator=(const ReverseInternalListIterator& i)
            {
                mpPtr = i.mpPtr;
            }
            
            bool operator==(const ReverseInternalListIterator& i) const
            {
                return mpPtr == i.mpPtr;
            }
            
            bool operator!=(const ReverseInternalListIterator& i) const
            {
                return mpPtr != i.mpPtr;
            }

            pointer operator->() const
            {
                return mpPtr;
            }

            reference operator*() const
            {
                return *mpPtr;
            }

            pointer Get() const
            {
                return mpPtr;
            }

            ReverseInternalListIterator& operator++()
            {
                mpPtr = mpPtr->GetPrevious();

                return *this;
            }

            ReverseInternalListIterator operator++(int)
            {
                ReverseInternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetPrevious();

                return ret;
            }

            ReverseInternalListIterator& operator--()
            {
                mpPtr = mpPtr->GetNext();

                return *this;
            }

            ReverseInternalListIterator operator--(int)
            {
                ReverseInternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetNext();

                return ret;
            }

        private:
            pointer mpPtr;
    };


    template <typename T>
    class ConstReverseInternalListIterator
    {
        public:
            typedef const T* const_pointer;
            typedef const T& Constreference;
                    
            ConstReverseInternalListIterator()
                : mpPtr(null)
            {}
            
            ConstReverseInternalListIterator(const_pointer apPtr)
                : mpPtr(apPtr)
            {}
            
            ConstReverseInternalListIterator(const ConstReverseInternalListIterator& i)
                : mpPtr(i.mpPtr)
            {}
            
            ConstReverseInternalListIterator& operator=(const ConstReverseInternalListIterator& i)
            {
                mpPtr = i.mpPtr;
            }
            
            bool operator==(const ConstReverseInternalListIterator& i) const
            {
                return mpPtr == i.mpPtr;
            }
            
            bool operator!=(const ConstReverseInternalListIterator& i) const
            {
                return mpPtr != i.mpPtr;
            }

            const_pointer operator->() const
            {
                return mpPtr;
            }

            Constreference operator*() const
            {
                return *mpPtr;
            }

            const_pointer Get() const
            {
                return mpPtr;
            }

            ConstReverseInternalListIterator& operator++()
            {
                mpPtr = mpPtr->GetPrevious();
            }

            ConstReverseInternalListIterator operator++(int)
            {
                ConstReverseInternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetPrevious();

                return ret;
            }

            ConstReverseInternalListIterator& operator--()
            {
                mpPtr = mpPtr->GetNext();
            }

            ConstReverseInternalListIterator operator--(int)
            {
                ConstReverseInternalListIterator ret(mpPtr);

                mpPtr = mpPtr->GetNext();

                return ret;
            }

        private:
            const_pointer mpPtr;
    };

}

#endif
