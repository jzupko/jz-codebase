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

#include <jz_graphics_directx9/DxEffectStateManager.h>

namespace jz
{
    namespace graphics
    {

        __inline DWORD Convert(float v)
        {
            return *((DWORD*)&v);
        }

        #pragma region Defaul states
        static const DWORD kDefaultRenderStates[DxEffectStateManager::kMaxRenderStates] =
        {
            0u, // EMPTY = 0
            0u, // EMPTY = 1
            0u, // EMPTY = 2
            0u, // EMPTY = 3
            0u, // EMPTY = 4
            0u, // EMPTY = 5
            0u, // EMPTY = 6
            D3DZB_TRUE, // D3DRS_ZENABLE = 7
            D3DFILL_SOLID, // D3DRS_FILLMODE = 8
            D3DSHADE_GOURAUD, // D3DRS_SHADEMODE = 9
            0u, // EMPTY = 10
            0u, // EMPTY = 11
            0u, // EMPTY = 12
            0u, // EMPTY = 13
            TRUE, // D3DRS_ZWRITEENABLE = 14
            FALSE, // D3DRS_ALPHATESTENABLE = 15
            TRUE, // D3DRS_LASTPIXEL = 16
            0u, // EMPTY = 17
            0u, // EMPTY = 18
            D3DBLEND_ONE, // D3DRS_SRCBLEND = 19
            D3DBLEND_ZERO, // D3DRS_DESTBLEND = 20
            0u, // EMPTY = 21
            D3DCULL_CCW, // D3DRS_CULLMODE = 22
            D3DCMP_LESSEQUAL, // D3DRS_ZFUNC = 23
            0u, // D3DRS_ALPHAREF = 24
            D3DCMP_ALWAYS, // D3DRS_ALPHAFUNC = 25
            FALSE, // D3DRS_DITHERENABLE = 26
            FALSE, // D3DRS_ALPHABLENDENABLE = 27
            FALSE, // D3DRS_FOGENABLE = 28
            FALSE, // D3DRS_SPECULARENABLE = 29
            0u, // EMPTY = 30
            0u, // EMPTY = 31
            0u, // EMPTY = 32
            0u, // EMPTY = 33
            0u, // D3DRS_FOGCOLOR = 34
            D3DFOG_NONE, // D3DRS_FOGTABLEMODE = 35
            Convert(0.0f), // D3DRS_FOGSTART = 36
            Convert(1.0f), // D3DRS_FOGEND = 37
            Convert(1.0f), // D3DRS_FOGDENSITY = 38
            0u, // EMPTY = 39
            0u, // EMPTY = 40
            0u, // EMPTY = 41
            0u, // EMPTY = 42
            0u, // EMPTY = 43
            0u, // EMPTY = 44
            0u, // EMPTY = 45
            0u, // EMPTY = 46
            0u, // EMPTY = 47
            FALSE, // D3DRS_RANGEFOGENABLE = 48
            0u, // EMPTY = 49
            0u, // EMPTY = 50
            0u, // EMPTY = 51
            FALSE, // D3DRS_STENCILENABLE = 52
            D3DSTENCILOP_KEEP, // D3DRS_STENCILFAIL = 53
            D3DSTENCILOP_KEEP, // D3DRS_STENCILZFAIL = 54
            D3DSTENCILOP_KEEP, // D3DRS_STENCILPASS = 55
            D3DCMP_ALWAYS, // D3DRS_STENCILFUNC = 56
            0, // D3DRS_STENCILREF = 57
            0xFFFFFFFF, // D3DRS_STENCILMASK = 58
            0xFFFFFFFF, // D3DRS_STENCILWRITEMASK = 59
            0xFFFFFFFF, // D3DRS_TEXTUREFACTOR = 60
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // EMPTY = 61-70
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // EMPTY = 71-80
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // EMPTY = 81-90
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // EMPTY = 91-100
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // EMPTY = 101-110
            0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // EMPTY = 111-120
            0u, 0u, 0u, 0u, 0u, 0u, 0u, // EMPTY = 121-127
            0u, // D3DRS_WRAP0 = 128
            0u, // D3DRS_WRAP1 = 129
            0u, // D3DRS_WRAP2 = 130
            0u, // D3DRS_WRAP3 = 131
            0u, // D3DRS_WRAP4 = 132
            0u, // D3DRS_WRAP5 = 133
            0u, // D3DRS_WRAP6 = 134
            0u, // D3DRS_WRAP7 = 135
            TRUE, // D3DRS_CLIPPING = 136
            TRUE, // D3DRS_LIGHTING = 137
            0u, // EMPTY = 138
            0, // D3DRS_AMBIENT = 139
            D3DFOG_NONE, // D3DRS_FOGVERTEXMODE = 140
            TRUE, // D3DRS_COLORVERTEX = 141
            TRUE, // D3DRS_LOCALVIEWER = 142
            FALSE, // D3DRS_NORMALIZENORMALS = 143
            0u, // EMPTY = 144
            D3DMCS_COLOR1, // D3DRS_DIFFUSEMATERIALSOURCE = 145
            D3DMCS_COLOR2, // D3DRS_SPECULARMATERIALSOURCE = 146
            D3DMCS_MATERIAL, // D3DRS_AMBIENTMATERIALSOURCE = 147
            D3DMCS_MATERIAL, // D3DRS_EMISSIVEMATERIALSOURCE = 148
            0u, // EMPTY = 149
            0u, // EMPTY = 150
            D3DVBF_DISABLE, // D3DRS_VERTEXBLEND = 151
            0, // D3DRS_CLIPPLANEENABLE = 152
            0u, // EMPTY = 153
            0, // D3DRS_POINTSIZE = 154
            Convert(1.0f), // D3DRS_POINTSIZE_MIN = 155
            FALSE, // D3DRS_POINTSPRITEENABLE = 156
            FALSE, // D3DRS_POINTSCALEENABLE = 157
            Convert(1.0f), // D3DRS_POINTSCALE_A = 158
            Convert(0.0f), // D3DRS_POINTSCALE_B = 159
            Convert(0.0f), // D3DRS_POINTSCALE_C = 160
            TRUE, // D3DRS_MULTISAMPLEANTIALIAS = 161
            0xFFFFFFFF, // D3DRS_MULTISAMPLEMASK = 162
            D3DPATCHEDGE_DISCRETE, // D3DRS_PATCHEDGESTYLE = 163
            0u, // EMPTY = 164
            D3DDMT_ENABLE, // D3DRS_DEBUGMONITORTOKEN = 165
            Convert(64.0f), // D3DRS_POINTSIZE_MAX = 166 
            FALSE, // D3DRS_INDEXEDVERTEXBLENDENABLE = 167
            0x0000000F, // D3DRS_COLORWRITEENABLE = 168
            0u, // EMPTY = 169
            Convert(0.0f), // D3DRS_TWEENFACTOR = 170
            D3DBLENDOP_ADD, // D3DRS_BLENDOP = 171
            D3DDEGREE_CUBIC, // D3DRS_POSITIONDEGREE = 172
            D3DDEGREE_LINEAR, // D3DRS_NORMALDEGREE = 173
            FALSE, // D3DRS_SCISSORTESTENABLE = 174
            0, // D3DRS_SLOPESCALEDEPTHBIAS = 175
            FALSE, // D3DRS_ANTIALIASEDLINEENABLE = 176
            0u, // EMPTY = 177
            Convert(1.0f), // D3DRS_MINTESSELLATIONLEVEL = 178
            Convert(1.0f), // D3DRS_MAXTESSELLATIONLEVEL = 179
            Convert(0.0f), // D3DRS_ADAPTIVETESS_X = 180
            Convert(0.0f), // D3DRS_ADAPTIVETESS_Y = 181
            Convert(1.0f), // D3DRS_ADAPTIVETESS_Z = 182
            Convert(0.0f), // D3DRS_ADAPTIVETESS_W = 183
            FALSE, // D3DRS_ENABLEADAPTIVETESSELLATION = 184
            FALSE, // D3DRS_TWOSIDEDSTENCILMODE = 185
            D3DSTENCILOP_KEEP, // D3DRS_CCW_STENCILFAIL = 186
            D3DSTENCILOP_KEEP, // D3DRS_CCW_STENCILZFAIL = 187
            D3DSTENCILOP_KEEP, // D3DRS_CCW_STENCILPASS = 188
            D3DCMP_ALWAYS, // D3DRS_CCW_STENCILFUNC = 189
            0x0000000F, // D3DRS_COLORWRITEENABLE1 = 190
            0x0000000F, // D3DRS_COLORWRITEENABLE2 = 191
            0x0000000F, // D3DRS_COLORWRITEENABLE3 = 192
            0xFFFFFFFF, // D3DRS_BLENDFACTOR = 193
            0, // D3DRS_SRGBWRITEENABLE = 194
            0, // D3DRS_DEPTHBIAS = 195
            0u, // EMPTY = 196
            0u, // EMPTY = 197
            0u, // D3DRS_WRAP8 = 198
            0u, // D3DRS_WRAP9 = 199
            0u, // D3DRS_WRAP10 = 200
            0u, // D3DRS_WRAP11 = 201
            0u, // D3DRS_WRAP12 = 202
            0u, // D3DRS_WRAP13 = 203
            0u, // D3DRS_WRAP14 = 204
            0u, // D3DRS_WRAP15 = 205
            FALSE, // D3DRS_SEPARATEALPHABLENDENABLE  = 206
            D3DBLEND_ONE, // D3DRS_SRCBLENDALPHA = 207
            D3DBLEND_ZERO, // D3DRS_DESTBLENDALPHA = 208
            D3DBLENDOP_ADD, // D3DRS_BLENDOPALPHA = 209
        };

