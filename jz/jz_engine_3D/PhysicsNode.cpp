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

#include <jz_engine_3D/PhysicsNode.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_engine_3D/SimpleEffect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_physics/World.h>
#include <jz_physics/narrowphase/Body.h>
#include <jz_physics/narrowphase/collision/TriangleTreeShape.h>
#include <jz_system/Time.h>

namespace jz
{
    namespace engine_3D
    {

        PhysicsNode::PhysicsNode()
            : SceneNode(),
            mpWorld(new physics::World3D()),
            mpWorldTree(new physics::TriangleTreeShape()),
            mpWorldBody(mpWorld->Create(mpWorldTree.Get(),
                physics::Body3D::kStatic, physics::Body3D::kDynamic)),
            mbDebugPhysics(false)
        {}

        PhysicsNode::PhysicsNode(const string& aId)
            : SceneNode(aId),
            mpWorld(new physics::World3D()),
            mpWorldTree(new physics::TriangleTreeShape()),
            mpWorldBody(mpWorld->Create(mpWorldTree.Get(),
                physics::Body3D::kStatic, physics::Body3D::kDynamic)),
            mbDebugPhysics(false)
        {}

        PhysicsNode::~PhysicsNode()
        {
            mpWorldBody.Reset();
            mpWorldTree.Reset();
            SafeDelete(mpWorld);
        }

        BoundingBox PhysicsNode::GetAABB() const
        {
            return (GetWorldBody()->GetLocalBounding());
        }

        static void DrawPhysicsNode(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            using namespace graphics;
            PhysicsNode const* pNode = static_cast<PhysicsNode const*>(apInstance);

            const MemoryBuffer<u16>& inds = pNode->GetWorldTree()->mTriangleTree.GetIndices();
            const MemoryBuffer<Vector3>& verts = pNode->GetWorldTree()->mTriangleTree.GetVertices();

            Graphics::GetSingleton().DrawIndexedPrimitives(
                PrimitiveType::kTriangleList, inds.Get(), (inds.size() / 3u), verts.Get(), verts.size(), sizeof(Vector3));
        }

        void PhysicsNode::PoseForRender()
        {
            if (mbDebugPhysics)
            {
                RenderMan& rm = RenderMan::GetSingleton();

                graphics::RenderPack pack(graphics::RenderPack::Create());
                pack.DrawFunc = DrawPhysicsNode;
                pack.DrawFuncParam = this;
                pack.pEffect = rm.GetSimpleEffect();
                pack.EffectTechnique = rm.GetSimpleEffect()->GetRenderTechnique();
                pack.pVertexDeclaration = rm.GetUnitBoxMesh()->GetVertexDeclaration();
                pack.Sort = 0.0f;

                RenderMan::GetSingleton().Pose(pack);
            }
        }

        SceneNode* PhysicsNode::_SpawnClone(const string& aCloneId)
        {
            throw std::exception(__FUNCTION__ "cannot clone PhysicsNode.");
        }

        void PhysicsNode::_PreUpdate(const Matrix4& aParentWorld, bool abParentChanged)
        {
            mpWorld->Tick(system::Time::GetSingleton().GetElapsedSeconds());

            SceneNode::_PreUpdate(aParentWorld, abParentChanged);
        }

    }
}

