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
#ifndef _JZ_ENGINE_3D_PHYSICS_SCENE_NODE_H_
#define _JZ_ENGINE_3D_PHYSICS_SCENE_NODE_H_

#include <jz_engine_3D/IRenderable.h>
#include <jz_engine_3D/SceneNode.h>
#include <jz_physics/World.h>
#include <jz_system/TriangleTree.h>

namespace jz
{
    namespace physics
    {
        class Body3D; typedef AutoPtr<Body3D> Body3DPtr;
        class TriangleTreeShape; typedef AutoPtr<TriangleTreeShape> TriangleTreeShapePtr;
    }

    namespace engine_3D
    {

        class PhysicsNode : public SceneNode, public IRenderable
        {
        public:
            PhysicsNode();
            PhysicsNode(const string& aBaseId, const string& aId);
            virtual ~PhysicsNode();

            virtual const BoundingBox& GetBoundingBox() const override { return mWorldAABB; }
            virtual void PoseForRender() override;

            physics::World3D* GetWorld() const { return mpWorld; }
            physics::Body3D* GetWorldBody() const { return mpWorldBody.Get(); }

            physics::TriangleTreeShape const* GetWorldTree() const { return (mpWorldTree.Get()); }
            physics::TriangleTreeShape* GetWorldTree()
            {
                mbTreeDirty = true;
                return (mpWorldTree.Get());
            }

            void SetDebugPhysics(bool b) { mbDebugPhysics = b; }

        protected:
            virtual void _PostUpdate(bool abChanged) override;
            virtual void _PreUpdateB(const Matrix4& aParentWorld, bool abParentChanged) override;
            virtual SceneNode* _SpawnClone(const string& aBaseId, const string& aCloneId) override;

        private:
            physics::World3D* mpWorld;
            physics::TriangleTreeShapePtr mpWorldTree;
            physics::Body3DPtr mpWorldBody;
            
            bool mbTreeDirty;
            bool mbDebugPhysics;

            friend void jz::__IncrementRefCount<engine_3D::PhysicsNode>(engine_3D::PhysicsNode*);
            friend void jz::__DecrementRefCount<engine_3D::PhysicsNode>(engine_3D::PhysicsNode*);

            PhysicsNode(const PhysicsNode&);
            PhysicsNode& operator=(const PhysicsNode&);
        };
        typedef AutoPtr<PhysicsNode> PhysicsNodePtr;

    }
}

#endif