        static const DWORD kDefaultSamplerStates[DxEffectStateManager::kMaxSamplerTypes] =
        {
            0u, // EMPTY
            D3DTADDRESS_WRAP, // D3DSAMP_ADDRESSU = 1
            D3DTADDRESS_WRAP, // D3DSAMP_ADDRESSV = 2
            D3DTADDRESS_WRAP, // D3DSAMP_ADDRESSW = 3
            0x00000000, // D3DSAMP_BORDERCOLOR = 4
            D3DTEXF_POINT, // D3DSAMP_MAGFILTER = 5
            D3DTEXF_POINT, // D3DSAMP_MINFILTER = 6
            D3DTEXF_NONE, // D3DSAMP_MIPFILTER = 7
            0, // D3DSAMP_MIPMAPLODBIAS = 8
            0, // D3DSAMP_MAXMIPLEVEL = 9
            1, // D3DSAMP_MAXANISOTROPY = 10
            0, // D3DSAMP_SRGBTEXTURE = 11
            0, // D3DSAMP_ELEMENTINDEX = 12
            0x100, // D3DSAMP_DMAPOFFSET = 13
        };
        #pragma endregion

    #   define STDMETHODDEF(method)        COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE method
    #   define STDMETHODDEF_(type,method)  COM_DECLSPEC_NOTHROW type STDMETHODCALLTYPE method

