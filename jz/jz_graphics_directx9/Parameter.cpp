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
#include <jz_graphics/Parameter.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/Texture.h>
#include <jz_graphics/VolatileTexture.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        void __SetParameterValue(Handle e, Handle p, bool v)
        {
            if (e)
            {
                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetBool(param, v));
            }
        }

        void __SetParameterValue(Handle e, Handle p, float v)
        {
            if (e)
            {
                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetFloat(param, v));
            }
        }

        void __SetParameterValue(Handle e, Handle p, voidc_p v, size_t aSizeInBytes)
        {
            if (e)
            {
                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetValue(param, v, aSizeInBytes));
            }
        }

        void __SetParameterValue(Handle e, Handle p, float const* v, size_t aNumberOfSingles)
        {
            if (e)
            {
                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetFloatArray(param, v, aNumberOfSingles));
            }
        }

        void __SetParameterValue(Handle e, Handle p, const Matrix4& v)
        {
            if (e)
            {
                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetMatrixTranspose(param, (D3DXMATRIX const *)v.pData));
            }
        }

        void __SetParameterValue(Handle e, Handle p, Matrix4 const* v, size_t aNumberOfMatrix4)
        {
            if (e)
            {
                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetMatrixTransposeArray(param, (D3DXMATRIX const *)v, aNumberOfMatrix4));
            }
        }

        void __SetParameterValue(Handle e, Handle p, Vector4 const* v, size_t aNumberOfVector4)
        {
            if (e)
            {
                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetVectorArray(param, (D3DXVECTOR4 const *)v, aNumberOfVector4));
            }
        }

        void __SetParameterValue(Handle e, Handle p, Target* v)
        {
            if (e)
            {
                IDirect3DTexture9* texture = null;
                if (v)
                {
                    if (v->IsReset()) { texture = StaticCast<IDirect3DTexture9*>(v->mTexture); }
                    else { texture = StaticCast<IDirect3DTexture9*>(Graphics::GetSingleton().GetDefaultTexture()->mHandle); }
                }

                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetTexture(param, texture));
            }
        }

        void __SetParameterValue(Handle e, Handle p, Texture* v)
        {
            if (e)
            {
                IDirect3DTexture9* texture = null;
                if (v)
                {
                    if (v->IsReset()) { texture = StaticCast<IDirect3DTexture9*>(v->mHandle); }
                    else { texture = StaticCast<IDirect3DTexture9*>(Graphics::GetSingleton().GetDefaultTexture()->mHandle); }
                }

                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetTexture(param, texture));
            }
        }

        void __SetParameterValue(Handle e, Handle p, VolatileTexture* v)
        {
            if (e)
            {
                IDirect3DTexture9* texture = null;
                if (v)
                {
                    if (v->IsReset()) { texture = StaticCast<IDirect3DTexture9*>(v->mHandle); }
                    else { texture = StaticCast<IDirect3DTexture9*>(Graphics::GetSingleton().GetDefaultTexture()->mHandle); }
                }

                D3DXHANDLE param = StaticCast<D3DXHANDLE>(p);
                ID3DXEffect* effect = StaticCast<ID3DXEffect*>(e);

                JZ_DEBUG_DX_FAIL(effect->SetTexture(param, texture));
            }
        }

    }
}
