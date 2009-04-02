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

#include <jz_system/Thread.h>
#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#endif

namespace jz
{
    namespace system
    {

    #if JZ_PLATFORM_WINDOWS
            DWORD WINAPI Thread::_ThreadStart(LPVOID apThread)
            {
                Thread* pThread = static_cast<Thread*>(apThread);

                (pThread->mFunc)(*pThread);

                pThread->mbThreadDone = true;
                
                return 0u;
            }


            Thread::Thread(ThreadFunc aFunc)
                : mFunc(aFunc), mbThreadDone(false)
            {
                DWORD d;
                mHandle = CreateThread(null, kDefaultStackSize, &_ThreadStart, this, 0u, &d);
            }
            
            Thread::Thread(ThreadFunc aFunc, size_t aStackSize)
                : mFunc(aFunc), mbThreadDone(false)
            {
                DWORD d;
                mHandle = CreateThread(null, aStackSize, &_ThreadStart, this, 0u, &d);
            }
            
            Thread::~Thread()
            {
                WaitForSingleObject(mHandle, INFINITE);
    		    CloseHandle(mHandle);
            }
            
            bool Thread::bCurrent() const
            {
                return (mHandle == GetCurrentThread());
            }

            void Thread::SetPriority(Priority aPriority)
            {
                switch (aPriority)
                {
                    case kLow: SetThreadPriority(mHandle, THREAD_PRIORITY_LOWEST); break;
                    case kMed: SetThreadPriority(mHandle, THREAD_PRIORITY_NORMAL); break;
                    case kHigh: SetThreadPriority(mHandle, THREAD_PRIORITY_HIGHEST); break;
                    case kCritical: SetThreadPriority(mHandle, THREAD_PRIORITY_TIME_CRITICAL); break;
                    default: break;
                }
            }

            void Thread::Sleep(ulong aMilliseconds)
            {
                ::Sleep(aMilliseconds);
            }
    #   endif

    }
}
