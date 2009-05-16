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

#include <jz_core/Memory.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        Mesh::Mesh(const string& aFilename)
            : IObject(aFilename),
            mAABB(BoundingBox::kZero),
            mBoundingSphere(BoundingSphere::kZero),
            mPrimitiveCount(0),
            mPrimitiveType((PrimitiveType::Enum)0),
            mVertexCount(0),
            mVertexStride(0)
        {}
        
        Mesh::~Mesh()
        {}

        static VertexDeclarationPtr ReadVertexDeclaration(system::IReadFilePtr& pFile)
        {
            string filename = system::ReadString(pFile);
            VertexDeclarationPtr pRet = Graphics::GetSingleton().Create<VertexDeclaration>(filename);

            return pRet;
        }

        VertexDeclaration* Mesh::GetVertexDeclaration() const
        {
            return (mpVertexDeclaration.Get());
        }

        void Mesh::SetIndices() const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetIndices(StaticCast<IDirect3DIndexBuffer9*>(mIndexBuffer)));
        }

        void Mesh::SetVertices() const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetStreamSource(0, StaticCast<IDirect3DVertexBuffer9*>(mVertexBuffer), 0, mVertexStride));
        }

        static Matrix4 _Get(natural index, const MemoryBuffer<Vector4>& aSkinning)
        {
            natural i = (index * 3);
            natural i0 = (i + 0);
            natural i1 = (i + 1);
            natural i2 = (i + 2);

            Matrix4 ret = Matrix4(
                aSkinning[i0].X, aSkinning[i1].X, aSkinning[i2].X, 0, 
                aSkinning[i0].Y, aSkinning[i1].Y, aSkinning[i2].Y, 0,
                aSkinning[i0].Z, aSkinning[i1].Z, aSkinning[i2].Z, 0,
                aSkinning[i0].W, aSkinning[i1].W, aSkinning[i2].W, 1);

            return ret;
        }

        static Vector3 _Calculate(const Vector3& pos, const Vector4& ind, const Vector4& weight, const MemoryBuffer<Vector4>& aSkinning)
        {
            Matrix4 m0 = _Get((natural)ind.X, aSkinning);
            Matrix4 m1 = _Get((natural)ind.Y, aSkinning);
            Matrix4 m2 = _Get((natural)ind.Z, aSkinning);
            Matrix4 m3 = _Get((natural)ind.W, aSkinning);

            Matrix4 m = (weight.X * m0) 
                      + (weight.Y * m1)
                      + (weight.Z * m2)
                      + (weight.W * m3);

            Vector3 ret = Vector3::TransformPosition(m, pos);

            return ret;
        }

        void Mesh::CalculateBoundings(const MemoryBuffer<Vector4>& aSkinning, BoundingBox& arAABB, BoundingSphere& arSphere) const
        {
            if (IsLoaded())
            {
                if (mpVertexDeclaration.IsValid() && mVertexBuffer)
                {
                    IDirect3DVertexDeclaration9* p = StaticCast<IDirect3DVertexDeclaration9*>(mpVertexDeclaration->mHandle);
                    
                    D3DVERTEXELEMENT9 elements[MAX_FVF_DECL_SIZE];
                    UINT elementCount = 0u;
                    if (SUCCEEDED(p->GetDeclaration(elements, &elementCount)))
                    {
                        natural boneIndex = -1;
                        natural boneWeight = -1;
                        natural position = -1;

                        for (UINT i = 0u; i < elementCount; i++)
                        {
                            if (elements[i].Usage == D3DDECLUSAGE_POSITION && elements[i].Type == D3DDECLTYPE_FLOAT3)
                            {
                                position = (natural)elements[i].Offset;
                            }
                            else if (elements[i].Usage == D3DDECLUSAGE_BLENDWEIGHT && elements[i].Type == D3DDECLTYPE_FLOAT4)
                            {
                                boneWeight = (natural)elements[i].Offset;
                            }
                            else if (elements[i].Usage == D3DDECLUSAGE_BLENDINDICES && elements[i].Type == D3DDECLTYPE_FLOAT4)
                            {
                                boneIndex = (natural)elements[i].Offset;
                            }
                        }

                        if (boneIndex >= 0 && boneWeight >= 0 && position >= 0)
                        {
                            void_p pLock;
                            MemoryBuffer<Vector3> positions(mVertexCount);
                            if (SUCCEEDED(StaticCast<IDirect3DVertexBuffer9*>(mVertexBuffer)->Lock(0u, (mVertexCount * mVertexStride), &pLock, 0u)))
                            {
                                u8c_p pBuf = (u8c_p)pLock;

                                for (natural i = 0; i < mVertexCount; i++)
                                {
                                    const natural kIndex = (i * mVertexStride);
                                    Vector3 pos;
                                    Vector4 ind;
                                    Vector4 wght;

                                    memcpy(&pos, (pBuf + kIndex + position), sizeof(Vector3));
                                    memcpy(&ind, (pBuf + kIndex + boneIndex), sizeof(Vector4));
                                    memcpy(&wght, (pBuf + kIndex + boneWeight), sizeof(Vector4));

                                    positions[i] = _Calculate(pos, ind, wght, aSkinning); 
                                }

                                JZ_DEBUG_DX_FAIL(StaticCast<IDirect3DVertexBuffer9*>(mVertexBuffer)->Unlock());

                                arAABB = BoundingBox::CreateFromPoints(positions.begin(), positions.end());
                                arSphere = BoundingSphere::CreateFromPoints(positions.begin(), positions.end());

                                return;
                            }
                        }
                    }
                }
            }

            arAABB = BoundingBox::kZero;
            arSphere = BoundingSphere::kZero;
        }

        void Mesh::Draw() const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->DrawIndexedPrimitive(Convert(mPrimitiveType), 0, 0, mVertexCount, 0, mPrimitiveCount));
        }

        void Mesh::Draw(natural aPrimitiveCount) const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->DrawIndexedPrimitive(Convert(mPrimitiveType), 0, 0, mVertexCount, 0, aPrimitiveCount));
        }

        IObject::State Mesh::_Load()
        {
            using namespace system;

            JZ_ASSERT(!mIndexBuffer);
            JZ_ASSERT(!mVertexBuffer);

            IReadFilePtr pFile;
            try
            {
                pFile = Files::GetSingleton().Open(GetFilename().c_str());
            }
            catch (std::exception&)
            {
                return (kErrorFileNotFound);
            }

            try
            {
                mAABB = ReadBoundingBox(pFile);
                mBoundingSphere = ReadBoundingSphere(pFile);
                mPrimitiveCount = ReadInt32(pFile);
                mPrimitiveType = (PrimitiveType::Enum)ReadInt32(pFile);
                mVertexCount = ReadInt32(pFile);
                mpVertexDeclaration = ReadVertexDeclaration(pFile);
                mVertexStride = ReadInt32(pFile);

                #pragma region Indices
                try
                {
                    const size_t kSize = ReadSizeT(pFile);
                    const size_t kSizeInBytes = (kSize * sizeof(u16));

                    if (kSizeInBytes > 0u)
                    {
                        IDirect3DIndexBuffer9* p;
                        JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateIndexBuffer(kSizeInBytes, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &p, null));
                        void_p pLock;
                        JZ_E_ON_DX_FAIL(p->Lock(0u, kSizeInBytes, &pLock, 0u));
                        pFile->Read(pLock, kSizeInBytes);
                        JZ_E_ON_DX_FAIL(p->Unlock());
                        mIndexBuffer = p;
                    }
                }
                catch (std::exception&)
                {
                    mpVertexDeclaration.Reset();
                    SafeRelease<IDirect3DIndexBuffer9>(mIndexBuffer);
                    throw;
                }
                #pragma endregion

                #pragma region Vertices
                try
                {
                    const size_t kSizeInBytes = ReadSizeT(pFile);

                    if (kSizeInBytes > 0u)
                    {
                        IDirect3DVertexBuffer9* p;
                        JZ_E_ON_DX_FAIL(gpD3dDevice9->CreateVertexBuffer(kSizeInBytes, 0, 0, D3DPOOL_MANAGED, &p, null));
                        void_p pLock;
                        JZ_E_ON_DX_FAIL(p->Lock(0u, kSizeInBytes, &pLock, 0u));
                        pFile->Read(pLock, kSizeInBytes);
                        JZ_E_ON_DX_FAIL(p->Unlock());
                        mVertexBuffer = p;
                    }
                }
                catch (std::exception&)
                {
                    mpVertexDeclaration.Reset();
                    SafeRelease<IDirect3DVertexBuffer9>(mVertexBuffer);
                    SafeRelease<IDirect3DIndexBuffer9>(mIndexBuffer);
                    throw;
                }
                #pragma endregion
            }
            catch (std::exception&)
            {
                _Clear();
                return (kErrorDataRead);
            }

            return (kLost);
        }

        void Mesh::_Clear()
        {
            mAABB = BoundingBox::kZero;
            mBoundingSphere = BoundingSphere::kZero;
            mPrimitiveCount = 0;
            mPrimitiveType = (PrimitiveType::Enum)0;
            mVertexCount = 0;
            mpVertexDeclaration.Reset();
            mVertexStride = 0;
        }

        IObject::State Mesh::_Unload()
        {
            _Clear();

            SafeRelease<IDirect3DIndexBuffer9>(mIndexBuffer);
            SafeRelease<IDirect3DVertexBuffer9>(mVertexBuffer);

            return (kUnloaded);
        }

    }
}
