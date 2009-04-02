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
#ifndef _JZ_GRAPHICS_TARGET_H_
#define _JZ_GRAPHICS_TARGET_H_

#include <jz_graphics/IObject.h>

namespace jz
{
    namespace graphics
    {

        class Effect;
        class Graphics;
        class Target : public IVolatileObject
        {
        public:
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
                kA32B32G32R32F,
                FORMAT_COUNT
            };

            static const int SortOrder = 0;

            void SetToDevice(unatural aTargetIndex);
            static void ResetTarget(unatural aTargetIndex);

        protected:
            JZ_EXPORT virtual ~Target();
            JZ_EXPORT virtual void _Create() override;
            JZ_EXPORT virtual void _Destroy() override;
            JZ_EXPORT virtual void _Lost() override;
            JZ_EXPORT virtual void _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class Effect;
            friend class Graphics;
            friend void __SetParameterValue(Handle e, Handle p, Target* v);
            friend void jz::__IncrementRefCount<graphics::Target>(graphics::Target* p);
            friend void jz::__DecrementRefCount<graphics::Target>(graphics::Target* p);

            // Note: 0 for a dimension parameter means to use the backbuffer value
            //       for that parameter.
            Target(unatural aWidth, unatural aHeight, Format aFormat, float aScaling = 1.0f);
            Target(const Target&);
            Target& operator=(const Target&);

            unatural mWidth;
            unatural mHeight;
            Format mFormat;
            float mScaling;

            Handle mSurface;
            Handle mTexture;
        };

        typedef AutoPtr<Target> TargetPtr;

    }    
}

#endif
