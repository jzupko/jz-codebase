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

#include <jz_engine_3D/ReflectionMan.h>
#include <jz_engine_3D/ReflectivePlaneNode.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_engine_3D/StandardEffect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/VertexDeclaration.h>

namespace jz
{
    namespace engine_3D
    {

        static void DrawMesh(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            ReflectionMan& rm = ReflectionMan::GetSingleton();
            const ReflectivePlaneNode* p = static_cast<const ReflectivePlaneNode*>(apInstance);
            StandardEffect* pEffect = p->GetEffect().Get();

            pEffect->SetWit(p->GetWit());
            pEffect->SetWorld(p->GetWorldTransform());

            p->SetReflectionScaleShift(rm.GetScaleShift(p->GetReflectionHandle()));
            p->SetReflectionTexture(rm.GetReflectionTarget());

            graphics::Graphics::GetSingleton().GetActivePass()->Commit();

            graphics::Mesh* pMesh = p->GetMesh().Get();
            pMesh->Draw();
        }
      
        ReflectivePlaneNode::ReflectivePlaneNode()
            : SceneNode(), mbNonDeferred(false), mPack(graphics::RenderPack::Create()), 
            mbVisible(true), mHandle(-1), mPlane(Vector3::kForward, 0.0f)
        {
            mHandle = ReflectionMan::GetSingleton().Grab();
        }

        ReflectivePlaneNode::ReflectivePlaneNode(const string& aBaseId, const string& aId)
            : SceneNode(aBaseId, aId), mbNonDeferred(false), mPack(graphics::RenderPack::Create()), 
            mbVisible(true), mHandle(-1), mPlane(Vector3::kForward, 0.0f)
        {
            mHandle = ReflectionMan::GetSingleton().Grab();
        }

        ReflectivePlaneNode::~ReflectivePlaneNode()
        {
            if (ReflectionMan::GetSingletonExists())
            {
                ReflectionMan::GetSingleton().Release(mHandle);
            }
        }

        void ReflectivePlaneNode::SetReflectionScaleShift(const Vector4& aScaleShift) const
        {
            jz_ReflectionScaleShift.Set(aScaleShift);
        }

        void ReflectivePlaneNode::SetReflectionTexture(graphics::Target* apTarget) const
        {
            jz_ReflectionTexture.Set(apTarget);
        }

        StandardEffectPtr ReflectivePlaneNode::GetEffect() const
        {
            return static_cast<StandardEffect*>(mPack.pEffect.Get());
        }

        void ReflectivePlaneNode::PoseForRender()
        {
            // Temp: need to finish automatic handling of parameters linked to effects.
            if (mPack.pEffect.IsValid() && mPack.pEffect->IsLoaded() && (!jz_ReflectionTexture.IsValid() || !jz_ReflectionScaleShift.IsValid()))
            {
                mPack.pEffect->GetParameterBySemantic("jz_ReflectionScaleShift", jz_ReflectionScaleShift);
                mPack.pEffect->GetParameterBySemantic("jz_ReflectionTexture", jz_ReflectionTexture);
            }

            if (mbVisible && mPack.pEffect.IsValid() && jz_ReflectionTexture.IsValid() && jz_ReflectionScaleShift.IsValid())
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

        void ReflectivePlaneNode::SetEffect(StandardEffect* p)
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

        void ReflectivePlaneNode::SetMaterial(graphics::Material* p)
        {
            if (mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(null); }
            mPack.pMaterial.Reset(p);
            if (mPack.pMaterial.IsValid()) { mPack.pMaterial->AttachTo(mPack.pEffect.Get()); }
        }

        void ReflectivePlaneNode::SetMesh(graphics::Mesh* p)
        {
            mPack.pMesh.Reset(p);
            mPack.pVertexDeclaration = p->GetVertexDeclaration();
        }

        void ReflectivePlaneNode::_PopulateClone(SceneNode* apNode)
        {
            ReflectivePlaneNode* p = static_cast<ReflectivePlaneNode*>(apNode);

            p->mPack = mPack;
        }

        SceneNode* ReflectivePlaneNode::_SpawnClone(const string& aBaseId, const string& aCloneId)
        {
            return new ReflectivePlaneNode(aBaseId, aCloneId);
        }

        void ReflectivePlaneNode::_PostUpdate(bool abChanged)
        {
            if (abChanged)
            {
                if (mPack.pMesh.IsValid())
                {
                    mWorldAABB = BoundingBox::Transform(mWorld, mPack.pMesh->GetAABB());
                    mWorldBounding = BoundingSphere::Transform(mWorld, mPack.pMesh->GetBoundingSphere());
                    mbValidBounding = true;
                }
            }

            SceneNode::_PostUpdate(abChanged);
        }

    }
}
