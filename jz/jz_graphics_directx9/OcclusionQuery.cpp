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

#include <jz_graphics/OcclusionQuery.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        OcclusionQuery::OcclusionQuery()
            : mLastPixelCount(0u)
        {}

        OcclusionQuery::~OcclusionQuery()
        {}

        bool OcclusionQuery::Begin()
        {
            if (IsReset() && mpQuery)
            {
                IDirect3DQuery9* p = mpQuery.Cast<IDirect3DQuery9>();
                JZ_DEBUG_DX_FAIL(p->Issue(D3DISSUE_BEGIN));
                return true;
            }

            return false;
        }

        void OcclusionQuery::End()
        {
            JZ_ASSERT(IsReset() && mpQuery);

            IDirect3DQuery9* p = mpQuery.Cast<IDirect3DQuery9>();
            JZ_DEBUG_DX_FAIL(p->Issue(D3DISSUE_END));
        }

        bool OcclusionQuery::IsComplete() const
        {
            if (IsReset() && mpQuery)
            {
                IDirect3DQuery9* p = mpQuery.Cast<IDirect3DQuery9>();
                
                bool bReturn = (p->GetData((void_p)(&mLastPixelCount), sizeof(unatural), 0) == S_OK);
                return (bReturn);
            }

            return false;
        }

        unatural OcclusionQuery::GetPixelCount() const
        {
            return (mLastPixelCount);
        }

        void OcclusionQuery::_Create()
        {}

        void OcclusionQuery::_Destroy()
        {}

        void OcclusionQuery::_Lost()
        {
            SafeRelease<IDirect3DQuery9>(mpQuery);
        }
        
        void OcclusionQuery::_Reset(natural aWidth, natural aHeight)
        {
            IDirect3DQuery9* p = null;

            JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateQuery(D3DQUERYTYPE_OCCLUSION, &p));
            mpQuery = p;
        }

    }
}
