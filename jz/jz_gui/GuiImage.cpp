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

#include <jz_engine_3D/RenderMan.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_gui/GuiImage.h>
#include <jz_gui/GuiImageEffect.h>

namespace jz
{
    namespace gui
    {

        struct GuiImageDrawHelper
        {
            Vector4 R0;
            Vector4 R1;
            Vector4 R2;
        };

        static bool GuiImagePrepare(graphics::BufferEntry& b, voidc_p apInstance)
        {
            if (b.CurrentOffset < 3u) { return false; }

            const GuiImageDrawHelper& helper = *static_cast<GuiImageDrawHelper const*>(apInstance);

            const size_t kIndex = (b.CurrentOffset - 3u);
            b.InstanceBuffer[kIndex+0u] = helper.R0;
            b.InstanceBuffer[kIndex+1u] = helper.R1;
            b.InstanceBuffer[kIndex+2u] = helper.R2;
            b.CurrentOffset -= 3u;

            return true;
        }

        static void GuiImageDraw(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            using namespace engine_3D;

            RenderMan& rm = RenderMan::GetSingleton();
            const graphics::BufferEntry& buffer = rm.GetInstanceBuffers()[reinterpret_cast<u32>(apInstance)];

            GuiImageEffect* pEffect = (GuiImageEffect*)graphics::Graphics::GetSingleton().GetActiveEffect();
            graphics::Mesh* pMesh = graphics::Graphics::GetSingleton().GetActiveMesh();

            pEffect->SetInstanceData(buffer.InstanceBuffer, buffer.CurrentOffset, (RenderMan::kMaxEntries - buffer.CurrentOffset));
            graphics::Graphics::GetSingleton().GetActivePass()->Commit();
            pMesh->Draw(((RenderMan::kMaxEntries - buffer.CurrentOffset) / 3u) * 2u);

            apNode->RenderChildren();
        }

        GuiImage::GuiImage()
            : IGuiElement(),
              mbTransparent(false),
              mScale(1, 1),
              mTranslation(0, 0),
              mTextureScaleShift(1, 1, 0, 0),
              mPack(graphics::RenderPack::Create())
        {
            engine_3D::RenderMan& rm = engine_3D::RenderMan::GetSingleton();
            mPack.InstancingDrawFunc = GuiImageDraw;
            mPack.InstancingPrepareFunc = GuiImagePrepare;
            mPack.InstancingPrepareParam = this;
            mPack.pMesh = rm.GetUnitInstanceableQuadMesh();
            mPack.pVertexDeclaration = rm.GetUnitInstanceableQuadMesh()->GetVertexDeclaration();
        }

        GuiImage::~GuiImage()
        {}

        void GuiImage::Pose(const Vector4& aParentScaleShift, float aParentZ)
        {
            float z = (aParentZ);

            if (mPack.pEffect.IsValid())
            {
                Vector2 translation = Vector2(
                    (2.0f * mTranslation.X - 1.0f) + (mScale.X),
                  -((2.0f * mTranslation.Y - 1.0f) + (mScale.Y)));
                Vector4 scaleShift = Vector4(
                    Vector2(aParentScaleShift.X, aParentScaleShift.Y) * mScale,
                    (Vector2(aParentScaleShift.X, aParentScaleShift.Y) * translation) + Vector2(aParentScaleShift.Z, aParentScaleShift.W));

                // add clipping checks and parent clip rectange.
                z += (kGuiElementDepthStep);
                float alpha = 1.0f;

                engine_3D::RenderMan& rm = engine_3D::RenderMan::GetSingleton();
                GuiImageDrawHelper helper;
                helper.R0 = scaleShift;
                helper.R1 = mTextureScaleShift;
                helper.R2 = Vector4(z, alpha, 0, 0);
                mPack.InstancingPrepareParam = &helper;
                mPack.Sort = -z;

                GuiImageEffect* pEffect = static_cast<GuiImageEffect*>(mPack.pEffect.Get());

                if (alpha < 1.0f || mbTransparent)
                {
                    mPack.EffectTechnique = pEffect->GetTransparentTechnique();
                    mPack.Flags = (graphics::RenderPack::kTransparent | graphics::RenderPack::kNoStrictSort | graphics::RenderPack::kGUI);
                }
                else
                {
                    mPack.EffectTechnique = pEffect->GetOpaqueTechnique();
                    mPack.Flags = (graphics::RenderPack::kOpaque | graphics::RenderPack::kGUI);
                }

                rm.Pose(mPack);
            }
        }

        void GuiImage::SetEffect(GuiImageEffect* p)
        {
            if (mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(null); }
            mPack.pEffect.Reset(p);
            if (mPack.pEffect.IsValid() && mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(mPack.pEffect.Get()); }
        }

        void GuiImage::SetMaterial(graphics::Material* p)
        {
            if (mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(null); }
            mPack.pMaterial.Reset(p);
            if (mPack.pEffect.IsValid() && mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(mPack.pEffect.Get()); }
        }

    }
}
