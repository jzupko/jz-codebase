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
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <jz_engine_3D/SimpleEffect.h>

namespace jz
{
    namespace engine_3D
    {

        SimpleEffect::SimpleEffect(const string& aFilename)
            : StandardEffect(aFilename)
        {}
        
        SimpleEffect::~SimpleEffect()
        {}

    #   define JZ_SIMPLE_SETTER(name, type) \
        void SimpleEffect::Set##name(const type& v) { m##name.Set(v); }
    #   include <jz_engine_3D/_SimpleEffectHelper.h>

        void SimpleEffect::SetGamma(float v) { mGamma.Set(v); }
        void SimpleEffect::SetTime(float v) { mTime.Set(v); }

        graphics::IObject::State SimpleEffect::_Load()
        {
            StandardEffect::_Load();
            if (IsError()) { return (mInternalState); }

            try
            {
    #           define JZ_QUOTE(n) #n
    #           define JZ_SIMPLE_PARAM(name) GetParameterBySemantic(JZ_QUOTE(jz_##name), m##name); \
                    JZ_E_ON_FAIL(m##name.IsValid(), #name " parameter not found."); 
    #           include <jz_engine_3D/_SimpleEffectHelper.h>
    #           undef JZ_QUOTE

                GetParameterBySemantic("jz_Gamma", mGamma);
                JZ_E_ON_FAIL(mGamma.IsValid(), "mGamma parameter not found.");

                GetParameterBySemantic("jz_Time", mTime);
                JZ_E_ON_FAIL(mTime.IsValid(), "mTime parameter not found.");
            }
            catch (std::exception&)
            {
                _Clear();
                StandardEffect::_Unload();

                return (kErrorDataRead);
            }

            return (kLost);
        }

        void SimpleEffect::_Clear()
        {
            mTime.Reset();
            mGamma.Reset();

#           define JZ_SIMPLE_PARAM(name) m##name.Reset();
#           include <jz_engine_3D/_SimpleEffectHelper.h>
        }

        graphics::IObject::State SimpleEffect::_Unload()
        {
            _Clear();
            return StandardEffect::_Unload();
        }

    }
}
