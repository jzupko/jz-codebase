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

#include <jz_engine_3D/LightNode.h>
#include <jz_engine_3D/MeshNode.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_engine_3D/ShadowMan.h>
#include <jz_engine_3D/SimpleEffect.h>
#include <jz_engine_3D/StandardEffect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/VertexDeclaration.h>

namespace jz
{
    namespace engine_3D
    {

        static void DrawMesh(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            const MeshNode* p = static_cast<const MeshNode*>(apInstance);
            
            StandardEffect* pEffect = p->GetEffect().Get();
            pEffect->SetWit(p->GetWit());
            pEffect->SetWorld(p->GetWorldTransform());

            graphics::Graphics::GetSingleton().GetActivePass()->Commit();

            graphics::Mesh* pMesh = p->GetMesh().Get();
            pMesh->Draw();
        }

        static void SetActiveShadowHandle(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            const LightNode* p = static_cast<const LightNode*>(apInstance);

            RenderMan& rm = RenderMan::GetSingleton();
            SimpleEffect* pEffect = rm.GetSimpleEffect().Get();

            int shadowHandle = p->GetShadowHandle();

            pEffect->SetProjection(p->GetShadowProjection());
            pEffect->SetView(p->GetShadowView());

            ShadowMan::GetSingleton().SetActive(shadowHandle);

            apNode->RenderChildren();
        }

        MeshNode::MeshNode()
            : SceneNode(), mbNonDeferred(false), mAABB(BoundingBox::kZero), mPack(graphics::RenderPack::Create()), mbVisible(true)
        {}

        MeshNode::MeshNode(const string& aId)
            : SceneNode(aId), mbNonDeferred(false), mAABB(BoundingBox::kZero), mPack(graphics::RenderPack::Create()), mbVisible(true)
        {}

        MeshNode::~MeshNode()
        {
        }

        StandardEffectPtr MeshNode::GetEffect() const
        {
            return static_cast<StandardEffect*>(mPack.pEffect.Get());
        }

        void MeshNode::PoseForRender()
        {
            if (mbVisible && mPack.pEffect.IsValid())
            {
                RenderMan& rm = RenderMan::GetSingleton();
                
                graphics::RenderPack pack = mPack;
                pack.DrawFunc = DrawMesh;
                pack.DrawFuncParam = this;
                pack.EffectTechnique = (mbNonDeferred) ? static_cast<StandardEffect*>(pack.pEffect.Get())->GetNonDeferredTechnique() : static_cast<StandardEffect*>(pack.pEffect.Get())->GetRenderTechnique();
                pack.Sort = (pack.pMesh.IsValid()) ? Vector3::TransformPosition(mWorld * rm.GetView(), pack.pMesh->GetAABB().Center()).Z : 0.0f;

                rm.Pose(pack);
            }
        }

        void MeshNode::PoseForShadow(LightNode* apLight)
        {
            if (mbVisible && mPack.pEffect.IsValid())
            {
                StandardEffect* se = static_cast<StandardEffect*>(mPack.pEffect.Get());

                if (se->IsShadowable())
                {
                    RenderMan& rm = RenderMan::GetSingleton();
                    
                    graphics::RenderPack pack = mPack;
                    pack.DrawFunc = DrawMesh;
                    pack.DrawFuncParam = this;
                    pack.Flags = (graphics::RenderPack::kShadow);
                    pack.EffectTechnique = se->GetShadowTechnique();
                    pack.Sort = (pack.pMesh.IsValid()) ? Vector3::TransformPosition(mWorld * rm.GetView(), pack.pMesh->GetAABB().Center()).Z : 0.0f;
                    pack.PreEffectFunc = SetActiveShadowHandle;
                    pack.PreEffectFuncParam = apLight;

                    rm.Pose(pack);
                }
            }
        }

        void MeshNode::SetEffect(StandardEffect* p)
        {
            if (mPack.pEffect.IsValid() && mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(null); }
            mPack.pEffect.Reset(p);
            if (mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(mPack.pEffect.Get()); }

            if (p)
            {
                mPack.Flags = (p->IsTransparent() && !p->IsAlpha1Bit()) ? (graphics::RenderPack::kTransparent) : (graphics::RenderPack::kOpaque);
            }
            else
            {
                mPack.Flags = (graphics::RenderPack::kOpaque);
            }
        }

        void MeshNode::SetMaterial(graphics::Material* p)
        {
            if (mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(null); }
            mPack.pMaterial.Reset(p);
            if (mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(mPack.pEffect.Get()); }
        }

        void MeshNode::SetMesh(graphics::Mesh* p)
        {
            mPack.pMesh.Reset(p);
            mPack.pVertexDeclaration = p->GetVertexDeclaration();
        }

        void MeshNode::_PopulateClone(SceneNode* apNode)
        {
            MeshNode* p = static_cast<MeshNode*>(apNode);

            p->mAABB = mAABB;
            p->mPack = mPack;
        }

        SceneNode* MeshNode::_SpawnClone(const string& aCloneId)
        {
            return new MeshNode(aCloneId);
        }

        void MeshNode::_PostUpdate(bool abChanged)
        {
            if (abChanged)
            {
                if (mPack.pMesh.IsValid())
                {
                    BoundingSphere bounding = BoundingSphere::Transform(mWorld, mPack.pMesh->GetBoundingSphere());

                    if (mbValidBounding)
                    {
                        mWorldBounding = BoundingSphere::Merge(mWorldBounding, bounding);
                    }
                    else
                    {
                        mWorldBounding = bounding;
                        mbValidBounding = true;
                    }

                    mAABB = BoundingBox::Transform(mWorld, mPack.pMesh->GetAABB());
                }
            }

            SceneNode::_PostUpdate(abChanged);
        }

    }
}
