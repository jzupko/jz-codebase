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
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DbAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#include <jz_engine_3D/PhysicsNode.h>
#include <jz_engine_3D/RigidBodyNode.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/BoxShape.h>
#include <jz_physics/narrowphase/collision/SphereShape.h>
#include <jz_physics/World.h>

namespace jz
{
    namespace engine_3D
    {

        RigidBodyNode::RigidBodyNode()
            : MeshNode()
        {}

        RigidBodyNode::RigidBodyNode(const string& aBaseId, const string& aId)
            : MeshNode(aBaseId, aId)
        {}

        RigidBodyNode::~RigidBodyNode()
        {
        }

        void RigidBodyNode::DisablePhysicsBody()
        {
            mUpdateConnection.Reset();
            mpPhysicsBody.Reset();
        }

        void RigidBodyNode::SetAsPhysicalBox(const Vector3& aHalfExtents, float aMass, float aFriction)
        {
            PhysicsNodePtr pNode = Get<PhysicsNode>(GetBaseId(), "physics_node");
            if (pNode.IsValid())
            {
                physics::World3D* pWorld = pNode->GetWorld();
                mpPhysicsBody = pWorld->Create(new physics::BoxShape(aHalfExtents), physics::Body3D::kDynamic, physics::Body3D::kDynamic | physics::Body3D::kStatic);
                mpPhysicsBody->SetMass(aMass);
                mpPhysicsBody->SetFriction(aFriction);
                mpPhysicsBody->SetFrame(CoordinateFrame3D::CreateFromMatrix4(mWorld));
                mpPhysicsBody->OnUpdate.Add<RigidBodyNode, &RigidBodyNode::__UpdateHandler>(this, mUpdateConnection);
            }
        }

        void RigidBodyNode::_PreUpdateA(const Matrix4& aParentWorld, bool abParentChanged)
        {
            if (mpPhysicsBody.IsValid())
            {
                if ((!IsIgnoringParent() && abParentChanged) || IsLocalDirty() || IsWorldDirty())
                {
                    mpPhysicsBody->SetFrame(CoordinateFrame3D::CreateFromMatrix4(_GetUpdatedWorldTransform(aParentWorld, abParentChanged)));
                }
            }

            MeshNode::_PreUpdateA(aParentWorld, abParentChanged);
        }

        void RigidBodyNode::_PopulateClone(SceneNode* apNode)
        {
            RigidBodyNode* p = static_cast<RigidBodyNode*>(apNode);

            if (mpPhysicsBody.IsValid())
            {
                PhysicsNodePtr pNode = Get<PhysicsNode>(GetBaseId(), "physics_node");
                if (pNode.IsValid())
                {
                    physics::World3D* pWorld = pNode->GetWorld();
                    p->mpPhysicsBody = pWorld->Create(mpPhysicsBody->GetCollisionShape(), mpPhysicsBody->GetType(), mpPhysicsBody->GetCollidesWith());
                    p->mpPhysicsBody->SetMass(mpPhysicsBody->GetMass());
                    p->mpPhysicsBody->SetFriction(mpPhysicsBody->GetFriction());
                    p->mpPhysicsBody->OnUpdate.Add<RigidBodyNode, &RigidBodyNode::__UpdateHandler>(p, p->mUpdateConnection);
                }
            }
        }

        SceneNode* RigidBodyNode::_SpawnClone(const string& aBaseId, const string& aCloneId)
        {
            return new RigidBodyNode(aBaseId, aCloneId);
        }

        void RigidBodyNode::__UpdateHandler(physics::Body3D* apBody)
        {
            ToTransform(apBody->GetFrame(), mWorld);
            mFlags |= SceneNodeFlags::kWorldDirty;
        }

    }
}
