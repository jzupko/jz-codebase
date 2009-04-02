//
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#include <jz_core/Logger.h>
#include <jz_ddraw/DirectDraw.h>
#include <jz_ddraw/DirectDrawPalette.h>
#include <jz_ddraw/DirectDrawSurface.h>
#include <jz_ddraw/DDEffect.h>
#include <jz_ddraw/DDHelper.h>
#include <jz_graphics/Graphics.h>
#include <jz_system/Files.h>
#include <jz_system/System.h>
#include <fstream>

namespace jz
{
    namespace ddraw
    {

        int _AssertOpenDD::msDepth = 0;

        static const char* kBltEffect = "DdBlt.cfx";

        HRESULT gDdLastError = DD_OK;

    #   define JZ_ASSERT_SYSTEM() if (!mpSystem) { DD_RET_ERROR(DDERR_GENERIC); }

        DirectDraw::DirectDraw()
            : mReferenceCount(1u),
            mpGraphics(null),
            mpSystem(null)
        {
            DD_ASSERT();
        }

        DirectDraw::~DirectDraw()
        {
            DD_ASSERT();

            mpBltEffect.Reset();
            SafeDelete(mpGraphics);
            SafeDelete(mpSystem);
        }

        void DirectDraw::Flip(DirectDrawSurface* p)
        {
            using namespace graphics;
            DD_ASSERT();

            if (!mpBltEffect.IsValid()) { gDdLastError = DDERR_GENERIC; return; }
            if (!mpGraphics) { gDdLastError = DDERR_GENERIC; return; }

            if (mpGraphics->Begin(ColorRGBA::kWhite, false))
            {
                DDSURFACEDESC desc;
                if (FAILED(p->GetSurfaceDesc(&desc))) { gDdLastError = DDERR_GENERIC; return; }
                if ((desc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) != 0)
                {
                    IDirectDrawPalette* pPalette = null;
                    if (FAILED(p->GetPalette(&pPalette))) { gDdLastError = DDERR_GENERIC; return; }
                    
                    DirectDrawPalette* pDdPalette = dynamic_cast<DirectDrawPalette*>(pPalette);
                    if (!pDdPalette) { gDdLastError = DDERR_GENERIC; return; }

                    mpBltEffect->Set(pDdPalette);
                    mpBltEffect->DrawWithPalette(p->mpTexture, Rect::kZeroToOne, Rect::kZeroToOne);
                }
                else
                {
                    mpBltEffect->DrawWithoutPalette(p->mpTexture, Rect::kZeroToOne, Rect::kZeroToOne);
                }

                mpGraphics->End();
            }
        }

        /*** IUnknown methods ***/
        STDMETHODDEF(DirectDraw::QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj)
        {
            DD_ASSERT();

            DD_RET_ERROR(E_NOINTERFACE);
        }

        STDMETHODDEF_(ULONG,DirectDraw::AddRef) (THIS)
        {
            DD_ASSERT();
            __IncrementRefCount(this);

            DD_REF_COUNT(mReferenceCount);

            return mReferenceCount;
        }

        STDMETHODDEF_(ULONG,DirectDraw::Release) (THIS)
        {
            DD_ASSERT();

            ULONG ret = (mReferenceCount - 1u);
            __DecrementRefCount(this);

            DD_REF_COUNT(ret);

            return ret;
        }

        /*** IDirectDraw methods ***/
        STDMETHODDEF(DirectDraw::Compact)(THIS)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::CreateClipper)(THIS_ DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::CreatePalette)(THIS_ 
            DWORD dwFlags, 
            LPPALETTEENTRY lpColorTable, 
            LPDIRECTDRAWPALETTE FAR* lplpDDPalette,
            IUnknown FAR * pUnkOuter)
        {
            DD_ASSERT();
            JZ_ASSERT_SYSTEM();

            if (pUnkOuter) { DD_RET_ERROR(DDERR_GENERIC); }
            if ((dwFlags & DDPCAPS_8BITENTRIES) != 0) { DD_RET_ERROR(DDERR_INVALIDPARAMS); }

            bool bits8 = true;
            if ((dwFlags & DDPCAPS_4BIT) != 0)
            {
                bits8 = false;
            }

            (*lplpDDPalette) = new DirectDrawPalette(bits8, ((dwFlags & DDPCAPS_INITIALIZE) != 0) ? lpColorTable : null);

            return DD_OK;
        }

        STDMETHODDEF(DirectDraw::CreateSurface)(THIS_
            LPDDSURFACEDESC lpDDSurfaceDesc,
            LPDIRECTDRAWSURFACE FAR * lplpDDSurface,
            IUnknown FAR * pUnkOuter)
        {
            DD_ASSERT();
            JZ_ASSERT_SYSTEM();

            if (pUnkOuter) { DD_RET_ERROR(DDERR_GENERIC); }

            LogMessage(ToString(*lpDDSurfaceDesc));

            (*lplpDDSurface) = new DirectDrawSurface(*lpDDSurfaceDesc, this);

            return DD_OK;
        }

