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
#ifndef _JZ_TREE_H_
#define _JZ_TREE_H_

#include <jz_core/Auto.h>
#include <jz_core/Memory.h>
#include <jz_core/Utility.h>

namespace jz
{

    template <typename T>
    class TreeNode
    {
        public:
            typedef AutoPtr<T> pointer;
            typedef ConstInternalListIterator<T> const_iterator;
            typedef const AutoPtr<const T> const_pointer;
            typedef const T& const_reference;
            typedef ConstReverseInternalListIterator<T> const_reverse_iterator;
            typedef const T* const_weak_pointer;
            typedef InternalListIterator<T> iterator;
            typedef T& reference;
            typedef ReverseInternalListIterator<T> reverse_iterator;
            typedef size_t size_type;
            typedef T value_type;
            typedef T* weak_pointer;
            
            const_reference back() const { return *mpLastChild; }
            reference back() { return *mpLastChild; }
            
            const_iterator begin() const { return const_iterator(mpFirstChild.Get()); }
            iterator begin() { return iterator(mpFirstChild.Get()); }
                       
            const_iterator end() const { return const_iterator(0); }
            iterator end() { return iterator(0); }

            const_reference front() const { return *mpFirstChild; }
            reference front() { return *mpFirstChild; }

            weak_pointer GetParent() const { return mpParent; }
            weak_pointer GetParent() { return mpParent; }

            virtual void SetParent(weak_pointer p)
            {
                _Leave();
                _Join(p);
            }

            const_reverse_iterator rbegin() const { return const_reverse_iterator(mpLastChild); }
            reverse_iterator rbegin() { return reverse_iterator(mpLastChild); }
            
            const_reverse_iterator rend() const { return const_reverse_iterator(null); }
            reverse_iterator rend() { return reverse_iterator(null); }

        protected:
            friend void ::jz::__IncrementRefCount<T>(T*);
            friend void ::jz::__DecrementRefCount<T>(T*);

            size_t mReferenceCount;

            TreeNode()
                : mReferenceCount(0u), mpLastChild(null), mpParent(null), mpPrevSibling(null)
            {}

            void clear()
            {
                for (reverse_iterator I = rbegin(); I != rend(); I++)
                {
                    if (I->mpNextSibling.IsValid())
                    {
                        I->mpNextSibling->mpParent = null;
                        I->mpNextSibling->mpPrevSibling = null;
                    }
                    
                    I->mpNextSibling.Reset();
                }

                mpFirstChild.Reset();            
            }

            virtual ~TreeNode()
            {
                clear();
            } 
            
            pointer mpFirstChild;
            weak_pointer mpLastChild;
            pointer mpNextSibling;
            weak_pointer mpParent;
            weak_pointer mpPrevSibling;            

        private:
            TreeNode(const TreeNode&);
            TreeNode& operator=(const TreeNode&);
        
            friend const_iterator;
            friend const_reverse_iterator;
            friend iterator;
            friend reverse_iterator;
            
            const_weak_pointer GetNext() const
            {
                return mpNextSibling.Get();
            }
            
            weak_pointer GetNext()
            {
                return mpNextSibling.Get();
            }

            const_weak_pointer GetPrevious() const
            {
                return mpPrevSibling;
            }
            
            weak_pointer GetPrevious()
            {
                return mpPrevSibling;
            }
            
            void _Join(weak_pointer p)
            {
                if (p)
                {
                    mpParent = p;
                    mpPrevSibling = p->mpLastChild;

                    if (p->mpLastChild) { p->mpLastChild->mpNextSibling.Reset(static_cast<T*>(this)); }

                    p->mpLastChild = static_cast<T*>(this);

                    if (!(p->mpFirstChild.IsValid())) { p->mpFirstChild.Reset(static_cast<T*>(this)); }
                }
            }

            void _Leave()
            {
                pointer pNext(mpNextSibling);
                mpNextSibling.Reset();

                if (pNext.IsValid()) { pNext->mpPrevSibling = mpPrevSibling; }

                weak_pointer pParent = mpParent;
                mpParent = null;

                if (pParent && !pNext.IsValid()) { pParent->mpLastChild = mpPrevSibling; }
                
                if (mpPrevSibling)
                {
                    weak_pointer pPrevious = mpPrevSibling;
                    mpPrevSibling = null;
                                        
                    pPrevious->mpNextSibling = pNext;
                }
                else if (pParent)
                {
                    pParent->mpFirstChild = pNext;
                }
            }
    };
    
}

#endif
