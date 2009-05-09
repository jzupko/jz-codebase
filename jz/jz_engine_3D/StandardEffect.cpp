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
#include <jz_core/Memory.h>
#include <jz_engine_3D/StandardEffect.h>
#include <jz_engine_3D/ThreePoint.h>
#include <jz_system/Files.h>

namespace jz
{
    namespace engine_3D
    {

        static const char* kStandardPickingTechnique = "jz_Pick";
        static const char* kStandardRenderTechnique = "jz_Render";
        static const char* kStandardReflectionTechnique = "jz_RenderReflection";
        static const char* kStandardRenderNonDeferredTechnique = "jz_RenderNonDeferred";
        static const char* kStandardShadowingTechnique = "jz_Shadow";
        static const char* kConstantColorParameter = "jz_ConstantColor";
        static const char* kStandardSkinningParameter = "jz_Skinning";
        static const char* kStandardThreePointParameter = "jz_ThreePoint";
        static const char* kStandardWitParameter = "jz_Wit";
        static const char* kStandardWorldParameter = "jz_World";
        static const char* kStandardIsTransparentAnnotation = "jz_IsTransparent";
        static const char* kStandardIsAlpha1BitAnnotation = "jz_IsAlpha1Bit";

        namespace StandardEffectFlags
        {
            enum Enum
            {
                kAnimateable = (1 << 0),
                kTransparent = (1 << 1),
                kAlpha1Bit = (1 << 2),
                kShadowable = (1 << 3),
                kThreePoint = (1 << 4),
                kNonDeferred = (1 << 5),
                kPickable = (1 << 6),
                kReflection = (1 << 7)
            };
        }

        StandardEffect::StandardEffect(const string& aFilename)
            : graphics::Effect(aFilename), mFlags(0u)
        {}
        
        StandardEffect::~StandardEffect()
        {}

        bool StandardEffect::IsAlpha1Bit() const
        {
            return ((mFlags & StandardEffectFlags::kAlpha1Bit) != 0);
        }

        bool StandardEffect::IsAnimated() const
        {
            return ((mFlags & StandardEffectFlags::kAnimateable) != 0);
        }

        bool StandardEffect::IsPickable() const
        {
            return ((mFlags & StandardEffectFlags::kPickable) != 0);
        }

        bool StandardEffect::IsShadowable() const
        {
            return ((mFlags & StandardEffectFlags::kShadowable) != 0);
        }

        bool StandardEffect::IsThreePoint() const
        {
            return ((mFlags & StandardEffectFlags::kThreePoint) != 0);
        }

        bool StandardEffect::IsTransparent() const
        {
            return ((mFlags & StandardEffectFlags::kTransparent) != 0);
        }

        bool StandardEffect::IsNonDeferred() const
        {
            return ((mFlags & StandardEffectFlags::kNonDeferred) != 0);
        }

        bool StandardEffect::IsReflectable() const
        {
            return ((mFlags & StandardEffectFlags::kReflection) != 0);
        }

        void StandardEffect::SetConstantColor(const ColorRGBA& c)
        {
            mConstantColorParameter.Set(c);
        }

        void StandardEffect::SetSkinning(const MemoryBuffer<Vector4>& m)
        {
            mSkinningParameter.Set(m);
        }

        void StandardEffect::SetThreePoint(const ThreePoint& tp)
        {
            mThreePointParameter.Set(tp);
        }

        void StandardEffect::SetWit(const Matrix4& m)
        {
            mWitParameter.Set(m);
        }

        void StandardEffect::SetWorld(const Matrix4& m)
        {
            mWorldParameter.Set(m);
        }

        graphics::IObject::State StandardEffect::_Load()
        {
            graphics::Effect::_Load();
            if (IsError()) { return (mInternalState); }

            try
            {
    #           define JZ_HELPER_P(p, n) \
                    GetParameterBySemantic(n, p); \
                    JZ_E_ON_FAIL(p.IsValid(), #p " not found in effect.")

    #           define JZ_HELPER_T(t, n) \
                    t = GetTechniqueByName(n); \
                    JZ_E_ON_FAIL(t.IsValid(), #t " not found in effect.")

                JZ_HELPER_T(mRenderTechnique, kStandardRenderTechnique);
                JZ_HELPER_P(mWitParameter, kStandardWitParameter);
                JZ_HELPER_P(mWorldParameter, kStandardWorldParameter);

    #           undef JZ_HELPER_T
    #           undef JZ_HELPER_P

                if (GetAnnotationByName(mRenderTechnique, kStandardIsTransparentAnnotation).IsValid())
                {
                    mFlags |= StandardEffectFlags::kTransparent;
                }

                if (GetAnnotationByName(mRenderTechnique, kStandardIsAlpha1BitAnnotation).IsValid())
                {
                    mFlags |= StandardEffectFlags::kAlpha1Bit;
                }

                mPickingTechnique = GetTechniqueByName(kStandardPickingTechnique);
                if (mPickingTechnique.IsValid())
                {
                    mFlags |= StandardEffectFlags::kPickable;
                }

                mReflectionTechnique = GetTechniqueByName(kStandardReflectionTechnique);
                if (mReflectionTechnique.IsValid())
                {
                    mFlags |= StandardEffectFlags::kReflection;
                }

                mShadowTechnique = GetTechniqueByName(kStandardShadowingTechnique);
                if (mShadowTechnique.IsValid())
                {
                    mFlags |= StandardEffectFlags::kShadowable;
                }

                GetParameterBySemantic(kConstantColorParameter, mConstantColorParameter);

                GetParameterBySemantic(kStandardSkinningParameter, mSkinningParameter);
                if (mSkinningParameter.IsValid())
                {
                    mFlags |= StandardEffectFlags::kAnimateable;
                }

                GetParameterBySemantic(kStandardThreePointParameter, mThreePointParameter);
                if (mThreePointParameter.IsValid())
                {
                    mFlags |= StandardEffectFlags::kThreePoint;
                }

                mNonDeferredTechnique = GetTechniqueByName(kStandardRenderNonDeferredTechnique);
                if (mNonDeferredTechnique.IsValid())
                {
                    mFlags |= StandardEffectFlags::kNonDeferred;
                }
            }
            catch (exception&)
            {
                _Clear();
                graphics::Effect::_Unload();
                return (kErrorDataRead);
            }

            return (kLost);
        }

        void StandardEffect::_Clear()
        {
            mFlags = 0u;
            mPickingTechnique.Reset();
            mShadowTechnique.Reset();
            mReflectionTechnique.Reset();
            mRenderTechnique.Reset();
            mConstantColorParameter.Reset();
            mSkinningParameter.Reset();
            mThreePointParameter.Reset();
            mWitParameter.Reset();
            mWorldParameter.Reset();
        }

        graphics::IObject::State StandardEffect::_Unload()
        {
            _Clear();

            return graphics::Effect::_Unload();
        }

    }
}
