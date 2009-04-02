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
#ifndef JZ_SYSTEM_LOADER_H_
#define JZ_SYSTEM_LOADER_H_

#include <jz_core/Auto.h>
#include <jz_core/Utility.h>
#include <jz_system/Mutex.h>
#include <jz_system/Thread.h>

namespace jz
{
    namespace system
    {
        class IReadFile;
        class Loader;
        class ILoaderEntry abstract
        {
        public:
            enum NextAction
            {
                kMainThread = 0,
                kLoaderThread = 1,
                kComplete = 2
            };
    
            virtual NextAction GetNextAction() const = 0;
            virtual NextAction MainThreadAction() = 0;
            virtual NextAction LoaderThreadAction() = 0;

        protected:
            ILoaderEntry()
                : mReferenceCount(0u), mpPrev(null)
            {}
            virtual ~ILoaderEntry() {}

            size_t mReferenceCount;

        private:
            friend void jz::__IncrementRefCount<system::ILoaderEntry>(system::ILoaderEntry* p);
            friend size_t jz::__GetRefCount<system::ILoaderEntry>(system::ILoaderEntry* p);
            friend void jz::__DecrementRefCount<system::ILoaderEntry>(system::ILoaderEntry* p);

            ILoaderEntry(const ILoaderEntry&);
            ILoaderEntry& operator=(const ILoaderEntry&);

            friend class Loader;
            AutoPtr<ILoaderEntry> mpNext;
            ILoaderEntry* mpPrev;
        };

        typedef AutoPtr<ILoaderEntry> ILoaderEntryPtr;

        class Loader sealed : public Singleton<Loader>
        {
        public:
            Loader();
            ~Loader();

            void Add(ILoaderEntry* p);
            void SetMainThreadMillisecondsPerTick(unatural v) { mMilliseconds = v; }
            void Tick();

        private:
            Loader(const Loader&);
            Loader& operator=(const Loader&);

            unatural mMilliseconds;

            ILoaderEntryPtr mpMainHead;
            ILoaderEntry* mpMainTail;
            ILoaderEntryPtr mpLoaderHead;
            ILoaderEntry* mpLoaderTail;

            void _Insert(ILoaderEntryPtr& pHead, ILoaderEntry*& pTail, ILoaderEntry* p);
            void _Remove(ILoaderEntryPtr& pHead, ILoaderEntry*& pTail, ILoaderEntry* p);

            static void _LoaderFunction(const Thread& t);
            volatile bool mbDone;

            Mutex mMutex;
            Thread mThread;
        };

    }
}

#endif
