// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#include <jz_core/Rect.h>
#include <jz_core/Logger.h>
#include <jz_core/StringUtility.h>
#include <jz_ddraw/DirectDraw.h>
#include <jz_ddraw/DirectDrawPalette.h>
#include <jz_ddraw/DirectDrawSurface.h>
#include <jz_ddraw/DDHelper.h>
#include <jz_graphics/Graphics.h>z

namespace jz
{
    namespace ddraw
    {

        static const D3DFORMAT kPaletteFormat = D3DFMT_A8;

        DirectDrawSurface::DirectDrawSurface(const DDSURFACEDESC& desc, DirectDraw* apDD)
            : mReferenceCount(1u), mDescription(desc), mpDD(apDD), mpPalette(null), mFormat(D3DFMT_UNKNOWN)
        {
            DD_ASSERT();

            SafeAcquire(mpDD);

            int attachedSurfaceCount = 0;
            bool bBackBuffer = false;

            #pragma region Determine if this surface has back buffers
            if ((mDescription.dwFlags & DDSD_CAPS) != 0 &&
                (mDescription.ddsCaps.dwCaps & (DDSCAPS_COMPLEX | DDSCAPS_FLIP)) != 0)
            {
                if ((mDescription.dwFlags & DDSD_BACKBUFFERCOUNT) != 0)
                {
                    if (mDescription.dwBackBufferCount < 1)
                    {
                        gDdLastError = DDERR_INVALIDPARAMS;
                        return;
                    }
                    else
                    {
                        attachedSurfaceCount = mDescription.dwBackBufferCount;
                    }
                }
                else
                {
                    bBackBuffer = true;
                }
            }
            #pragma endregion

            #pragma region If this is not a backbuffer and is a primary surface, take settings from DD
            if (!bBackBuffer && mpDD)
            {
                bool bPrimary = (mDescription.dwFlags & DDSD_CAPS) != 0 && (mDescription.ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE)) != 0;
                if (bPrimary)
                {
                    if (FAILED(mpDD->GetDisplayMode(&mDescription)))
                    {
                        gDdLastError = DDERR_GENERIC;
                        return;
                    }
                    mDescription.ddsCaps.dwCaps |= DDSCAPS_FRONTBUFFER;
                }
            }
            #pragma endregion

            #pragma region Determine D3D Format
            if ((mDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) != 0)
            {
                mFormat = kPaletteFormat;
            }
            else
            {
                mFormat = graphics::GetDxFormat(mDescription.ddpfPixelFormat.dwRGBBitCount,
                                                mDescription.ddpfPixelFormat.dwRBitMask,
                                                mDescription.ddpfPixelFormat.dwGBitMask,
                                                mDescription.ddpfPixelFormat.dwBBitMask,
                                                mDescription.ddpfPixelFormat.dwRGBAlphaBitMask);
            }
            #pragma endregion

            #pragma region Create texture
            LPDIRECT3DTEXTURE9 p;
            if (SUCCEEDED(graphics::gpD3dDevice9->CreateTexture(
                mDescription.dwWidth,
                mDescription.dwHeight,
                1,
                0,
                mFormat,
                D3DPOOL_MANAGED,
                &p,
                null)))
            {
                mpTexture = p;
            }
            else
            {
                gDdLastError = DDERR_UNSUPPORTEDFORMAT;
                return;
            }
            #pragma endregion

            #pragma region Generate any attached surfaces
            if (attachedSurfaceCount > 0)
            {
                mDescription.dwFlags |= (DDSD_BACKBUFFERCOUNT | DDSD_CAPS);
                mDescription.ddsCaps.dwCaps |= (DDSCAPS_COMPLEX | DDSCAPS_FLIP);
                mDescription.dwBackBufferCount = attachedSurfaceCount;

                DDSURFACEDESC desc = mDescription;
                desc.dwFlags &= ~DDSD_BACKBUFFERCOUNT;
                desc.dwBackBufferCount = 0;
                desc.ddsCaps.dwCaps &= ~DDSCAPS_FRONTBUFFER;

                DirectDrawSurface* p = new DirectDrawSurface(desc, mpDD);
                p->mDescription.ddsCaps.dwCaps |= DDSCAPS_BACKBUFFER;
                mAttachedSurfaces.push_back(p);

                for (int i = 1; i < attachedSurfaceCount; i++)
                {
                    mAttachedSurfaces.push_back(new DirectDrawSurface(desc, mpDD));
                }
            }
            #pragma endregion
        }

