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
#ifndef _JZ_SYSTEM_THREAD_H_
#define _JZ_SYSTEM_THREAD_H_

#if JZ_MULTITHREADED
#   include <jz_core/Prereqs.h>
#   include <functional>

#   if JZ_PLATFORM_WINDOWS
        typedef jz::void_p HANDLE;
#   endif

    namespace jz
    {
        namespace system
        {

            class Thread
            {
                public:
                    enum Priority
                    {
                        kLow = 0,
                        kMed = 1,
                        kHigh = 2,
                        kCritical = 3
                    };
                
                    static const size_t kDefaultStackSize = (1 << 15);
                
                    typedef tr1::function<void(const Thread&)> ThreadFunc;
                
                    Thread(ThreadFunc aFunc);
                    Thread(ThreadFunc aFunc, size_t aStackSize);
                    ~Thread();

                    bool bCurrent() const;
                    bool bDone() const { return mbThreadDone; }

                    void SetPriority(Priority aPriority);

                    static void Sleep(ulong aMilliseconds);

                private:
        #           if JZ_PLATFORM_WINDOWS
                        static unsigned long __stdcall _ThreadStart(void_p apThread);
        #           endif
        	            
                    Thread(const Thread&);
                    Thread& operator=(const Thread&);
                    
                    ThreadFunc mFunc;
                    
                    volatile bool mbThreadDone;
                    
                    HANDLE mHandle;
            };

        }
    }
#endif

#endif
