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

#include <jz_core/Matrix3.h>
#include <jz_core/Logger.h>
#include <jz_engine_2D/Cell.h>
#include <jz_engine_2D/DrawMan.h>
#include <jz_engine_2D/DrawEffect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/RenderOps.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_system/Files.h>

// Temp:
#include <iostream>
// ENd temp:

namespace jz
{
    static const char* kDefaultEffect = "engine_2D_draw.cfx";
    static const char* kUnitQuad = "built-in_instancing-quad.mesh";

    engine_2D::DrawMan* engine_2D::DrawMan::mspSingleton = null;

    namespace engine_2D
    {

        static const graphics::Target::Format kHdrFormat = graphics::Target::kA16B16G16R16F;

        DrawMan::DrawMan()
            : mCurrentBuffer(0u),
            mpInstanceBuffer(new vector<graphics::BufferEntry>())
        {
            if (!graphics::Graphics::GetSingletonExists())
            {
                SafeDelete(mpInstanceBuffer);
                throw std::exception(__FUNCTION__ ": Graphics must be instantiated before DrawMan");
            }

            mpInstanceBuffer->push_back(graphics::BufferEntry(kMaxEntries, kMaxEntries));

            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
            mpEffect = graphics.Create<DrawEffect>(kDefaultEffect);
            if (!mpEffect->IsLoadable())
            {
                mpEffect.Reset();
                SafeDelete(mpInstanceBuffer);
                throw std::exception("engine 2D draw effect cannot be loaded.");
            }

            mpMesh = graphics.Create<graphics::Mesh>(kUnitQuad);
            if (!mpMesh->IsLoadable())
            {
                mpMesh.Reset();
                mpEffect.Reset();
                SafeDelete(mpInstanceBuffer);
                throw std::exception("engine 2D quad mesh cannot be loaded.");
            }

            mpHdrTarget = graphics.Create<graphics::Target>(0u, 0u, kHdrFormat);
        }

        DrawMan::~DrawMan()
        {
            SafeDelete(mpInstanceBuffer);
        }

        DrawEffect* DrawMan::GetEffect() const
        {
            return mpEffect.Get();
        }

        graphics::Mesh* DrawMan::GetMesh() const
        {
            return mpMesh.Get();
        }

        void DrawMan::Draw()
        {
            using namespace graphics;
            Graphics& graphics = Graphics::GetSingleton();

            if (graphics.Begin(ColorRGBA::kBlack, false))
            {
                mpHdrTarget->SetToDevice(0u);
                graphics.Clear(Graphics::kColor | Graphics::kDepth | Graphics::kStencil);

                mRenderOpaque.RenderChildren();
                mRenderTransparent.RenderChildren();

                Target::ResetTarget(0u);

                #pragma region Hdr to Ldr pass
                {
                    MemoryBuffer<Vector4>& ins = (*mpInstanceBuffer)[0].InstanceBuffer;
                    ins[0] = Vector4(1, 1, 0, 0);
                    ins[1] = Vector4(1, 1, 0, 0);
                    ins[2] = Vector4(0, 1, 0, 0);

                    mpEffect->SetDiffuseTexture(mpHdrTarget.Get());
                    mpEffect->SetInstanceData(ins, 0u, 3u);
                    mpEffect->SetTechnique(mpEffect->GetHdrToLdrTechnique());

                    mpMesh->GetVertexDeclaration()->SetToDevice();
                    mpMesh->SetIndices();
                    mpMesh->SetVertices();
                    
                    Pass pass = mpEffect->Begin();
                    if (pass.IsValid())
                    {
                        while (pass.IsValid())
                        {
                            if (pass.Begin())
                            {
                                mpMesh->Draw(2u);
                                pass.End();
                            }
                            pass = pass.Next();
                        }
                        mpEffect->End();
                    }
                }
                #pragma endregion

                graphics.End();
            }
            _ResetTrees();

            mCurrentBuffer = 0u;
            size_t size = mpInstanceBuffer->size();
            for (size_t i = 0u; i < size; i++)
            {
                (*mpInstanceBuffer)[i].CurrentOffset = kMaxEntries;
            }
        }

