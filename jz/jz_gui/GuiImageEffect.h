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
#ifndef _JZ_GUI_GUI_IMAGE_EFFECT_H_
#define _JZ_GUI_GUI_IMAGE_EFFECT_H_

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

    namespace gui
    {

        class GuiImageEffect : public graphics::Effect
        {
        public:
#           define JZ_HELPER(name) private: graphics::Technique m##name##Technique; public: graphics::Technique Get##name##Technique() const { return m##name##Technique; }
            JZ_HELPER(Opaque);
            JZ_HELPER(Transparent);
#           undef JZ_HELPER

            void SetInstanceData(const MemoryBuffer<Vector4>& v, size_t offset, size_t size);

        protected:
            GuiImageEffect(const string& aFilename);
            JZ_EXPORT virtual ~GuiImageEffect();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;

            graphics::Parameter<Vector4> mInstanceDataParameter;

        private:
            friend class graphics::Graphics;
            friend void jz::__IncrementRefCount<GuiImageEffect>(GuiImageEffect* p);
            friend void jz::__DecrementRefCount<GuiImageEffect>(GuiImageEffect* p);

            void _Clear();

            GuiImageEffect(const GuiImageEffect&);
            GuiImageEffect& operator=(const GuiImageEffect&);
        };

        typedef AutoPtr<GuiImageEffect> GuiImageEffectPtr;

    }
}

#endif
