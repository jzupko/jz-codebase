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
#include <jz_engine_3D/PickMan.h>
#include <jz_engine_3D/ReflectionMan.h>
#include <jz_engine_3D/ReflectivePlaneNode.h>
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
            
            StandardEffect* pEffect = p->GetEffect();

            if (p->bThreePoint())
            {
                pEffect->SetThreePoint(p->GetThreePoint());
            }

            Matrix4 scale = Matrix4::CreateScale(p->GetScale());

            pEffect->SetWit(scale * p->GetWit());
            pEffect->SetWorld(scale * p->GetWorldTransform());

            graphics::Graphics::GetSingleton().GetActivePass()->Commit();

            graphics::Mesh* pMesh = p->GetMesh();
            pMesh->Draw();
        }

        static void SetActiveShadowHandle(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            const LightNode* p = static_cast<const LightNode*>(apInstance);

            RenderMan& rm = RenderMan::GetSingleton();
            SimpleEffect* pEffect = rm.GetSimpleEffect();

            int shadowHandle = p->GetShadowHandle();

            pEffect->SetProjection(p->GetShadowProjection());
            pEffect->SetView(p->GetShadowView());

            ShadowMan::GetSingleton().SetActive(shadowHandle);

            apNode->RenderChildren();
        }

        static void SetActiveReflectivePlaneHandle(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            const ReflectivePlaneNode* p = static_cast<const ReflectivePlaneNode*>(apInstance);

            int reflectionHandle = p->GetReflectionHandle();
            ReflectionMan::GetSingleton().SetActive(reflectionHandle);
            RenderMan& rm = RenderMan::GetSingleton();

            const Plane& plane = (p->GetReflectionPlane());
            Plane worldPlane = Plane::Transform(p->GetWorldTransform(), plane);
            Vector4 w = Vector4(worldPlane.GetNormal(), worldPlane.GetD());
            Plane wvpPlane = Plane::Transform(rm.GetView() * rm.GetProjection(), worldPlane);
            // - to reflect the wvp
            Vector4 wvp = -Vector4(wvpPlane.GetNormal(), wvpPlane.GetD());

            RenderMan::GetSingleton().GetSimpleEffect()->SetReflectionPlane(w);

            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
            graphics.EnableClipPlane(0u);
            graphics.SetClipPlane(0u, wvp);
            apNode->RenderChildren();
            graphics.DisableClipPlanes();
        }

        MeshNode::MeshNode()
            : SceneNode(), 
            mbVisible(true),
            mbNonDeferred(false),
            mPack(graphics::RenderPack::Create()),
            mbHasTp(false),
            mTp(ThreePoint::Create()),
            mbCastShadow(true),
            mScale(Vector3::kOne)
        {}

        MeshNode::MeshNode(const string& aBaseId, const string& aId)
            : SceneNode(aBaseId, aId), 
            mbVisible(true),
            mbNonDeferred(false),
            mPack(graphics::RenderPack::Create()),
            mbHasTp(false),
            mTp(ThreePoint::Create()),
            mbCastShadow(true),
           mScale(Vector3::kOne)
        {}

        MeshNode::~MeshNode()
        {
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

        void MeshNode::Pick(const Ray3D& aRay)
        {
            if (mbVisible && mPack.pEffect.IsValid())
            {
                StandardEffect* pEffect = static_cast<StandardEffect*>(mPack.pEffect.Get());

                if (pEffect->IsPickable())
                {
                    PickMan& pm = PickMan::GetSingleton();
                    RenderMan& rm = RenderMan::GetSingleton();
        
                    graphics::RenderPack pack = mPack;
                    pack.DrawFunc = DrawMesh;
                    pack.DrawFuncParam = this;
                    pack.EffectTechnique = pEffect->GetPickingTechnique();
                    pack.Sort = (pack.pMesh.IsValid()) ? Vector3::TransformPosition(mWorld * rm.GetView(), pack.pMesh->GetAABB().Center()).Z : 0.0f;

                    pm.Pose(pack, this);
                }
            }
        }

        void MeshNode::PoseForReflection(ReflectivePlaneNode* apReflectivePlane)
        {
            if (mbVisible && mPack.pEffect.IsValid())
            {
                StandardEffect* se = static_cast<StandardEffect*>(mPack.pEffect.Get());

                if (se->IsReflectable())
                {
                    RenderMan& rm = RenderMan::GetSingleton();
                    
                    graphics::RenderPack pack = mPack;
                    pack.DrawFunc = DrawMesh;
                    pack.DrawFuncParam = this;
                    pack.Flags |= (graphics::RenderPack::kReflection);
                    pack.EffectTechnique = se->GetReflectionTechnique();
                    pack.Sort = (pack.pMesh.IsValid()) ? Vector3::TransformPosition(mWorld * rm.GetView(), pack.pMesh->GetAABB().Center()).Z : 0.0f;
                    pack.PreEffectFunc = SetActiveReflectivePlaneHandle;
                    pack.PreEffectFuncParam = apReflectivePlane;

                    rm.Pose(pack);
                }
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

            p->mPack = mPack;
        }

        SceneNode* MeshNode::_SpawnClone(const string& aBaseId, const string& aCloneId)
        {
            return new MeshNode(aBaseId, aCloneId);
        }

        void MeshNode::_PostUpdate(bool abChanged)
        {
            if (abChanged && mPack.pMesh.IsValid())
            {
                mWorldAABB = BoundingBox::Transform(mWorld, mPack.pMesh->GetAABB());
                mWorldBounding = BoundingSphere::Transform(mWorld, mPack.pMesh->GetBoundingSphere());
                mbValidBounding = true;
            }

            SceneNode::_PostUpdate(abChanged);
        }

    }
}
