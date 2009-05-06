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
#ifndef _JZ_ENGINE_3D_STANDARD_EFFECT_H_
#define _JZ_ENGINE_3D_STANDARD_EFFECT_H_

#include <jz_graphics/Effect.h>
#include <string>

namespace jz
{
    struct Matrix4;
    namespace engine_3D
    {

        struct ThreePoint;
        class StandardEffect : public graphics::Effect
        {
        public:
            bool IsAlpha1Bit() const;
            bool IsAnimated() const;
            bool IsNonDeferred() const;
            bool IsShadowable() const;
            bool IsThreePoint() const;
            bool IsTransparent() const;
                   
            graphics::Technique GetRenderTechnique() const { return mRenderTechnique; }
            graphics::Technique GetShadowTechnique() const { return mShadowTechnique; }
            graphics::Technique GetNonDeferredTechnique() const { return mNonDeferredTechnique; }

            void SetSkinning(const MemoryBuffer<Vector4>& m);
            void SetThreePoint(const ThreePoint& tp);
            void SetWit(const Matrix4& m);
            void SetWorld(const Matrix4& m);

        protected:
            StandardEffect(const string& aFilename);
            JZ_EXPORT virtual ~StandardEffect();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;

            graphics::Technique mNonDeferredTechnique;
            graphics::Technique mRenderTechnique;
            graphics::Technique mShadowTechnique;
            graphics::Parameter<Vector4> mSkinningParameter;
            graphics::Parameter<ThreePoint> mThreePointParameter;
            graphics::Parameter<Matrix4> mWorldParameter;
            graphics::Parameter<Matrix4> mWitParameter;

        private:
            friend class graphics::Graphics;
            friend void jz::__IncrementRefCount<engine_3D::StandardEffect>(engine_3D::StandardEffect* p);
            friend void jz::__DecrementRefCount<engine_3D::StandardEffect>(engine_3D::StandardEffect* p);

            void _Clear();

            StandardEffect(const StandardEffect&);
            StandardEffect& operator=(const StandardEffect&);

            unatural mFlags;
        };

        typedef AutoPtr<StandardEffect> StandardEffectPtr;

    }
}

#endif
