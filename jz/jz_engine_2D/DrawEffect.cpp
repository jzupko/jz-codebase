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

#include <jz_engine_2D/DrawEffect.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/Texture.h>

namespace jz
{
    namespace engine_2D
    {

        static const char* kDiffuseTextureParameter = "jz_Diffuse";
        static const char* kInstanceDataParameter = "jz_InstanceData";
        static const char* kLightAttenuationParameter = "jz_LightAttenuation";
        static const char* kLightColorParameter = "jz_LightColor";
        static const char* kLightVParameter = "jz_LightV";

        DrawEffect::DrawEffect(const string& aFilename)
            : Effect(aFilename)
        {}

        DrawEffect::~DrawEffect()
        {}

        void DrawEffect::SetDiffuseTexture(graphics::Target* p)
        {
            mDiffuseTextureParameter.Set(p);
        }

        void DrawEffect::SetDiffuseTexture(graphics::Texture* p)
        {
            mDiffuseTextureParameter.Set(p);
        }

        void DrawEffect::SetInstanceData(const MemoryBuffer<Vector4>& v, size_t offset, size_t size)
        {
            mInstanceDataParameter.Set(v, offset, size);
        }

        void DrawEffect::SetLightAttenuation(float v)
        {
            mLightAttenuationParameter.Set(v);
        }

        void DrawEffect::SetLightColor(const ColorRGB& v)
        {
            mLightColorParameter.Set(v);
        }

        void DrawEffect::SetLightV(const Vector3& v)
        {
            mLightVParameter.Set(v);
        }

        graphics::IObject::State DrawEffect::_Load()
        {
            using namespace graphics;
            Effect::_Load();
            if (IsError()) { return (mInternalState); }

            GetParameterBySemantic(kDiffuseTextureParameter, mDiffuseTextureParameter);
            GetParameterBySemantic(kInstanceDataParameter, mInstanceDataParameter);
            GetParameterBySemantic(kLightAttenuationParameter, mLightAttenuationParameter);
            GetParameterBySemantic(kLightColorParameter, mLightColorParameter);
            GetParameterBySemantic(kLightVParameter, mLightVParameter);

#           define JZ_HELPER(name) m##name##Technique = GetTechniqueByName(#name)

            JZ_HELPER(BaseOpaque);
            JZ_HELPER(BaseTransparent);
            JZ_HELPER(HdrToLdr);
            JZ_HELPER(OpaqueDirectionalLight);
            JZ_HELPER(OpaquePointLight);
            JZ_HELPER(ShadowVolume);
            JZ_HELPER(TransparentDirectionalLight);
            JZ_HELPER(TransparentPointLight);

#           undef JZ_HELPER

            return (kLost);
        }

        void DrawEffect::_Clear()
        {
#           define JZ_HELPER(name) m##name##Technique.Reset();
            
            JZ_HELPER(BaseOpaque);
            JZ_HELPER(BaseTransparent);
            JZ_HELPER(HdrToLdr);
            JZ_HELPER(OpaqueDirectionalLight);
            JZ_HELPER(OpaquePointLight);
            JZ_HELPER(ShadowVolume);
            JZ_HELPER(TransparentDirectionalLight);
            JZ_HELPER(TransparentPointLight);

#           undef JZ_HELPER

            mLightVParameter.Reset();
            mLightColorParameter.Reset();
            mLightAttenuationParameter.Reset();
            mInstanceDataParameter.Reset();
            mDiffuseTextureParameter.Reset();
        }

        graphics::IObject::State DrawEffect::_Unload()
        {
            _Clear();
            return Effect::_Unload();
        }

    }
}
