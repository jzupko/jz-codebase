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
#include <jz_system/ReadHelpers.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_graphics_opengl/OpenGL.h>

namespace jz
{
    namespace graphics
    {

        void Mesh::CalculateBoundings(const MemoryBuffer<Vector4>& aSkinning, BoundingBox& arAABB, BoundingSphere& arSphere) const
        {
            // TODO: implement.
            arAABB = mAABB;
            arSphere = mBoundingSphere;  
        }

        GLenum Convert(PrimitiveType::Enum v)
        {
            switch (v)
            {
                case PrimitiveType::kPointList: return GL_POINTS;
                case PrimitiveType::kLineList: return GL_LINES;
                case PrimitiveType::kLineStrip: return GL_LINE_STRIP;
                case PrimitiveType::kTriangleList: return GL_TRIANGLES;
                case PrimitiveType::kTriangleStrip: return GL_TRIANGLE_STRIP;
                case PrimitiveType::kTriangleFan: return GL_TRIANGLE_FAN;
                default:
                    JZ_ASSERT(false);
                    return GL_TRIANGLES;
            }
        }

        uint GetIndexCount(PrimitiveType::Enum v, unatural aPrimitiveCount)
        {
            switch (v)
            {
                case PrimitiveType::kPointList: return (aPrimitiveCount);
                case PrimitiveType::kLineList: return (aPrimitiveCount * 2u);
                case PrimitiveType::kLineStrip: return (aPrimitiveCount + 1u);
                case PrimitiveType::kTriangleList: return (aPrimitiveCount * 3u);
                case PrimitiveType::kTriangleStrip: return (aPrimitiveCount + 2u);
                case PrimitiveType::kTriangleFan: return (aPrimitiveCount + 2u);
                default:
                    JZ_ASSERT(false);
                    return 0u;
            }
        }

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
            string filename = ReadString(pFile);
            VertexDeclarationPtr pRet = Graphics::GetSingleton().Create<VertexDeclaration>(filename);

            return pRet;
        }

        VertexDeclaration* Mesh::GetVertexDeclaration() const
        {
            return (mpVertexDeclaration.Get());
        }

        void Mesh::SetIndices() const
        {
            OPENGL_ASSERT();

            if (GlIdIsValid(mIndexBuffer))
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
                glEnableClientState(GL_INDEX_ARRAY);
            }
        }

        void Mesh::SetVertices() const
        {
            OPENGL_ASSERT();

            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        }

        void Mesh::Draw() const
        {
            OPENGL_ASSERT();

            if (GlIdIsValid(mIndexBuffer))
            {
                glDrawElements(Convert(mPrimitiveType), GetIndexCount(mPrimitiveType, mPrimitiveCount), GL_UNSIGNED_SHORT, 0);
            }
            else
            {
                // TODO: draw vertices with no index buffer.
            }
        }

        void Mesh::Draw(natural aPrimitiveCount) const
        {
            OPENGL_ASSERT();

            if (GlIdIsValid(mIndexBuffer))
            {
                glDrawElements(Convert(mPrimitiveType), GetIndexCount(mPrimitiveType, aPrimitiveCount), GL_UNSIGNED_SHORT, 0); 
            }
            else
            {
                // TODO: draw vertices with no index buffer.
            }
        }

        IObject::State Mesh::_Load()
        {
            OPENGL_ASSERT();

            MemoryBuffer<u16> indices;
            ByteBuffer vertices;

            try
            {
                system::IReadFilePtr pFile = system::Files::GetSingleton().Open(GetFilename().c_str());

                mAABB = system::ReadBoundingBox(pFile);
                mBoundingSphere = system::ReadBoundingSphere(pFile);
                mPrimitiveCount = system::ReadInt32(pFile);
                mPrimitiveType = (PrimitiveType::Enum)ReadInt32(pFile);
                mVertexCount = ReadInt32(pFile);
                mpVertexDeclaration = ReadVertexDeclaration(pFile);
                mVertexStride = ReadInt32(pFile);

                system::ReadBuffer(pFile, indices);
                system::ReadBuffer(pFile, vertices);

                if (vertices.size() == 0u) { throw exception(__FUNCTION__ ": empty vertex buffer."); }
            }
            catch (exception& e)
            {
                mVertexStride = 0;
                mpVertexDeclaration.Reset();
                mVertexCount = 0;
                mPrimitiveType = (PrimitiveType::Enum)0;
                mPrimitiveCount = 0;
                mBoundingSphere = BoundingSphere::kZero;
                mAABB = BoundingBox::kZero;

                LogMessage(e.what(), Logger::kError);
                return (kErrorDataRead);
            }

            if (indices.size() > 0u)
            {
                uint indexBuffer = 0u;
                glGenBuffersARB(1, &indexBuffer);
                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, indices.GetSizeInBytes(), indices, GL_STATIC_DRAW);
                mIndexBuffer = (indexBuffer);
            }

            {
                uint vertexBuffer = 0u;
                glGenBuffersARB(1, &vertexBuffer);
                glBindBufferARB(GL_ARRAY_BUFFER, vertexBuffer);
                glBufferDataARB(GL_ARRAY_BUFFER, vertices.GetSizeInBytes(), vertices, GL_STATIC_DRAW);
                mVertexBuffer = (vertexBuffer);
            }

            return (kLost);
        }

        IObject::State Mesh::_Unload()
        {
            OPENGL_ASSERT();

            mVertexStride = 0;
            mpVertexDeclaration.Reset();
            mVertexCount = 0;
            mPrimitiveType = (PrimitiveType::Enum)0;
            mPrimitiveCount = 0;
            mBoundingSphere = BoundingSphere::kZero;
            mAABB = BoundingBox::kZero;

            uint vertexBuffer = (mVertexBuffer.CastUInt());
            glDeleteBuffersARB(1, &vertexBuffer);
            mVertexBuffer.Reset();

            uint indexBuffer = (mIndexBuffer.CastUInt());
            if (GlIdIsValid(indexBuffer))
            {
                glDeleteBuffersARB(1, &indexBuffer);
                mIndexBuffer.Reset();
            }

            return (kUnloaded);
        }

    }
}
