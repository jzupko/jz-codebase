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

        const char* GetDXFormatAsString(D3DFORMAT f)
        {
    #       define DX_FORMAT_HELPER(code) case code: return #code; break;

            switch (f)
            {
                DX_FORMAT_HELPER(D3DFMT_UNKNOWN)
                DX_FORMAT_HELPER(D3DFMT_R8G8B8)
                DX_FORMAT_HELPER(D3DFMT_A8R8G8B8)
                DX_FORMAT_HELPER(D3DFMT_X8R8G8B8)
                DX_FORMAT_HELPER(D3DFMT_R5G6B5)
                DX_FORMAT_HELPER(D3DFMT_X1R5G5B5)
                DX_FORMAT_HELPER(D3DFMT_A1R5G5B5)
                DX_FORMAT_HELPER(D3DFMT_A4R4G4B4)
                DX_FORMAT_HELPER(D3DFMT_R3G3B2)
                DX_FORMAT_HELPER(D3DFMT_A8)
                DX_FORMAT_HELPER(D3DFMT_A8R3G3B2)
                DX_FORMAT_HELPER(D3DFMT_X4R4G4B4)
                DX_FORMAT_HELPER(D3DFMT_A2B10G10R10)
                DX_FORMAT_HELPER(D3DFMT_A8B8G8R8)
                DX_FORMAT_HELPER(D3DFMT_X8B8G8R8)
                DX_FORMAT_HELPER(D3DFMT_G16R16)
                DX_FORMAT_HELPER(D3DFMT_A2R10G10B10)
                DX_FORMAT_HELPER(D3DFMT_A16B16G16R16)
                DX_FORMAT_HELPER(D3DFMT_A8P8)
                DX_FORMAT_HELPER(D3DFMT_P8)
                DX_FORMAT_HELPER(D3DFMT_L8)
                DX_FORMAT_HELPER(D3DFMT_A8L8)
                DX_FORMAT_HELPER(D3DFMT_A4L4)
                DX_FORMAT_HELPER(D3DFMT_V8U8)
                DX_FORMAT_HELPER(D3DFMT_L6V5U5)
                DX_FORMAT_HELPER(D3DFMT_X8L8V8U8)
                DX_FORMAT_HELPER(D3DFMT_Q8W8V8U8)
                DX_FORMAT_HELPER(D3DFMT_V16U16)
                DX_FORMAT_HELPER(D3DFMT_A2W10V10U10)
                DX_FORMAT_HELPER(D3DFMT_UYVY)
                DX_FORMAT_HELPER(D3DFMT_R8G8_B8G8)
                DX_FORMAT_HELPER(D3DFMT_YUY2)
                DX_FORMAT_HELPER(D3DFMT_G8R8_G8B8)
                DX_FORMAT_HELPER(D3DFMT_DXT1)
                DX_FORMAT_HELPER(D3DFMT_DXT2)
                DX_FORMAT_HELPER(D3DFMT_DXT3)
                DX_FORMAT_HELPER(D3DFMT_DXT4)
                DX_FORMAT_HELPER(D3DFMT_DXT5)
                DX_FORMAT_HELPER(D3DFMT_D16_LOCKABLE)
                DX_FORMAT_HELPER(D3DFMT_D32)
                DX_FORMAT_HELPER(D3DFMT_D15S1)
                DX_FORMAT_HELPER(D3DFMT_D24S8)
                DX_FORMAT_HELPER(D3DFMT_D24X8)
                DX_FORMAT_HELPER(D3DFMT_D24X4S4)
                DX_FORMAT_HELPER(D3DFMT_D16)
                DX_FORMAT_HELPER(D3DFMT_D32F_LOCKABLE)
                DX_FORMAT_HELPER(D3DFMT_D24FS8)
                DX_FORMAT_HELPER(D3DFMT_L16)
                DX_FORMAT_HELPER(D3DFMT_VERTEXDATA)
                DX_FORMAT_HELPER(D3DFMT_INDEX16)
                DX_FORMAT_HELPER(D3DFMT_INDEX32)
                DX_FORMAT_HELPER(D3DFMT_Q16W16V16U16)
                DX_FORMAT_HELPER(D3DFMT_MULTI2_ARGB8)
                DX_FORMAT_HELPER(D3DFMT_R16F)
                DX_FORMAT_HELPER(D3DFMT_G16R16F)
                DX_FORMAT_HELPER(D3DFMT_A16B16G16R16F)
                DX_FORMAT_HELPER(D3DFMT_R32F)
                DX_FORMAT_HELPER(D3DFMT_G32R32F)
                DX_FORMAT_HELPER(D3DFMT_A32B32G32R32F)
                DX_FORMAT_HELPER(D3DFMT_CxV8U8)
            }

            return "";

    #       undef DX_FORMAT_HELPER
        }

        const char* GetDXErrorAsString(HRESULT h)
        {
    #       define DX_ERROR_HELPER(code) case code: return #code; break;

	        switch (h)
	        {
		        DX_ERROR_HELPER(D3D_OK)
		        DX_ERROR_HELPER(D3DERR_WRONGTEXTUREFORMAT)
		        DX_ERROR_HELPER(D3DERR_UNSUPPORTEDCOLOROPERATION)
		        DX_ERROR_HELPER(D3DERR_UNSUPPORTEDCOLORARG)
		        DX_ERROR_HELPER(D3DERR_UNSUPPORTEDALPHAOPERATION)
		        DX_ERROR_HELPER(D3DERR_UNSUPPORTEDALPHAARG)
		        DX_ERROR_HELPER(D3DERR_TOOMANYOPERATIONS)
		        DX_ERROR_HELPER(D3DERR_CONFLICTINGTEXTUREFILTER)
		        DX_ERROR_HELPER(D3DERR_UNSUPPORTEDFACTORVALUE)
		        DX_ERROR_HELPER(D3DERR_CONFLICTINGRENDERSTATE)
		        DX_ERROR_HELPER(D3DERR_UNSUPPORTEDTEXTUREFILTER)
		        DX_ERROR_HELPER(D3DERR_CONFLICTINGTEXTUREPALETTE)
		        DX_ERROR_HELPER(D3DERR_DRIVERINTERNALERROR)

		        DX_ERROR_HELPER(D3DERR_NOTFOUND)
		        DX_ERROR_HELPER(D3DERR_MOREDATA)
		        DX_ERROR_HELPER(D3DERR_DEVICELOST)
		        DX_ERROR_HELPER(D3DERR_DEVICENOTRESET)
		        DX_ERROR_HELPER(D3DERR_NOTAVAILABLE)
		        DX_ERROR_HELPER(D3DERR_OUTOFVIDEOMEMORY)
		        DX_ERROR_HELPER(D3DERR_INVALIDDEVICE)

		        DX_ERROR_HELPER(D3DERR_INVALIDCALL)
		        DX_ERROR_HELPER(D3DERR_DRIVERINVALIDCALL)
		        DX_ERROR_HELPER(D3DERR_WASSTILLDRAWING)
		        DX_ERROR_HELPER(D3DOK_NOAUTOGEN)
		        DX_ERROR_HELPER(D3DERR_DEVICEREMOVED)
		        DX_ERROR_HELPER(S_NOT_RESIDENT)
		        DX_ERROR_HELPER(S_RESIDENT_IN_SHARED_MEMORY)

		        DX_ERROR_HELPER(S_PRESENT_MODE_CHANGED)
		        DX_ERROR_HELPER(S_PRESENT_OCCLUDED)
		        DX_ERROR_HELPER(D3DERR_DEVICEHUNG)
	        }

	        return "";

    #       undef DX_ERROR_HELPER
        }

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
