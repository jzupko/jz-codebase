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

#include <jz_graphics/Graphics.h>
#include <jz_graphics/SystemMemorySurface.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        static D3DFORMAT Convert(SystemMemorySurface::Format aFormat)
        {   
    #       define JZ_HELPER(e) case SystemMemorySurface::k##e: return D3DFMT_##e; break

            switch (aFormat)
            {
                JZ_HELPER(A16B16G16R16F);
                JZ_HELPER(R16F);
                JZ_HELPER(A8R8G8B8);
                JZ_HELPER(X8R8G8B8);
                JZ_HELPER(R5G6B5);
                JZ_HELPER(X1R5G5B5);
                JZ_HELPER(A1R5G5B5);
                JZ_HELPER(A4R4G4B4);
                JZ_HELPER(R3G3B2);
                JZ_HELPER(A8);
                JZ_HELPER(A8R3G3B2);
                JZ_HELPER(X4R4G4B4);
                JZ_HELPER(A2B10G10R10);
                JZ_HELPER(A8B8G8R8);
                JZ_HELPER(X8B8G8R8);
                JZ_HELPER(G16R16);
                JZ_HELPER(A2R10G10B10);
                JZ_HELPER(A16B16G16R16);
            default:
                JZ_ASSERT(false);
                return D3DFMT_X8R8G8B8;
            }

    #       undef JZ_HELPER
        }

        SystemMemorySurface::SystemMemorySurface(unatural aWidth, unatural aHeight, Format aFormat)
            : mWidth(aWidth), mHeight(aHeight), mFormat(aFormat)
        {}

        SystemMemorySurface::~SystemMemorySurface()
        {}

        void SystemMemorySurface::_Create()
        {
        }

        void SystemMemorySurface::_Destroy()
        {
        }

        void SystemMemorySurface::_Lost()
        {
            SafeRelease<IDirect3DSurface9>(mpSurface);
        }
        
        void SystemMemorySurface::_Reset(natural aWidth, natural aHeight)
        {
            unatural width = (mWidth > 0u) ? mWidth : aWidth;
            unatural height = (mHeight > 0u) ? mHeight : aHeight;

            IDirect3DSurface9* pSurface = null;
            JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateOffscreenPlainSurface(
                width, height, Convert(mFormat), D3DPOOL_SYSTEMMEM, &pSurface, null));

            mpSurface = pSurface;
        }

    }
}
