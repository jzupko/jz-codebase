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
#ifndef _JZ_GRAPHICS_MESH_H_
#define _JZ_GRAPHICS_MESH_H_

#include <jz_core/BoundingBox.h>
#include <jz_core/BoundingSphere.h>
#include <jz_graphics/IObject.h>
#include <jz_graphics/PrimitiveType.h>
#include <string>

namespace jz
{
    namespace graphics
    {

        class Graphics;
        class VertexDeclaration; typedef AutoPtr<VertexDeclaration> VertexDeclarationPtr;
        class Mesh : public IObject
        {
        public:
            static const int SortOrder = 1;

            const BoundingBox& GetAABB() const { return mAABB; }
            const BoundingSphere& GetBoundingSphere() const { return mBoundingSphere; }
            VertexDeclaration* GetVertexDeclaration() const;

            void SetIndices() const;
            void SetVertices() const;

            void CalculateBoundings(const MemoryBuffer<Vector4>& aSkinning, BoundingBox& arAABB, BoundingSphere& arSphere) const;

            void Draw() const;
            void Draw(natural aPrimitiveCount) const;

        protected:
            JZ_EXPORT virtual ~Mesh();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;

        private:
            friend class Graphics;
            friend void jz::__IncrementRefCount<graphics::Mesh>(graphics::Mesh* p);
            friend void jz::__DecrementRefCount<graphics::Mesh>(graphics::Mesh* p);

            Mesh(const string& aFilename);

            Mesh(const Mesh&);
            Mesh& operator=(const Mesh&);

            BoundingBox mAABB;
            BoundingSphere mBoundingSphere;
            Handle mIndexBuffer;
            Handle mVertexBuffer;
            natural mPrimitiveCount;
            PrimitiveType::Enum mPrimitiveType;
            natural mVertexCount;
            natural mVertexStride;
            VertexDeclarationPtr mpVertexDeclaration;

            void _Clear();
        };

        typedef AutoPtr<Mesh> MeshPtr;

    }
}

#endif
