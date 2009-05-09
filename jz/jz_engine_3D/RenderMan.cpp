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

#include <jz_core/Matrix4.h>
#include <jz_core/Vector4.h>
#include <jz_engine_3D/Deferred.h>
#include <jz_engine_3D/MeshNode.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_engine_3D/ReflectionMan.h>
#include <jz_engine_3D/ShadowMan.h>
#include <jz_engine_3D/SimpleEffect.h>
#include <jz_engine_3D/StandardEffect.h>
#include <jz_system/Files.h>
#include <jz_system/Time.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Mesh.h>
#include <jz_graphics/RenderOps.h>
#include <jz_graphics/VertexDeclaration.h>
#include <algorithm>

// Temp:
#include <jz_graphics/Font.h>

namespace jz
{
    engine_3D::RenderMan* engine_3D::RenderMan::mspSingleton = null;
    namespace engine_3D
    {
        static float kGamma = 2.2f;

        static const char* kSimpleEffect = "..\\media\\SimpleEffect.cfx";
        static const char* kUnitBox = "built-in_unit-box.mesh";
        static const char* kUnitFrustum = "built-in_unit-frustum.mesh";
        static const char* kUnitQuad = "built-in_unit-quad.mesh";
        static const char* kUnitSphere = "built-in_unit-sphere.mesh";

        BoundingSphere CalculateFrustumBoundingSphere(const Matrix4& aProjection, const Matrix4& aInverseView)
        {
            float n;
            float f;
            Matrix4::ExtractNearFarDirectX(aProjection, n, f);
            Radian fov = Matrix4::ExtractFov(aProjection);
            float aspectRatio = Matrix4::ExtractAspectRatio(aProjection);

            float length = (f - n);
            float height = (length * Tan(fov * 0.5f));
            float width = (aspectRatio * height);

            float centerD = (n + f) * 0.5f;
            Vector3 u = Vector3(width, height, centerD);

            float radius = 2.0f * u.Length();
            Vector3 center = Vector3::TransformPosition(aInverseView, Vector3(0, 0, -centerD));

            BoundingSphere ret = BoundingSphere(center, radius);

            return ret;
        }

        RenderMan::RenderMan()
            : mProjection(Matrix4::kIdentity),
            mView(Matrix4::kIdentity), 
            mPrevView(Matrix4::kIdentity),
            mPrevProjection(Matrix4::kIdentity),
            mpDeferred(null),
            mGamma(kGamma),
            mClearColor(ColorRGBA::kBlack)
        {
            using namespace graphics;
            Graphics& graphics = Graphics::GetSingleton();

            // Temp:
            mpConsole = graphics.Create<Font>(16u, 32u, 0u, "Arial");
            // End Temp:

            mpSimpleEffect = graphics.Create<SimpleEffect>(kSimpleEffect);
            mpUnitBox = graphics.Create<Mesh>(kUnitBox);
            mpUnitFrustum = graphics.Create<Mesh>(kUnitFrustum);
            mpUnitQuad = graphics.Create<Mesh>(kUnitQuad);
            mpUnitSphere = graphics.Create<Mesh>(kUnitSphere);

#           define JZ_HELPER(n) if (!n->IsLoadable()) { throw exception(__FUNCTION__ ": " #n " is not loadable."); }

            JZ_HELPER(mpSimpleEffect);
            JZ_HELPER(mpUnitBox);
            JZ_HELPER(mpUnitFrustum);
            JZ_HELPER(mpUnitQuad);
            JZ_HELPER(mpUnitSphere);

#           undef JZ_HELPER

            mpDeferred = new Deferred();
            mpReflectionMan = new ReflectionMan();
            mpShadowMan = new ShadowMan();
        }

        RenderMan::~RenderMan()
        {
            SafeDelete(mpShadowMan);
            SafeDelete(mpReflectionMan);
            SafeDelete(mpDeferred);

            mpUnitSphere.Reset();
            mpUnitQuad.Reset();
            mpUnitFrustum.Reset();
            mpUnitBox.Reset();

            mpSimpleEffect.Reset();
        }

