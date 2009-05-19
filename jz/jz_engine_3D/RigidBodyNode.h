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
#ifndef _JZ_ENGINE_3D_RIGID_BODY_NODE_H_
#define _JZ_ENGINE_3D_RIGID_BODY_NODE_H_

#include <jz_engine_3D/MeshNode.h>

namespace jz
{
    namespace physics
    {
        class Body3D; typedef AutoPtr<Body3D> Body3DPtr;
    }

    namespace engine_3D
    {

        class RigidBodyNode : public MeshNode
        {
        public:
            RigidBodyNode();
            RigidBodyNode(const string& aBaseId, const string& aId);
            virtual ~RigidBodyNode();

            void DisablePhysicsBody();
            void SetAsPhysicalBox(const Vector3& aHalfExtents, float aMass = 0.0f, float aFriction = 1.0f);

        protected:
            virtual void _PreUpdateA(const Matrix4& aParentWorld, bool abParentChanged) override;
            virtual void _PopulateClone(SceneNode* apNode) override;
            virtual SceneNode* _SpawnClone(const string& aBaseId, const string& aCloneId) override;

            physics::Body3DPtr mpPhysicsBody;
            void __UpdateHandler(physics::Body3D* apBody);
            Event<void(physics::Body3D*)>::Entry mUpdateConnection;

        private:
            friend void jz::__IncrementRefCount<engine_3D::RigidBodyNode>(engine_3D::RigidBodyNode*);
            friend void jz::__DecrementRefCount<engine_3D::RigidBodyNode>(engine_3D::RigidBodyNode*);

            RigidBodyNode(const RigidBodyNode&);
            RigidBodyNode& operator=(const RigidBodyNode&);
        };

        typedef AutoPtr<RigidBodyNode> RigidBodyNodePtr;

    }
}

#endif
