// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#ifndef _JZ_DIRECT_DRAW_PALETTE_H_
#define _JZ_DIRECT_DRAW_PALETTE_H_

#include <jz_core/Auto.h>
#include <jz_core/Memory.h>
#include <jz_graphics_directx9/DirectX9.h>
#include <ddraw.h>

namespace jz
{
    namespace ddraw
    {
        static const size_t kMaxPaletteSize = 256;

        class DirectDraw;
        class DirectDrawSurface;
        class DirectDrawPalette : public IDirectDrawPalette
        {
        public:
            DirectDrawPalette(bool ab8bits = true, LPPALETTEENTRY lpColorTable = null);
            virtual ~DirectDrawPalette();

            Handle GetPalette() const;

            /*** IUnknown methods ***/
            STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * ppvObj);
            STDMETHOD_(ULONG,AddRef) (THIS);
            STDMETHOD_(ULONG,Release) (THIS);
            
            /*** IDirectDrawPalette methods ***/
            STDMETHOD(GetCaps)(THIS_ LPDWORD);
            STDMETHOD(GetEntries)(THIS_ DWORD,DWORD,DWORD,LPPALETTEENTRY);
            STDMETHOD(Initialize)(THIS_ LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
            STDMETHOD(SetEntries)(THIS_ DWORD,DWORD,DWORD,LPPALETTEENTRY);

        private:
            DirectDrawPalette(const DirectDrawPalette&);
            DirectDrawPalette& operator=(const DirectDrawPalette&);

            friend void ::jz::__IncrementRefCount<DirectDrawPalette>(DirectDrawPalette*);
            friend void ::jz::__DecrementRefCount<DirectDrawPalette>(DirectDrawPalette*);

            bool _Set(LPPALETTEENTRY p, size_t aStart, size_t aCount);

            size_t mReferenceCount;
            size_t mPaletteSize;
            bool mbDirty;
            PALETTEENTRY mData[kMaxPaletteSize];
            Handle mpPalette;

            friend class DirectDrawSurface;
            DirectDrawSurface* mpSurface;
        };

    }
}

#endif
