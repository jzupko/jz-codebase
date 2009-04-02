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

#pragma once
#ifndef _JZ_GRAPHICS_DIRECTX9_DX_EFFECT_STATE_MANAGER_H_
#define _JZ_GRAPHICS_DIRECTX9_DX_EFFECT_STATE_MANAGER_H_

#include <jz_graphics_directx9/DirectX9.h>
#include <vector>

namespace jz
{
    namespace graphics
    {

        class Graphics;
        // note:
        // - all sampler, renderstate, texture set changes should go through this class
        // - behavior:
        //   - does not submit redundant state changes to the graphics api.
        //   - restores all set textures to null when endpass() is called (prevents lingering
        //     texture pointers to avoid problems, i.e., render targets)
        // design:
        // - two ways I can imagine to manage state:
        // - method 1
        //   - periodically (say, at the beginning of certain major passes, like "draw
        //     opaque objects") set a base normal state
        //   - after draw calls (or effect passes?) restore any state modifications
        //     to the normal state to restore the normal state - this is the same
        //     idea as function calls where the callee is reponsible for stack handling
        //   - in this scheme, effects (or anything setting state) should only set
        //     states that are different from the known normal state for any given
        //     portion of the rendering pipeline.
        // - method 2
        //   - don't restore state, just filter-out redundant state changes
        //   - effects (or anything setting state) need to explicitly set all states
        //     as they need them to be.
        // - I've chosen method 2:
        // pros:
        // - this design allows an effect to be written to explicitly include all the
        //   states necessary for it to work correctly, which is safer and more
        //   verbose than assuming certain states will be correct at certain stages in
        //   the rendering pipeline. If a decision is made to change a normal state for
        //   opaque objects for example, then all effects dependent on that normal state 
        //   may need to be changed.
        // - the render state can "settle" into common statas - i.e. if 99% of draw calls
        //   use linear filtering for min/mag then they will just remain set that way vs.
        //   unsetting/resetting with state blocks
        // cons:
        // - effect passes will probably include a lot of redundant render state calls.
        //   while these will be filtered out by the state manager, it still incurs
        //   cpu overhead in making all the actual calls.
        class DxEffectStateManager : public ID3DXEffectStateManager
        {
        public:
            static const int kMaxRenderStates = 256;
            static const int kMinState = 7;
            static const int kMaxState = 209;

            static const int kMaxSamplers = 16;
            static const int kMaxSamplerTypes = 14;
            static const int kMinSamplerType = 1;
            static const int kMaxSamplerType = 13;

            static const int kMaxTextures = 16;

            STDMETHOD(QueryInterface)(THIS_ REFIID iid, LPVOID *ppv);
            STDMETHOD_(ULONG, AddRef)(THIS);
            STDMETHOD_(ULONG, Release)(THIS);

            STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix);
            STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9 *pMaterial);
            STDMETHOD(SetLight)(THIS_ DWORD Index, CONST D3DLIGHT9 *pLight);
            STDMETHOD(LightEnable)(THIS_ DWORD Index, BOOL Enable);
            STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State, DWORD Value);
            STDMETHOD(SetTexture)(THIS_ DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture);
            STDMETHOD(SetTextureStageState)(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
            STDMETHOD(SetSamplerState)(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value);
            STDMETHOD(SetNPatchMode)(THIS_ FLOAT NumSegments);
            STDMETHOD(SetFVF)(THIS_ DWORD FVF);
            STDMETHOD(SetVertexShader)(THIS_ LPDIRECT3DVERTEXSHADER9 pShader);
            STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
            STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
            STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);
            STDMETHOD(SetPixelShader)(THIS_ LPDIRECT3DPIXELSHADER9 pShader);
            STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount);
            STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount);
            STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount);

            virtual ~DxEffectStateManager();
            void EndPass();
            void Reset();

        private:
            ulong mReferenceCount;

            friend class Graphics;

            vector<DWORD> mTextureSets;
            bool mbTxDirty[kMaxTextures];

            DWORD mRenderState[kMaxRenderStates];
            bool mValidRsMask[kMaxRenderStates];

            DWORD mSamplerState[kMaxSamplers][kMaxSamplerTypes];

            DxEffectStateManager(
                DWORD aInitialRenderState[kMaxRenderStates],
                bool aValidRsMask[kMaxRenderStates]);
            DxEffectStateManager(const DxEffectStateManager&);
            DxEffectStateManager& operator=(const DxEffectStateManager&);
        };

    }
}

#endif
