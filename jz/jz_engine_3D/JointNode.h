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
#ifndef _JZ_ENGINE_3D_JOINT_NODE_H_
#define _JZ_ENGINE_3D_JOINT_NODE_H_

#include <jz_engine_3D/Animation.h>
#include <jz_engine_3D/SceneNode.h>

namespace jz
{
    namespace engine_3D
    {

        class JointNode : public SceneNode
        {
        public:
            JointNode();
            JointNode(const string& aId);
            virtual ~JointNode();

            const Animation& GetAnimation() const { return mAnimation; }
            Animation& GetAnimation() { return mAnimation; }

            const AnimationControlPtr& GetAnimationControl() const { return mpAnimationControl; }
            void SetAnimationControl(AnimationControl* p) { mpAnimationControl.Reset(p); }

        protected:
            virtual void _PopulateClone(SceneNode* apNode) override;
            virtual void _PreUpdate(const Matrix4& aParentWorld, bool abParentChanged)  override;
            virtual SceneNode* _SpawnClone(const string& aCloneId) override;

            Animation mAnimation;
            AnimationControlPtr mpAnimationControl;

        private:
            friend void jz::__IncrementRefCount<engine_3D::JointNode>(engine_3D::JointNode*);
            friend void jz::__DecrementRefCount<engine_3D::JointNode>(engine_3D::JointNode*);

            JointNode(const JointNode&);
            JointNode& operator=(const JointNode&);
        };

        typedef AutoPtr<JointNode> JointNodePtr;

    }
}

#endif
