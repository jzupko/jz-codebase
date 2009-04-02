// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#ifndef _JZ_DIRECT_DRAW_H_
#define _JZ_DIRECT_DRAW_H_

#include <jz_core/Auto.h>
#include <jz_core/Rect.h>
#include <jz_graphics_directx9/DirectX9.h>
#include <ddraw.h>

namespace jz
{
    namespace graphics
    {
        class Graphics;
    }

    namespace system
    {
        class System;
    }

    namespace ddraw
    {
        class DirectDrawSurface;
        class DDEffect; typedef AutoPtr<DDEffect> DDEffectPtr;
        class DirectDraw : public IDirectDraw
        {
        public:
            DirectDraw();
            virtual ~DirectDraw();

            /*** IUnknown methods ***/
            STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj);
            STDMETHOD_(ULONG,AddRef) (THIS);
            STDMETHOD_(ULONG,Release) (THIS);

            /*** IDirectDraw methods ***/
            STDMETHOD(Compact)(THIS);
            STDMETHOD(CreateClipper)(THIS_ DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR * );
            STDMETHOD(CreatePalette)(THIS_ DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR * );
            STDMETHOD(CreateSurface)(THIS_  LPDDSURFACEDESC, LPDIRECTDRAWSURFACE FAR *, IUnknown FAR *);
            STDMETHOD(DuplicateSurface)( THIS_ LPDIRECTDRAWSURFACE, LPDIRECTDRAWSURFACE FAR * );
            STDMETHOD(EnumDisplayModes)( THIS_ DWORD, LPDDSURFACEDESC, LPVOID, LPDDENUMMODESCALLBACK );
            STDMETHOD(EnumSurfaces)(THIS_ DWORD, LPDDSURFACEDESC, LPVOID,LPDDENUMSURFACESCALLBACK );
            STDMETHOD(FlipToGDISurface)(THIS);
            STDMETHOD(GetCaps)( THIS_ LPDDCAPS, LPDDCAPS);
            STDMETHOD(GetDisplayMode)( THIS_ LPDDSURFACEDESC);
            STDMETHOD(GetFourCCCodes)(THIS_  LPDWORD, LPDWORD );
            STDMETHOD(GetGDISurface)(THIS_ LPDIRECTDRAWSURFACE FAR *);
            STDMETHOD(GetMonitorFrequency)(THIS_ LPDWORD);
            STDMETHOD(GetScanLine)(THIS_ LPDWORD);
            STDMETHOD(GetVerticalBlankStatus)(THIS_ LPBOOL );
            STDMETHOD(Initialize)(THIS_ GUID FAR *);
            STDMETHOD(RestoreDisplayMode)(THIS);
            STDMETHOD(SetCooperativeLevel)(THIS_ HWND, DWORD);
            STDMETHOD(SetDisplayMode)(THIS_ DWORD, DWORD,DWORD);
            STDMETHOD(WaitForVerticalBlank)(THIS_ DWORD, HANDLE );

            void Flip(DirectDrawSurface* p);

        private:
            DDEffectPtr mpBltEffect;
            graphics::Graphics* mpGraphics;
            system::System* mpSystem;

            DirectDraw(const DirectDraw&);
            DirectDraw& operator=(const DirectDraw&);

            friend void jz::__IncrementRefCount<ddraw::DirectDraw>(ddraw::DirectDraw*);
            friend void jz::__DecrementRefCount<ddraw::DirectDraw>(ddraw::DirectDraw*);

            size_t mReferenceCount;

            struct Mode
            {
                Mode()
                    : Width(0u), Height(0u), BPP(0u)
                {}

                bool operator==(const Mode& b) const
                {
                    return (Width == b.Width) &&
                        (Height == b.Height) &&
                        (BPP == b.BPP);
                }

                size_t Width;
                size_t Height;
                size_t BPP;
            } mMode;

            DDSURFACEDESC mDisplayMode;
        };

    }
}

extern HRESULT WINAPI DirectDrawCreate( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );

#endif
