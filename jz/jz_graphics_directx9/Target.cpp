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
#include <jz_graphics/Target.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        static D3DFORMAT Convert(Target::Format aFormat)
        {   
    #       define JZ_HELPER(e) case Target::k##e: return D3DFMT_##e; break

            switch (aFormat)
            {
                JZ_HELPER(A8R8G8B8);
                JZ_HELPER(X8R8G8B8);
                JZ_HELPER(R5G6B5);
                JZ_HELPER(X1R5G5B5);
                JZ_HELPER(A1R5G5B5);
                JZ_HELPER(A4R4G4B4);
                JZ_HELPER(R3G3B2);
                JZ_HELPER(A8);
                JZ_HELPER(X4R4G4B4);
                JZ_HELPER(A2B10G10R10);
                JZ_HELPER(A8B8G8R8);
                JZ_HELPER(X8B8G8R8);
                JZ_HELPER(G16R16);
                JZ_HELPER(A2R10G10B10);
                JZ_HELPER(A16B16G16R16);
                JZ_HELPER(R16F);
                JZ_HELPER(G16R16F);
                JZ_HELPER(A16B16G16R16F);
                JZ_HELPER(R32F);
                JZ_HELPER(G32R32F);
                JZ_HELPER(A32B32G32R32F);

            default:
                JZ_ASSERT(false);
                return D3DFMT_X8R8G8B8;
            }

    #       undef JZ_HELPER
        }

        Target::Target(unatural aWidth, unatural aHeight, Format aFormat, float aScaling)
            : mWidth(aWidth), mHeight(aHeight), mFormat(aFormat), mScaling(aScaling)
        {}

        Target::~Target()
        {}

        void Target::SetToDevice(unatural aTargetIndex)
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetRenderTarget(aTargetIndex, StaticCast<IDirect3DSurface9*>(mSurface)));
        }

        void Target::ResetTarget(unatural aTargetIndex)
        {
            if (aTargetIndex != 0u)
            {
                JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetRenderTarget(aTargetIndex, null));
            }
            else
            {
                IDirect3DSurface9* p = null;
                JZ_DEBUG_DX_FAIL(gpD3dDevice9->GetBackBuffer(0u, 0, D3DBACKBUFFER_TYPE_MONO, &p));
                JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetRenderTarget(0u, p));
                SafeRelease(p);
            }
        }

        void Target::_Create()
        {}

        void Target::_Destroy()
        {}

        void Target::_Lost()
        {
            SafeRelease<IDirect3DTexture9>(mTexture);
            SafeRelease<IDirect3DSurface9>(mSurface);
        }
        
        void Target::_Reset(natural aWidth, natural aHeight)
        {
            IDirect3DTexture9* pTexture = null;
            unatural width = (mWidth > 0u) ? mWidth : ((unatural)(aWidth * mScaling));
            unatural height = (mHeight > 0u) ? mHeight : ((unatural)(aHeight * mScaling));

            JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateTexture(
                width, height, 1, 
                D3DUSAGE_RENDERTARGET, 
                Convert(mFormat), D3DPOOL_DEFAULT, &pTexture, null));

            IDirect3DSurface9* pSurface = null;
            HRESULT hr = pTexture->GetSurfaceLevel(0u, &pSurface);

            if (SUCCEEDED(hr))
            {
                mSurface = pSurface;
                mTexture = pTexture;
            }
            else
            {
                SafeRelease<IDirect3DTexture9>(pTexture);
                JZ_E_ON_DX_FAIL(hr);
            }
        }

    }
}
