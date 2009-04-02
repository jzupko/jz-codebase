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
#ifndef _JZ_ENGINE_2D_DRAWABLE_SPRITE_H_
#define _JZ_ENGINE_2D_DRAWABLE_SPRITE_H_

#include <jz_core/Auto.h>
#include <jz_core/Matrix2.h>
#include <jz_core/Matrix3.h>
#include <jz_core/Vector4.h>
#include <jz_graphics/RenderPack.h>
#include <jz_engine_2D/IDrawable.h>
#include <jz_engine_2D/ISerializeable.h>

namespace jz
{
    namespace graphics
    {
        class Material; typedef AutoPtr<Material> MaterialPtr;
    }
    
    namespace system
    {
        class IWriteFile; typedef AutoPtr<IWriteFile> IWriteFilePtr;
        class IReadFile; typedef AutoPtr<IReadFile> IReadFilePtr;
    }

    namespace engine_2D
    {

        struct SpriteDrawHelper
        {
            Vector4 R0;
            Vector4 R1;
            Vector4 R2;
        };

        bool SpritePrepare(graphics::BufferEntry& b, voidc_p apInstance);
        void SpriteDraw(graphics::RenderNode* apNode, voidc_p apInstance);

        class Sprite : public IDrawable, public ISerializeable
        {
        public:
            Sprite();
            virtual ~Sprite();

            enum
            {
                kNone = 0,
                kSerializeable = (1 << 0)
            };

            float GetAlpha() const { return mAlpha; }
            void SetAlpha(float v) { mAlpha = v; }

            virtual void Pose(IDrawable* apDrawable) override;

            virtual bool bSerializeable() const override { return ((mFlags & kSerializeable) != 0); }
            virtual void Serialize(system::IWriteFilePtr& p) override;
            virtual void Deserialize(system::IReadFilePtr& p) override;

            void SetSerializeable(bool b)
            {
                if (b) { mFlags |= kSerializeable; }
                else { mFlags &= ~kSerializeable; }
            }

            virtual BoundingRectangle GetBounding() const override
            {
                return (BoundingRectangle(
                    mTranslation - mScale, mTranslation + mScale));
            }

            const Vector2& GetXYDepthFactors() const { return mXYDepthFactors; }
            void SetXYDepthFactors(const Vector2& v) { mXYDepthFactors = v; }

            const Vector2& GetScale() const { return mScale; }
            void SetScale(const Vector2& v) { mScale = v; }
            void SetScale(float x, float y) { mScale.X = x; mScale.Y = y; }

            const Vector2& GetTranslation() const { return mTranslation; }
            void SetTranslation(const Vector2& v) { mTranslation = v; }
            void SetTranslation(float x, float y) { mTranslation.X = x; mTranslation.Y = y; }

            const Vector4& GetTextureScaleShift() const { return mTextureScaleShift; }
            void SetTextureScaleShift(const Vector4& v) { mTextureScaleShift = v; }

            virtual float GetZdepth() const override;
            void SetZdepth(float v);

            MemoryBuffer<Vector2>* GetShadowEdges() { return mpShadowEdges; }
            void SetShadowEdges(MemoryBuffer<Vector2>* p) { mpShadowEdges = p; }
            
            graphics::Material* GetMaterial() const;
            void SetMaterial(graphics::Material* p);

        protected:
            friend void jz::__IncrementRefCount<Sprite>(Sprite*);
            friend void jz::__DecrementRefCount<Sprite>(Sprite*);

            u32 mFlags;
            float mAlpha;
            Vector2 mScale;
            Vector2 mTranslation;
            Vector4 mTextureScaleShift;
            Vector2 mXYDepthFactors;
            float mZdepth;

            graphics::MaterialPtr mpMaterial;
            graphics::RenderPack mPack;

            MemoryBuffer<Vector2>* mpShadowEdges;

        private:
            Sprite(const Sprite&);
            Sprite& operator=(const Sprite&);

        };

        typedef AutoPtr<Sprite> SpritePtr;

    }
}

#endif
