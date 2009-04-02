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
#ifndef _JZ_GRAPHICS_VOLATILE_TEXTURE_H_
#define _JZ_GRAPHICS_VOLATILE_TEXTURE_H_

#include <jz_graphics/IObject.h>
#include <functional>
#include <string>

namespace jz
{
    namespace graphics
    {

        class Effect;
        class Graphics;
        class Target;
        class Texture;
        class VolatileTexture : public IVolatileObject
        {
        public:
            typedef tr1::function<void(natural aWidth, natural aHeight, void_p apData, size_t aPitch)> PopulateFunction;

            enum Format
            {
                kA8R8G8B8,
                kX8R8G8B8,
                kR5G6B5,
                kX1R5G5B5,
                kA1R5G5B5,
                kA4R4G4B4,
                kR3G3B2,
                kA8,
                kA8R3G3B2,
                kX4R4G4B4,
                kA2B10G10R10,
                kA8B8G8R8,
                kX8B8G8R8,
                kG16R16,
                kA2R10G10B10,
                kA16B16G16R16,
                kR16F,
                kG16R16F,
                kA16B16G16R16F,
                kR32F,
                kG32R32F,
                kA32B32G32R32F
            };

            static const int SortOrder = 0;

        protected:
            JZ_EXPORT virtual ~VolatileTexture();
            JZ_EXPORT virtual void _Create() override;
            JZ_EXPORT virtual void _Destroy() override;
            JZ_EXPORT virtual void _Lost() override;
            JZ_EXPORT virtual void _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class Effect;
            friend class Graphics;
            friend void __SetParameterValue(Handle e, Handle p, Target* v);
            friend void __SetParameterValue(Handle e, Handle p, Texture* v);
            friend void __SetParameterValue(Handle e, Handle p, VolatileTexture* v);
            friend void jz::__IncrementRefCount<graphics::VolatileTexture>(graphics::VolatileTexture* p);
            friend void jz::__DecrementRefCount<graphics::VolatileTexture>(graphics::VolatileTexture* p);

            // Note: 0 for a dimension parameter means to use the backbuffer value
            //       for that parameter.
            VolatileTexture(unatural aWidth, unatural aHeight, Format aFormat, PopulateFunction aFunction, float aScaling = 1.0f);
            VolatileTexture(const VolatileTexture&);
            VolatileTexture& operator=(const VolatileTexture&);

            void _CreateHelper(unatural width, unatural height);

            unatural mWidth;
            unatural mHeight;
            Format mFormat;
            PopulateFunction mPopulateFunction;
            float mScaling;

            Handle mHandle;
        };

        typedef AutoPtr<VolatileTexture> VolatileTexturePtr;

    }
}

#endif
