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

#include <jz_system/Time.h>

namespace jz
{

    system::Time* system::Time::mspSingleton = 0;
    namespace system
    {

        Time::Time()
        {
            Reset();
            Tick();
        }

        Time::~Time()
        {}

#   if JZ_PLATFORM_WINDOWS
        unatural Time::GetAbsoluteMilliseconds() const
        {
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            LONGLONG tick = (currentTime.QuadPart - mCounterStartTime.QuadPart);
            
            return (unatural)((float)(double(tick) / double(mCounterFrequency.QuadPart)) * 1000.0f);
        }

        float Time::GetElapsedSeconds() const
        {
            return (float)(double(mCounterTickTime - mCounterLastTime) / double(mCounterFrequency.QuadPart));
        }

        float Time::GetSeconds() const
        {
            return (float)(double(mCounterTickTime) / double(mCounterFrequency.QuadPart));
        }

        unatural Time::GetMilliseconds() const
        {
            return (unatural)((float)(double(mCounterTickTime) / double(mCounterFrequency.QuadPart)) * 1000.0f);            
        }

        void Time::Reset()
        {
            QueryPerformanceFrequency(&mCounterFrequency);
            QueryPerformanceCounter(&mCounterStartTime);
            mCounterLastTime = 0;
            mCounterTickTime = 0;
        }
        
        void Time::Tick()
        {
            Update();
        }
        
        void Time::Update()
        {
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            mCounterLastTime = mCounterTickTime;
            mCounterTickTime = (currentTime.QuadPart - mCounterStartTime.QuadPart);
        }
#   endif

    }
}
