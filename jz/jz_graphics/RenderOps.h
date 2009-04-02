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
#ifndef _JZ_GRAPHICS_RENDEROPS_H_
#define _JZ_GRAPHICS_RENDEROPS_H_

#include <jz_graphics/RenderNode.h>

namespace jz
{
    namespace graphics
    {
        
        namespace RenderOperations
        {
    #       define DECL_ROP(name) void name(RenderNode* apNode, voidc_p apInstance)

            DECL_ROP(SetEffect);
            DECL_ROP(SetEffectTechnique);
            DECL_ROP(SetMaterial);
            DECL_ROP(SetMesh);
            DECL_ROP(SetVertexDeclaration);
            DECL_ROP(SetWit);
            DECL_ROP(SetWorldTransformAndDrawIndexed);

    #       undef DECL_ROP
        }

    }
}

#endif
