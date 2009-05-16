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

#include <jz_core/Logger.h>
#include <jz_core/Memory.h>
#include <jz_system/Files.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Effect.h>
#include <jz_graphics/Pass.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/Texture.h>
#include <jz_graphics/VolatileTexture.h>
#include <jz_graphics_directx9/DxEffectStateManager.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        __inline uint Pack(uint n, uint count)
        {
            uint ret = (((count << 16) & 0xFFFF0000) | (0x0000FFFF & n));

            return ret;
        }

        __inline void Unpack(uint v, uint& n, uint& count)
        {
            n = (v & 0x0000FFFF);
            count = ((v >> 16) & 0x0000FFFF);
        }

        extern ID3DXEffectPool* gpEffectPool;
        extern DxEffectStateManager* gpEffectStateManager;

        static const char* kCompiledDirectXExtension = "cfx";

        Effect::Effect(const string& aFilename)
            : IObject(aFilename)
        { }
        
        Effect::~Effect()
        {}

        Pass Effect::Begin() const
        {
            uint passes = 0u;
            if (mHandle && SUCCEEDED(StaticCast<ID3DXEffect*>(mHandle)->Begin(&passes, D3DXFX_DONOTSAVESTATE)))
            {
                Pass ret;
                ret.mEffect = mHandle;
                ret.mHandle = Pack(0u, passes);
                return ret;
            }
            else
            {
                return (Pass());
            }
        }

        void Effect::End() const
        {
            JZ_DEBUG_DX_FAIL(StaticCast<ID3DXEffect*>(mHandle)->End());
        }

        void Effect::SetTechnique(Technique aTechnique)
        {
            mActiveTechnique = aTechnique;

            D3DXHANDLE t = (D3DXHANDLE)aTechnique.mHandle;
            ID3DXEffect* e = StaticCast<ID3DXEffect*>(mHandle);
            
            if (e)
            {
                JZ_DEBUG_DX_FAIL(e->SetTechnique(t));
            }
        }

        IObject::State Effect::_Load()
        {
            using namespace system;
            JZ_ASSERT(!mHandle);

            IReadFilePtr pFile;
            try
            {
                pFile = Files::GetSingleton().Open(GetFilename().c_str());
            }
            catch (exception&)
            {
                return (kErrorFileNotFound);
            }
            const size_t kSize = pFile->GetSize();

            MemoryBuffer<u8> buf(kSize);
            if (pFile->Read(buf.Get(), kSize) != kSize) { return (kErrorDataRead); }

            ID3DXEffect* p;
            HRESULT hr = D3DXCreateEffect(
                gpD3dDevice9,
                buf.Get(),
                buf.GetSizeInBytes(),
                null, null, 0u,
                gpEffectPool,
                &p, null);
            if (!SUCCEEDED(hr)) { return (kErrorGraphics); }

            p->SetStateManager(gpEffectStateManager);

            mHandle = p;

            return (kLost);
        }

        IObject::State Effect::_Unload()
        {
            SafeRelease<ID3DXEffect>(mHandle);

            return (kUnloaded);
        }

        IObject::State Effect::_Lost()
        {
            if (mHandle)
            {
                JZ_DEBUG_DX_FAIL(StaticCast<ID3DXEffect*>(mHandle)->OnLostDevice());
            }

            return (kLost);
        }

        IObject::State Effect::_Reset(natural aWidth, natural aHeight)
        {
            if (mHandle)
            {
                JZ_DEBUG_DX_FAIL(StaticCast<ID3DXEffect*>(mHandle)->OnResetDevice());
            }

            return (kReset);
        }

        Handle Effect::_GetParameterAnnotationByName(voidc_p apObject, const string& aName) const
        {
            ID3DXEffect* e = StaticCast<ID3DXEffect*>(mHandle);

            if (e)
            {
                D3DXHANDLE o = (D3DXHANDLE)apObject;

                return (e->GetAnnotationByName(o, aName.c_str()));
            }

            return (Handle());
        }

        Handle Effect::_GetTechniqueAnnotationByName(voidc_p apObject, const string& aName) const
        {
            ID3DXEffect* e = StaticCast<ID3DXEffect*>(mHandle);

            if (e)
            {
                D3DXHANDLE o = (D3DXHANDLE)apObject;

                return (e->GetAnnotationByName(o, aName.c_str()));
            }

            return (Handle());
        }

        void Effect::_GetParameterBySemantic(const string& s, Handle& arEffect, Handle& arHandle) const
        {
            arEffect = mHandle;
            arHandle = Handle();

            if (mHandle)
            {
                ID3DXEffect* p = StaticCast<ID3DXEffect*>(mHandle);
                arHandle = p->GetParameterBySemantic(null, s.c_str());
            }
        }

        Handle Effect::_GetTechniqueByName(const string& aName) const
        {
            ID3DXEffect* e = StaticCast<ID3DXEffect*>(mHandle);

            if (e)
            {
                return (e->GetTechniqueByName(aName.c_str()));
            }

            return (Handle());
        }

    }
}
