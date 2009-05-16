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
#ifndef _JZ_ENGINE_3D_SHADOWMAN_H_
#define _JZ_ENGINE_3D_SHADOWMAN_H_

#include <jz_core/Auto.h>
#include <jz_core/Utility.h>
#include <jz_core/Vector4.h>
#include <vector>

namespace jz
{
    namespace graphics
    {
        class DepthStencilSurface; typedef AutoPtr<DepthStencilSurface> DepthStencilSurfacePtr;
        class Target; typedef AutoPtr<Target> TargetPtr;
    }

    namespace engine_3D
    {

        namespace ShadowManConstants
        {
            static const int kShadowTargetDimension = 2048;
            static const int kMapsPerDimension = 4;
            static const int kPadding = 10; // allows for filtering operations on the shadow map target.
            static const int kSize = (kShadowTargetDimension / kMapsPerDimension);
            static const int kSizePadding = (kSize) - (2 * kPadding);
        }

        class ShadowMan sealed : public Singleton<ShadowMan>
        {
        public:
            JZ_ALIGNED_NEW

            ShadowMan();
            ~ShadowMan();

            void Pre();
            void SetActive(int aHandle);
            void Post();

            Vector4 GetScaleShift(int aHandle) const;
            Vector2 GetShadowDelta() const;
            Matrix4 GetShadowPostTransform() const;

            graphics::DepthStencilSurface* GetShadowDepthStencil() const;
            graphics::Target* GetShadowTarget() const;

            int Grab();
            void Release(int& aHandle);

        private:
            bool mbAfterPre;

            ShadowMan(const ShadowMan&);
            ShadowMan& operator=(const ShadowMan&);

            graphics::DepthStencilSurfacePtr mpDsSurface;
            graphics::TargetPtr mpShadowTarget;
            vector<int> mFreeList;
        };

    }
}

#endif
