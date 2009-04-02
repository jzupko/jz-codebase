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
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/RenderOps.h>
#include <jz_graphics/VertexDeclaration.h>

namespace jz
{
    namespace graphics
    {

        namespace RenderOperations
        {

    #       define DEF_ROP(name) void name(RenderNode* apNode, voidc_p apInstance)

            DEF_ROP(SetEffect)
            {
                Effect* p = const_cast<Effect*>(static_cast<const Effect*>(apInstance));

                Graphics::GetSingleton().SetActiveEffect(p);
                apNode->RenderChildren();
                Graphics::GetSingleton().SetActiveEffect(null);
            }

            DEF_ROP(SetEffectTechnique)
            {
                Graphics& graphics = Graphics::GetSingleton();
                Effect* p = graphics.GetActiveEffect();
                p->SetTechnique(Technique::FromVoidP(apInstance));

                Pass pass = p->Begin();
                if (pass.IsValid())
                {
                    while (pass.IsValid())
                    {
                        if (pass.Begin())
                        {
                            graphics.SetActivePass(&pass);
                            apNode->RenderChildren();
                            graphics.SetActivePass(null);
                            pass.End();
                        }
                        pass = pass.Next();
                    }
                    p->End();
                }
            }

            DEF_ROP(SetMaterial)
            {
                const Material* p = static_cast<const Material*>(apInstance);
                p->Apply();

                apNode->RenderChildren();
            }

            DEF_ROP(SetMesh)
            {
                Mesh* p = const_cast<Mesh*>(static_cast<const Mesh*>(apInstance));

                p->SetIndices();
                p->SetVertices();

                Graphics::GetSingleton().SetActiveMesh(p);
                apNode->RenderChildren();
                Graphics::GetSingleton().SetActiveMesh(null);
            }

            DEF_ROP(SetVertexDeclaration)
            {
                const VertexDeclaration* p = static_cast<const VertexDeclaration*>(apInstance);
                p->SetToDevice();

                apNode->RenderChildren();
            }

    #       undef DEF_ROP
        }

    }
}
