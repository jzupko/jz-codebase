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
#ifndef _JZ_ENGINE_2D_DRAWEFFECT_H_
#define _JZ_ENGINE_2D_DRAWEFFECT_H_

#include <jz_core/Memory.h>
#include <jz_core/Vector4.h>
#include <jz_graphics/Effect.h>

namespace jz
{
    namespace graphics
    {
        class Target;
        class Texture;
    }

    namespace engine_2D
    {

        class DrawEffect : public graphics::Effect
        {
        public:
#           define JZ_HELPER(name) private: graphics::Technique m##name##Technique; public: graphics::Technique Get##name##Technique() const { return m##name##Technique; }

            JZ_HELPER(BaseOpaque);
            JZ_HELPER(BaseTransparent);
            JZ_HELPER(HdrToLdr);
            JZ_HELPER(OpaqueDirectionalLight);
            JZ_HELPER(OpaquePointLight);
            JZ_HELPER(ShadowVolume);
            JZ_HELPER(TransparentDirectionalLight);
            JZ_HELPER(TransparentPointLight);

#           undef JZ_HELPER

            void SetDiffuseTexture(graphics::Target* p);
            void SetDiffuseTexture(graphics::Texture* p);
            void SetInstanceData(const MemoryBuffer<Vector4>& v, size_t offset, size_t size);
            void SetLightAttenuation(float v);
            void SetLightColor(const ColorRGB& v);
            void SetLightV(const Vector3& v);

        protected:
            DrawEffect(const string& aFilename);
            JZ_EXPORT virtual ~DrawEffect();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;

            graphics::Parameter<graphics::Texture> mDiffuseTextureParameter;
            graphics::Parameter<Vector4> mInstanceDataParameter;
            graphics::Parameter<float> mLightAttenuationParameter;
            graphics::Parameter<ColorRGB> mLightColorParameter;
            graphics::Parameter<Vector3> mLightVParameter;

        private:
            friend class graphics::Graphics;
            friend void jz::__IncrementRefCount<DrawEffect>(DrawEffect* p);
            friend void jz::__DecrementRefCount<DrawEffect>(DrawEffect* p);

            void _Clear();

            DrawEffect(const DrawEffect&);
            DrawEffect& operator=(const DrawEffect&);
        };

        typedef AutoPtr<DrawEffect> DrawEffectPtr;

    }
}

#endif