        // Temp:
        void RenderMan::AddConsoleLine(const string& s)
        {
            mConsoleText += (s + "\n");
        }
        // End temp:

        graphics::Mesh* RenderMan::GetUnitBoxMesh() const
        {
            return (mpUnitBox.Get());
        }

        graphics::Mesh* RenderMan::GetUnitQuadMesh() const
        {
            return (mpUnitQuad.Get());
        }

        graphics::Mesh* RenderMan::GetUnitFrustumMesh() const
        {
            return (mpUnitFrustum.Get());
        }

        graphics::Mesh* RenderMan::GetUnitSphereMesh() const
        {
            return (mpUnitSphere.Get());
        }

        inline float SortForOpaque(float v) { return -v; }
        inline float SortForTransparent(float v) { return v; }

        void RenderMan::Pose(const graphics::RenderPack& r)
        {
            using namespace graphics;
            if ((r.Flags & RenderPack::kTransparent) != 0)
            {
                float sort = SortForTransparent(r.Sort);
                RenderNode* node = ((r.Flags & RenderPack::kReflection) != 0) ? &mRenderReflectionTransparent : &mRenderTransparent;
                if (r.PreEffectFunc) { node = node->AdoptSorted(r.PreEffectFunc, r.PreEffectFuncParam, sort); }
                if (r.pEffect.IsValid()) { node = node->AdoptSorted(RenderOperations::SetEffect, r.pEffect.Get(), sort); }
                if (r.PostEffectFunc) { node = node->AdoptSorted(r.PostEffectFunc, r.PostEffectFuncParam, sort); }
                if (r.EffectTechnique.IsValid()) { node = node->AdoptSorted(RenderOperations::SetEffectTechnique, graphics::Technique::ToVoidP(r.EffectTechnique), sort); }
                if (r.pVertexDeclaration.IsValid()) { node = node->AdoptSorted(RenderOperations::SetVertexDeclaration, r.pVertexDeclaration.Get(), sort); }
                if (r.pMaterial.IsValid()) { node = node->AdoptSorted(RenderOperations::SetMaterial, r.pMaterial.Get(), sort); }
                if (r.pMesh.IsValid()) { node = node->AdoptSorted(RenderOperations::SetMesh, r.pMesh.Get(), sort); }
                if (r.DrawFunc) { node = node->AdoptAndUpdateSort(r.DrawFunc, r.DrawFuncParam, sort); }
            }
            else if ((r.Flags & RenderPack::kOpaque) != 0)
            {
                float sort = SortForOpaque(r.Sort);
                RenderNode* node = ((r.Flags & RenderPack::kReflection) != 0) ? &mRenderReflectionOpaque : &mRenderOpaque;
                if (r.PreEffectFunc) { node = node->AdoptAndUpdateSort(r.PreEffectFunc, r.PreEffectFuncParam, sort); }
                if (r.pEffect.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffect, r.pEffect.Get(), sort); }
                if (r.PostEffectFunc) { node = node->AdoptAndUpdateSort(r.PostEffectFunc, r.PostEffectFuncParam, sort); }
                if (r.EffectTechnique.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffectTechnique, graphics::Technique::ToVoidP(r.EffectTechnique), sort); }
                if (r.pVertexDeclaration.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetVertexDeclaration, r.pVertexDeclaration.Get(), sort); }
                if (r.pMaterial.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMaterial, r.pMaterial.Get(), sort); }
                if (r.pMesh.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMesh, r.pMesh.Get(), sort); }
                if (r.DrawFunc) { node = node->AdoptAndUpdateSort(r.DrawFunc, r.DrawFuncParam, sort); }
            }
            else if ((r.Flags & RenderPack::kShadow) != 0)
            {
                float sort = SortForOpaque(r.Sort);
                RenderNode* node = &mRenderShadow;
                if (r.PreEffectFunc) { node = node->AdoptAndUpdateSort(r.PreEffectFunc, r.PreEffectFuncParam, sort); }
                if (r.pEffect.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffect, r.pEffect.Get(), sort); }
                if (r.PostEffectFunc) { node = node->AdoptAndUpdateSort(r.PostEffectFunc, r.PostEffectFuncParam, sort); }
                if (r.EffectTechnique.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffectTechnique, graphics::Technique::ToVoidP(r.EffectTechnique), sort); }
                if (r.pVertexDeclaration.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetVertexDeclaration, r.pVertexDeclaration.Get(), sort); }
                if (r.pMaterial.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMaterial, r.pMaterial.Get(), sort); }
                if (r.pMesh.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMesh, r.pMesh.Get(), sort); }
                if (r.DrawFunc) { node = node->AdoptAndUpdateSort(r.DrawFunc, r.DrawFuncParam, sort); }
            }
        }

        void RenderMan::Render()
        {
            using namespace graphics;
            Graphics& graphics = Graphics::GetSingleton();

            if (graphics.Begin(mClearColor, !mpDeferred->bActive()))
            {
                if (mpDeferred->bActive())
                {
                    graphics.BeginGraphicsEventMark("Shadow map generation.");
                    mpShadowMan->Pre();
                    mRenderShadow.RenderChildren();
                    mpShadowMan->Post();
                    graphics.EndGraphicsEventMark();

                    graphics.BeginGraphicsEventMark("Reflection map generation.");
                    SetStandardParameters();
                    mpReflectionMan->Pre();
                    graphics.BeginGraphicsEventMark("Opaque.");
                    mRenderReflectionOpaque.RenderChildren();
                    graphics.EndGraphicsEventMark();
                    graphics.BeginGraphicsEventMark("Transparent.");
                    mRenderReflectionTransparent.RenderChildren();
                    graphics.EndGraphicsEventMark();
                    mpReflectionMan->Post();
                    graphics.EndGraphicsEventMark();

                    graphics.BeginGraphicsEventMark("Main pass.");
                    mpDeferred->Begin(mClearColor);
                }
                else
                {
                    graphics.BeginGraphicsEventMark("Main pass.");
                    SetStandardParameters();
                }

                graphics.BeginGraphicsEventMark("Opaque.");
                mRenderOpaque.RenderChildren();
                graphics.EndGraphicsEventMark();

                graphics.BeginGraphicsEventMark("Transparent.");
                if (mpDeferred->bActive()) { mpDeferred->PreTransparency(); }
                mRenderTransparent.RenderChildren();
                graphics.EndGraphicsEventMark();
                if (mpDeferred->bActive()) { mpDeferred->End(); }
                graphics.EndGraphicsEventMark();

                // Temp:
                if (mConsoleText.size() > 0u)
                {
                    if (mpConsole->IsReset()) 
                    {
                        mpConsole->RenderText(mConsoleText.c_str(),
                            0, 0, graphics.GetViewportWidth(), (natural)(graphics.GetViewportHeight() * 0.5), 0, ColorRGBA::kWhite);
                    }
                    mConsoleText.clear();
                }
                // End Temp:

                graphics.End();

                mPrevView = mView;
                 mPrevProjection = mProjection;
            }

            _ResetTrees();
        }

        void RenderMan::_ResetTrees()
        {
            mRenderOpaque.Reset();
            mRenderReflectionOpaque.Reset();
            mRenderReflectionTransparent.Reset();
            mRenderShadow.Reset();
            mRenderTransparent.Reset();
            graphics::RenderNode::ResetGlobal();
        }

        void RenderMan::SetStandardParameters()
        {
            using namespace graphics;
            Graphics& g = Graphics::GetSingleton();

            float width = (float)g.GetViewportWidth();
            float height = (float)g.GetViewportHeight();

            Vector2 nf;
            Matrix4::ExtractNearFarDirectX(mProjection, nf.X, nf.Y);

            float fly = 1.0f / Tan(Matrix4::ExtractFov(GetProjection()) * 0.5f);
            float flx = fly * (width / height);

            mpSimpleEffect->SetGamma(mGamma);
            mpSimpleEffect->SetCameraFocalLength(Vector2(flx, fly));
            mpSimpleEffect->SetScreenDimensions(Vector2(width, height));
            mpSimpleEffect->SetTime(system::Time::GetSingleton().GetSeconds());

            mpSimpleEffect->SetProjection(mProjection);
            mpSimpleEffect->SetView(mView);
        }

    }
}
