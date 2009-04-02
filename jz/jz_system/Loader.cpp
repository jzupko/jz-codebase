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

#include <jz_system/Loader.h>
#include <jz_system/Time.h>

namespace jz
{
    system::Loader* system::Loader::mspSingleton = null;
    namespace system
    {

        Loader::Loader()
            : mMilliseconds(4u),
              mpMainTail(null), mpLoaderTail(null),
              mbDone(false), mThread(Loader::_LoaderFunction)
        {}

        Loader::~Loader()
        {
            mbDone = true;
            while (!mThread.bDone()) ;
        }

        void Loader::_Remove(ILoaderEntryPtr& pHead, ILoaderEntry*& pTail, ILoaderEntry* p)
        {
            JZ_ASSERT(p != null);
            JZ_ASSERT(pHead.Get() == p && !p->mpPrev);

            ILoaderEntryPtr pNext = (p->mpNext);
            p->mpNext.Reset();
         
            if (pNext.IsValid()) { pNext->mpPrev = null; }
            else { pTail = null; }

            pHead = pNext;
        }

        void Loader::_Insert(ILoaderEntryPtr& pHead, ILoaderEntry*& pTail, ILoaderEntry* p)
        {
            JZ_ASSERT(p != null);
            JZ_ASSERT(!p->mpPrev && !p->mpNext.IsValid());

            p->mpPrev = pTail;
            if (pTail) { pTail->mpNext.Reset(p); }
            pTail = p;

            if (!pHead.IsValid()) { pHead = p; }
        }

        void Loader::Add(ILoaderEntry* p)
        {
            if (p->GetNextAction() == ILoaderEntry::kLoaderThread)
            {
                Lock lock(mMutex);
                _Insert(mpLoaderHead, mpLoaderTail, p);
            }
            else if (p->GetNextAction() == ILoaderEntry::kMainThread)
            {
                Lock lock(mMutex);
                _Insert(mpMainHead, mpMainTail, p);
            }
        }

        void Loader::Tick()
        {
            if (mThread.bCurrent()) { return; }

            unatural startTick = system::Time::GetSingleton().GetAbsoluteMilliseconds();
            unatural currentTick = startTick;

            while ((currentTick - startTick) < mMilliseconds)
            {
                ILoaderEntryPtr p;
                {
                    Lock lock(mMutex);
                    p = mpMainHead;
                }

                if (!p.IsValid()) { break; }

                ILoaderEntry::NextAction next = p->MainThreadAction();

                {
                    Lock lock(mMutex);
                    _Remove(mpMainHead, mpMainTail, p.Get());
                    if (next == ILoaderEntry::kMainThread) 
                    {
                        _Insert(mpMainHead, mpMainTail, p.Get());
                    }
                    else if (next == ILoaderEntry::kLoaderThread)
                    {
                        _Insert(mpLoaderHead, mpLoaderTail, p.Get());
                    }
                }

                currentTick = system::Time::GetSingleton().GetAbsoluteMilliseconds();
            }
        }

        void Loader::_LoaderFunction(const Thread& t)
        {
            Loader& loader = Loader::GetSingleton();
            while (!loader.mbDone)
            {
                ILoaderEntryPtr p;
                {
                    Lock lock(loader.mMutex);
                    p = loader.mpLoaderHead;
                }

                if (!p.IsValid())
                {
                    Thread::Sleep(50);
                    continue;
                }

                ILoaderEntry::NextAction next = p->LoaderThreadAction();

                {
                    Lock lock(loader.mMutex);
                    loader._Remove(loader.mpLoaderHead, loader.mpLoaderTail, p.Get());
                    if (next == ILoaderEntry::kMainThread) 
                    {
                        loader._Insert(loader.mpMainHead, loader.mpMainTail, p.Get());
                    }
                    else if (next == ILoaderEntry::kLoaderThread)
                    {
                        loader._Insert(loader.mpLoaderHead, loader.mpLoaderTail, p.Get());
                    }
                }
            }
        }

    }
}