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
#include <jz_engine_3D/ReflectionMan.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/Viewport.h>

namespace jz
{
    engine_3D::ReflectionMan* engine_3D::ReflectionMan::mspSingleton = null;
    namespace engine_3D
    {

        static const graphics::Target::Format kReflectionTargetFormat = graphics::Target::kA8R8G8B8;

        ReflectionMan::ReflectionMan()
            : mbAfterPre(false)
        { 
            mpReflectionTarget = graphics::Graphics::GetSingleton().Create<graphics::Target>(0, 0, kReflectionTargetFormat);

            for (int i = 0; i < (ReflectionManConstants::kPerDimension * ReflectionManConstants::kPerDimension); i++)
            {
                mFreeList.push_back(i);
            }
        }

        ReflectionMan::~ReflectionMan()
        { }

        Vector4 ReflectionMan::GetScaleShift(int aHandle) const
        {
            JZ_ASSERT(aHandle >= 0 && aHandle < (ReflectionManConstants::kPerDimension * ReflectionManConstants::kPerDimension));
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

            const float width = (float)graphics.GetViewportWidth();
            const float height = (float)graphics.GetViewportHeight();
            const float xSize = (width / ReflectionManConstants::kPerDimension);            
            const float ySize = (height / ReflectionManConstants::kPerDimension);

            const float xScale = 1.0f / ReflectionManConstants::kPerDimension;
            const float yScale = 1.0f / ReflectionManConstants::kPerDimension;
            const float xShift = (float)((aHandle % ReflectionManConstants::kPerDimension) * xSize) / width;
            const float yShift = (float)((aHandle / ReflectionManConstants::kPerDimension) * ySize) / height;

            Vector4 v = Vector4(xScale, yScale, xShift, yShift);

            return v;
        }

        graphics::Target* ReflectionMan::GetReflectionTarget() const
        {
            return (mpReflectionTarget.Get());
        }

        void ReflectionMan::Pre()
        {
            using namespace graphics;
            mpReflectionTarget->SetToDevice(0u);
            Graphics::GetSingleton().Clear(Graphics::kColor | Graphics::kDepth, ColorRGBA::kBlack, 1.0f);

            mbAfterPre = true;
        }

        void ReflectionMan::SetActive(int aHandle)
        {
            using namespace graphics;
            JZ_ASSERT(mbAfterPre);
            JZ_ASSERT(aHandle >= 0 && aHandle < (ReflectionManConstants::kPerDimension * ReflectionManConstants::kPerDimension));
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

            const float width = (float)graphics.GetViewportWidth();
            const float height = (float)graphics.GetViewportHeight();
            const float xSize = (width / ReflectionManConstants::kPerDimension);            
            const float ySize = (height / ReflectionManConstants::kPerDimension);

            Viewport vp;
            int x = (aHandle % ReflectionManConstants::kPerDimension);
            int y = (aHandle / ReflectionManConstants::kPerDimension);

            vp.X = (u16)(x * xSize);
            vp.Y = (u16)(y * ySize);
            vp.Height = (u16)(ySize);
            vp.MaxZ = 1.0f;
            vp.MinZ = 0.0f;
            vp.Width = (u16)(xSize);

            Graphics::GetSingleton().SetViewport(vp);
        }

        void ReflectionMan::Post()
        {
            using namespace graphics;
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

        int ReflectionMan::Grab()
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

        void ReflectionMan::Release(int& aHandle)
        {
    #   ifndef NDEBUG
            JZ_ASSERT(aHandle >= 0 && aHandle < (ReflectionManConstants::kPerDimension * ReflectionManConstants::kPerDimension));
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
