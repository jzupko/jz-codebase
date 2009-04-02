// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#include <jz_core/Logger.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <jz_ddraw/DDEffect.h>
#include <jz_ddraw/DirectDrawPalette.h>
#include <jz_graphics_directx9/DirectX9.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/Texture.h>

namespace jz
{
    namespace ddraw
    {

        DDEffect::DDEffect(const string& aFilename)
            : Effect(aFilename)
        {}
        
        DDEffect::~DDEffect()
        {}

        static void DoPasses(Handle e, const Rect& src, const Rect& dest)
        {
            using namespace graphics;

            Rect d = dest;
            d.BottomLeft *= 2.0f; d.BottomLeft -= Vector2::kOne;
            d.TopRight *= 2.0f; d.TopRight -= Vector2::kOne;
            
            // note the y flip to handle textures stored from the upper-left.
            float vertices[4 * 5] =
            {
                d.BottomLeft.X, d.BottomLeft.Y, 0, src.BottomLeft.X, src.TopRight.Y,
                d.BottomLeft.X, d.TopRight.Y, 0, src.BottomLeft.X,  src.BottomLeft.Y,
                d.TopRight.X, d.TopRight.Y, 0, src.TopRight.X, src.BottomLeft.Y,
                d.TopRight.X, d.BottomLeft.Y, 0, src.TopRight.X, src.TopRight.Y
            };

            ID3DXEffect* pEffect = e.Cast<ID3DXEffect>();

            gpD3dDevice9->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0));

            uint passes = 0u;
            if (SUCCEEDED(pEffect->Begin(&passes, D3DXFX_DONOTSAVESTATE)))
            {
                for (uint i = 0u; i < passes; i++)
                {
                    JZ_DEBUG_DX_FAIL(pEffect->BeginPass(i));
                    gpD3dDevice9->DrawPrimitiveUP(
                        D3DPT_TRIANGLEFAN,
                        2u,
                        vertices,
                        5 * sizeof(float));
                    JZ_DEBUG_DX_FAIL(pEffect->EndPass());
                }
                JZ_DEBUG_DX_FAIL(pEffect->End());
            }
        }

        void DDEffect::DrawWithPalette(Handle apTexture, const Rect& src, const Rect& dest)
        {
            if (IsReset())
            {
                D3DXHANDLE param = mpFbTexture.Cast<D3DXHANDLE>();
                ID3DXEffect* e = mHandle.Cast<ID3DXEffect>();
                IDirect3DTexture9* t = apTexture.Cast<IDirect3DTexture9>();
                
                if (e)
                {
                    JZ_DEBUG_DX_FAIL(e->SetTexture(param, t));
                    SetTechnique(mpPalettedTechnique);
                    DoPasses(mHandle, src, dest);
                }
            }
        }

        void DDEffect::DrawWithoutPalette(Handle apTexture, const Rect& src, const Rect& dest)
        {
            if (IsReset())
            {
                D3DXHANDLE param = mpFbTexture.Cast<D3DXHANDLE>();
                ID3DXEffect* e = mHandle.Cast<ID3DXEffect>();
                IDirect3DTexture9* t = apTexture.Cast<IDirect3DTexture9>();

                if (e)
                {
                    JZ_DEBUG_DX_FAIL(e->SetTexture(param, t));
                    SetTechnique(mpUnpalettedTechnique);
                    DoPasses(mHandle, src, dest);
                }
            }
        }

        void DDEffect::Set(DirectDrawPalette* p)
        {
            if (IsReset())
            {
                D3DXHANDLE param = mpPalette.Cast<D3DXHANDLE>();
                ID3DXEffect* e = mHandle.Cast<ID3DXEffect>();
                IDirect3DTexture9* t = (p) ? p->GetPalette().Cast<IDirect3DTexture9>() : null;

                if (e)
                {
                    JZ_DEBUG_DX_FAIL(e->SetTexture(param, t));
                }
            }
        }

        graphics::IObject::State DDEffect::_Load()
        {
            graphics::Effect::_Load();
            if (IsError()) { return (mInternalState); }

            try
            {
                mpPalette = _GetParameterBySemantic("jz_Palette");
                JZ_E_ON_FAIL(mpPalette, "jz_Palette not found.");

                mpFbTexture = _GetParameterBySemantic("jz_FbTexture");
                JZ_E_ON_FAIL(mpFbTexture, "jz_FbTexture not found.");

                mpPalettedTechnique = _GetTechniqueByName("jz_PalettedTechnique");
                JZ_E_ON_FAIL(mpPalettedTechnique, "jz_PalettedTechnique not found.");

                mpUnpalettedTechnique = _GetTechniqueByName("jz_UnpalettedTechnique");
                JZ_E_ON_FAIL(mpUnpalettedTechnique, "jz_UnpalettedTechnique");
            }
            catch (std::exception&)
            {
                _Clear();
                graphics::Effect::_Unload();
                return (kErrorDataRead);
            }

            return (kLost);
        }

        void DDEffect::_Clear()
        {
            mpUnpalettedTechnique = Handle();
            mpPalettedTechnique = Handle();
            mpFbTexture = Handle();
            mpPalette = Handle();
        }

        graphics::IObject::State DDEffect::_Unload()
        {
            _Clear();
            return graphics::Effect::_Unload();
        }

    }
}
