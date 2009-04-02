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
#ifndef _JZ_SYSTEM_TIME_H_
#define _JZ_SYSTEM_TIME_H_

#include <jz_core/Prereqs.h>
#include <jz_core/Utility.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#endif

namespace jz
{
    namespace system
    {

        class Time : public Singleton<Time>
        {
            public:
                Time();
                ~Time();

                JZ_EXPORT unatural GetAbsoluteMilliseconds() const;
                JZ_EXPORT float GetElapsedSeconds() const;
                JZ_EXPORT unatural GetMilliseconds() const;
                JZ_EXPORT float GetSeconds() const;

                void Reset();
                JZ_EXPORT void Tick();

            private:
    #           if JZ_PLATFORM_WINDOWS        
                    LARGE_INTEGER mCounterStartTime;
                    LARGE_INTEGER mCounterFrequency;
                    LONGLONG      mCounterLastTime;
                    LONGLONG      mCounterTickTime;
    #           endif
                
                void Update();
        };

    }
}

#endif
