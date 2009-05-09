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
#include <jz_engine_3D/ShadowMan.h>
#include <jz_graphics/DepthStencilSurface.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/Viewport.h>

namespace jz
{
    engine_3D::ShadowMan* engine_3D::ShadowMan::mspSingleton = null;
    namespace engine_3D
    {

        static const graphics::DepthStencilSurface::Format kShadowDsFormat = graphics::DepthStencilSurface::kD16;
        static const graphics::Target::Format kShadowTargetFormat = graphics::Target::kR16F;

        ShadowMan::ShadowMan()
            : mbAfterPre(false)
        { 
            mpDsSurface = graphics::Graphics::GetSingleton().Create<graphics::DepthStencilSurface>(ShadowManConstants::kShadowTargetDimension, ShadowManConstants::kShadowTargetDimension, kShadowDsFormat);
            mpShadowTarget = graphics::Graphics::GetSingleton().Create<graphics::Target>(ShadowManConstants::kShadowTargetDimension, ShadowManConstants::kShadowTargetDimension, kShadowTargetFormat);

            for (int i = 0; i < (ShadowManConstants::kMapsPerDimension * ShadowManConstants::kMapsPerDimension); i++)
            {
                mFreeList.push_back(i);
            }
        }

        ShadowMan::~ShadowMan()
        { }

        Vector4 ShadowMan::GetScaleShift(int aHandle) const
        {
            JZ_ASSERT(aHandle >= 0 && aHandle < (ShadowManConstants::kMapsPerDimension * ShadowManConstants::kMapsPerDimension));

            const float xScale =  (float)ShadowManConstants::kSizePadding / (float)ShadowManConstants::kShadowTargetDimension;
            const float yScale =  (float)ShadowManConstants::kSizePadding / (float)ShadowManConstants::kShadowTargetDimension;
            const float xShift = (float)(((aHandle % ShadowManConstants::kMapsPerDimension) * ShadowManConstants::kSize) + ShadowManConstants::kPadding) / (float)ShadowManConstants::kShadowTargetDimension;
            const float yShift = (float)(((aHandle / ShadowManConstants::kMapsPerDimension) * ShadowManConstants::kSize) + ShadowManConstants::kPadding) / (float)ShadowManConstants::kShadowTargetDimension;

            Vector4 v = Vector4(xScale, yScale, xShift, yShift);

            return v;
        }

        Vector2 ShadowMan::GetShadowDelta() const
        {
            const float x = (1.0f / (float)ShadowManConstants::kShadowTargetDimension);
            const float y = (1.0f / (float)ShadowManConstants::kShadowTargetDimension);

            Vector2 ret = Vector2(x, y);

            return ret;
        }

        Matrix4 ShadowMan::GetShadowPostTransform() const
        {
            const Matrix4 ret(
                0.5f,  0, 0, 0,
                0, -0.5f, 0, 0,
                0,     0, 1, 0,
                0.5f + (0.5f * (1.0f / (float)ShadowManConstants::kShadowTargetDimension)), 0.5f + (0.5f * (1.0f / (float)ShadowManConstants::kShadowTargetDimension)), 0, 1);

            return ret;
        }

        graphics::DepthStencilSurface* ShadowMan::GetShadowDepthStencil() const
        {
            return (mpDsSurface.Get());
        }

        graphics::Target* ShadowMan::GetShadowTarget() const
        {
            return (mpShadowTarget.Get());
        }

        void ShadowMan::Pre()
        {
            using namespace graphics;
            mpShadowTarget->SetToDevice(0u);
            mpDsSurface->SetToDevice();
            Graphics::GetSingleton().Clear(Graphics::kColor | Graphics::kDepth, ColorRGBA::kWhite, 1.0f);

            mbAfterPre = true;
        }

        void ShadowMan::SetActive(int aHandle)
        {
            using namespace graphics;
            JZ_ASSERT(mbAfterPre);
            JZ_ASSERT(aHandle >= 0 && aHandle < (ShadowManConstants::kMapsPerDimension * ShadowManConstants::kMapsPerDimension));

            Viewport vp;
            int x = (aHandle % ShadowManConstants::kMapsPerDimension);
            int y = (aHandle / ShadowManConstants::kMapsPerDimension);

            vp.X = (x * ShadowManConstants::kSize) + (ShadowManConstants::kPadding);
            vp.Y = (y * ShadowManConstants::kSize) + (ShadowManConstants::kPadding);
            vp.Height = ShadowManConstants::kSizePadding;
            vp.MaxZ = 1.0f;
            vp.MinZ = 0.0f;
            vp.Width = ShadowManConstants::kSizePadding;

            Graphics::GetSingleton().SetViewport(vp);
        }

        void ShadowMan::Post()
        {
            using namespace graphics;
            mpDsSurface->RestorePrevToDevice();
            Target::ResetTarget(0u);

            Graphics& graphics = Graphics::GetSingleton();
            
            Viewport vp;
            vp.X = 0;
            vp.Y = 0;
            vp.Height = (u16)graphics.GetViewportHeight();
            vp.MaxZ = 1.0f;
            vp.MinZ = 0.0f;
            vp.Width = (u16)graphics.GetViewportWidth();

            Graphics::GetSingleton().SetViewport(vp);

            mbAfterPre = false;
        }

        int ShadowMan::Grab()
        {
            if (mFreeList.size() > 0u)
            {
                int ret = (mFreeList[mFreeList.size() - 1u]);
                mFreeList.pop_back();

                return ret;
            }
            else
            {
                return -1;
            }
        }

        void ShadowMan::Release(int& aHandle)
        {
    #   ifndef NDEBUG
            JZ_ASSERT(aHandle >= 0 && aHandle < (ShadowManConstants::kMapsPerDimension * ShadowManConstants::kMapsPerDimension));
            for (size_t i = 0u; i < mFreeList.size(); i++)
            {
                JZ_ASSERT(mFreeList[i] != aHandle);
            }
    #   endif

            mFreeList.push_back(aHandle);
            aHandle = -1;
        }

    }
}
