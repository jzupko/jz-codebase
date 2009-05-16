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
#ifndef _JZ_GUI_FONT_H_
#define _JZ_GUI_FONT_H_

#include <jz_core/Rectangle.h>
#include <jz_core/Vector4.h>
#include <jz_graphics/IObject.h>
#include <map>

namespace jz
{
    namespace graphics
    {
        class Graphics;
        class VolatileTexture; typedef AutoPtr<VolatileTexture> VolatileTexturePtr;
    }

    namespace gui
    {

        class Font : public graphics::IVolatileObject
        {
        public:
            struct Entry
            {
                natural XDrawOffset;
                natural YDrawOffset;
                natural XStepSize;
                natural Width;
                natural Height;
                Vector4 TextureScaleShift;
            };

            enum CreateFlags
            {
                kNone = 0,
                kAntiAliased = (1 << 0)
            };

            static const int SortOrder = 1;

            bool GetEntry(wchar_t aChar, Entry& arEntry) const;

            size_t GetFontHeight() const { return mFontHeight; }
            graphics::VolatileTexture* GetTexture() const { return (mpFont.Get()); }

        protected:
            JZ_EXPORT virtual ~Font();
            JZ_EXPORT virtual void _Create() override;
            JZ_EXPORT virtual void _Destroy() override;
            JZ_EXPORT virtual void _Lost() override;
            JZ_EXPORT virtual void _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class graphics::Graphics;
            friend void jz::__IncrementRefCount<gui::Font>(gui::Font* p);
            friend void jz::__DecrementRefCount<gui::Font>(gui::Font* p);

            Font(u16 aCharacterWidth, u16 aCharacterHeight, u32 aCreateFlags, const char* apTypefaceName);
            Font(const Font&);
            Font& operator=(const Font&);

            u32 mCreateFlags;
            u16 mCharacterWidth;
            u16 mCharacterHeight;
            string mTypefaceName;

            graphics::VolatileTexturePtr mpFont;
            size_t mFontHeight;
            
            typedef map<wchar_t, Entry> CharSet;
            CharSet mCharSet;

            void _GetTextureDimensions(void_p apObject, size_t& arWidth, size_t& arHeight) const;
            void _PopulateHelper(natural aWidth, natural aHeight, void_p apData, size_t aPitch);
            void _Draw(natural aDestWidth, natural aDestHeight, void_p apDestData, size_t aDestPitch,
                size_t x, size_t y, void_p apSrcData);
        };

        typedef AutoPtr<Font> FontPtr;

    }        
}

#endif
