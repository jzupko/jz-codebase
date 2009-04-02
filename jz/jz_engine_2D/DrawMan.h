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
#ifndef _JZ_ENGINE_2D_DRAWMAN_H_
#define _JZ_ENGINE_2D_DRAWMAN_H_

#include <jz_core/Angle.h>
#include <jz_core/Memory.h>
#include <jz_core/Utility.h>
#include <jz_core/Vector4.h>
#include <jz_graphics/RenderNode.h>
#include <jz_graphics/RenderPack.h>
#include <vector>

namespace jz
{
    template <typename T> class AutoPtr;

    namespace graphics
    {
        class Material; typedef AutoPtr<Material> MaterialPtr;
        class Mesh; typedef AutoPtr<Mesh> MeshPtr;
        class Target; typedef AutoPtr<Target> TargetPtr;
    }

    namespace engine_2D
    {
        class DrawEffect; typedef AutoPtr<DrawEffect> DrawEffectPtr;
        class DrawMan : public Singleton<DrawMan>
        {
        public:
            static const size_t kMaxInstances = 80u;
            static const size_t kMaxEntries = (kMaxInstances * 3u);

            DrawMan();
            ~DrawMan();

            DrawEffect* GetEffect() const;
            graphics::Mesh* GetMesh() const;

            void Draw();
            void Pose(const graphics::RenderPack& r);

            const vector<graphics::BufferEntry>& GetInstanceBuffers() const { return (*mpInstanceBuffer); }

        private:
            DrawMan(const DrawMan&);
            DrawMan& operator=(const DrawMan&);

            DrawEffectPtr mpEffect;
            graphics::MeshPtr mpMesh;
            graphics::TargetPtr mpHdrTarget;

            graphics::RenderNode mRenderOpaque;
            graphics::RenderNode mRenderTransparent;

            void _ResetTrees();

            u32 mCurrentBuffer;
            vector<graphics::BufferEntry>* mpInstanceBuffer;  

            void _InsertInstancingOp(graphics::RenderNode* p, graphics::InstancingOp iOp, voidc_p apInstanceOpParam, graphics::DrawOp dOp, graphics::AdoptOp aOp, float aSortOrder);
        };

    }
}

#endif
