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
#ifndef _JZ_GUI_GUI_IMAGE_H_
#define _JZ_GUI_GUI_IMAGE_H_

#include <jz_gui/IGuiElement.h>

namespace jz
{
    namespace graphics
    {
        class Effect; typedef AutoPtr<Effect> EffectPtr;
        class Material; typedef AutoPtr<Material> MaterialPtr;
    }
    
    namespace gui
    {

        class GuiImageEffect;
        class GuiImage : public IGuiElement
        {
        public:
            GuiImage();
            virtual ~GuiImage();

            virtual void Pose(const Vector4& aParentScaleShift = Vector4(1, 1, 0, 0), float aParentZ = 0.0f) override;

            const Vector2& GetScale() const { return mScale; }
            void SetScale(const Vector2& v) { mScale = v; }
            void SetScale(float x, float y) { mScale.X = x; mScale.Y = y; }

            const Vector2& GetTranslation() const { return mTranslation; }
            void SetTranslation(const Vector2& v) { mTranslation = v; }
            void SetTranslation(float x, float y) { mTranslation.X = x; mTranslation.Y = y; }

            const Vector4& GetTextureScaleShift() const { return mTextureScaleShift; }
            void SetTextureScaleShift(const Vector4& v) { mTextureScaleShift = v; }

            graphics::Effect* GetEffect() const { return (mPack.pEffect.Get()); }
            graphics::Material* GetMaterial() const { return (mPack.pMaterial.Get()); }

            void SetEffect(GuiImageEffect* p);
            void SetMaterial(graphics::Material* p);

            void SetTransparent(bool b) { mbTransparent = b; }
            bool bTransparent() const { return mbTransparent; }

        protected:
            friend void jz::__IncrementRefCount<GuiImage>(GuiImage*);
            friend void jz::__DecrementRefCount<GuiImage>(GuiImage*);

            bool mbTransparent;
            Vector2 mScale;
            Vector2 mTranslation;
            Vector4 mTextureScaleShift;
            graphics::RenderPack mPack;

        private:
            GuiImage(const GuiImage&);
            GuiImage& operator=(const GuiImage&);

        };

        typedef AutoPtr<GuiImage> GuiImagePtr;

    }
}

#endif
