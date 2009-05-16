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
#ifndef _JZ_GRAPHICS_RENDER_PACK_H_
#define _JZ_GRAPHICS_RENDER_PACK_H_

#include <jz_core/Auto.h>
#include <jz_core/Delegate.h>
#include <jz_core/Memory.h>
#include <jz_core/Vector4.h>
#include <jz_graphics/Technique.h>

namespace jz
{
    namespace graphics
    {

        class Effect; typedef AutoPtr<Effect> EffectPtr;
        class Material; typedef AutoPtr<Material> MaterialPtr;
        class Mesh; typedef AutoPtr<Mesh> MeshPtr;
        class VertexDeclaration; typedef AutoPtr<VertexDeclaration> VertexDeclarationPtr;
        class RenderNode;

        struct BufferEntry
        {
            BufferEntry(size_t aCurrentOffset, size_t aMemoryBufferSize)
                : CurrentOffset(aCurrentOffset), InstanceBuffer(aMemoryBufferSize)
            {}

            size_t CurrentOffset;
            MemoryBuffer<Vector4> InstanceBuffer;
        };

        typedef void (*DrawOp)(RenderNode* apNode, voidc_p apInstance);
        typedef bool (*InstancingOp)(BufferEntry& b, voidc_p apInstance);

        struct RenderPack
        {
            enum
            {
                kBase = (1 << 0),
                kOpaque = (1 << 1),
                kTransparent = (1 << 2),
                kShadow = (1 << 3),
                kReflection = (1 << 4),
                kGUI = (1 << 5),
                kNoStrictSort = (1 << 6)
            };

            unatural Flags;
            float Sort;

            EffectPtr pEffect;
            Technique EffectTechnique;
            VertexDeclarationPtr pVertexDeclaration;
            MaterialPtr pMaterial;
            MeshPtr pMesh;

            DrawOp PreEffectFunc;
            voidc_p PreEffectFuncParam;

            DrawOp PostEffectFunc;
            voidc_p PostEffectFuncParam;

            InstancingOp InstancingPrepareFunc;
            voidc_p InstancingPrepareParam;
            DrawOp InstancingDrawFunc;

            DrawOp DrawFunc;
            voidc_p DrawFuncParam;

            static RenderPack Create();
        };

    }
}

#endif
