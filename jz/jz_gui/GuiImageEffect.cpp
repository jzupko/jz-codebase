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

#include <jz_gui/GuiImageEffect.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/Texture.h>

namespace jz
{
    namespace gui
    {

        static const char* kInstanceDataParameter = "jz_InstanceData";

        GuiImageEffect::GuiImageEffect(const string& aFilename)
            : Effect(aFilename)
        {}

        GuiImageEffect::~GuiImageEffect()
        {}

        void GuiImageEffect::SetInstanceData(const MemoryBuffer<Vector4>& v, size_t offset, size_t size)
        {
            mInstanceDataParameter.Set(v, offset, size);
        }

        graphics::IObject::State GuiImageEffect::_Load()
        {
            using namespace graphics;
            Effect::_Load();
            if (IsError()) { return (mInternalState); }

            GetParameterBySemantic(kInstanceDataParameter, mInstanceDataParameter);

#           define JZ_HELPER(name) m##name##Technique = GetTechniqueByName(#name)
            JZ_HELPER(Opaque);
            JZ_HELPER(Transparent);
#           undef JZ_HELPER

            return (kLost);
        }

        void GuiImageEffect::_Clear()
        {
#           define JZ_HELPER(name) m##name##Technique.Reset();
            JZ_HELPER(Opaque);
            JZ_HELPER(Transparent);
#           undef JZ_HELPER

            mInstanceDataParameter.Reset();
        }

        graphics::IObject::State GuiImageEffect::_Unload()
        {
            _Clear();
            return Effect::_Unload();
        }

    }
}
