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
#ifndef _JZ_GRAPHICS_SYSTEM_MEMORY_SURFACE_H_
#define _JZ_GRAPHICS_SYSTEM_MEMORY_SURFACE_H_

#include <jz_core/Rectangle.h>
#include <jz_graphics/IObject.h>

namespace jz
{
    namespace graphics
    {

        class Graphics;
        class SystemMemorySurface : public IVolatileObject
        {
        public:
            enum Format
            {
                kA16B16G16R16F,
                kR16F,
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
                kA16B16G16R16
            };

            enum LockFlags
            {
                kNone = 0,
                kReadonly = (1 << 0)
            };

            static const int SortOrder = 0;

            void Lock(const RectangleU& aRect, u32 aLockFlags, void_p& arpLock, unatural& arPitch);
            void Unlock();

            void PopulateFromBackbuffer();

        protected:
            JZ_EXPORT virtual ~SystemMemorySurface();
            JZ_EXPORT virtual void _Create() override;
            JZ_EXPORT virtual void _Destroy() override;
            JZ_EXPORT virtual void _Lost() override;
            JZ_EXPORT virtual void _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class Graphics;
            friend void jz::__IncrementRefCount<graphics::SystemMemorySurface>(graphics::SystemMemorySurface* p);
            friend void jz::__DecrementRefCount<graphics::SystemMemorySurface>(graphics::SystemMemorySurface* p);

            // Note: 0 for a dimension parameter means to use the backbuffer value
            //       for that parameter.
            SystemMemorySurface(unatural aWidth, unatural aHeight, Format aFormat);
            SystemMemorySurface(const SystemMemorySurface&);
            SystemMemorySurface& operator=(const SystemMemorySurface&);

            unatural mWidth;
            unatural mHeight;
            Format mFormat;

            Handle mpSurface;
        };

        typedef AutoPtr<SystemMemorySurface> SystemMemorySurfacePtr;

    }        
}

#endif
