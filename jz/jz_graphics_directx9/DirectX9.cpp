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

#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        DirectXFormatDescription DxFormatDescriptions[kDxFormatDescriptorCount] =
        {
            // enum-------------- bits bits-per-comp  masks -----------------------------------------//
            { D3DFMT_R8G8B8,      24,  8,  8,  8,  0, 0xFF0000,   0x00FF00,   0x0000FF,   0          },
            { D3DFMT_A8R8G8B8,    32,  8,  8,  8,  8, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 },
            { D3DFMT_X8R8G8B8,    32,  8,  8,  8,  0, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 },
            { D3DFMT_R5G6B5,      16,  5,  6,  5,  0, 0xF800,     0x07E0,     0x001F,     0          },
            { D3DFMT_X1R5G5B5,    16,  5,  5,  5,  0, 0x7C00,     0x03E0,     0x001F,     0          },
            { D3DFMT_A1R5G5B5,    16,  5,  5,  5,  1, 0x7C00,     0x03E0,     0x001F,     0x8000     },
            { D3DFMT_A4R4G4B4,    16,  4,  4,  4,  4, 0x0F00,     0x00F0,     0x000F,     0xF000     },
            { D3DFMT_R3G3B2,      8,   3,  3,  2,  0, 0xE0,       0x1C,       0x03,       0          },
            { D3DFMT_A8,          8,   0,  0,  0,  8, 0,          0,          0,          0xFF       },
            { D3DFMT_A8R3G3B2,    16,  3,  3,  2,  8, 0xE0,       0x1C,       0x03,       0x0000FF00 },
            { D3DFMT_X4R4G4B4,    16,  4,  4,  4,  0, 0x0F00,     0x00F0,     0x000F,     0          },
            { D3DFMT_A2B10G10R10, 32,  10, 10, 10, 2, 0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000 },
            { D3DFMT_A8B8G8R8,    32,  8,  8,  8,  8, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 },
            { D3DFMT_X8B8G8R8,    32,  8,  8,  8,  0, 0x000000FF, 0x0000FF00, 0x00FF0000, 0          },
            { D3DFMT_G16R16,      32,  16, 16, 0,  0, 0x0000FFFF, 0xFFFF0000, 0,          0          },
            { D3DFMT_A2R10G10B10, 32,  10, 10, 10, 2, 0x3FF00000, 0x000FFC00, 0x000003FF, 0xC0000000 }
        };

        D3DFORMAT GetDxFormat(u32 aBits, u32 aMaskR, u32 aMaskG, u32 aMaskB, u32 aMaskA)
        {
            for (size_t i = 0; i < kDxFormatDescriptorCount; i++)
            {
                if (DxFormatDescriptions[i].Bits == aBits &&
                    DxFormatDescriptions[i].BitMaskR == aMaskR &&
                    DxFormatDescriptions[i].BitMaskG == aMaskG &&
                    DxFormatDescriptions[i].BitMaskB == aMaskB &&
                    DxFormatDescriptions[i].BitMaskA == aMaskA)
                {
                    return DxFormatDescriptions[i].EnumValue;
                }
            }

            return D3DFMT_UNKNOWN;
        }

    }
}
