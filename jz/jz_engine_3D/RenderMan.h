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

namespace jz
{
    namespace graphics
    {
        class Mesh; typedef AutoPtr<Mesh> MeshPtr;
    }

    namespace engine_3D
    {
        
        BoundingSphere CalculateFrustumBoundingSphere(const Matrix4& aProjection, const Matrix4& aInverseView);

        class Deferred;
        class ShadowMan;
        class SimpleEffect; typedef AutoPtr<SimpleEffect> SimpleEffectPtr;
        class RenderMan sealed : public Singleton<RenderMan>
        {
        public:
            RenderMan();
            ~RenderMan();

            JZ_EXPORT void Pose(const graphics::RenderPack& r);
            JZ_EXPORT void Render();

            const SimpleEffectPtr& GetSimpleEffect() const { return mpSimpleEffect; }

            const graphics::MeshPtr& GetUnitBoxMesh() const;
            const graphics::MeshPtr& GetUnitQuadMesh() const;
            const graphics::MeshPtr& GetUnitFrustumMesh() const;
            const graphics::MeshPtr& GetUnitSphereMesh() const;

            const Matrix4& GetInverseView() const { return mInverseView; }
            const Matrix4& GetProjection() const { return mProjection; }
            const Matrix4& GetView() const { return mView; }

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

        private:
            ColorRGBA mClearColor;
            SimpleEffectPtr mpSimpleEffect;

            graphics::MeshPtr mpUnitBox;
            graphics::MeshPtr mpUnitFrustum;
            graphics::MeshPtr mpUnitQuad;
            graphics::MeshPtr mpUnitSphere;

            Matrix4 mProjection;
            Matrix4 mInverseView;
            Matrix4 mView;
            Region mWorldFrustum;
            BoundingSphere mWorldFrustumBoundingSphere;
            float mGamma;

            Deferred* mpDeferred;
            ShadowMan* mpShadowMan;

            RenderMan(const RenderMan&);
            RenderMan& operator=(const RenderMan&);

            graphics::RenderNode mRenderOpaque;
            graphics::RenderNode mRenderShadow;
            graphics::RenderNode mRenderTransparent;

            void _ResetTrees();

            void _SetStandardParameters();
        };

    }
}

#endif
