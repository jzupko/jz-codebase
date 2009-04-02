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
#ifndef _JZ_GRAPHICS_VOLATILE_MESH_H_
#define _JZ_GRAPHICS_VOLATILE_MESH_H_

#include <jz_graphics/IObject.h>
#include <functional>
#include <string>

namespace jz
{
    namespace graphics
    {

        class Graphics;
        class VertexDeclaration; typedef AutoPtr<VertexDeclaration> VertexDeclarationPtr;
        class VolatileMesh : public IVolatileObject
        {
        public:
            typedef tr1::function<void(size_t aSizeInBytes, void_p apData)> PopulateFunction;

            size_t GetIndexBufferSizeInBytes() const { return mIndexBufferSizeInBytes; }
            size_t GetVertexBufferSizeInBytes() const { return mVertexBufferSizeInBytes; }

            natural GetPrimitiveCount() const { return mPrimitiveCount; }
            void SetPrimitiveCount(natural v) { mPrimitiveCount = v; }

            PrimitiveType::Enum GetPrimitiveType() const { return mPrimitiveType; }
            void SetPrimitiveType(PrimitiveType::Enum v) { mPrimitiveType = v; }

            void SetSizes(size_t aIndexBufferSizeInBytes, size_t aVertexBufferSizeInBytes);

            natural GetVertexCount() const { return (mVertexCount); }
            void SetVertexCount(natural v) { mVertexCount = v; }

            natural GetVertexStride() const { return (mVertexStride); }
            void SetVertexStride(natural v) { mVertexStride = v; }

            void SetVertexDeclaration(VertexDeclaration* p);
            VertexDeclaration* GetVertexDeclaration() const;

            void SetIndices() const;
            void SetVertices() const;

            void Draw() const;
            void Draw(natural aPrimitiveCount) const;

            static const int SortOrder = 0;

        protected:
            JZ_EXPORT virtual ~VolatileMesh();
            JZ_EXPORT virtual void _Create() override;
            JZ_EXPORT virtual void _Destroy() override;
            JZ_EXPORT virtual void _Lost() override;
            JZ_EXPORT virtual void _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class Graphics;
            friend void jz::__IncrementRefCount<graphics::VolatileMesh>(graphics::VolatileMesh* p);
            friend void jz::__DecrementRefCount<graphics::VolatileMesh>(graphics::VolatileMesh* p);

            VolatileMesh(size_t aIndexBufferSizeInBytes, size_t aVertexBufferSizeInBytes, 
                PopulateFunction aIndexBufferPopulate,
                PopulateFunction aVertexBufferPopulate);
            VolatileMesh(const VolatileMesh&);
            VolatileMesh& operator=(const VolatileMesh&);

            natural mPrimitiveCount;
            PrimitiveType::Enum mPrimitiveType;
            natural mVertexCount;
            natural mVertexStride;
            VertexDeclarationPtr mpVertexDeclaration;

            size_t mIndexBufferSizeInBytes;
            size_t mVertexBufferSizeInBytes;
            PopulateFunction mPopulateIndices;
            PopulateFunction mPopulateVertices;
            Handle mpIndices;
            Handle mpVertices;
        };

        typedef AutoPtr<VolatileMesh> VolatileMeshPtr;

    }
}

#endif
