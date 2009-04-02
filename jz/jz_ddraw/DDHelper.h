// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#ifndef _JZ_DDHELPER_H_
#define _JZ_DDHELPER_H_

#include <iostream>

#define JZ_DX_NOT_IMPLEMENTED() \
    ::jz::LogMessage("D3D Interface Function: \"" __FUNCTION__ "\" is not implemented."); \
    return E_FAIL

#define STDMETHODDEF(method)        COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE method
#define STDMETHODDEF_(type,method)  COM_DECLSPEC_NOTHROW type STDMETHODCALLTYPE method

#include <jz_core/Logger.h>
#include <jz_core/StringUtility.h>

namespace jz
{
    namespace ddraw
    {

        extern HRESULT gDdLastError;

    #   define DD_RET_OK() return DD_OK;
    #   define DD_RET_ERROR(err) gDdLastError = err; return err;

    #   ifndef NDEBUG
    #       define DD_REF_COUNT(val) LogMessage(__FUNCTION__ ": ref_count, " + StringUtility::ToString(val))
    #   else
    #       define DD_REF_COUNT(val) {}
    #   endif

        inline string GetDDErrorAsString(HRESULT h)
        {
    #       define DD_ERROR_HELPER(code) case code: return #code; break;

	        switch (h)
	        {
		        DD_ERROR_HELPER(DD_OK)
		        DD_ERROR_HELPER(DDERR_ALREADYINITIALIZED)
		        DD_ERROR_HELPER(DDERR_CANNOTATTACHSURFACE)
		        DD_ERROR_HELPER(DDERR_CANNOTDETACHSURFACE)
		        DD_ERROR_HELPER(DDERR_CURRENTLYNOTAVAIL)
		        DD_ERROR_HELPER(DDERR_EXCEPTION)
		        DD_ERROR_HELPER(DDERR_GENERIC)
		        DD_ERROR_HELPER(DDERR_HEIGHTALIGN)
		        DD_ERROR_HELPER(DDERR_INCOMPATIBLEPRIMARY)
		        DD_ERROR_HELPER(DDERR_INVALIDCAPS)
		        DD_ERROR_HELPER(DDERR_INVALIDCLIPLIST)
		        DD_ERROR_HELPER(DDERR_INVALIDMODE)
		        DD_ERROR_HELPER(DDERR_INVALIDOBJECT)

		        DD_ERROR_HELPER(DDERR_INVALIDPARAMS)
		        DD_ERROR_HELPER(DDERR_INVALIDPIXELFORMAT)
		        DD_ERROR_HELPER(DDERR_INVALIDRECT)
		        DD_ERROR_HELPER(DDERR_LOCKEDSURFACES)
		        DD_ERROR_HELPER(DDERR_NO3D)
		        DD_ERROR_HELPER(DDERR_NOALPHAHW)
		        DD_ERROR_HELPER(DDERR_NOSTEREOHARDWARE)

		        DD_ERROR_HELPER(DDERR_NOSURFACELEFT)
		        DD_ERROR_HELPER(DDERR_NOCLIPLIST)
		        DD_ERROR_HELPER(DDERR_NOCOLORCONVHW)
		        DD_ERROR_HELPER(DDERR_NOCOOPERATIVELEVELSET)
		        DD_ERROR_HELPER(DDERR_NOCOLORKEY)
		        DD_ERROR_HELPER(DDERR_NOCOLORKEYHW)
		        DD_ERROR_HELPER(DDERR_NODIRECTDRAWSUPPORT)

		        DD_ERROR_HELPER(DDERR_NOEXCLUSIVEMODE)
		        DD_ERROR_HELPER(DDERR_NOFLIPHW)
		        DD_ERROR_HELPER(DDERR_NOGDI)

                DD_ERROR_HELPER(DDERR_NOMIRRORHW)
                DD_ERROR_HELPER(DDERR_NOTFOUND)
                DD_ERROR_HELPER(DDERR_NOOVERLAYHW)

                DD_ERROR_HELPER(DDERR_OVERLAPPINGRECTS)
                DD_ERROR_HELPER(DDERR_NORASTEROPHW)
                DD_ERROR_HELPER(DDERR_NOROTATIONHW)
                DD_ERROR_HELPER(DDERR_NOSTRETCHHW)
                DD_ERROR_HELPER(DDERR_NOT4BITCOLOR)
                DD_ERROR_HELPER(DDERR_NOT4BITCOLORINDEX)
                DD_ERROR_HELPER(DDERR_NOT8BITCOLOR)
                DD_ERROR_HELPER(DDERR_NOTEXTUREHW)
                DD_ERROR_HELPER(DDERR_NOVSYNCHW)
                DD_ERROR_HELPER(DDERR_NOZBUFFERHW)
                DD_ERROR_HELPER(DDERR_NOZOVERLAYHW)

                DD_ERROR_HELPER(DDERR_OUTOFCAPS)
                DD_ERROR_HELPER(DDERR_OUTOFMEMORY)
                DD_ERROR_HELPER(DDERR_OUTOFVIDEOMEMORY)
                DD_ERROR_HELPER(DDERR_OVERLAYCANTCLIP)
                DD_ERROR_HELPER(DDERR_OVERLAYCOLORKEYONLYONEACTIVE)

                DD_ERROR_HELPER(DDERR_PALETTEBUSY)
                DD_ERROR_HELPER(DDERR_COLORKEYNOTSET)
                DD_ERROR_HELPER(DDERR_SURFACEALREADYATTACHED)
                DD_ERROR_HELPER(DDERR_SURFACEALREADYDEPENDENT)
                DD_ERROR_HELPER(DDERR_SURFACEBUSY)

                DD_ERROR_HELPER(DDERR_CANTLOCKSURFACE)
                DD_ERROR_HELPER(DDERR_SURFACEISOBSCURED)
                DD_ERROR_HELPER(DDERR_SURFACELOST)
                DD_ERROR_HELPER(DDERR_SURFACENOTATTACHED)
                DD_ERROR_HELPER(DDERR_TOOBIGHEIGHT)
                DD_ERROR_HELPER(DDERR_TOOBIGSIZE)

                DD_ERROR_HELPER(DDERR_UNSUPPORTED)
                DD_ERROR_HELPER(DDERR_UNSUPPORTEDFORMAT)
                DD_ERROR_HELPER(DDERR_UNSUPPORTEDMASK)
                DD_ERROR_HELPER(DDERR_INVALIDSTREAM)
                DD_ERROR_HELPER(DDERR_VERTICALBLANKINPROGRESS)
                DD_ERROR_HELPER(DDERR_WASSTILLDRAWING)

                DD_ERROR_HELPER(DDERR_DDSCAPSCOMPLEXREQUIRED)
                DD_ERROR_HELPER(DDERR_XALIGN)
                DD_ERROR_HELPER(DDERR_INVALIDDIRECTDRAWGUID)
                DD_ERROR_HELPER(DDERR_DIRECTDRAWALREADYCREATED)

                DD_ERROR_HELPER(DDERR_NODIRECTDRAWHW)
                DD_ERROR_HELPER(DDERR_PRIMARYSURFACEALREADYEXISTS)
                DD_ERROR_HELPER(DDERR_NOEMULATION)
                DD_ERROR_HELPER(DDERR_REGIONTOOSMALL)
                DD_ERROR_HELPER(DDERR_CLIPPERISUSINGHWND)
                DD_ERROR_HELPER(DDERR_NOCLIPPERATTACHED)
                DD_ERROR_HELPER(DDERR_NOHWND)

                DD_ERROR_HELPER(DDERR_HWNDSUBCLASSED)
                DD_ERROR_HELPER(DDERR_HWNDALREADYSET)
                DD_ERROR_HELPER(DDERR_NOPALETTEATTACHED)
                DD_ERROR_HELPER(DDERR_NOPALETTEHW)
                DD_ERROR_HELPER(DDERR_BLTFASTCANTCLIP)
                DD_ERROR_HELPER(DDERR_NOBLTHW)
                DD_ERROR_HELPER(DDERR_NODDROPSHW)

                DD_ERROR_HELPER(DDERR_OVERLAYNOTVISIBLE)
                DD_ERROR_HELPER(DDERR_NOOVERLAYDEST)
                DD_ERROR_HELPER(DDERR_INVALIDPOSITION)
                DD_ERROR_HELPER(DDERR_NOTAOVERLAYSURFACE)
                DD_ERROR_HELPER(DDERR_EXCLUSIVEMODEALREADYSET)
                DD_ERROR_HELPER(DDERR_NOTFLIPPABLE)
                DD_ERROR_HELPER(DDERR_CANTDUPLICATE)
                DD_ERROR_HELPER(DDERR_NOTLOCKED)

                DD_ERROR_HELPER(DDERR_CANTCREATEDC)
                DD_ERROR_HELPER(DDERR_NODC)
                DD_ERROR_HELPER(DDERR_WRONGMODE)
                DD_ERROR_HELPER(DDERR_IMPLICITLYCREATED)
                DD_ERROR_HELPER(DDERR_NOTPALETTIZED)
                DD_ERROR_HELPER(DDERR_UNSUPPORTEDMODE)
                DD_ERROR_HELPER(DDERR_NOMIPMAPHW)
                DD_ERROR_HELPER(DDERR_INVALIDSURFACETYPE)
                DD_ERROR_HELPER(DDERR_NOOPTIMIZEHW)

                DD_ERROR_HELPER(DDERR_NOTLOADED)
                DD_ERROR_HELPER(DDERR_NOFOCUSWINDOW)
                DD_ERROR_HELPER(DDERR_NOTONMIPMAPSUBLEVEL)
                DD_ERROR_HELPER(DDERR_DCALREADYCREATED)
                
                DD_ERROR_HELPER(DDERR_NONONLOCALVIDMEM)
                DD_ERROR_HELPER(DDERR_CANTPAGELOCK)
                DD_ERROR_HELPER(DDERR_CANTPAGEUNLOCK)
                DD_ERROR_HELPER(DDERR_NOTPAGELOCKED)
                DD_ERROR_HELPER(DDERR_MOREDATA)
                DD_ERROR_HELPER(DDERR_EXPIRED)
                DD_ERROR_HELPER(DDERR_TESTFINISHED)
                DD_ERROR_HELPER(DDERR_NEWMODE)

                DD_ERROR_HELPER(DDERR_D3DNOTINITIALIZED)
                DD_ERROR_HELPER(DDERR_VIDEONOTACTIVE)
                DD_ERROR_HELPER(DDERR_NOMONITORINFORMATION)
                DD_ERROR_HELPER(DDERR_NODRIVERSUPPORT)
                DD_ERROR_HELPER(DDERR_DEVICEDOESNTOWNSURFACE)
                DD_ERROR_HELPER(DDERR_NOTINITIALIZED)

                DD_ERROR_HELPER(E_NOINTERFACE)
	        }

	        return string();

    #       undef DD_ERROR_HELPER
        }

