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
#ifndef _JZ_GRAPHICS_RENDER_NODE_H_
#define _JZ_GRAPHICS_RENDER_NODE_H_

#include <jz_core/Prereqs.h>

namespace jz
{
    namespace graphics
    {

        class RenderNode;
        typedef void (*RenderNodeDelegate)(RenderNode* apNode, voidc_p apInstance);

        class RenderNode sealed
        {   
        public:
            RenderNode()
                : mpHead(null),
                mpNext(null),
                mDelegate(null),
                mpInstance(null),
                mSortOrder(Constants<float>::kMax)
            {}

            ~RenderNode()
            {}

            RenderNode* Adopt(RenderNodeDelegate aDelegate, voidc_p apInstance);
            RenderNode* AdoptAndUpdateSort(RenderNodeDelegate aDelegate, voidc_p apInstance, float aSortOrder);
            RenderNode* AdoptFront(RenderNodeDelegate aDelegate, voidc_p apInstance);
            RenderNode* AdoptSorted(RenderNodeDelegate aDelegate, voidc_p apInstance, float aSortOrder);

            RenderNode* GetHead() const { return mpHead; }
            RenderNode* GetNext() const { return mpNext; }
            voidc_p GetInstance() const { return mpInstance; }

            void Render()
            {
                mDelegate(this, mpInstance);
            }
            
            void RenderChildren()
            {
                for (RenderNode* e = mpHead; e != null; e = e->mpNext)
                {
                    e->Render();
                }
            }

            static void ResetGlobal()
            {
                RenderPool::Reset();
            }

            void Reset()
            {
                _Reset();
            }

        private:
            void _Reset();
            void _ReinsertSorted(RenderNode* pNode);

            RenderNode(const RenderNode&);
            RenderNode& operator=(const RenderNode&);

            RenderNode* mpHead;
            RenderNode* mpNext;
            RenderNodeDelegate mDelegate;
            voidc_p mpInstance;
            float mSortOrder;

            class RenderPool
            {
            public:
                static RenderNode* Grab(RenderNodeDelegate aDelegate, voidc_p apInstance);
                static RenderNode* Grab(RenderNodeDelegate aDelegate, voidc_p apInstance, float aSortOrder);
                static void Reset();

            private:
                RenderPool();
                RenderPool(const RenderPool&);
                RenderPool& operator=(const RenderPool&);
            };
        };   

        typedef RenderNode* (RenderNode::*AdoptOp)(RenderNodeDelegate aDelegate, voidc_p apInstance, float aSortOrder);

    }
}

#endif