        DxEffectStateManager::DxEffectStateManager(
            DWORD aInitialRenderState[kMaxRenderStates],
            bool aValidRsMask[kMaxRenderStates])
            : mReferenceCount(1u)
        {
            memcpy(mRenderState, aInitialRenderState, (kMaxRenderStates * sizeof(DWORD)));
            memcpy(mValidRsMask, aValidRsMask, (kMaxRenderStates * sizeof(bool)));

            memset(mbTxDirty, 0, kMaxTextures);

            for (int i = 0; i < kMaxSamplers; i++)
            {
                mSamplerState[i][D3DSAMP_ADDRESSU] = D3DTADDRESS_WRAP;
                mSamplerState[i][D3DSAMP_ADDRESSV] = D3DTADDRESS_WRAP;
                mSamplerState[i][D3DSAMP_ADDRESSW] = D3DTADDRESS_WRAP;
                mSamplerState[i][D3DSAMP_BORDERCOLOR] = 0;
                mSamplerState[i][D3DSAMP_MAGFILTER] = D3DTEXF_POINT;
                mSamplerState[i][D3DSAMP_MINFILTER] = D3DTEXF_POINT;
                mSamplerState[i][D3DSAMP_MIPFILTER] = D3DTEXF_NONE;
                mSamplerState[i][D3DSAMP_MIPMAPLODBIAS] = 0;
                mSamplerState[i][D3DSAMP_MAXMIPLEVEL] = 0;
                mSamplerState[i][D3DSAMP_MAXANISOTROPY] = 1;
                mSamplerState[i][D3DSAMP_SRGBTEXTURE] = 0;
                mSamplerState[i][D3DSAMP_ELEMENTINDEX] = 0;
                mSamplerState[i][D3DSAMP_DMAPOFFSET] = 0x100;
            }
        }

