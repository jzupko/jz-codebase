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
#ifndef _JZ_GRAPHICS_DIRECTX9_DIRECTX9_H_
#define _JZ_GRAPHICS_DIRECTX9_DIRECTX9_H_

#include <jz_core/Prereqs.h>

#if JZ_PLATFORM_WINDOWS

#include <jz_graphics_directx9/DirectX9Prereqs.h>

#include <jz_core/Auto.h>
#include <jz_core/Logger.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Memory.h>
#include <jz_core/StringUtility.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <jz_core/Vector4.h>
#include <jz_graphics/PrimitiveType.h>
#include <string>

namespace jz
{
    namespace graphics
    {

        extern struct IDirect3DDevice9* gpD3dDevice9;

    #   if !NDEBUG
    #       define JZ_DEBUG_DX_FAIL(a) if ((a) < 0) { ::jz::LogMessage((string(__FUNCTION__) + "," + ::jz::StringUtility::ToString(__LINE__) + ":" + string(::jz::graphics::GetDXErrorAsString(a))).c_str(), Logger::kError); JZ_ASSERT(false); }
    #   else
    #       define JZ_DEBUG_DX_FAIL(a) a
    #   endif

    #   define JZ_E_ON_DX_FAIL(a) if ((a) < 0) { throw std::exception((string(__FUNCTION__) + "," + StringUtility::ToString(__LINE__) + ":" + string(::jz::graphics::GetDXErrorAsString(a))).c_str()); }

        struct DirectXFormatDescription
        {
            D3DFORMAT EnumValue;
            u32 Bits;
            u32 BitDepthR, BitDepthG, BitDepthB, BitDepthA;
            u32 BitMaskR, BitMaskG, BitMaskB, BitMaskA;
        };

        const int kDxFormatDescriptorCount = 16;
        extern DirectXFormatDescription DxFormatDescriptions[kDxFormatDescriptorCount];

        D3DFORMAT GetDxFormat(u32 aBits, u32 aMaskR, u32 aMaskG, u32 aMaskB, u32 aMaskA);

        static D3DPRIMITIVETYPE Convert(PrimitiveType::Enum v)
        {
            switch (v)
            {
            case PrimitiveType::kPointList: return D3DPT_POINTLIST;
            case PrimitiveType::kLineList: return D3DPT_LINELIST;
            case PrimitiveType::kLineStrip: return D3DPT_LINESTRIP;
            case PrimitiveType::kTriangleList: return D3DPT_TRIANGLELIST;
            case PrimitiveType::kTriangleStrip: return D3DPT_TRIANGLESTRIP;
            case PrimitiveType::kTriangleFan: return D3DPT_TRIANGLEFAN;
            default:
                JZ_ASSERT(false);
                return D3DPT_FORCE_DWORD;
            }
        }

    }
}

#endif

#endif
