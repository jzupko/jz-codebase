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
#include <jz_graphics/Graphics.h>
#include <jz_graphics/RenderOps.h>
#include <jz_graphics/SystemMemorySurface.h>
#include <jz_graphics_directx9/DirectX9.h>
#include <jz_engine_3D/PickMan.h>
#include <jz_engine_3D/RenderMan.h>
#include <jz_engine_3D/StandardEffect.h>

namespace jz
{
    engine_3D::PickMan* engine_3D::PickMan::mspSingleton = null;
    namespace engine_3D
    {

        static const ColorRGBAu kDefaultPickColor = { 255, 0, 0, 255 };

        PickMan::PickMan()
            : mPickColor(kDefaultPickColor)
        { 
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
            mpPickSurface = graphics.Create<graphics::SystemMemorySurface>(0, 0, graphics::SystemMemorySurface::kX8R8G8B8);
        }

        PickMan::~PickMan()
        { }

        Vector3 PickMan::Project(const Vector3& v)
        {
            RenderMan& rm = RenderMan::GetSingleton();
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

            Vector4 u = Vector4(v, 1.0f);
            u = Vector4::Transform(rm.GetView() * rm.GetProjection(), Vector4(v, 1.0f));
            u /= u.W;

            float width = (float)graphics.GetViewportWidth();
            float height = (float)graphics.GetViewportHeight();

            float mouseX = ((u.X + 1.0f) * 0.5f) * (width - 1.0f);
            float mouseY = ((u.Y + 1.0f) * 0.5f) * (height - 1.0f);

            Vector3 ret = Vector3(mouseX, mouseY, u.Z);

            return ret;
        }

        Vector3 PickMan::UnProject(const Vector3& v)
        {
            RenderMan& rm = RenderMan::GetSingleton();
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

            Matrix4 ivp = Matrix4::Invert(rm.GetView() * rm.GetProjection());

            natural width = graphics.GetViewportWidth();
            natural height = graphics.GetViewportHeight();

            float x = (float)(2.0 * (((double)v.X) / ((double)(width - 1))) - 1.0);
            float y = (float)(2.0 * (1.0 - (((double)v.Y) / ((double)(height - 1)))) - 1.0);

            Vector4 u = Vector4(x, y, v.Z, 1.0f);
            u = Vector4::Transform(ivp, u);
            u /= u.W;

            return Vector3(u.X, u.Y, u.Z);
        }

        Ray3D PickMan::GetPickingRay(natural x, natural y)
        {
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

            natural width = graphics.GetViewportWidth();
            natural height = graphics.GetViewportHeight();

            Vector3 n = Vector3((float)x, (float)y, 0.0f);
            Vector3 f = Vector3((float)x, (float)y, 1.0f);

            Vector3 wn = UnProject(n);
            Vector3 wf = UnProject(f);

            Vector3 direction = Vector3::Normalize(wf - wn);

            Ray3D ret = Ray3D(wn, direction);

            return ret;
        }

        __inline float SortForOpaque(float v) { return -v; }

        static void SetPickColor(graphics::RenderNode* apNode, voidc_p apInstance)
        {
            ColorRGBAu pickColor = *(reinterpret_cast<ColorRGBAu const*>(&apInstance));
            StandardEffect* p = (StandardEffect*)graphics::Graphics::GetSingleton().GetActiveEffect();

            p->SetConstantColor(pickColor);

            apNode->RenderChildren();
        }

        void PickMan::Pose(const graphics::RenderPack& r, IPickable* apPickable)
        {
            using namespace graphics;

            mPickColor.A = 255;
            mPickColor.R = 255;
            mPickColor.G = (mPickColor.G == 255) ? 0 : ((mPickColor.B == 255) ? (mPickColor.G + 1) : mPickColor.G);
            mPickColor.B = (mPickColor.B == 255) ? 0 : (mPickColor.B + 1);

            mPickTable.insert(make_pair(mPickColor, apPickable));

            float sort = SortForOpaque(r.Sort);
            RenderNode* node = &mRenderPick;
            if (r.PreEffectFunc) { node = node->AdoptAndUpdateSort(r.PreEffectFunc, r.PreEffectFuncParam, sort); }
            if (r.pEffect.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffect, r.pEffect.Get(), sort); }
            if (r.PostEffectFunc) { node = node->AdoptAndUpdateSort(r.PostEffectFunc, r.PostEffectFuncParam, sort); }
            if (r.EffectTechnique.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetEffectTechnique, graphics::Technique::ToVoidP(r.EffectTechnique), sort); }
            if (r.pVertexDeclaration.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetVertexDeclaration, r.pVertexDeclaration.Get(), sort); }
            if (r.pMaterial.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMaterial, r.pMaterial.Get(), sort); }
            if (r.pMesh.IsValid()) { node = node->AdoptAndUpdateSort(RenderOperations::SetMesh, r.pMesh.Get(), sort); }
            node = node->AdoptAndUpdateSort(SetPickColor, mPickColorObject, sort);
            if (r.DrawFunc) { node = node->AdoptAndUpdateSort(r.DrawFunc, r.DrawFuncParam, sort); }
        }

        bool PickMan::Pick(const RectangleU& aRectangle, IPickable*& arpPickable, float& arDepth)
        {
            using namespace graphics;
            bool bReturn = false;

            if (!mPickTable.empty())
            {
                RenderMan& rm = RenderMan::GetSingleton();
                Graphics& graphics = Graphics::GetSingleton();

                if (graphics.Begin(ColorRGBA::kBlack, true))
                {
                    rm.SetStandardParameters();
                    mRenderPick.RenderChildren();
                    mRenderPick.Reset();
                    mPickColor = kDefaultPickColor;

                    mpPickSurface->PopulateFromBackbuffer();
                    graphics.End(false);

                    void_p pData = null;
                    unatural pitch = 0u;

                    mpPickSurface->Lock(aRectangle, SystemMemorySurface::kReadonly, pData, pitch);
                    if (pData)
                    {
                        u8c_p pBits = (u8c_p)(pData);

                        ColorRGBAu pickColor;
                        pickColor.B = pBits[0];
                        pickColor.G = pBits[1];
                        pickColor.R = 255;
                        pickColor.A = 255;

                        float depth = (pBits[2] / 255.0f);
                        mpPickSurface->Unlock();

                        PickTable::iterator I = mPickTable.find(pickColor);
                        if (I != mPickTable.end())
                        {
                            arpPickable = (I->second);
                            arDepth = depth;
                            bReturn = true;
                        }
                    }
                }
            }

            mPickTable.clear();
            return bReturn;
        }

    }
}
