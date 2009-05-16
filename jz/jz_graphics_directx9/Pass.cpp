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

#include <jz_graphics/Pass.h>
#include <jz_graphics_directx9/DirectX9.h>
#include <jz_graphics_directx9/DxEffectStateManager.h>

namespace jz
{
    namespace graphics
    {

        extern DxEffectStateManager* gpEffectStateManager;

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

        void Pass::Commit() const
        {
            if (mEffect)
            {
                ID3DXEffect* p = StaticCast<ID3DXEffect*>(mEffect);
                JZ_DEBUG_DX_FAIL(p->CommitChanges());
            }
        }

        bool Pass::Begin() const
        {
            uint params = StaticCast<uint>(mHandle);
            uint n;
            uint count;
            Unpack(params, n, count);

            if (n < count)
            {
                ID3DXEffect* p = StaticCast<ID3DXEffect*>(mEffect);
                if (SUCCEEDED(p->BeginPass(n)))
                {
                    return true;
                }
            }

            return false;
        }

        void Pass::End() const
        {
            ID3DXEffect* p = StaticCast<ID3DXEffect*>(mEffect);
            JZ_DEBUG_DX_FAIL(p->EndPass());
            gpEffectStateManager->EndPass();
        }

        Pass Pass::Next() const
        {
            uint params = StaticCast<uint>(mHandle);
            uint n;
            uint count;
            Unpack(params, n, count);
            n++;

            if (n < count)
            {
                Pass ret;
                ret.mEffect = mEffect;
                ret.mHandle = Pack(n, count);
                return ret;
            }
            else
            {
                return (Pass());
            }
        }

    }
}
