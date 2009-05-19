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
#ifndef _JZ_ENGINE_3D_ANIMATED_MESH_NODE_H_
#define _JZ_ENGINE_3D_ANIMATED_MESH_NODE_H_

#include <jz_engine_3D/Animation.h>
#include <jz_engine_3D/JointNode.h>
#include <jz_engine_3D/MeshNode.h>

#include <string>
#include <vector>

namespace jz
{
    namespace engine_3D
    {

        class AnimatedMeshNode : public MeshNode
        {
        public:
            AnimatedMeshNode();
            AnimatedMeshNode(const string& aBaseId, const string& aId);
            virtual ~AnimatedMeshNode();

            virtual void Pick(const Ray3D& aRay) override;
            virtual void PoseForRender() override;
            virtual void PoseForShadow(LightNode* apLight) override;

            const AnimationControlPtr& GetAnimationControl() const { return mpAnimationControl; }
            
            const Matrix4& GetBindTransform() const { return mBind; }
            void SetBindTransform(const Matrix4& m) { mBind = m; }

            const MemoryBuffer<Matrix4>& GetInvBindTransforms() const { return mInvBinds; }
            MemoryBuffer<Matrix4>& GetInvBindTransforms() { return mInvBinds; }

            const vector<string>& GetJointIds() const { return mJointIds; }
            vector<string>& GetJointIds() { return mJointIds; mbJointsDirty = true; }

            const string& GetRootJointId() const { return mRootJointId; }
            void SetRootJointId(const string& s) { mRootJointId = s; mbJointsDirty = true; }

            const MemoryBuffer<Vector4>& GetSkinning() const { return mSkinning; }

        protected:
            virtual void _PopulateClone(SceneNode* apNode) override;
            virtual void _PostUpdate(bool abChanged)  override;
            virtual SceneNode* _SpawnClone(const string& aBaseId, const string& aCloneId) override;

            Matrix4 mBind;
            AnimationControlPtr mpAnimationControl;
            MemoryBuffer<Matrix4> mInvBinds;
            vector<JointNodePtr> mJoints;
            vector<string> mJointIds;
            bool mbJointsDirty;
            int mRootIndex;
            JointNodePtr mpRootJoint;
            string mRootJointId;
            MemoryBuffer<Vector4> mSkinning;
            
            vector<BoundingBox> mAABBCache;
            vector<BoundingSphere> mBoundingCache;
            vector<bool> mBoundingCached;

        private:
            friend void ::jz::__IncrementRefCount<AnimatedMeshNode>(AnimatedMeshNode*);
            friend void ::jz::__DecrementRefCount<AnimatedMeshNode>(AnimatedMeshNode*);

            AnimatedMeshNode(const AnimatedMeshNode&);
            AnimatedMeshNode& operator=(const AnimatedMeshNode&);

            void _SetJoint(size_t i, SceneNode* p);
            void _SetRootJoint(SceneNode* p);
        };

        typedef AutoPtr<AnimatedMeshNode> AnimatedMeshNodeNodePtr;

    }
}

#endif
