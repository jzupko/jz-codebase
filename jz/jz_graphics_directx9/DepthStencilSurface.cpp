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

#include <jz_graphics/DepthStencilSurface.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        static D3DFORMAT Convert(DepthStencilSurface::Format aFormat)
        {   
    #       define JZ_HELPER(e) case DepthStencilSurface::k##e: return D3DFMT_##e; break

            switch (aFormat)
            {
                JZ_HELPER(D16_LOCKABLE);
                JZ_HELPER(D32);
                JZ_HELPER(D15S1);
                JZ_HELPER(D24S8);
                JZ_HELPER(D24X8);
                JZ_HELPER(D24X4S4);
                JZ_HELPER(D16);
            default:
                JZ_ASSERT(false);
                return D3DFMT_X8R8G8B8;
            }

    #       undef JZ_HELPER
        }

        DepthStencilSurface::DepthStencilSurface(unatural aWidth, unatural aHeight, Format aFormat)
            : mWidth(aWidth), mHeight(aHeight), mFormat(aFormat)
        {}

        DepthStencilSurface::~DepthStencilSurface()
        {}

        void DepthStencilSurface::SetToDevice()
        {
            if (mHandle)
            {
                IDirect3DSurface9* p = null;
                JZ_DEBUG_DX_FAIL(gpD3dDevice9->GetDepthStencilSurface(&p));
                mPrevHandle = p;

                JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetDepthStencilSurface(StaticCast<IDirect3DSurface9*>(mHandle)));
            }
        }

        void DepthStencilSurface::RestorePrevToDevice()
        {
            if (mPrevHandle)
            {
                JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetDepthStencilSurface(StaticCast<IDirect3DSurface9*>(mPrevHandle)));
                SafeRelease<IDirect3DSurface9>(mPrevHandle);
            }
        }

        void DepthStencilSurface::_Create()
        {
        }

        void DepthStencilSurface::_Destroy()
        {
        }

        void DepthStencilSurface::_Lost()
        {
            SafeRelease<IDirect3DSurface9>(mHandle);
        }
        
        void DepthStencilSurface::_Reset(natural aWidth, natural aHeight)
        {
            unatural width = (mWidth > 0u) ? mWidth : aWidth;
            unatural height = (mHeight > 0u) ? mHeight : aHeight;

            IDirect3DSurface9* pSurface = null;
            JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateDepthStencilSurface(
                width, height,
                Convert(mFormat), D3DMULTISAMPLE_NONE, 0, TRUE,
                &pSurface, null));

            mHandle = pSurface;
        }

    }
}