        DxEffectStateManager::~DxEffectStateManager()
        {}

        void DxEffectStateManager::EndPass()
        {
            for (vector<DWORD>::reverse_iterator I = mTextureSets.rbegin(); I != mTextureSets.rend(); I++)
            {
                gpD3dDevice9->SetTexture(*I, null);
                mbTxDirty[*I] = false;
            }
            mTextureSets.clear();
        }

        void DxEffectStateManager::Reset()
        {
            for (int i = kMinState; i <= kMaxState; i++)
            {
                if (mValidRsMask[i] && mRenderState[i] != kDefaultRenderStates[i])
                {
                    gpD3dDevice9->SetRenderState((D3DRENDERSTATETYPE)i, mRenderState[i]);
                }
            }

            for (int i = 0; i < kMaxSamplers; i++)
            {
                for (int j = kMinSamplerType; j <= kMaxSamplerType; j++)
                {
                    if (mSamplerState[i][j] != kDefaultSamplerStates[j])
                    {
                        gpD3dDevice9->SetSamplerState(i, (D3DSAMPLERSTATETYPE)j, mSamplerState[i][j]);
                    }
                }
            }
        }

        STDMETHODDEF(DxEffectStateManager::QueryInterface)(THIS_ REFIID iid, LPVOID *ppv)
        {
            return E_NOINTERFACE;
        }

        STDMETHODDEF_(ULONG, DxEffectStateManager::AddRef)(THIS)
        {
            mReferenceCount++;
            
            return mReferenceCount;
        }

        STDMETHODDEF_(ULONG, DxEffectStateManager::Release)(THIS)
        {
            mReferenceCount--;

            ulong ret = mReferenceCount;

            if (mReferenceCount == 0u) { delete this; }

            return ret;
        }

        STDMETHODDEF(DxEffectStateManager::SetTransform)(THIS_
            D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX *pMatrix)
        {
            return (gpD3dDevice9->SetTransform(State, pMatrix));
        }

        STDMETHODDEF(DxEffectStateManager::SetMaterial)(THIS_
            CONST D3DMATERIAL9 *pMaterial)
        {
            return (gpD3dDevice9->SetMaterial(pMaterial));
        }

        STDMETHODDEF(DxEffectStateManager::SetLight)(THIS_
            DWORD Index, CONST D3DLIGHT9 *pLight)
        {
            return (gpD3dDevice9->SetLight(Index, pLight));
        }

        STDMETHODDEF(DxEffectStateManager::LightEnable)(THIS_
            DWORD Index, BOOL Enable)
        {
            return (gpD3dDevice9->LightEnable(Index, Enable));
        }

