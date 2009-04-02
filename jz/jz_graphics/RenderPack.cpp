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

#include <jz_graphics/Effect.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/RenderPack.h>
#include <jz_graphics/VertexDeclaration.h>

namespace jz
{
    namespace graphics
    {

        RenderPack RenderPack::Create()
        {
            RenderPack ret;
            ret.Flags = kOpaque;
            ret.InstancingPrepareFunc = null;
            ret.InstancingPrepareParam = null;
            ret.InstancingDrawFunc = null;
            ret.DrawFunc = null;
            ret.DrawFuncParam = null;
            ret.Sort = 0.0f;
            ret.PreEffectFunc = null;
            ret.PreEffectFuncParam = null;
            ret.PostEffectFunc = null;
            ret.PostEffectFuncParam = null;

            return ret;
        }

    }
}
