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
#ifndef _JZ_SYSTEM_SYSTEM_H_
#define _JZ_SYSTEM_SYSTEM_H_

#include <jz_core/Prereqs.h>
#include <jz_core/Utility.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#endif

namespace jz
{
    namespace system
    {

    #   if JZ_PLATFORM_WINDOWS
            extern HINSTANCE gpWindowInstance;
            extern HWND gpWindowHandle;

            class System : public Singleton<System>
            {
                public:
                    JZ_EXPORT System(HINSTANCE apWindowInstance = null, HWND apWindowHandle = null, WNDPROC apMessageHandler = null);
                    JZ_EXPORT ~System();
            };
            
            LRESULT CALLBACK GlobalMessageHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWordParam, LPARAM aLongParam);
    #   endif

    }
}

#endif
