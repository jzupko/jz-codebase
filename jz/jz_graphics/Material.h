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
#ifndef _JZ_GRAPHICS_MATERIAL_H_
#define _JZ_GRAPHICS_MATERIAL_H_

#include <jz_core/Event.h>
#include <jz_graphics/IObject.h>
#include <string>
#include <vector>

namespace jz
{
    struct Matrix4;
    struct Vector2;
    struct Vector3;
    struct Vector4;
    namespace graphics
    {
        class Texture;
        class VolatileTexture;
        class Graphics;
        class Effect; typedef AutoPtr<Effect> EffectPtr;
        class IMaterialParameter abstract
        {
        public:
            virtual ~IMaterialParameter() {}

            virtual string GetSemantic() const = 0;
            virtual void Apply() = 0;
            virtual void AttachTo(Effect* p) = 0;
            virtual bool Validate(Effect* p) = 0;
        };

        class Material : public IObject
        {
        public:
            static const int SortOrder = 1;

            void AttachTo(Effect* p);
            void Apply() const;

            virtual void Save() const;

            void AddParameter(const string& s, bool v);
            void AddParameter(const string& s, float v);
            void AddParameter(const string& s, const Matrix4& v);
            void AddParameter(const string& s, const Vector2& v);
            void AddParameter(const string& s, const Vector3& v);
            void AddParameter(const string& s, const Vector4& v);
            void AddParameter(const string& s, Texture* v);
            void AddParameter(const string& s, VolatileTexture* v);

        protected:
            JZ_EXPORT virtual ~Material();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;

            bool mbDirty;
            EffectPtr mpAttachedTo;
            vector<IMaterialParameter*> mParameters;

        private:
            friend class Graphics;
            friend void jz::__IncrementRefCount<graphics::Material>(graphics::Material* p);
            friend void jz::__DecrementRefCount<graphics::Material>(graphics::Material* p);

            void _Attach();
            void _DestroyParameters();

            Material(const string& aFilename);

            Material(const Material&);
            Material& operator=(const Material&);
        };

        typedef AutoPtr<Material> MaterialPtr;

    }
}

#endif
