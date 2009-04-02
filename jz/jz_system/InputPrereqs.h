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
#ifndef _JZ_SYSTEM_INPUT_PREREQS_H
#define _JZ_SYSTEM_INPUT_PREREQS_H

#include <jz_core/Prereqs.h>

namespace jz
{
    namespace system
    {

    #   if JZ_PLATFORM_WINDOWS
        namespace Key
        {
            enum Code
            {
		        kLbutton      = 0x01,  
		        kRbutton      = 0x02,  
		        kCancel       = 0x03,  
		        kMbutton      = 0x04,  
		        kXbutton1     = 0x05,  
		        kXbutton2     = 0x06,  
		        kBack         = 0x08,  
		        kTab          = 0x09,  
		        kClear        = 0x0c,  
		        kReturn       = 0x0d,  
		        kShift        = 0x10,  
		        kControl      = 0x11,  
		        kMenu         = 0x12,  
		        kPause        = 0x13,  
		        kCapital      = 0x14,  
		        kKana         = 0x15,  
		        kHanguel      = 0x15,  
		        kHangul       = 0x15,  
		        kJunja        = 0x17,  
		        kFinal        = 0x18,  
		        kHanja        = 0x19,  
		        kKanji        = 0x19,  
		        kEscape       = 0x1b,  
		        kConvert      = 0x1c,  
		        kNonconvert   = 0x1d,  
		        kAccept       = 0x1e,  
		        kModeChange   = 0x1f,  
		        kSpace        = 0x20,  
		        kPrior        = 0x21,  
		        kNext         = 0x22,  
		        kEnd          = 0x23,  
		        kHome         = 0x24,  
		        kLeft         = 0x25,  
		        kUp           = 0x26,  
		        kRight        = 0x27,  
		        kDown         = 0x28,  
		        kSelect       = 0x29,  
		        kPrint        = 0x2a,  
		        kExecut       = 0x2b,  
		        kSnapshot     = 0x2c,  
		        kInsert       = 0x2d,  
		        kDelete       = 0x2e,  
		        kHelp         = 0x2f,  
		        k0            = 0x30,  
		        k1            = 0x31,  
		        k2            = 0x32,  
		        k3            = 0x33,  
		        k4            = 0x34,  
		        k5            = 0x35,  
		        k6            = 0x36,  
		        k7            = 0x37,  
		        k8            = 0x38,  
		        k9            = 0x39,  
		        kA            = 0x41,  
		        kB            = 0x42,  
		        kC            = 0x43,  
		        kD            = 0x44,  
		        kE            = 0x45,  
		        kF            = 0x46,  
		        kG            = 0x47,  
		        kH            = 0x48,  
		        kI            = 0x49,  
		        kJ            = 0x4a,  
		        kK            = 0x4b,  
		        kL            = 0x4c,  
		        kM            = 0x4d,  
		        kN            = 0x4e,  
		        kO            = 0x4f,  
		        kP            = 0x50,  
		        kQ            = 0x51,  
		        kR            = 0x52,  
		        kS            = 0x53,  
		        kT            = 0x54,  
		        kU            = 0x55,  
		        kV            = 0x56,  
		        kW            = 0x57,  
		        kX            = 0x58,  
		        kY            = 0x59,  
		        kZ            = 0x5a,  
		        kLwin         = 0x5b,  
		        kRwin         = 0x5c,  
		        kApps         = 0x5d,  
		        kSleep        = 0x5f,  
		        kNumpad0      = 0x60,  
		        kNumpad1      = 0x61,  
		        kNumpad2      = 0x62,  
		        kNumpad3      = 0x63,  
		        kNumpad4      = 0x64,  
		        kNumpad5      = 0x65,  
		        kNumpad6      = 0x66,  
		        kNumpad7      = 0x67,  
		        kNumpad8      = 0x68,  
		        kNumpad9      = 0x69,  
		        kMultiply     = 0x6a,  
		        kAdd          = 0x6b,  
		        kSeparator    = 0x6c,  
		        kSubtract     = 0x6d,  
		        kDecimal      = 0x6e,  
		        kDivide       = 0x6f,  
		        kF1           = 0x70,  
		        kF2           = 0x71,  
		        kF3           = 0x72,  
		        kF4           = 0x73,  
		        kF5           = 0x74,  
		        kF6           = 0x75,  
		        kF7           = 0x76,  
		        kF8           = 0x77,  
		        kF9           = 0x78,  
		        kF10          = 0x79,  
		        kF11          = 0x7a,  
		        kF12          = 0x7b,  
		        kF13          = 0x7c,  
		        kF14          = 0x7d,  
		        kF15          = 0x7e,  
		        kF16          = 0x7f,  
		        kF17          = 0x80,  
		        kF18          = 0x81,  
		        kF19          = 0x82,  
		        kF20          = 0x83,  
		        kF21          = 0x84,  
		        kF22          = 0x85,  
		        kF23          = 0x86,  
		        kF24          = 0x87,  
		        kNumlock      = 0x90,  
		        kScroll       = 0x91,  
		        kLshift       = 0xa0,  
		        kRshift       = 0xa1,  
		        kLcontrol     = 0xa2,  
		        kRcontrol     = 0xa3,  
		        kLmenu        = 0xa4,  
		        kRmenu        = 0xa5,  
		        kPlus         = 0xbb,  
		        kComma        = 0xbc,  
		        kMinus        = 0xbd,  
		        kPeriod       = 0xbe,  
		        kAttn         = 0xf6,  
		        kCrsel        = 0xf7,  
		        kExsel        = 0xf8,  
		        kEreof        = 0xf9,  
		        kPlay         = 0xfa,  
		        kZoom         = 0xfb,
		        kPa1          = 0xfd,
		        kOem_clear    = 0xfe,
		        kCodeCount        = 0xff // not a code, 1 + last code value.
            };

            enum State
            {
                kPressed = 0,
                kReleased = 1,
                kStateCount = 2
            };
        }

        namespace Mouse
        {
            enum Button
            {
                kLeft,
                kMiddle,
                kRight,
                kX1,
                kX2,
                kButtonCount
            };

            enum State
            {
                kPressed = 0,
                kReleased = 1,
                kStateCount = 2
            };
        }
    #   endif

    }
}

#endif
