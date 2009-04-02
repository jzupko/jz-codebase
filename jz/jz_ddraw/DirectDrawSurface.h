// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#ifndef _JZ_DIRECT_DRAW_SURFACE_H_
#define _JZ_DIRECT_DRAW_SURFACE_H_

#include <jz_core/Auto.h>
#include <jz_core/Memory.h>
#include <jz_graphics_directx9/DirectX9.h>
#include <ddraw.h>
#include <vector>

namespace jz
{
    namespace ddraw
    {
        class DirectDraw;
        class DirectDrawPalette;
        class DirectDrawSurface : public IDirectDrawSurface
        {
        public:
            DirectDrawSurface(const DDSURFACEDESC& desc, DirectDraw* apDD);
            virtual ~DirectDrawSurface();

            /*** IUnknown methods ***/
            STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj);
            STDMETHOD_(ULONG,AddRef) (THIS);
            STDMETHOD_(ULONG,Release) (THIS);

            /*** IDirectDrawSurface methods ***/
            STDMETHOD(AddAttachedSurface)(THIS_ LPDIRECTDRAWSURFACE);
            STDMETHOD(AddOverlayDirtyRect)(THIS_ LPRECT);
            STDMETHOD(Blt)(THIS_ LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, LPDDBLTFX);
            STDMETHOD(BltBatch)(THIS_ LPDDBLTBATCH, DWORD, DWORD );
            STDMETHOD(BltFast)(THIS_ DWORD,DWORD,LPDIRECTDRAWSURFACE, LPRECT,DWORD);
            STDMETHOD(DeleteAttachedSurface)(THIS_ DWORD,LPDIRECTDRAWSURFACE);
            STDMETHOD(EnumAttachedSurfaces)(THIS_ LPVOID,LPDDENUMSURFACESCALLBACK);
            STDMETHOD(EnumOverlayZOrders)(THIS_ DWORD,LPVOID,LPDDENUMSURFACESCALLBACK);
            STDMETHOD(Flip)(THIS_ LPDIRECTDRAWSURFACE, DWORD);
            STDMETHOD(GetAttachedSurface)(THIS_ LPDDSCAPS, LPDIRECTDRAWSURFACE FAR *);
            STDMETHOD(GetBltStatus)(THIS_ DWORD);
            STDMETHOD(GetCaps)(THIS_ LPDDSCAPS);
            STDMETHOD(GetClipper)(THIS_ LPDIRECTDRAWCLIPPER FAR*);
            STDMETHOD(GetColorKey)(THIS_ DWORD, LPDDCOLORKEY);
            STDMETHOD(GetDC)(THIS_ HDC FAR *);
            STDMETHOD(GetFlipStatus)(THIS_ DWORD);
            STDMETHOD(GetOverlayPosition)(THIS_ LPLONG, LPLONG );
            STDMETHOD(GetPalette)(THIS_ LPDIRECTDRAWPALETTE FAR*);
            STDMETHOD(GetPixelFormat)(THIS_ LPDDPIXELFORMAT);
            STDMETHOD(GetSurfaceDesc)(THIS_ LPDDSURFACEDESC);
            STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, LPDDSURFACEDESC);
            STDMETHOD(IsLost)(THIS);
            STDMETHOD(Lock)(THIS_ LPRECT,LPDDSURFACEDESC,DWORD,HANDLE);
            STDMETHOD(ReleaseDC)(THIS_ HDC);
            STDMETHOD(Restore)(THIS);
            STDMETHOD(SetClipper)(THIS_ LPDIRECTDRAWCLIPPER);
            STDMETHOD(SetColorKey)(THIS_ DWORD, LPDDCOLORKEY);
            STDMETHOD(SetOverlayPosition)(THIS_ LONG, LONG );
            STDMETHOD(SetPalette)(THIS_ LPDIRECTDRAWPALETTE);
            STDMETHOD(Unlock)(THIS_ LPVOID);
            STDMETHOD(UpdateOverlay)(THIS_ LPRECT, LPDIRECTDRAWSURFACE,LPRECT,DWORD, LPDDOVERLAYFX);
            STDMETHOD(UpdateOverlayDisplay)(THIS_ DWORD);
            STDMETHOD(UpdateOverlayZOrder)(THIS_ DWORD, LPDIRECTDRAWSURFACE);

        private:
            friend class DirectDraw;
            friend class DirectDrawPalette;

            DirectDrawSurface(const DirectDrawSurface&);
            DirectDrawSurface& operator=(const DirectDrawSurface&);

            friend void ::jz::__IncrementRefCount<DirectDrawSurface>(DirectDrawSurface*);
            friend void ::jz::__DecrementRefCount<DirectDrawSurface>(DirectDrawSurface*);

            size_t mReferenceCount;
            DDSURFACEDESC mDescription;
            LPDIRECTDRAWPALETTE mpPalette;
            DirectDraw* mpDD;

            D3DFORMAT mFormat;
            Handle mpTexture;

            typedef vector<DirectDrawSurface*> AttachedSurfaces;
            AttachedSurfaces mAttachedSurfaces;
        };

    }
}

#endif
