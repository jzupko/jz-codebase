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
#include <jz_graphics/VertexDeclaration.h>
#include <jz_graphics/VolatileMesh.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        VolatileMesh::VolatileMesh(size_t aIndexBufferSizeInBytes, size_t aVertexBufferSizeInBytes, 
                PopulateFunction aIndexBufferPopulate,
                PopulateFunction aVertexBufferPopulate)
            : mPrimitiveCount(0),
              mPrimitiveType((PrimitiveType::Enum)0),
              mVertexCount(0),
              mVertexStride(0),
              mpVertexDeclaration(null),
              mIndexBufferSizeInBytes(aIndexBufferSizeInBytes),
              mVertexBufferSizeInBytes(aVertexBufferSizeInBytes),
              mPopulateIndices(aIndexBufferPopulate),
              mPopulateVertices(aVertexBufferPopulate)
        {}

        VolatileMesh::~VolatileMesh()
        {}

        void VolatileMesh::SetVertexDeclaration(VertexDeclaration* p)
        {
            mpVertexDeclaration.Reset(p);
        }

        VertexDeclaration* VolatileMesh::GetVertexDeclaration() const
        {
            return (mpVertexDeclaration.Get());
        }

        void VolatileMesh::SetIndices() const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetIndices(StaticCast<IDirect3DIndexBuffer9*>(mpIndices)));
        }

        void VolatileMesh::SetVertices() const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetStreamSource(0, StaticCast<IDirect3DVertexBuffer9*>(mpVertices), 0, mVertexStride));
        }

        void VolatileMesh::Draw() const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->DrawIndexedPrimitive(Convert(mPrimitiveType), 0, 0, mVertexCount, 0, mPrimitiveCount));
        }

        void VolatileMesh::Draw(natural aPrimitiveCount) const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->DrawIndexedPrimitive(Convert(mPrimitiveType), 0, 0, mVertexCount, 0, aPrimitiveCount));
        }

        void VolatileMesh::SetSizes(size_t aIndexBufferSizeInBytes, size_t aVertexBufferSizeInBytes)
        {
            Lost();
            Destroy();

            mIndexBufferSizeInBytes = aIndexBufferSizeInBytes;
            mVertexBufferSizeInBytes = aVertexBufferSizeInBytes;

            if (!Graphics::GetSingleton().IsLost()) { Reset(Graphics::GetSingleton().GetViewportWidth(), Graphics::GetSingleton().GetViewportHeight()); }
            if (Graphics::GetSingleton().IsLoaded()) { Create(); }
        }

        void VolatileMesh::_Create()
        {
            IDirect3DIndexBuffer9* pIndex = null;
            IDirect3DVertexBuffer9* pVertex = null;

            if (mIndexBufferSizeInBytes > 0u)
            {
                JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateIndexBuffer(mIndexBufferSizeInBytes, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIndex, null));

                if (mPopulateIndices)
                {
                    void_p pLock;
                    JZ_E_ON_DX_FAIL(pIndex->Lock(0u, mIndexBufferSizeInBytes, &pLock, 0u));
                    try
                    {
                        mPopulateIndices(mIndexBufferSizeInBytes, pLock);
                    }
                    catch (std::exception&)
                    {
                        JZ_DEBUG_DX_FAIL(pIndex->Unlock());
                        SafeRelease(pIndex);
                        throw;
                    }
                    JZ_DEBUG_DX_FAIL(pIndex->Unlock());
                }
            }

            try
            {
                if (mVertexBufferSizeInBytes > 0u)
                {
                    JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateVertexBuffer(mVertexBufferSizeInBytes, 0, 0, D3DPOOL_MANAGED, &pVertex, null));

                    if (mPopulateVertices)
                    {
                        void_p pLock;
                        JZ_E_ON_DX_FAIL(pVertex->Lock(0u, mVertexBufferSizeInBytes, &pLock, 0u));
                        try
                        {
                            mPopulateVertices(mVertexBufferSizeInBytes, pLock);
                        }
                        catch (std::exception&)
                        {
                            JZ_DEBUG_DX_FAIL(pVertex->Unlock());
                            throw;
                        }
                        JZ_DEBUG_DX_FAIL(pVertex->Unlock());
                    }
                }
            }
            catch (std::exception&)
            {
                SafeRelease(pVertex);
                SafeRelease(pIndex);
                throw;
            }

            mpIndices = pIndex;
            mpVertices = pVertex;
        }

        void VolatileMesh::_Destroy()
        {
            SafeRelease<IDirect3DIndexBuffer9>(mpIndices);
            SafeRelease<IDirect3DVertexBuffer9>(mpVertices);
        }

        void VolatileMesh::_Lost()
        {}
        
        void VolatileMesh::_Reset(natural aWidth, natural aHeight)
        {}

    }
}