        DirectDrawSurface::~DirectDrawSurface()
        {
            DD_ASSERT();

            for (AttachedSurfaces::reverse_iterator I = mAttachedSurfaces.rbegin(); I != mAttachedSurfaces.rend(); I++)
            {
                (*I)->Release();
            }

            SafeRelease(mpPalette);
            SafeRelease<IDirect3DTexture9>(mpTexture);
            SafeRelease(mpDD);
        }

        /*** IUnknown methods ***/
        STDMETHODDEF(DirectDrawSurface::QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj)
        {
            DD_ASSERT();

            DD_RET_ERROR(E_NOINTERFACE);
        }

        STDMETHODDEF_(ULONG,DirectDrawSurface::AddRef) (THIS) 
        {
            DD_ASSERT();
            __IncrementRefCount(this);

            DD_REF_COUNT(mReferenceCount);

            return mReferenceCount;
        }

        STDMETHODDEF_(ULONG,DirectDrawSurface::Release) (THIS)
        {
            DD_ASSERT();
            ULONG ret = (mReferenceCount - 1u);
            __DecrementRefCount(this);

            DD_REF_COUNT(ret);

            return ret;
        }

        /*** IDirectDrawSurface methods ***/
        STDMETHODDEF(DirectDrawSurface::AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::AddOverlayDirtyRect)(THIS_ LPRECT)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        static RECT Helper(LPRECT p, const DDSURFACEDESC& desc)
        {
            RECT rect;

            if (p) { rect = *p; }
            else
            {
                rect.left = 0;
                rect.top = 0;
                rect.bottom = desc.dwHeight;
                rect.right = desc.dwWidth;
            }

            return rect;
        }

        static string ToString(const D3DLOCKED_RECT& r)
        {
            string ret = "D3DLOCKD_RECT = {\n";
            ret += "    Pitch: " + StringUtility::ToString(r.Pitch) + "\n";
            ret += "    Bits: " + StringUtility::ToString(r.pBits) + "\n";

            return ret;
        }

