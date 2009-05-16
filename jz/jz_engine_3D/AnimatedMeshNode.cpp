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

#include <jz_engine_3D/AnimatedMeshNode.h>
#include <jz_engine_3D/LightNode.h>
#include <jz_engine_3D/PickMan.h>
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

        static void DrawAnimatedMesh(graphics::RenderNode* apNode, voidc_p apInstance)
        {
           const AnimatedMeshNode* p = static_cast<const AnimatedMeshNode*>(apInstance);
            
            StandardEffect* pEffect = p->GetEffect();
            if (p->bThreePoint())
            {
                pEffect->SetThreePoint(p->GetThreePoint());
            }

            pEffect->SetSkinning(p->GetSkinning());
            pEffect->SetWit(p->GetWit());
            pEffect->SetWorld(p->GetWorldTransform());
            
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

        AnimatedMeshNode::AnimatedMeshNode()
            : MeshNode(),
            mpAnimationControl(new AnimationControl()),
            mBind(Matrix4::kIdentity),
            mbJointsDirty(false),
            mRootIndex(-1)
        {}

        AnimatedMeshNode::AnimatedMeshNode(const string& aId)
            : MeshNode(aId),
            mpAnimationControl(new AnimationControl()),
            mBind(Matrix4::kIdentity),
            mbJointsDirty(false),
            mRootIndex(-1)
        { }

        AnimatedMeshNode::~AnimatedMeshNode()
        {
        }

        void AnimatedMeshNode::PoseForRender()
        {
            if (mbVisible && mPack.pEffect.IsValid())
            {
                RenderMan& rm = RenderMan::GetSingleton();
                
                graphics::RenderPack pack = mPack;
                pack.DrawFunc = DrawAnimatedMesh;
                pack.DrawFuncParam = this;
                pack.EffectTechnique = (mbNonDeferred) ? static_cast<StandardEffect*>(pack.pEffect.Get())->GetNonDeferredTechnique() : static_cast<StandardEffect*>(pack.pEffect.Get())->GetRenderTechnique();
                pack.Sort = (pack.pMesh.IsValid()) ? Vector3::TransformPosition((mWorld * rm.GetView()), pack.pMesh->GetAABB().Center()).Z : 0.0f;

                rm.Pose(pack);
            }
        }

        void AnimatedMeshNode::PoseForShadow(LightNode* apLight)
        {
            if (mbVisible && mPack.pEffect.IsValid())
            {
                StandardEffect* se = static_cast<StandardEffect*>(mPack.pEffect.Get());

                if (se->IsShadowable())
                {
                    RenderMan& rm = RenderMan::GetSingleton();
                    
                    graphics::RenderPack pack = mPack;
                    pack.DrawFunc = DrawAnimatedMesh;
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

        void AnimatedMeshNode::Pick(const Ray3D& aRay)
        {
            if (mbVisible && mPack.pEffect.IsValid())
            {
                StandardEffect* pEffect = static_cast<StandardEffect*>(mPack.pEffect.Get());

                if (pEffect->IsPickable())
                {
                    PickMan& pm = PickMan::GetSingleton();
                    RenderMan& rm = RenderMan::GetSingleton();
        
                    graphics::RenderPack pack = mPack;
                    pack.DrawFunc = DrawAnimatedMesh;
                    pack.DrawFuncParam = this;
                    pack.EffectTechnique = pEffect->GetPickingTechnique();
                    pack.Sort = (pack.pMesh.IsValid()) ? Vector3::TransformPosition(mWorld * rm.GetView(), pack.pMesh->GetAABB().Center()).Z : 0.0f;

                    pm.Pose(pack, this);
                }
            }
        }

        void AnimatedMeshNode::_PopulateClone(SceneNode* apNode)
        {
            AnimatedMeshNode* p = static_cast<AnimatedMeshNode*>(apNode);

            p->mpAnimationControl = mpAnimationControl;
            p->mBind = mBind;
            p->mInvBinds = mInvBinds;
            p->mJoints = mJoints;
            p->mJointIds = mJointIds;
            p->mbJointsDirty = mbJointsDirty;
            p->mRootIndex = mRootIndex;
            p->mpRootJoint = mpRootJoint;
            p->mRootJointId = mRootJointId;
            p->mSkinning = mSkinning;
        }

        SceneNode* AnimatedMeshNode::_SpawnClone(const string& aCloneId)
        {
            return new AnimatedMeshNode(aCloneId);
        }

        void AnimatedMeshNode::_SetJoint(size_t i, SceneNode* p)
        {
            if (i < mJoints.size())
            {
                if (mJoints[i].IsValid())
                {
                    mJoints[i]->SetAnimationControl(null);
                }
                mJoints[i].Reset(dynamic_cast<JointNode*>(p));
                if (mJoints[i].IsValid())
                {
                    mJoints[i]->SetAnimationControl(mpAnimationControl.Get());
                }
            }
        }

        void AnimatedMeshNode::_SetRootJoint(SceneNode* p)
        {
            if (mpRootJoint.IsValid())
            {
                mpRootJoint->SetAnimationControl(null);
                mpRootJoint.Reset();
                mRootIndex = -1;
            }

            mpRootJoint.Reset(dynamic_cast<JointNode*>(p));
            
            if (mpRootJoint.IsValid())
            {
                mpRootJoint->SetAnimationControl(mpAnimationControl.Get());

                for (size_t i = 0; i < mJointIds.size(); i++)
                {
                    if (mJoints[i] == mpRootJoint)
                    {
                        mRootIndex = (int)i;
                        break;
                    }
                }
            }
        }

        void AnimatedMeshNode::_PostUpdate(bool abChanged)
        {
            if (mbJointsDirty)
            {
                size_t count = mJointIds.size();
                mJoints.resize(count);
                mSkinning.resize(count * 3u);
                mBoundingCached.assign(mBoundingCached.size(), false);

                for (size_t i = 0; i < count; i += 3)
                {
                    mSkinning[i+0] = Vector4::kUnitX;
                    mSkinning[i+1] = Vector4::kUnitY;
                    mSkinning[i+2] = Vector4::kUnitZ;
                }

                for (size_t i = 0; i < count; i++)
                {
                    Get(mJointIds[i], bind(&AnimatedMeshNode::_SetJoint, this, i, tr1::placeholders::_1));
                }

                Get(mRootJointId, bind(&AnimatedMeshNode::_SetRootJoint, this, tr1::placeholders::_1));

                mbJointsDirty = false;
                abChanged = true;
            }

            if (mpRootJoint.IsValid() && mpRootJoint->bDirty())
            {
                size_t size = mJoints.size();
                for (size_t i = 0u; i < size; i++)
                {
                    size_t entryIndex = (i * 3u);

                    if (mJoints[i].IsValid())
                    {
                        Matrix4 transform = (mBind * mInvBinds[i] * mJoints[i]->GetWorldTransform());

                        mSkinning[entryIndex + 0u] = transform.GetCol(0);
                        mSkinning[entryIndex + 1u] = transform.GetCol(1);
                        mSkinning[entryIndex + 2u] = transform.GetCol(2);
                    }
                    else
                    {
                        mSkinning[entryIndex + 0u] = Vector4::kUnitX;
                        mSkinning[entryIndex + 1u] = Vector4::kUnitY;
                        mSkinning[entryIndex + 2u] = Vector4::kUnitZ;
                    }
                }
                abChanged = true;
            }

            if (abChanged && mpAnimationControl.IsValid() && mPack.pMesh.IsValid())
            {
                size_t curIndex = (size_t)mpAnimationControl->GetCurrentIndex();
                if (mBoundingCache.size() <= curIndex)
                {
                    mAABBCache.resize(curIndex + 1u);
                    mBoundingCache.resize(curIndex + 1u);
                    mBoundingCached.resize(curIndex + 1u);
                }

                if (!mBoundingCached[curIndex])
                {
                    mPack.pMesh->CalculateBoundings(mSkinning, mAABBCache[curIndex], mBoundingCache[curIndex]);
                    mBoundingCached[curIndex] = true;
                }
            
                mAABB = BoundingBox::Transform(mWorld, mAABBCache[curIndex]);
                mWorldBounding = BoundingSphere::Transform(mWorld, mBoundingCache[curIndex]);
                mbValidBounding = true;

                // Need to calculate tighter AABB
                // Need to reset bounding cache if mesh, animationcontrol, or joints change.
            }
        }

    }
}
