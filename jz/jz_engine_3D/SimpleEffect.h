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
#ifndef _JZ_ENGINE_3D_SIMPLE_EFFECT_H_
#define _JZ_ENGINE_3D_SIMPLE_EFFECT_H_

#include <jz_engine_3D/StandardEffect.h>
#include <string>

namespace jz
{
    struct Matrix4;
    struct Vector2;
    struct Vector3;
    namespace engine_3D
    {

        class SimpleEffect : public StandardEffect
        {
        public:
    #       define JZ_SIMPLE_SETTER(name, type) void Set##name(const type& v)
    #       include <jz_engine_3D/_SimpleEffectHelper.h>

            void SetGamma(float v);
            void SetTime(float v);

        protected:
            JZ_EXPORT virtual ~SimpleEffect();
            JZ_EXPORT virtual graphics::IObject::State _Load() override;
            JZ_EXPORT virtual graphics::IObject::State _Unload() override;

    #       define JZ_SIMPLE_SETTER(name, type) graphics::Parameter<type> m##name
    #       include <jz_engine_3D/_SimpleEffectHelper.h>

            graphics::Parameter<float> mGamma;
            graphics::Parameter<float> mTime;

        private:
            friend class graphics::Graphics;
            friend void jz::__IncrementRefCount<engine_3D::SimpleEffect>(engine_3D::SimpleEffect* p);
            friend void jz::__DecrementRefCount<engine_3D::SimpleEffect>(engine_3D::SimpleEffect* p);

            void _Clear();

            SimpleEffect(const string& aFilename);

            SimpleEffect(const SimpleEffect&);
            SimpleEffect& operator=(const SimpleEffect&);
        };

        typedef AutoPtr<SimpleEffect> SimpleEffectPtr;

    }
}

#endif
