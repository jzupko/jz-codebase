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
#ifndef _JZ_GI_RADIOSITY_MAN_H_
#define _JZ_GI_RADIOSITY_MAN_H_

#include <jz_core/Color.h>
#include <jz_core/Memory.h>
#include <jz_core/Utility.h>
#include <jz_graphics/Effect.h>
#include <jz_system/TriangleTree.h>
#include <vector>

namespace jz
{
    namespace graphics
    {
        class VertexDeclaration; typedef AutoPtr<VertexDeclaration> VertexDeclarationPtr;
    }

    namespace gi
    {

        class RadiosityMan sealed : public Singleton<RadiosityMan>
        {
        public:
            JZ_ALIGNED_NEW

            RadiosityMan();
            ~RadiosityMan();

            const system::TriangleTree& GetTree() const { return mTree; }
            system::TriangleTree& GetTree()
            {
                mbDirty = true;
                return mTree;
            }

            void AddInitialRadiance(u16 aVertex, const ColorRGB& c) { mInitialRadiances.push_back(make_pair(aVertex, c)); }
            void ClearInitialRadiances() { mInitialRadiances.clear(); }

            void Draw() const;

        private:
            bool mbDirty;

            graphics::EffectPtr mpEffect;
            graphics::Technique mRenderTechnique;
            graphics::VertexDeclarationPtr mpVertexDeclaration;

            MemoryBuffer<ColorRGB> mRadiances;
            system::TriangleTree mTree;
            MemoryBuffer<Vector3> mDrawBuffer;

            typedef pair<u16, ColorRGB> Pair;
            typedef vector<Pair> Container;
             
            Container mInitialRadiances;

            void _Update();
            bool _Visible(u16 i, u16 j) const;
        };

    }
}

#endif
