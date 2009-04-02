#include <jz_core/Prereqs.h>
#include <jz_test/Tests.h>
#include <ddraw.h>

namespace tut
{
    DUMMY(TestsDDraw);

    typedef HRESULT (WINAPI *FuncDirectDrawCreate)(GUID FAR*, LPDIRECTDRAW FAR*, IUnknown FAR*);

    template<> template<>
    void Object::test<1>()
    {
        using namespace jz;

#       if !NDEBUG
            HMODULE lib = LoadLibrary("debug\\ddraw.dll");
#       else
            HMODULE lib = LoadLibrary("release\\ddraw.dll");
#       endif
        ensure(lib != null);

        FuncDirectDrawCreate func = (FuncDirectDrawCreate)GetProcAddress(lib, "DirectDrawCreate");
        ensure(func != null);

        IDirectDraw* ddraw = null;
        func(null, &ddraw, null);
        ensure(ddraw != null);
        {
            ensure(ddraw->SetCooperativeLevel(null, (DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE)) == DD_OK);

            IDirectDrawPalette* pPalette = null;
            ensure(ddraw->CreatePalette(DDPCAPS_8BIT, null, &pPalette, null) == DD_OK);
            ensure(pPalette != null);

            {
                IDirectDrawSurface* pSurface = null;
                DDSURFACEDESC desc;
                memset(&desc, 0, sizeof(DDSURFACEDESC));
                ensure(ddraw->CreateSurface(&desc, &pSurface, null) == DD_OK);
                ensure(pSurface != null);
                
                {
                    
                }

                ensure (pSurface->Release() == 0u);
                pSurface = null;
            }

            ensure (pPalette->Release() == 0u);
            pPalette = null;
        }
        ensure(ddraw->Release() == 0u);
        ddraw = null;

        ensure(FreeLibrary(lib) == TRUE);
        lib = null;
        func = null;
    }

}
