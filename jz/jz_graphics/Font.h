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
#ifndef _JZ_GRAPHICS_FONT_H_
#define _JZ_GRAPHICS_FONT_H_

#include <jz_graphics/IObject.h>

namespace jz
{
    struct ColorRGBA;
    namespace graphics
    {

        class Graphics;
        class Font : public IVolatileObject
        {
        public:
            enum CreateFlags
            {
                kNone = 0,
                kBold = (1 << 0),
                kItalic = (1 << 1)
            };

            enum DrawFlags
            {
                kAlignTop = 0x00000000,
                kAlignLeft = 0x00000000,
                kAlignHCenter = 0x00000001,
                kAlignRight = 0x00000002,
                kAlignVCenter = 0x00000004,
                kAlignBottom = 0x00000008,
                kWordBreak = 0x00000010,
                kSingleLine = 0x00000020,
                kExpandTabs = 0x00000040,
                kTabStop = 0x00000080,
                kNoClip = 0x00000100,
                kCalcRect = 0x00000400
            };

            static const int SortOrder = 0;

            void RenderText(const char* apString,
                natural x, natural y, natural width, natural height,
                u32 aDrawFlags, const ColorRGBA& aColor);

        protected:
            JZ_EXPORT virtual ~Font();
            JZ_EXPORT virtual void _Create() override;
            JZ_EXPORT virtual void _Destroy() override;
            JZ_EXPORT virtual void _Lost() override;
            JZ_EXPORT virtual void _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class Graphics;
            friend void jz::__IncrementRefCount<graphics::Font>(graphics::Font* p);
            friend void jz::__DecrementRefCount<graphics::Font>(graphics::Font* p);

            Font(u16 aWidth, u16 aHeight, u32 aCreateFlags, const char* apTypefaceName);
            Font(const Font&);
            Font& operator=(const Font&);

            u32 mCreateFlags;
            u16 mWidth;
            u16 mHeight;
            string mTypefaceName;

            Handle mpFont;
        };

        typedef AutoPtr<Font> FontPtr;

    }        
}

#endif
