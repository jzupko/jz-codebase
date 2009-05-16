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

#include <jz_system/Profiler.h>
#include <jz_system/Time.h>
#include <string>

namespace jz
{
    namespace system
    {

        void Profiler::Begin(const string& s)
        {
            Table::iterator I = mTable.find(s);
            if (I == mTable.end())
            {
                Stats stats = Stats::Create();
                mTable.insert(make_pair(s, stats));
                I = mTable.find(s);
            }

            I->second.LastMilliseconds = Time::GetSingleton().GetAbsoluteMilliseconds();
        }

        void Profiler::End(const string& s)
        {
            unatural tick = Time::GetSingleton().GetAbsoluteMilliseconds();

            Table::iterator I = mTable.find(s);
            if (I != mTable.end())
            {
                if (I->second.AverageMilliseconds == 0u)
                {
                    I->second.AverageMilliseconds = (tick - I->second.LastMilliseconds);
                }
                else
                {
                    I->second.AverageMilliseconds = (I->second.AverageMilliseconds + (tick - I->second.LastMilliseconds)) / 2u;
                }

                I->second.LastMilliseconds = 0u;
            }
        }

        unatural Profiler::GetAverageMilliseconds(const string& s) const
        {
            Table::const_iterator I = mTable.find(s);
            if (I != mTable.end())
            {
                return (I->second.AverageMilliseconds);
            }
            else
            {
                return (Constants<unatural>::kMax);
            }
        }

    }
}
