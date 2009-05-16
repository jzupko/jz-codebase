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

#include <jz_core/ResetPool.h>
#include <jz_graphics/RenderNode.h>

namespace jz
{
    graphics::RenderNode* ResetPool<graphics::RenderNode>::mspData = null;
    graphics::RenderNode* ResetPool<graphics::RenderNode>::mspOldData = null;
    size_t ResetPool<graphics::RenderNode>::msCount = 0u;
    size_t ResetPool<graphics::RenderNode>::msSize = 0u;
    namespace graphics
    {

        RenderNode* RenderNode::Adopt(RenderNodeDelegate aDelegate, voidc_p apInstance)
        {
            for (RenderNode* e = mpHead; e != null; e = e->mpNext)
            {
                if (e->mpInstance == apInstance) { return e; }
            }

            return AdoptFront(aDelegate, apInstance);
        }

        RenderNode* RenderNode::AdoptAndUpdateSort(RenderNodeDelegate aDelegate, voidc_p apInstance, float aSortOrder)
        {
            RenderNode* prev = null;

            for (RenderNode* e = mpHead; e != null; e = e->mpNext)
            {
                if (e->mpInstance == apInstance)
                {
                    e->mSortOrder = Min(e->mSortOrder, aSortOrder);
                    if (prev) { prev->mpNext = e->mpNext; }
                    else { mpHead = e->mpNext; }

                    _ReinsertSorted(e);

                    return e;
                }

                prev = e;
            }

            return AdoptSorted(aDelegate, apInstance, aSortOrder);
        }

        RenderNode* RenderNode::AdoptFront(RenderNodeDelegate aDelegate, voidc_p apInstance)
        {
            RenderNode* ret = RenderPool::Grab(aDelegate, apInstance);
            ret->mpNext = mpHead;
            mpHead = ret;

            return ret;
        }

        RenderNode* RenderNode::AdoptSorted(RenderNodeDelegate aDelegate, voidc_p apInstance, float aSortOrder)
        {
            if (!mpHead || mpHead->mSortOrder > aSortOrder)
            {
                RenderNode* ret = AdoptFront(aDelegate, apInstance);
                ret->mSortOrder = aSortOrder;

                return ret;
            }
            else if (mpHead->mSortOrder == aSortOrder && mpHead->mpInstance == apInstance)
            {
                return mpHead;
            }
            else
            {
                RenderNode* prev = mpHead;
                RenderNode* e = mpHead->mpNext;
                for (; e != null; e = e->mpNext)
                {
                    if (e->mSortOrder == aSortOrder && e->mpInstance == apInstance) { return e; }
                    else if (e->mSortOrder > aSortOrder) { break; }

                    prev = e;
                }

                RenderNode* ret = RenderPool::Grab(aDelegate, apInstance, aSortOrder);
                prev->mpNext = ret;
                ret->mpNext = e;

                return ret;
            }
        }

        void RenderNode::_Reset()
        {
            mpHead = null;
            mpNext = null;
            mSortOrder = Constants<float>::kMax;
        }

        void RenderNode::_ReinsertSorted(RenderNode* pNode)
        {
            if (!mpHead || mpHead->mSortOrder >= pNode->mSortOrder)
            {
                pNode->mpNext = mpHead;
                mpHead = pNode;
            }
            else
            {
                RenderNode* prev = mpHead;
                RenderNode* e = mpHead->mpNext;

                if (pNode->mpNext && pNode->mSortOrder < pNode->mSortOrder)
                {
                    prev = pNode->mpNext;
                    e = prev->mpNext;
                }

                for (; e != null; e = e->mpNext)
                {
                    if (e->mSortOrder >= pNode->mSortOrder) { break; }
                    prev = e;
                }

                prev->mpNext = pNode;
                pNode->mpNext = e;
            }
        }

        RenderNode* RenderNode::RenderPool::Grab(RenderNodeDelegate aDelegate, voidc_p apInstance)
        {
            RenderNode* node = ResetPool<RenderNode>::Grab();
            node->mDelegate = aDelegate;
            node->mpInstance = apInstance;
            node->_Reset();

            return node;
        }

        RenderNode* RenderNode::RenderPool::Grab(RenderNodeDelegate aDelegate, voidc_p apInstance, float aSortOrder)
        {
            RenderNode* node = Grab(aDelegate, apInstance);
            node->mSortOrder = aSortOrder;

            return node;
        }

        void RenderNode::RenderPool::Reset()
        {
            ResetPool<RenderNode>::Reset();
        }

    }
}
