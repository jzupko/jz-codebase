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

#include <jz_engine_2D/DrawEffect.h>
#include <jz_engine_2D/DrawMan.h>
#include <jz_engine_2D/Light.h>
#include <jz_engine_2D/Sprite.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>
#include <jz_system/WriteHelpers.h>

namespace jz
{
    namespace engine_2D
    {

        bool SpritePrepare(graphics::BufferEntry& b, voidc_p apInstance)
        {
            if (b.CurrentOffset < 3u) { return false; }

            const SpriteDrawHelper& helper = *static_cast<SpriteDrawHelper const*>(apInstance);

            const size_t kIndex = (b.CurrentOffset - 3u);
            b.InstanceBuffer[kIndex+0u] = helper.R0;
            b.InstanceBuffer[kIndex+1u] = helper.R1;
            b.InstanceBuffer[kIndex+2u] = helper.R2;
            b.CurrentOffset -= 3u;

            return true;
        }

        void LightDraw(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
            Light const* pLight = static_cast<Light const*>(apInstance);

            DrawEffect* pEffect = (DrawEffect*)graphics.GetActiveEffect();
            Vector3 lv = (pLight->GetType() == Light::kDirectional) ? pLight->GetDirection() : Vector3(pLight->GetPosition());
            
            pEffect->SetLightColor(pLight->GetColor());
            pEffect->SetLightV(lv);

            bool bNeedsClear = false;
            if (pLight->bShadow())
            {
                const vector<Vector3>& vertices = pLight->GetShadowVertices();
                if (vertices.size() > 0u)
                {
                    const size_t aPrimitiveCount = (vertices.size() / 6u);
                    const size_t kVertexSizeInBytes = (sizeof(Vector3) * 2u);

                    DrawMan::GetSingleton().GetMesh()->GetVertexDeclaration()->SetToDevice();
                    pEffect->SetTechnique(pEffect->GetShadowVolumeTechnique());
                    graphics::Pass pass = pEffect->Begin();
                    if (pass.IsValid())
                    {
                        while (pass.IsValid())
                        {
                            if (pass.Begin())
                            {
                                graphics.DrawPrimitives(graphics::PrimitiveType::kTriangleList, (voidc_p)(&(vertices[0])), aPrimitiveCount, kVertexSizeInBytes);
                                pass.End();
                            }
                            pass = pass.Next();
                        }
                        pEffect->End();
                        bNeedsClear = true;
                    }
                }
            }

            apNode->RenderChildren();

            if (bNeedsClear)
            {
                graphics.Clear(graphics::Graphics::kStencil);
            }
        }

        void SpriteDraw(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            DrawMan& dm = DrawMan::GetSingleton();
            const graphics::BufferEntry& buffer = dm.GetInstanceBuffers()[reinterpret_cast<u32>(apInstance)];

            DrawEffect* pEffect = (DrawEffect*)graphics::Graphics::GetSingleton().GetActiveEffect();
            graphics::Mesh* pMesh = graphics::Graphics::GetSingleton().GetActiveMesh();

            pEffect->SetInstanceData(buffer.InstanceBuffer, buffer.CurrentOffset, (DrawMan::kMaxEntries - buffer.CurrentOffset));
            graphics::Graphics::GetSingleton().GetActivePass()->Commit();
            pMesh->Draw(((DrawMan::kMaxEntries - buffer.CurrentOffset) / 3u) * 2u);

            apNode->RenderChildren();
        }

        Sprite::Sprite()
            : IDrawable(IDrawable::kSprite),
              mFlags(Sprite::kNone),
              mAlpha(1),
              mScale(1, 1),
              mTranslation(0, 0),
              mXYDepthFactors(0, 0),
              mTextureScaleShift(1.0f, 1.0f, 0.0f, 0.0f),
              mPack(graphics::RenderPack::Create()),
              mpShadowEdges(null)
        {
            DrawMan& dm = DrawMan::GetSingleton();
            mPack.InstancingDrawFunc = SpriteDraw;
            mPack.InstancingPrepareFunc = SpritePrepare;
            mPack.InstancingPrepareParam = this;
            mPack.pEffect = dm.GetEffect();
            mPack.pMesh = dm.GetMesh();
            mPack.pVertexDeclaration = dm.GetMesh()->GetVertexDeclaration();
            mPack.Sort = 0.0f;
        }

        Sprite::~Sprite()
        {}

        void Sprite::Pose(IDrawable* apDrawable)
        {
            Light* pLight = dynamic_cast<Light*>(apDrawable);
            DrawMan& dm = DrawMan::GetSingleton();
            SpriteDrawHelper helper;
            helper.R0 = Vector4(mScale, mTranslation);
            helper.R1 = mTextureScaleShift;
            helper.R2 = Vector4(mPack.Sort, mAlpha, mXYDepthFactors.X, mXYDepthFactors.Y);
            mPack.InstancingPrepareParam = &helper;

            if (pLight)
            {
                if (mAlpha < 1.0f)
                {
                    mPack.EffectTechnique = (pLight->GetType() == Light::kPoint) ? dm.GetEffect()->GetTransparentPointLightTechnique() : dm.GetEffect()->GetTransparentDirectionalLightTechnique();
                    mPack.Flags = graphics::RenderPack::kTransparent;
                }
                else
                {
                    mPack.EffectTechnique = (pLight->GetType() == Light::kPoint) ? dm.GetEffect()->GetOpaquePointLightTechnique() : dm.GetEffect()->GetOpaqueDirectionalLightTechnique();
                    mPack.Flags = graphics::RenderPack::kOpaque;
                }

                mPack.PostEffectFunc = &LightDraw;
                mPack.PostEffectFuncParam = apDrawable;
            }
            else
            {
                if (mAlpha < 1.0f)
                {
                    mPack.EffectTechnique = dm.GetEffect()->GetBaseTransparentTechnique();
                    mPack.Flags = (graphics::RenderPack::kBase | graphics::RenderPack::kTransparent);
                }
                else
                {
                    mPack.EffectTechnique = dm.GetEffect()->GetBaseOpaqueTechnique();
                    mPack.Flags = (graphics::RenderPack::kBase | graphics::RenderPack::kOpaque);
                }

                mPack.PostEffectFunc = null;
                mPack.PostEffectFuncParam = null;
            }

            DrawMan::GetSingleton().Pose(mPack);
        }

        void Sprite::Serialize(system::IWriteFilePtr& p)
        {
            JZ_ASSERT(false);
        }

        void Sprite::Deserialize(system::IReadFilePtr& p)
        {
            JZ_ASSERT(false);
        }

        float Sprite::GetZdepth() const
        {
            return (mPack.Sort);
        }

        void Sprite::SetZdepth(float v)
        {
            (mPack.Sort = v);
        }

        graphics::Material* Sprite::GetMaterial() const
        {
            return mpMaterial.Get();
        }

        void Sprite::SetMaterial(graphics::Material* p)
        {
            if (mpMaterial.IsValid()) { mpMaterial->AttachTo(null); }
            mpMaterial.Reset(p);
            if (mpMaterial.IsValid()) { mpMaterial->AttachTo(DrawMan::GetSingleton().GetEffect()); }

            mPack.pMaterial = mpMaterial.Get();
        }

    }
}