        struct _AssertOpenDD
        {
        private:
            static int msDepth;

        public:
            _AssertOpenDD(const char* apFunction)
                : mFunction(apFunction)
            {
                Begin();
                msDepth++;
            }
            
            ~_AssertOpenDD()
            {
                msDepth--;
                JZ_ASSERT(msDepth >= 0);
                End();
            }

            static string _Pre()
            {
                string ret;
                for (int i = 0; i < msDepth; i++) { ret += "----"; }

                return ret;
            }

            void Begin()
            {
                if (gDdLastError != DD_OK)
                {
                    LogMessage(_Pre() + "Begin: " + mFunction + ", err: " + GetDDErrorAsString(gDdLastError));
                    gDdLastError = DD_OK;
                }
                else
                {
                    LogMessage(_Pre() + "Begin: " + mFunction);
                }
            }

            void End()
            {
                if (gDdLastError != DD_OK)
                {
                    LogMessage(_Pre() + "End: " + mFunction + ", err: " + GetDDErrorAsString(gDdLastError));
                    gDdLastError = DD_OK;
                }
                else
                {
                    LogMessage(_Pre() + "End: " + mFunction);
                }
            }

        private:
            string mFunction;
        };

    #   ifndef NDEBUG
    #       define DD_ASSERT() ::jz::ddraw::_AssertOpenDD __assert_Object_(__FUNCTION__)
    #   else
    #       define DD_ASSERT() {}
    #   endif

