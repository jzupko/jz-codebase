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
#ifndef _JZ_GRAPHICS_PARAMETER_H_
#define _JZ_GRAPHICS_PARAMETER_H_

#include <jz_core/Auto.h>
#include <jz_core/Color.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Memory.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <jz_core/Vector4.h>

namespace jz
{
    namespace graphics
    {

        class Effect;
        class Target; typedef AutoPtr<Target> TargetPtr;
        class Texture; typedef AutoPtr<Texture> TexturePtr;
        class VolatileTexture; typedef AutoPtr<VolatileTexture> VolatileTexturePtr;

        void __SetParameterValue(Handle e, Handle p, bool v);
        void __SetParameterValue(Handle e, Handle p, float v);
        void __SetParameterValue(Handle e, Handle p, voidc_p v, size_t aSizeInBytes);
        void __SetParameterValue(Handle e, Handle p, float const* v, size_t aNumberOfFloats);
        void __SetParameterValue(Handle e, Handle p, const Matrix4& v);
        void __SetParameterValue(Handle e, Handle p, Matrix4 const* v, size_t aNumberOfMatrix4);
        void __SetParameterValue(Handle e, Handle p, Vector4 const* v, size_t aNumberOfVector4);
        void __SetParameterValue(Handle e, Handle p, Target* v);
        void __SetParameterValue(Handle e, Handle p, Texture* v);
        void __SetParameterValue(Handle e, Handle p, VolatileTexture* v);

#       define JZ_EFFECT_PARAM_HELPER \
            public: \
                JZ_ALIGNED_NEW \
                Parameter() {} \
                ~Parameter() {} \
                bool IsValid() const { return (mEffect && mHandle); } \
                void Reset() { mEffect.Reset(); mHandle.Reset(); } \
            private: \
                friend class Effect; \
                Handle mEffect; \
                Handle mHandle; \
                Parameter(const Parameter&); \
                Parameter& operator=(const Parameter&); \
            public:

        template <typename T>
        class Parameter sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const T& v) const { __SetParameterValue(mEffect, mHandle, (voidc_p)(&v), sizeof(T)); }
        };

        template <>
        class Parameter<bool> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(bool v) const { __SetParameterValue(mEffect, mHandle, v); }
        };

        template <>
        class Parameter<float> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(float v) const { __SetParameterValue(mEffect, mHandle, v); }

            template <int I>
            void Set(float v[I]) const
            {
                Set<I>(v, 0u, I);
            }
            template <int I>
            void Set(float v[I], size_t aOffset, size_t aSize) const
            {
                JZ_ASSERT(aOffset < I);
                JZ_ASSERT((aOffset + aSize) <= I);

                __SetParameterValue(mEffect, mHandle, (float const*)(v + aOffset), aSize);
            }

            void Set(const MemoryBuffer<float>& v) const
            {
                Set(v, 0u, v.size());
            }
            void Set(const MemoryBuffer<float>& v, size_t aOffset, size_t aSize) const
            {
                JZ_ASSERT(aOffset < v.size());
                JZ_ASSERT((aOffset + aSize) <= v.size());

                __SetParameterValue(mEffect, mHandle, (float const*)(v.Get() + aOffset), aSize);
            }
        };

        template <>
        class Parameter<ColorRGB> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const ColorRGB& v) const
            {
                __SetParameterValue(mEffect, mHandle, v.pData, 3);
            }
        };

        template <>
        class Parameter<Matrix4> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const Matrix4& v) const
            {    
                __SetParameterValue(mEffect, mHandle, v);
            }

            void Set(const MemoryBuffer<Matrix4>& v) const
            {
                Set(v, 0u, v.size());
            }
            void Set(const MemoryBuffer<Matrix4>& v, size_t aOffset, size_t aSize) const
            {
                JZ_ASSERT(aOffset < v.size());
                JZ_ASSERT((aOffset + aSize) <= v.size());

                __SetParameterValue(mEffect, mHandle, (v.Get() + aOffset), aSize);
            } 
        };

        template <>
        class Parameter<Target> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const TargetPtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
            void Set(const TexturePtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
            void Set(const VolatileTexturePtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
        };

        template <>
        class Parameter<Texture> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const TargetPtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
            void Set(const TexturePtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
            void Set(const VolatileTexturePtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
        };

        template <>
        class Parameter<VolatileTexture> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const TargetPtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
            void Set(const TexturePtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
            void Set(const VolatileTexturePtr& v) const { __SetParameterValue(mEffect, mHandle, v.Get()); }
        };

        template <>
        class Parameter<Vector2> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const Vector2& v) const
            {
                __SetParameterValue(mEffect, mHandle, v.pData, 2);
            }
        };

        template <>
        class Parameter<Vector3> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const Vector3& v) const
            {
                __SetParameterValue(mEffect, mHandle, v.pData, 3);
            }
        };

        template <>
        class Parameter<Vector4> sealed
        {
            JZ_EFFECT_PARAM_HELPER

            void Set(const Vector4& v) const
            {
                __SetParameterValue(mEffect, mHandle, v.pData, 4);
            }

            template <int I>
            void Set(Vector4 v[I]) const
            {
                Set<I>(v, 0u, I);
            }
            template <int I>
            void Set(Vector4 v[I], size_t aOffset, size_t aSize) const
            {
                JZ_ASSERT(aOffset < I);
                JZ_ASSERT((aOffset + aSize) <= I);

                __SetParameterValue(mEffect, mHandle, (v + aOffset), aSize);
            }

            void Set(const MemoryBuffer<Vector4>& v) const
            {
                Set(v, 0u, v.size());
            }
            void Set(const MemoryBuffer<Vector4>& v, size_t aOffset, size_t aSize) const
            {
                JZ_ASSERT(aOffset < v.size());
                JZ_ASSERT((aOffset + aSize) <= v.size());

                __SetParameterValue(mEffect, mHandle, (v.Get() + aOffset), aSize);
            }
        };

#       undef JZ_EFFECT_PARAM_HELPER

    }
}

#endif