        void DrawMan::_InsertInstancingOp(graphics::RenderNode* p, graphics::InstancingOp iOp, voidc_p apInstanceOpParam, graphics::DrawOp dOp, graphics::AdoptOp aOp, float aSortOrder)
        {
            using namespace graphics;
            #pragma region If a child with space exists, add this instance to that child
            RenderNode* node = p->GetHead();
            bool bIncrement = false;
            while (node)
            {
                u32 index = reinterpret_cast<u32>(node->GetInstance());
                BufferEntry& buffer = (*mpInstanceBuffer)[index];

                if (buffer.CurrentOffset > 0u)
                {
                    if (iOp(buffer, apInstanceOpParam))
                    {
                        // reinserting to update z-sorting order.
                        (p->*aOp)(dOp, reinterpret_cast<voidc_p>(index), aSortOrder);
                        return;
                    }
                }
                node = node->GetNext();
            }
            #pragma endregion

            #pragma region else
            // Either there is no existing draw op, or all the existing instancing buffers are
            // full. We need to insert a new draw op.
            {
                if ((*mpInstanceBuffer)[mCurrentBuffer].CurrentOffset < kMaxEntries) { mCurrentBuffer++; }

                if (mCurrentBuffer >= mpInstanceBuffer->size())
                {
                    mpInstanceBuffer->push_back(BufferEntry(kMaxEntries, kMaxEntries));
                }

                iOp((*mpInstanceBuffer)[mCurrentBuffer], apInstanceOpParam);
                (p->*aOp)(dOp, reinterpret_cast<voidc_p>(mCurrentBuffer), aSortOrder);
            }
            #pragma endregion
        }

        static const float kOffset = (float)(254.0 / (255.0 * 255.0));
        static const float kOffset2 = (kOffset + kOffset);

        static bool Sort(const graphics::RenderPack& r, float& arSort)
        {
            if ((r.Flags & graphics::RenderPack::kBase) != 0)
            {
                if ((r.Flags & graphics::RenderPack::kOpaque) != 0)
                {
                    arSort = (Clamp(r.Sort, 0.0f, 1.0f) - 1.0f - kOffset);
                    return true;
                }
                else if ((r.Flags & graphics::RenderPack::kTransparent) != 0)
                {
                    arSort = ((1.0f - Clamp(r.Sort, 0.0f, 1.0f)) + 1.0f + kOffset);
                    return true;
                }
            }
            else if ((r.Flags & graphics::RenderPack::kOpaque) != 0)
            {
                arSort = (Clamp(r.Sort, 0.0f, 1.0f));
                return true;
            }
            else if ((r.Flags & graphics::RenderPack::kTransparent) != 0)
            {
                arSort = ((1.0f - Clamp(r.Sort, 0.0f, 1.0f)) + 2.0f + kOffset2);
                return true;
            }

            return false;
        }

        void DrawMan::Pose(const graphics::RenderPack& r)
        {
            using namespace graphics;

            float sort;
            if (Sort(r, sort))
            {
                RenderNode* node = (r.Flags & RenderPack::kOpaque) ? &mRenderOpaque : &mRenderTransparent;

                if (r.PreEffectFunc) { node = node->AdoptAndUpdateSort(r.PreEffectFunc, r.PreEffectFuncParam, sort); }
                if (r.pEffect.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffect, r.pEffect.Get(), sort); }
                if (r.PostEffectFunc) { node = node->AdoptAndUpdateSort(r.PostEffectFunc, r.PostEffectFuncParam, sort); }
                if (r.EffectTechnique.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffectTechnique, graphics::Technique::ToVoidP(r.EffectTechnique), sort); }
                if (r.pVertexDeclaration.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetVertexDeclaration, r.pVertexDeclaration.Get(), sort); }
                if (r.pMaterial.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMaterial, r.pMaterial.Get(), sort); }
                if (r.pMesh.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMesh, r.pMesh.Get(), sort); }
                if (r.DrawFunc) { node = node->AdoptAndUpdateSort(r.DrawFunc, r.DrawFuncParam, sort); }
                if (r.InstancingPrepareFunc && r.InstancingDrawFunc)
                {
                    _InsertInstancingOp(node, r.InstancingPrepareFunc, r.InstancingPrepareParam, r.InstancingDrawFunc, &RenderNode::AdoptAndUpdateSort, sort);
                }
            }
        }

        void DrawMan::_ResetTrees()
        {
            mRenderOpaque.Reset();
            mRenderTransparent.Reset();
            graphics::RenderNode::ResetGlobal();
        }

    }
}