        static string ToString(const DDBLTFX& fx)
        {
    #       define JZ_HELPER(name) ret += "    " #name " = " + StringUtility::ToString(name) + "\n"

            string ret = "DDBLTFX {\n";

            JZ_HELPER(fx.dwSize);
            JZ_HELPER(fx.dwDDFX);
            JZ_HELPER(fx.dwROP);
            JZ_HELPER(fx.dwDDROP);
            JZ_HELPER(fx.dwRotationAngle);
            JZ_HELPER(fx.dwZBufferOpCode);
            JZ_HELPER(fx.dwZBufferLow);
            JZ_HELPER(fx.dwZBufferHigh);
            JZ_HELPER(fx.dwZBufferBaseDest);
            JZ_HELPER(fx.dwZDestConstBitDepth);

    #       undef JZ_HELPER

            return ret;
        }

        static string ToString(const DDSURFACEDESC& desc)
        {
    #       define JZ_HELPER(name) ret += "    " #name " = " + StringUtility::ToString(name) + "\n"
    #       define JZ_CAPS_HELPER(name) if ((desc.ddsCaps.dwCaps & name) != 0) { ret += #name " | "; }
    #       define JZ_FLAGS_HELPER(name) if ((desc.dwFlags & name) != 0) { ret += #name " | "; }
    #       define JZ_PF_FLAGS_HELPER(name) if ((desc.ddpfPixelFormat.dwFlags & name) != 0) { ret += #name " | "; }

            string ret = "DDSURFACEDESC {\n";

            JZ_HELPER(desc.dwSize);
            ret += "    desc.dwFlags = ";
            JZ_FLAGS_HELPER(DDSD_CAPS);
            JZ_FLAGS_HELPER(DDSD_HEIGHT);
            JZ_FLAGS_HELPER(DDSD_WIDTH);
            JZ_FLAGS_HELPER(DDSD_PITCH);
            JZ_FLAGS_HELPER(DDSD_BACKBUFFERCOUNT);
            JZ_FLAGS_HELPER(DDSD_ZBUFFERBITDEPTH);
            JZ_FLAGS_HELPER(DDSD_ALPHABITDEPTH);
            JZ_FLAGS_HELPER(DDSD_LPSURFACE);
            JZ_FLAGS_HELPER(DDSD_PIXELFORMAT);
            JZ_FLAGS_HELPER(DDSD_CKDESTOVERLAY);
            JZ_FLAGS_HELPER(DDSD_CKDESTBLT);
            JZ_FLAGS_HELPER(DDSD_CKSRCOVERLAY);
            JZ_FLAGS_HELPER(DDSD_CKSRCBLT);
            JZ_FLAGS_HELPER(DDSD_MIPMAPCOUNT);
            JZ_FLAGS_HELPER(DDSD_REFRESHRATE);
            JZ_FLAGS_HELPER(DDSD_LINEARSIZE);
            JZ_FLAGS_HELPER(DDSD_TEXTURESTAGE);
            JZ_FLAGS_HELPER(DDSD_FVF);
            JZ_FLAGS_HELPER(DDSD_SRCVBHANDLE);
            JZ_FLAGS_HELPER(DDSD_DEPTH);
            JZ_FLAGS_HELPER(DDSD_ALL);
            ret += "\n";

            JZ_HELPER(desc.dwHeight);
            JZ_HELPER(desc.dwWidth);
            JZ_HELPER(desc.lPitch);
            JZ_HELPER(desc.dwBackBufferCount);
            JZ_HELPER(desc.dwRefreshRate);
            JZ_HELPER(desc.dwAlphaBitDepth);
            JZ_HELPER(desc.dwReserved);
            JZ_HELPER(desc.lpSurface);
            JZ_HELPER(desc.ddckCKDestOverlay.dwColorSpaceLowValue);
            JZ_HELPER(desc.ddckCKDestOverlay.dwColorSpaceHighValue);
            JZ_HELPER(desc.ddckCKDestBlt.dwColorSpaceLowValue);
            JZ_HELPER(desc.ddckCKDestBlt.dwColorSpaceHighValue);
            JZ_HELPER(desc.ddckCKSrcOverlay.dwColorSpaceLowValue);
            JZ_HELPER(desc.ddckCKSrcOverlay.dwColorSpaceHighValue);
            JZ_HELPER(desc.ddckCKSrcBlt.dwColorSpaceLowValue);
            JZ_HELPER(desc.ddckCKSrcBlt.dwColorSpaceHighValue);
            JZ_HELPER(desc.ddpfPixelFormat.dwSize);

            ret += "    desc.ddpfPixelFormat.dwFlags = ";
            JZ_PF_FLAGS_HELPER(DDPF_ALPHA);
            JZ_PF_FLAGS_HELPER(DDPF_ALPHAPIXELS);
            JZ_PF_FLAGS_HELPER(DDPF_ALPHAPREMULT);
            JZ_PF_FLAGS_HELPER(DDPF_FOURCC);
            JZ_PF_FLAGS_HELPER(DDPF_PALETTEINDEXED4);
            JZ_PF_FLAGS_HELPER(DDPF_PALETTEINDEXEDTO8);
            JZ_PF_FLAGS_HELPER(DDPF_PALETTEINDEXED8);
            JZ_PF_FLAGS_HELPER(DDPF_RGB);
            JZ_PF_FLAGS_HELPER(DDPF_COMPRESSED);
            JZ_PF_FLAGS_HELPER(DDPF_RGBTOYUV);
            JZ_PF_FLAGS_HELPER(DDPF_YUV);
            JZ_PF_FLAGS_HELPER(DDPF_ZBUFFER);
            JZ_PF_FLAGS_HELPER(DDPF_PALETTEINDEXED1);
            JZ_PF_FLAGS_HELPER(DDPF_PALETTEINDEXED2);
            JZ_PF_FLAGS_HELPER(DDPF_ZPIXELS);
            JZ_PF_FLAGS_HELPER(DDPF_STENCILBUFFER);
            JZ_PF_FLAGS_HELPER(DDPF_ALPHAPREMULT);
            JZ_PF_FLAGS_HELPER(DDPF_LUMINANCE);
            JZ_PF_FLAGS_HELPER(DDPF_BUMPLUMINANCE);
            JZ_PF_FLAGS_HELPER(DDPF_BUMPDUDV);
            ret += "\n";

            JZ_HELPER(desc.ddpfPixelFormat.dwFourCC);
            JZ_HELPER(desc.ddpfPixelFormat.dwRGBBitCount);
            JZ_HELPER(desc.ddpfPixelFormat.dwRBitMask);
            JZ_HELPER(desc.ddpfPixelFormat.dwGBitMask);
            JZ_HELPER(desc.ddpfPixelFormat.dwBBitMask);
            JZ_HELPER(desc.ddpfPixelFormat.dwRGBAlphaBitMask);

            ret += "    desc.ddsCaps.dwCaps = ";
            JZ_CAPS_HELPER(DDSCAPS_ALPHA);
            JZ_CAPS_HELPER(DDSCAPS_BACKBUFFER );
            JZ_CAPS_HELPER(DDSCAPS_COMPLEX);
            JZ_CAPS_HELPER(DDSCAPS_FLIP);
            JZ_CAPS_HELPER(DDSCAPS_FRONTBUFFER);
            JZ_CAPS_HELPER(DDSCAPS_OFFSCREENPLAIN);
            JZ_CAPS_HELPER(DDSCAPS_OVERLAY);
            JZ_CAPS_HELPER(DDSCAPS_PALETTE);
            JZ_CAPS_HELPER(DDSCAPS_PRIMARYSURFACE);
            JZ_CAPS_HELPER(DDSCAPS_SYSTEMMEMORY);
            JZ_CAPS_HELPER(DDSCAPS_TEXTURE);
            JZ_CAPS_HELPER(DDSCAPS_3DDEVICE);
            JZ_CAPS_HELPER(DDSCAPS_VIDEOMEMORY);
            JZ_CAPS_HELPER(DDSCAPS_VISIBLE);
            JZ_CAPS_HELPER(DDSCAPS_WRITEONLY);
            JZ_CAPS_HELPER(DDSCAPS_ZBUFFER);
            JZ_CAPS_HELPER(DDSCAPS_OWNDC);
            JZ_CAPS_HELPER(DDSCAPS_LIVEVIDEO);
            JZ_CAPS_HELPER(DDSCAPS_HWCODEC);
            JZ_CAPS_HELPER(DDSCAPS_MODEX);
            JZ_CAPS_HELPER(DDSCAPS_MIPMAP);
            JZ_CAPS_HELPER(DDSCAPS_ALLOCONLOAD);
            JZ_CAPS_HELPER(DDSCAPS_VIDEOPORT);
            JZ_CAPS_HELPER(DDSCAPS_LOCALVIDMEM);
            JZ_CAPS_HELPER(DDSCAPS_NONLOCALVIDMEM);
            JZ_CAPS_HELPER(DDSCAPS_STANDARDVGAMODE);
            JZ_CAPS_HELPER(DDSCAPS_OPTIMIZED);
            ret += "\n";

            return ret;

    #       undef JZ_PF_FLAGS_HELPER
    #       undef JZ_FLAGS_HELPER
    #       undef JZ_CAPS_HELPER
    #       undef JZ_HELPER
        }

    }
}

#endif