        STDMETHODDEF(DirectDrawSurface::Blt)(THIS_
            LPRECT lpDestRect,
            LPDIRECTDRAWSURFACE lpDDSrcSurface,
            LPRECT lpSrcRect,
            DWORD dwFlags,
            LPDDBLTFX lpDDBltFx)
        {
            DD_ASSERT();

            // blt fx not supported.
            if (lpDDBltFx)
            {
                LogMessage(ToString(*lpDDBltFx));
                //JZ_DX_NOT_IMPLEMENTED();
            }

            DirectDrawSurface* pSrc = dynamic_cast<DirectDrawSurface*>(lpDDSrcSurface);
            if (!pSrc) { DD_RET_ERROR(DDERR_GENERIC); }
            if (!mpTexture || !(pSrc->mpTexture)) { DD_RET_ERROR(DDERR_GENERIC); }

            // no format conversion supported.
            if (mFormat != pSrc->mFormat) { JZ_DX_NOT_IMPLEMENTED(); }

            LPDIRECT3DTEXTURE9 srcTex = pSrc->mpTexture.Cast<LPDIRECT3DTEXTURE9>();
            LPDIRECT3DTEXTURE9 desTex = mpTexture.Cast<LPDIRECT3DTEXTURE9>();

            RECT srcRect = Helper(lpSrcRect, pSrc->mDescription);
            RECT desRect = Helper(lpDestRect, mDescription);

            // currently only support non-stretching blts (straight copy)
            if ((srcRect.right - srcRect.left) != (desRect.right - desRect.left) ||
                (srcRect.bottom - srcRect.top) != (desRect.bottom - desRect.top))
            {
                JZ_DX_NOT_IMPLEMENTED(); 
            }

            D3DLOCKED_RECT src;
            D3DLOCKED_RECT des;
           
            if (FAILED(srcTex->LockRect(0u, &src, lpSrcRect, 0u))) { DD_RET_ERROR(DDERR_GENERIC); }
            if (FAILED(desTex->LockRect(0u, &des, lpDestRect, 0u))) { DD_RET_ERROR(DDERR_GENERIC); }

            size_t size = (mDescription.ddpfPixelFormat.dwRGBBitCount / 8) *
               (srcRect.bottom - srcRect.top) * (srcRect.right - srcRect.left);

            #pragma region memcpy by row
            {
                size_t pitch = (mDescription.ddpfPixelFormat.dwRGBBitCount / 8) * (srcRect.right - srcRect.left);

                size_t count = 0u;
                size_t srcOffset = 0u;
                size_t desOffset = 0u;

                while (count < size)
                {
                    memcpy((u8_p)des.pBits + desOffset, (u8c_p)src.pBits + srcOffset, pitch);
                    srcOffset += src.Pitch;
                    desOffset += des.Pitch;
                    count += pitch;
                }
            }
            #pragma endregion

            if (FAILED(srcTex->UnlockRect(0u))) { DD_RET_ERROR(DDERR_GENERIC); }
            if (FAILED(desTex->UnlockRect(0u))) { DD_RET_ERROR(DDERR_GENERIC); }

            if (mpDD && (mDescription.ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE)) != 0)
            {
                mpDD->Flip(this);
            }

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::BltBatch)(THIS_ LPDDBLTBATCH, DWORD, DWORD )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::BltFast)(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE, LPRECT,DWORD)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::DeleteAttachedSurface)(THIS_ DWORD,LPDIRECTDRAWSURFACE)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::EnumAttachedSurfaces)(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::EnumOverlayZOrders)(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::Flip)(THIS_ LPDIRECTDRAWSURFACE, DWORD)
        {
            size_t count = mAttachedSurfaces.size();
            for (size_t i = 0u; i < count; i++)
            {
                const DDSURFACEDESC& desc = mAttachedSurfaces[i]->mDescription;
                if ((desc.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER) != 0)
                {
                    mpDD->Flip(mAttachedSurfaces[i]);
                    return DD_OK;
                }
            }

            DD_RET_ERROR(DDERR_NOTFLIPPABLE);
        }

        STDMETHODDEF(DirectDrawSurface::GetAttachedSurface)(THIS_
            LPDDSCAPS lpDDSCaps,
            LPDIRECTDRAWSURFACE FAR *lplpDDAttachedSurface)
        {
            size_t count = mAttachedSurfaces.size();
            size_t i = 0;
            int iFound = -1;

            for (; i < count; i++)
            {
                const DDSURFACEDESC& desc = mAttachedSurfaces[i]->mDescription;

                if ((desc.ddsCaps.dwCaps & lpDDSCaps->dwCaps) != 0)
                {
                    iFound = (int)i;
                    i++;
                    break;
                }
            }

            for (; i < count; i++)
            {
                const DDSURFACEDESC& desc = mAttachedSurfaces[i]->mDescription;

                // two surfaces match request.
                if ((desc.ddsCaps.dwCaps & lpDDSCaps->dwCaps) != 0)
                {
                    DD_RET_ERROR(DDERR_GENERIC);
                }
            }

            if (iFound >= 0)
            {
                (*lplpDDAttachedSurface) = mAttachedSurfaces[iFound];
                return DD_OK;
            }
            else
            {
                DD_RET_ERROR(DDERR_NOTFOUND);
            }
        }

        STDMETHODDEF(DirectDrawSurface::GetBltStatus)(THIS_ DWORD)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::GetCaps)(THIS_ LPDDSCAPS)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR*)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::GetColorKey)(THIS_ DWORD, LPDDCOLORKEY)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::GetDC)(THIS_ HDC FAR *)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::GetFlipStatus)(THIS_
            DWORD dwFlags)
        {
                 if ((dwFlags & DDGFS_CANFLIP) != 0) { return DD_OK; }
            else if ((dwFlags & DDGFS_ISFLIPDONE) != 0) { return DD_OK; }
            else { DD_RET_ERROR(DDERR_GENERIC); }
        }

        STDMETHODDEF(DirectDrawSurface::GetOverlayPosition)(THIS_ LPLONG, LPLONG )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::GetPalette)(THIS_
            LPDIRECTDRAWPALETTE FAR* lplpPalette)
        {
            DD_ASSERT();

            (*lplpPalette) = mpPalette;

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::GetPixelFormat)(THIS_
            LPDDPIXELFORMAT lpDDPixelFormat)
        {
            DD_ASSERT();

            (*lpDDPixelFormat) = (mDescription.ddpfPixelFormat);

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::GetSurfaceDesc)(THIS_
            LPDDSURFACEDESC lpDDSurfaceDesc)
        {
            DD_ASSERT();

            (*lpDDSurfaceDesc) = mDescription;

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC)
        {
            DD_ASSERT();

            DD_RET_ERROR(DDERR_ALREADYINITIALIZED);
        }

        STDMETHODDEF(DirectDrawSurface::IsLost)(THIS)
        {
            DD_ASSERT();

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::Lock)(THIS_
            LPRECT lpDestRect,
            LPDDSURFACEDESC lpDDSurfaceDesc,
            DWORD dwFlags,
            HANDLE hEvent)
        {
            DD_ASSERT();

            if (hEvent) { DD_RET_ERROR(DDERR_GENERIC); }
            if (!mpTexture) { DD_RET_ERROR(DDERR_SURFACELOST); }

            LPDIRECT3DTEXTURE9 pTexture = mpTexture.Cast<LPDIRECT3DTEXTURE9>();

            D3DLOCKED_RECT lockedRect;
            if (FAILED(pTexture->LockRect(0u, &lockedRect, lpDestRect, 0))) { DD_RET_ERROR(DDERR_GENERIC); }

            (*lpDDSurfaceDesc) = mDescription;
            lpDDSurfaceDesc->lpSurface = lockedRect.pBits;
            lpDDSurfaceDesc->lPitch = lockedRect.Pitch;
            lpDDSurfaceDesc->dwFlags |= (DDSD_LPSURFACE | DDSD_PITCH);

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::ReleaseDC)(THIS_ HDC)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::Restore)(THIS)
        {
            DD_ASSERT();

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::SetClipper)(THIS_ LPDIRECTDRAWCLIPPER)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::SetColorKey)(THIS_ DWORD, LPDDCOLORKEY)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::SetOverlayPosition)(THIS_ LONG, LONG )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::SetPalette)(THIS_
            LPDIRECTDRAWPALETTE lpDDPalette)
        {
            DD_ASSERT();

            {
                DirectDrawPalette* p = dynamic_cast<DirectDrawPalette*>(mpPalette);
                if (p) { p->mpSurface = null; }
            }
            SafeRelease(mpPalette);
            mpPalette = lpDDPalette;
            SafeAcquire(mpPalette);
            {
                DirectDrawPalette* p = dynamic_cast<DirectDrawPalette*>(mpPalette);
                if (p) { p->mpSurface = this; }
            } 

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::Unlock)(THIS_ LPVOID)
        {
            DD_ASSERT();

            if (!mpTexture) { DD_RET_ERROR(DDERR_SURFACELOST); }

            LPDIRECT3DTEXTURE9 pTexture = mpTexture.Cast<LPDIRECT3DTEXTURE9>();
            if (FAILED(pTexture->UnlockRect(0u))) { DD_RET_ERROR(DDERR_GENERIC); }

            if (mpDD && (mDescription.ddsCaps.dwCaps & (DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE)) != 0)
            {
                mpDD->Flip(this);
            }

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawSurface::UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::UpdateOverlayDisplay)(THIS_ DWORD)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDrawSurface::UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

    }
}
