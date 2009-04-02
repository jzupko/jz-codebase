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
#ifndef _JZ_SYSTEM_WIN32_H_
#define _JZ_SYSTEM_WIN32_H_

#include <jz_system/Win32Resource.h>

#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOMENUS
#define NOKEYSTATES
// #define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODEFERWINDOWPOS
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMCX
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifndef GET_X_LPARAM
#   define GET_X_LPARAM(lParam) ((int)(short)LOWORD(lParam))
#endif

#ifndef GET_Y_LPARAM
#   define GET_Y_LPARAM(lParam) ((int)(short)HIWORD(lParam))
#endif

#ifndef WM_MOUSEWHEEL
#   define WM_MOUSEWHEEL 0x020A
#endif

#ifndef WHEEL_DELTA
#   define WHEEL_DELTA 120
#endif

#define JZ_IDI_JZ_ICON MAKEINTRESOURCE(JZ_PLATFORM_WINDOWS_TASKBAR_ICON)

#endif
