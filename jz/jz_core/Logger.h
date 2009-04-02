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
#ifndef _JZ_LOGGER_H_
#define _JZ_LOGGER_H_

#include <jz_core/Prereqs.h>
#include <fstream>
#include <string>
#include <vector>

namespace jz
{

    class Logger
    {
    public:
        static const unatural kBufferLineCount = 10u;
        static const unatural kLineWidth = 80u;

        static const char* kLogFilename;
        static const char* kTab;

        enum MessageType
        {
            kNormal,
            kWarning,
            kError
        };

        static bool IsLogBufferDirty() { return msbLogBufferDirty; }

        JZ_EXPORT static string GetLogBuffer();
        JZ_EXPORT static void GetLogBuffer(vector<string>& arLogBuffer);
        JZ_EXPORT static void LogMessage(const string& s, MessageType t = kNormal);

        JZ_EXPORT static void SetLogFilename(const char* apFilename, bool abAppend = false);

    private:
        static ofstream msLogStream;
        static string msLogBuffer[kBufferLineCount];
        static bool msbLogBufferDirty;
        static uint msLogBufferTop;
        static bool msbAttemptedOpen;

        static const char* _TypeToString(MessageType t)
        {
            switch (t)
            {
                case kNormal: return "NORMAL"; break;
                case kWarning: return "WARNING"; break;
                case kError: return "ERROR"; break;
                default:
                    JZ_ASSERT(false);
                    return "";
                break;
            }
        }
    };

    inline void LogMessage(const string& s, Logger::MessageType t = Logger::kNormal)
    {
        Logger::LogMessage(s, t);
    }

}

#endif
