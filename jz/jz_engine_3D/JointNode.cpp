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

#include <jz_engine_3D/JointNode.h>

namespace jz
{
    namespace engine_3D
    {

        JointNode::JointNode()
            : SceneNode()
        {}

        JointNode::JointNode(const string& aBaseId, const string& aId)
            : SceneNode(aBaseId, aId)
        { }

        JointNode::~JointNode()
        {
        }

        void JointNode::_PopulateClone(SceneNode* apNode)
        {
            JointNode* p = static_cast<JointNode*>(apNode);

            // Temp: Animation should be referenced.
            p->mAnimation = mAnimation;
            p->mpAnimationControl = mpAnimationControl;
        }

        SceneNode* JointNode::_SpawnClone(const string& aBaseId, const string& aCloneId)
        {
            return new JointNode(aBaseId, aCloneId);
        }

        void JointNode::_PreUpdateA(const Matrix4& aParentWorld, bool abParentChanged)
        {
            if (mpParent && typeid(*mpParent) != typeid(JointNode)) { mFlags |= SceneNodeFlags::kIgnoreParent; }
            else { mFlags &= ~SceneNodeFlags::kIgnoreParent; }

            SceneNode::_PreUpdateA(aParentWorld, abParentChanged);

            if (mpAnimationControl.IsValid() && mpAnimationControl->Tick(mAnimation, mLocal))
            {
                mFlags |= SceneNodeFlags::kLocalDirty; 
            }
        }

    }
}

