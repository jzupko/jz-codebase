// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#include <jz_core/Color.h>
#include <jz_core/Logger.h>
#include <jz_graphics/Graphics.h>
#include <jz_ddraw/DirectDraw.h>
#include <jz_ddraw/DirectDrawPalette.h>
#include <jz_ddraw/DirectDrawSurface.h>
#include <jz_ddraw/DDHelper.h>

namespace jz
{
    namespace ddraw
    {

        static const D3DFORMAT kPaletteFormat = D3DFMT_A16B16G16R16F;
        typedef D3DXVECTOR4_16F* PalettePtrType;

        DirectDrawPalette::DirectDrawPalette(bool ab8bits, LPPALETTEENTRY lpColorTable)
            : mReferenceCount(1u),
              mPaletteSize((ab8bits) ? (1 << 8) : (1 << 4)),
              mbDirty(false)
        {
            DD_ASSERT();

            LPDIRECT3DTEXTURE9 p;
            if (SUCCEEDED(graphics::gpD3dDevice9->CreateTexture(
                kMaxPaletteSize,
                1u,
                1u,
                0,
                kPaletteFormat,
                D3DPOOL_MANAGED,
                &p, null)))
            {
                mpPalette = p;
            }
            else
            {
                gDdLastError = DDERR_GENERIC;
                return;
            }

            if (lpColorTable)
            {
                _Set(lpColorTable, 0u, mPaletteSize);
            }
        }

        DirectDrawPalette::~DirectDrawPalette()
        {
            DD_ASSERT();

            SafeRelease<IDirect3DTexture9>(mpPalette);
        }

        Handle DirectDrawPalette::GetPalette() const
        {
            if (mbDirty && mpPalette)
            {
                IDirect3DTexture9* p = const_cast<Handle&>(mpPalette).Cast<IDirect3DTexture9>();

                D3DLOCKED_RECT rect;
                ColorHSL colorHSL;
                if (SUCCEEDED(p->LockRect(0u, &rect, null, 0u)))
                {
                    PalettePtrType pBits = (PalettePtrType)rect.pBits;

                    for (size_t i = 0; i < kMaxPaletteSize; i++)
                    {
                        const PALETTEENTRY& entry = mData[i];

                        colorHSL.SetFromRGBu(ColorRGBu(entry.peRed, entry.peGreen, entry.peBlue));

                        pBits[i].x = colorHSL.H;
                        pBits[i].y = colorHSL.S;
                        pBits[i].z = colorHSL.L;
                        pBits[i].w = 1.0f;
                    }

                    const_cast<bool&>(mbDirty) = false;

                    p->UnlockRect(0u);
                }
            }

             return mpPalette;
        }

        bool DirectDrawPalette::_Set(LPPALETTEENTRY p, size_t aStart, size_t aCount)
        {
            DD_ASSERT();

            if ((aStart + aCount) > mPaletteSize) { return false; }

            memcpy((mData + aStart), p, sizeof(PALETTEENTRY) * aCount);
            mbDirty = true;

            return true;
        }

        STDMETHODDEF(DirectDrawPalette::QueryInterface)(THIS_ REFIID riid, LPVOID FAR * ppvObj)
        {
            DD_ASSERT();

            DD_RET_ERROR(E_NOINTERFACE);
        }

        STDMETHODDEF_(ULONG,DirectDrawPalette::AddRef) (THIS)
        {
            DD_ASSERT();
            __IncrementRefCount(this);

            DD_REF_COUNT(mReferenceCount);

            return mReferenceCount;
        }

        STDMETHODDEF_(ULONG,DirectDrawPalette::Release) (THIS)
        {
            DD_ASSERT();

            ULONG ret = (mReferenceCount - 1u);
            __DecrementRefCount(this);

            DD_REF_COUNT(ret);

            return ret;
        }
        
        STDMETHODDEF(DirectDrawPalette::GetCaps)(THIS_
            LPDWORD lpdwCaps)
        {
            DD_ASSERT();

            (*lpdwCaps) = (DDPCAPS_8BIT | DDPCAPS_ALPHA | DDPCAPS_ALLOW256 | DDPCAPS_PRIMARYSURFACE | DDPCAPS_VSYNC);

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawPalette::GetEntries)(THIS_ 
            DWORD dwFlags,
            DWORD dwBase,
            DWORD dwNumEntries,
            LPPALETTEENTRY lpEntries)
        {
            DD_ASSERT();

            size_t count = dwNumEntries;
            if ((dwBase + count) > mPaletteSize)
            {
                if (dwBase >= mPaletteSize) { count = 0u; }
                else { count = (mPaletteSize - dwBase); }
            }

            memcpy(lpEntries, (mData + dwBase), count);

            return DD_OK;
        }

        STDMETHODDEF(DirectDrawPalette::Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY)
        {
            DD_ASSERT();

            DD_RET_ERROR(DDERR_ALREADYINITIALIZED);
        }

        STDMETHODDEF(DirectDrawPalette::SetEntries)(THIS_
            DWORD dwFlags,
            DWORD dwStartingEntry,
            DWORD dwCount,
            LPPALETTEENTRY lpEntries)
        {
            DD_ASSERT();

            if (dwFlags != 0u) { DD_RET_ERROR(DDERR_INVALIDPARAMS); }

            if (_Set(lpEntries, dwStartingEntry, dwCount))
            {
                if (mpSurface && (mpSurface->mDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) != 0)
                {
                    mpSurface->mpDD->Flip(mpSurface);
                }

                return DD_OK;
            }
            else
            {
                DD_RET_ERROR(DDERR_INVALIDPARAMS);
            }
        }

    }
}
