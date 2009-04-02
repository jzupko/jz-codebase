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

#include <jz_core/Logger.h>
#include <jz_core/StringUtility.h>
#include <iostream>
#include <time.h>

namespace jz
{

    const char* Logger::kLogFilename = "jz.log";
    const char* Logger::kTab = "    ";

    bool Logger::msbLogBufferDirty = false;
    uint Logger::msLogBufferTop = 0u;

    ofstream Logger::msLogStream;
    string Logger::msLogBuffer[Logger::kBufferLineCount];

    bool Logger::msbAttemptedOpen = false;

    void Logger::LogMessage(const string& s, Logger::MessageType t)
    {
        static uint sLogBufferIndex = 0u;

        struct tm localTime;

        time_t curTime;
        time(&curTime);

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
        errno_t error = localtime_s(&localTime, &curTime);
        JZ_ASSERT(error == 0);
#else
        localTime = localtime(&curTime);
#endif
        string message = string("[") + _TypeToString(t) + ": " + 
            StringUtility::ToString(localTime.tm_hour) + ":" +
            StringUtility::ToString(localTime.tm_min)  + ":" +
            StringUtility::ToString(localTime.tm_sec) + "]: " + s;

        if (msLogStream.is_open())
        {
            msLogStream << message << endl;
#ifdef NDEBUG
            if (t == kError)
            {
#endif
                msLogStream.flush();
#ifdef NDEBUG
            }
#endif
        }
        else if (!msLogStream.is_open() && !msbAttemptedOpen)
        {
            try
            {
                msLogStream.open(kLogFilename);
                msLogStream << message << endl;
            }
            catch (...)
            {
                cerr << __FUNCTION__ ": failed to open log file. Using cerr instead." << endl;
                cerr.flush();
            }
            msbAttemptedOpen = true;
        }
        else
        {
            cerr << message << endl;
#ifdef NDEBUG
            if (t == kError)
            {
#endif
                cerr.flush();
#ifdef NDEBUG
            }
#endif
        }

        string buf;

        while (message.size() > 0u)
        {
            if (sLogBufferIndex >= kBufferLineCount)
            {
                sLogBufferIndex = 0;
            }

            if (message.size() > kLineWidth)
            {
                size_t i = (kLineWidth - 1u);

                while (i > 0u && !isspace(message[i]))
                {
                    i--;
                }

                if (i == 0u)
                {
                    buf = message;
                    message.clear();
                }
                else
                {
                    buf = message.substr(0u, i + 1u);
                    message = message.substr(i + 1u);
                }
            }
            else
            {
                buf = message;
                message.clear();
            }

            msLogBuffer[sLogBufferIndex] = (buf + "\n");
            sLogBufferIndex++;
            msLogBufferTop = sLogBufferIndex;

            if (msLogBufferTop >= kBufferLineCount)
            {
                msLogBufferTop = 0u;
            }
        }

        msbLogBufferDirty = true;
    }

    string Logger::GetLogBuffer()
    {
        string output;

        for (size_t i = msLogBufferTop; i < kBufferLineCount; i++)
        {
            output += msLogBuffer[i];
        }

        if (msLogBufferTop > 0u)
        {
            for (size_t i = 0u; i < msLogBufferTop; i++)
            {
                output += msLogBuffer[i];
            }
        }

        msbLogBufferDirty = false;

        return output;
    }

    void Logger::GetLogBuffer(vector<string>& arLogBuffer)
    {
        arLogBuffer.clear();
        
        for (size_t i = msLogBufferTop; i < kBufferLineCount; i++)
        {
            arLogBuffer.push_back(msLogBuffer[i]);
        }

        if (msLogBufferTop > 0u)
        {
            for (size_t i = 0u; i < msLogBufferTop; i++)
            {
                arLogBuffer.push_back(msLogBuffer[i]);
            }
        }

        msbLogBufferDirty = false;
    }

    void Logger::SetLogFilename(const char* apFilename, bool abAppend)
    {
        try
        {
            if (msLogStream.is_open())
            {
                msLogStream.flush();
                msLogStream.close();
            }

            msLogStream.open(apFilename, (abAppend) ? ios_base::app : ios_base::out);
        }
        catch (...)
        {
            cerr << __FUNCTION__ ": failed to open log file. Using cerr instead." << endl;
            cerr.flush();
        }
        msbAttemptedOpen = true;        
    }

}