        STDMETHODDEF(DxEffectStateManager::SetRenderState)(THIS_
            D3DRENDERSTATETYPE State, DWORD Value)
        {
            if (mValidRsMask[State] && mRenderState[State] != Value)
            {
                mRenderState[State] = Value;

                return (gpD3dDevice9->SetRenderState(State, Value));
            }
            else
            {
                return D3D_OK;
            }
        }

        STDMETHODDEF(DxEffectStateManager::SetTexture)(THIS_ DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture)
        {
            if (pTexture != null)
            {
                if (!mbTxDirty[Stage])
                {
                    mTextureSets.push_back(Stage);
                    mbTxDirty[Stage] = true;
                }

                return (gpD3dDevice9->SetTexture(Stage, pTexture));
            }
            else
            {
                return D3D_OK;
            }
        }

        STDMETHODDEF(DxEffectStateManager::SetTextureStageState)(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
        {
            return (gpD3dDevice9->SetTextureStageState(Stage, Type, Value));
        }

        STDMETHODDEF(DxEffectStateManager::SetSamplerState)(THIS_ DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value)
        {
            if (mSamplerState[Sampler][Type] != Value)
            {
                mSamplerState[Sampler][Type] = Value;

                return (gpD3dDevice9->SetSamplerState(Sampler, Type, Value));
            }
            else
            {
                return D3D_OK;
            }
        }

        STDMETHODDEF(DxEffectStateManager::SetNPatchMode)(THIS_ FLOAT NumSegments)
        {
            return (gpD3dDevice9->SetNPatchMode(NumSegments));
        }

        STDMETHODDEF(DxEffectStateManager::SetFVF)(THIS_ DWORD FVF)
        {
            return (gpD3dDevice9->SetFVF(FVF));
        }

        STDMETHODDEF(DxEffectStateManager::SetVertexShader)(THIS_ LPDIRECT3DVERTEXSHADER9 pShader)
        {
            return (gpD3dDevice9->SetVertexShader(pShader));
        }

        STDMETHODDEF(DxEffectStateManager::SetVertexShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
        {
            return (gpD3dDevice9->SetVertexShaderConstantF(RegisterIndex, pConstantData, RegisterCount));
        }

        STDMETHODDEF(DxEffectStateManager::SetVertexShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
        {
            return (gpD3dDevice9->SetVertexShaderConstantI(RegisterIndex, pConstantData, RegisterCount));
        }

        STDMETHODDEF(DxEffectStateManager::SetVertexShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
        {
            return (gpD3dDevice9->SetVertexShaderConstantB(RegisterIndex, pConstantData, RegisterCount));
        }

        STDMETHODDEF(DxEffectStateManager::SetPixelShader)(THIS_ LPDIRECT3DPIXELSHADER9 pShader)
        {
            return (gpD3dDevice9->SetPixelShader(pShader));
        }

        STDMETHODDEF(DxEffectStateManager::SetPixelShaderConstantF)(THIS_ UINT RegisterIndex, CONST FLOAT *pConstantData, UINT RegisterCount)
        {
            return (gpD3dDevice9->SetPixelShaderConstantF(RegisterIndex, pConstantData, RegisterCount));
        }

        STDMETHODDEF(DxEffectStateManager::SetPixelShaderConstantI)(THIS_ UINT RegisterIndex, CONST INT *pConstantData, UINT RegisterCount)
        {
            return (gpD3dDevice9->SetPixelShaderConstantI(RegisterIndex, pConstantData, RegisterCount));
        }

        STDMETHODDEF(DxEffectStateManager::SetPixelShaderConstantB)(THIS_ UINT RegisterIndex, CONST BOOL *pConstantData, UINT RegisterCount)
        {
            return (gpD3dDevice9->SetPixelShaderConstantB(RegisterIndex, pConstantData, RegisterCount));
        }

    }
}