        STDMETHODDEF(DirectDraw::DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::EnumDisplayModes)( THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::EnumSurfaces)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::FlipToGDISurface)(THIS)
        {
            return DD_OK;
            //JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::GetCaps)( THIS_ LPDDCAPS, LPDDCAPS)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::GetDisplayMode)(THIS_
            LPDDSURFACEDESC lpddSurfaceDesc)
        {
            DD_ASSERT();

            if (mMode == Mode()) { DD_RET_ERROR(DDERR_GENERIC); }

            (*lpddSurfaceDesc) = mDisplayMode;

            return DD_OK;
        }

        STDMETHODDEF(DirectDraw::GetFourCCCodes)(THIS_  LPDWORD, LPDWORD )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR *)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::GetMonitorFrequency)(THIS_ LPDWORD)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::GetScanLine)(THIS_ LPDWORD)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::GetVerticalBlankStatus)(THIS_ LPBOOL )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::Initialize)(THIS_ GUID FAR *)
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

        STDMETHODDEF(DirectDraw::RestoreDisplayMode)(THIS)
        {
            DD_ASSERT();
            JZ_ASSERT_SYSTEM();

            return DD_OK;
        }

        STDMETHODDEF(DirectDraw::SetCooperativeLevel)(THIS_ 
            HWND hWnd, 
            DWORD dwFlags)
        {
            using namespace graphics;
            DD_ASSERT();

            // Fail if system exists, means SetCooperative was already called.
            if (mpGraphics || mpSystem) { DD_RET_ERROR(DDERR_GENERIC); }

            if (!(dwFlags == (DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE) ||
                  dwFlags == (DDSCL_NORMAL)))
            {
                DD_RET_ERROR(DDERR_GENERIC);
            }

            try
            {
                mpSystem = new system::System((hWnd) ? null : GetModuleHandle(null), hWnd, null);
                mpGraphics = new graphics::Graphics();
            }
            catch (std::exception& e)
            {
                SafeDelete(mpGraphics);
                SafeDelete(mpSystem);
                
                LogMessage(e.what(), Logger::kError);
                DD_RET_ERROR(DDERR_GENERIC);
            }

            mpGraphics->SetFullscreen(dwFlags == (DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE));

            gpD3dDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
            gpD3dDevice9->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
            gpD3dDevice9->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

            mpBltEffect = Graphics::GetSingleton().Create<DDEffect>(kBltEffect);
            if (!mpBltEffect->IsLoadable())
            {
                mpBltEffect.Reset();
                SafeDelete(mpGraphics);
                SafeDelete(mpSystem);

                LogMessage("blt effect is not loadable.", Logger::kError);
                DD_RET_ERROR(DDERR_GENERIC);
            }

            return DD_OK;
        }

        STDMETHODDEF(DirectDraw::SetDisplayMode)(THIS_
            DWORD dwWidth,
            DWORD dwHeight,
            DWORD dwBPP)
        {
            DD_ASSERT();
            JZ_ASSERT_SYSTEM();

            mMode.Width = dwWidth;
            mMode.Height = dwHeight;
            mMode.BPP = dwBPP;

            if (mMode.Width == 0 || mMode.Height == 0)
            {
                LogMessage("Invalid dimensions, w: " + StringUtility::ToString(mMode.Width) + 
                    ", h: " + StringUtility::ToString(mMode.Height));

                return DDERR_INVALIDMODE;
            }

            memset(&mDisplayMode, 0, sizeof(DDSURFACEDESC));
            mDisplayMode.dwSize = sizeof(DDSURFACEDESC);
            mDisplayMode.dwFlags = (DDSD_CAPS | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_WIDTH);
            mDisplayMode.dwWidth = dwWidth;
            mDisplayMode.dwHeight = dwHeight;
            mDisplayMode.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
            mDisplayMode.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);

            if (mMode.BPP == 8)
            {
                mDisplayMode.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
                mDisplayMode.ddpfPixelFormat.dwRGBBitCount = 8;
                mDisplayMode.ddpfPixelFormat.dwRBitMask = 0;
                mDisplayMode.ddpfPixelFormat.dwGBitMask = 0;
                mDisplayMode.ddpfPixelFormat.dwBBitMask = 0;
                mDisplayMode.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
            }
            else if (mMode.BPP == 16)
            {
                mDisplayMode.ddpfPixelFormat.dwFlags = DDPF_RGB;
                mDisplayMode.ddpfPixelFormat.dwRGBBitCount = 16;
                mDisplayMode.ddpfPixelFormat.dwRBitMask = 0xF800;
                mDisplayMode.ddpfPixelFormat.dwGBitMask = 0x07E0;
                mDisplayMode.ddpfPixelFormat.dwBBitMask = 0x001F;
                mDisplayMode.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
            }
            else if (mMode.BPP == 24)
            {
                mDisplayMode.ddpfPixelFormat.dwFlags = DDPF_RGB;
                mDisplayMode.ddpfPixelFormat.dwRGBBitCount = 24;
                mDisplayMode.ddpfPixelFormat.dwRBitMask = 0xFF0000;
                mDisplayMode.ddpfPixelFormat.dwGBitMask = 0x00FF00;
                mDisplayMode.ddpfPixelFormat.dwBBitMask = 0x0000FF;
                mDisplayMode.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
            }
            else if (mMode.BPP == 32)
            {
                mDisplayMode.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
                mDisplayMode.ddpfPixelFormat.dwRGBBitCount = 32;
                mDisplayMode.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
                mDisplayMode.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
                mDisplayMode.ddpfPixelFormat.dwBBitMask = 0x000000FF;
                mDisplayMode.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
            }
            else
            {
                LogMessage("Invalid BPP: " + StringUtility::ToString(mMode.BPP));

                return DDERR_INVALIDMODE;
            }

            return DD_OK;
        }

        STDMETHODDEF(DirectDraw::WaitForVerticalBlank)(THIS_ DWORD, HANDLE )
        {
            JZ_DX_NOT_IMPLEMENTED();
        }

    }
}

HRESULT WINAPI DirectDrawCreate(
    GUID FAR *lpGUID, 
    LPDIRECTDRAW FAR *lplpDD,
    IUnknown FAR *pUnkOuter)
{
    DD_ASSERT();

    (*lplpDD) = new jz::ddraw::DirectDraw();

    return DD_OK;
}