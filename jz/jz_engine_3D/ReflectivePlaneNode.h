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
#ifndef _JZ_ENGINE_3D_REFLECTIVE_PLANE_NODE_H_
#define _JZ_ENGINE_3D_REFLECTIVE_PLANE_NODE_H_

#include <jz_graphics/Parameter.h>
#include <jz_graphics/RenderPack.h>
#include <jz_engine_3D/IPickable.h>
#include <jz_engine_3D/IReflectable.h>
#include <jz_engine_3D/IRenderable.h>
#include <jz_engine_3D/IShadowable.h>
#include <jz_engine_3D/SceneNode.h>
#include <jz_engine_3D/ThreePoint.h>

namespace jz
{
    namespace graphics
    {
        class Target; typedef AutoPtr<Target> TargetPtr;
    }

    namespace engine_3D
    {

        class StandardEffect; typedef AutoPtr<StandardEffect> StandardEffectPtr;
        class ReflectivePlaneNode : public SceneNode, public IRenderable
        {
        public:
            ReflectivePlaneNode();
            ReflectivePlaneNode(const string& aId);
            virtual ~ReflectivePlaneNode();

            bool bVisible() const { return mbVisible; }
            void SetVisible(bool b) { mbVisible = b; }

            virtual void PoseForRender() override;
            virtual BoundingBox GetAABB() const override { return mAABB; }

            StandardEffectPtr GetEffect() const;
            graphics::MaterialPtr GetMaterial() const { return mPack.pMaterial; }
            graphics::MeshPtr GetMesh() const { return mPack.pMesh; }

            void SetEffect(StandardEffect* p);
            void SetMaterial(graphics::Material* p);
            void SetMesh(graphics::Mesh* p);

            bool GetNonDeferred() const { return mbNonDeferred; }
            void SetNonDeferred(bool b) { mbNonDeferred = b; }

            int GetReflectionHandle() const { return mHandle; }

            const Plane& GetReflectionPlane() const { return mPlane; }
            void SetReflectionPlane(const Plane& v) { mPlane = v; }

            void SetReflectionScaleShift(const Vector4& aScaleShift) const;
            void SetReflectionTexture(graphics::Target* apTarget) const;

        protected:
            virtual void _PopulateClone(SceneNode* apNode) override;
            virtual void _PostUpdate(bool abChanged)  override;
            virtual SceneNode* _SpawnClone(const string& aCloneId) override;

            bool mbVisible;
            BoundingBox mAABB;
            bool mbNonDeferred;
            graphics::RenderPack mPack;
            int mHandle;

            Plane mPlane;
            graphics::Parameter<Vector4> jz_ReflectionScaleShift;
            graphics::Parameter<graphics::Target> jz_ReflectionTexture;

        private:
            friend void jz::__IncrementRefCount<engine_3D::ReflectivePlaneNode>(engine_3D::ReflectivePlaneNode*);
            friend void jz::__DecrementRefCount<engine_3D::ReflectivePlaneNode>(engine_3D::ReflectivePlaneNode*);

            ReflectivePlaneNode(const ReflectivePlaneNode&);
            ReflectivePlaneNode& operator=(const ReflectivePlaneNode&);
        };

        typedef AutoPtr<ReflectivePlaneNode> ReflectivePlaneNodePtr;

    }
}

#endif
