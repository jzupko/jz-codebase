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
#ifndef _JZ_ENGINE_3D_RENDERMAN_H_
#define _JZ_ENGINE_3D_RENDERMAN_H_

#include <jz_core/BoundingSphere.h>
#include <jz_core/Color.h>
#include <jz_core/Region.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Memory.h>
#include <jz_core/Utility.h>
#include <jz_graphics/RenderNode.h>
#include <jz_graphics/RenderPack.h>
#include <vector>

namespace jz
{
    namespace graphics
    {
        // Temp:
        class Font; typedef AutoPtr<Font> FontPtr;
        class Mesh; typedef AutoPtr<Mesh> MeshPtr;
    }

    namespace engine_3D
    {
        
        BoundingSphere CalculateFrustumBoundingSphere(const Matrix4& aProjection, const Matrix4& aInverseView);

        class Deferred;
        class ReflectionMan;
        class ShadowMan;
        class SimpleEffect; typedef AutoPtr<SimpleEffect> SimpleEffectPtr;
        class RenderMan sealed : public Singleton<RenderMan>
        {
        public:
            JZ_ALIGNED_NEW

            static const size_t kMaxInstances = 80u;
            static const size_t kMaxEntries = (kMaxInstances * 3u);

            RenderMan();
            ~RenderMan();

            JZ_EXPORT void ClearWithoutRender();
            JZ_EXPORT void Pose(const graphics::RenderPack& r);
            JZ_EXPORT void Render();

            const vector<graphics::BufferEntry>& GetInstanceBuffers() const { return (*mpInstanceBuffer); }
            SimpleEffect* GetSimpleEffect() const { return (mpSimpleEffect.Get()); }

            graphics::Mesh* GetUnitBoxMesh() const { return (mpUnitBox.Get()); }
            graphics::Mesh* GetUnitQuadMesh() const { return (mpUnitQuad.Get()); }
            graphics::Mesh* GetUnitInstanceableQuadMesh() const { return (mpInstanceableQuad.Get()); }
            graphics::Mesh* GetUnitFrustumMesh() const { return (mpUnitFrustum.Get()); }
            graphics::Mesh* GetUnitSphereMesh() const { return (mpUnitSphere.Get()); }

            const Matrix4& GetInverseView() const { return mInverseView; }
            const Matrix4& GetProjection() const { return mProjection; }
            const Matrix4& GetView() const { return mView; }

            const Matrix4& GetPrevFrameView() const { return mPrevView; }
            const Matrix4& GetPrevFrameProjection() const { return mPrevProjection; }

            const Region& GetWorldFrustum() const { return mWorldFrustum; }
            const BoundingSphere& GetWorldFrustumBoundingSphere() const { return mWorldFrustumBoundingSphere; }

            const ColorRGBA& GetClearColor() const { return mClearColor; }
            void SetClearColor(const ColorRGBA& c) { mClearColor = c; }

            float GetGamma() const { return mGamma; }
            void SetGamma(float v) { mGamma = v; }

            void SetProjection(const Matrix4& v)
            {
                mProjection = v;
                
                mWorldFrustum.Set(mInverseView.GetTranslation(), (mView * mProjection));
                mWorldFrustumBoundingSphere = CalculateFrustumBoundingSphere(mProjection, mInverseView);
            }

            void SetView(const Matrix4& v)
            {
                mView = v;
                mInverseView = Matrix4::Invert(v);

                mWorldFrustum.Set(mInverseView.GetTranslation(), (mView * mProjection));
                mWorldFrustumBoundingSphere = CalculateFrustumBoundingSphere(mProjection, mInverseView);
            }

            // Temp: until a real GUI system is added.
            void AddConsoleLine(const string& s);

            // Pushes standard shared parameters to the effect system.
            void SetStandardParameters();

        private:
            // Temp:
            graphics::FontPtr mpConsole;
            string mConsoleText;

            ColorRGBA mClearColor;
            SimpleEffectPtr mpSimpleEffect;

            graphics::MeshPtr mpUnitBox;
            graphics::MeshPtr mpUnitFrustum;
            graphics::MeshPtr mpInstanceableQuad;
            graphics::MeshPtr mpUnitQuad;
            graphics::MeshPtr mpUnitSphere;

            Matrix4 mProjection;
            Matrix4 mInverseView;
            Matrix4 mView;
            Matrix4 mPrevView;
            Matrix4 mPrevProjection;
            Region mWorldFrustum;
            BoundingSphere mWorldFrustumBoundingSphere;
            float mGamma;

            Deferred* mpDeferred;
            ReflectionMan* mpReflectionMan;
            ShadowMan* mpShadowMan;

            RenderMan(const RenderMan&);
            RenderMan& operator=(const RenderMan&);

            graphics::RenderNode mRenderOpaque;
            graphics::RenderNode mRenderTransparent;
            graphics::RenderNode mRenderGuiOpaque;
            graphics::RenderNode mRenderGuiTransparent;
            graphics::RenderNode mRenderReflectionOpaque;
            graphics::RenderNode mRenderReflectionTransparent;
            graphics::RenderNode mRenderShadow;

            graphics::RenderNode* _GetNode(const graphics::RenderPack& r);
            void _ResetTrees();

            u32 mCurrentBuffer;
            vector<graphics::BufferEntry>* mpInstanceBuffer;

            void _ClearInstanceBuffers();
            void _InsertInstancingOp(graphics::RenderNode* p, graphics::InstancingOp iOp, voidc_p apInstanceOpParam, graphics::DrawOp dOp, graphics::AdoptOp aOp, float aSortOrder);
        };

    }
}

#endif
