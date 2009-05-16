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

#include <jz_core/Color.h>
#include <jz_gui/Font.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/VolatileTexture.h>
#include <jz_system/Files.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#if JZ_PLATFORM_WINDOWS
#include <jz_system/Win32.h>
#endif

namespace jz
{
    namespace gui
    {

        static const unatural kPadding = 2;
        static const float kFreeTypePositionFactor = (float)(1.0 / 64.0);

        Font::Font(u16 aCharacterWidth, u16 aCharacterHeight, u32 aCreateFlags, const char* apTypefaceName)
            : mCharacterWidth(aCharacterWidth),
              mCharacterHeight(aCharacterHeight), 
              mCreateFlags(aCreateFlags), 
              mTypefaceName(apTypefaceName),
              mFontHeight(0u)
        {
#if JZ_PLATFORM_WINDOWS
            if (!system::Files::Exists(mTypefaceName.c_str()))
            {
                char buffer[system::Files::kMaximumPathLength];
                if (ExpandEnvironmentStringsA("%WinDir%\\Fonts\\", buffer, system::Files::kMaximumPathLength))
                {
                    mTypefaceName = string(buffer) + system::Files::ExtractBase(mTypefaceName) + ".ttf";
                }
            }
#endif

            using namespace graphics;
            FT_Library lib = null;
            FT_Init_FreeType(&lib);
            if (lib)
            {
                FT_Face face = null;
                FT_New_Face(lib, mTypefaceName.c_str(), 0u, &face);
                if (face)
                {
                    if (FT_Set_Pixel_Sizes(face, mCharacterWidth, mCharacterHeight) == 0)
                    {
                        size_t width;
                        size_t height;

                        _GetTextureDimensions(face, width, height);

                        mpFont = Graphics::GetSingleton().Create<VolatileTexture>(
                            width, height, graphics::VolatileTexture::kA8R8G8B8, 
                            bind(&Font::_PopulateHelper, this, tr1::placeholders::_1, tr1::placeholders::_2, tr1::placeholders::_3, tr1::placeholders::_4));
                    }

                    FT_Done_Face(face);
                    face = null;
                }
                FT_Done_FreeType(lib);
                lib = null;
            }
        }

        Font::~Font()
        {}

        bool Font::GetEntry(wchar_t aChar, Font::Entry& arEntry) const
        {
            CharSet::const_iterator I = mCharSet.find(aChar);
            if (I != mCharSet.end())
            {
                arEntry = I->second;
                return true;
            }

            return false;
        }

        void Font::_Create()
        {}

        void Font::_Destroy()
        {}

        void Font::_Lost()
        {}
        
        void Font::_Reset(natural aWidth, natural aHeight)
        {}

        void Font::_GetTextureDimensions(void_p apObject, size_t& arWidth, size_t& arHeight) const
        {
            FT_Face face = static_cast<FT_Face>(apObject);
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();

            const_cast<size_t&>(mFontHeight) = 0u;
            const size_t kMaxTexDimension = graphics.GetMaxTextureDimension();
            size_t ret = 0u;
            const size_t kGlyphCount = face->num_glyphs;

            size_t totalHeight = 16u;
            size_t totalWidth = kPadding;
            size_t maxCurrentHeight = 0u;
            size_t runningHeight = kPadding;

            FT_ULong code = FT_Get_First_Char(face, null);
            while (code > 0u)
            {
                unatural options = (FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
                options |= ((mCreateFlags & kAntiAliased) != 0) ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO;
                if (FT_Load_Char(face, code, options) != 0) { continue; }

                const size_t width = (size_t)(face->glyph->bitmap.width) + kPadding;
                const size_t height = (size_t)(face->glyph->bitmap.rows) + kPadding;

                maxCurrentHeight = Max(maxCurrentHeight, height);
                const_cast<size_t&>(mFontHeight) = Max(maxCurrentHeight, mFontHeight);

                totalWidth += width;
                if (totalWidth > kMaxTexDimension)
                {
                    totalWidth = kPadding;
                    runningHeight += maxCurrentHeight;
                    maxCurrentHeight = 0u;
                }

                code = FT_Get_Next_Char(face, code, null);
            }

            runningHeight += maxCurrentHeight;
            while (totalHeight < runningHeight) { totalHeight *= 2u; }

            JZ_E_ON_FAIL(totalHeight <= kMaxTexDimension, "font texture is too big.");

            arWidth = kMaxTexDimension;
            arHeight = totalHeight;

        }

        void Font::_PopulateHelper(natural aWidth, natural aHeight, void_p apData, size_t aPitch)
        {
            mCharSet.clear();

            using namespace graphics;
            FT_Library lib = null;
            FT_Init_FreeType(&lib);
            if (lib)
            {
                FT_Face face = null;
                FT_New_Face(lib, mTypefaceName.c_str(), 0u, &face);
                if (face)
                {
                    if (FT_Set_Pixel_Sizes(face, mCharacterWidth, mCharacterHeight) == 0)
                    {
                        size_t x = kPadding;
                        size_t y = kPadding;
                        size_t maxCurrentHeight = 0u;

                        FT_ULong code = FT_Get_First_Char(face, null);
                        while (code > 0u)
                        {
                            unatural options = (FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT);
                            options |= ((mCreateFlags & kAntiAliased) != 0) ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO;
                            if (FT_Load_Char(face, code, options) != 0) { continue; }

                            const size_t width = (size_t)(face->glyph->bitmap.width) + kPadding;
                            const size_t height = (size_t)(face->glyph->bitmap.rows) + kPadding;
                            maxCurrentHeight = Max(maxCurrentHeight, height);

                            if (x + width > (size_t)aWidth)
                            {
                                x = kPadding;
                                y += maxCurrentHeight;
                                maxCurrentHeight = 0u;
                            }

                            _Draw(aWidth, aHeight, apData, aPitch, x, y, face);

                            Vector4 scaleShift;
                            scaleShift.X = ((float)(width - kPadding) / (float)(aWidth - 1));
                            scaleShift.Y = ((float)(height - kPadding) / (float)(aHeight - 1));
                            scaleShift.Z = ((float)x / (float)(aWidth - 1));
                            scaleShift.W = ((float)y / (float)(aHeight - 1));

                            Entry entry;
                            entry.XStepSize = (natural)Ceil(face->glyph->advance.x * kFreeTypePositionFactor);
                            entry.Width = (natural)(width - kPadding);
                            entry.Height = (natural)(height - kPadding);
                            entry.XDrawOffset = face->glyph->bitmap_left;
                            entry.YDrawOffset = (mFontHeight - face->glyph->bitmap_top);
                            entry.TextureScaleShift = scaleShift;

                            mCharSet.insert(make_pair((wchar_t)code, entry));

                            x += width;
                            code = FT_Get_Next_Char(face, code, null);
                        }
                    }
                    FT_Done_Face(face);
                    face = null;
                }
                FT_Done_FreeType(lib);
                lib = null;
            }
        }

        void Font::_Draw(natural aDestWidth, natural aDestHeight, void_p apDestData, size_t aDestPitch,
            size_t x, size_t y, void_p apSrcData)
        {
            FT_Face face = static_cast<FT_Face>(apSrcData);
            const FT_Bitmap& bitmap = (face->glyph->bitmap);
            u8c_p pSrc = (bitmap.buffer);
            u8_p pDest = (u8_p)(apDestData);

            if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
            {
                for (int j = 0; j < bitmap.rows; j++)
                {
                    for (int i = 0; i < bitmap.width; i++)
                    {
                        u8c_p pIn = (pSrc + ((j * bitmap.pitch) + i));
                        u8_p pOut = (pDest + (((y + j) * aDestPitch) + ((i + x) * 4u)));

                        pOut[0] = 255;
                        pOut[1] = 255;
                        pOut[2] = 255;
                        pOut[3] = *pIn;
                    }
                }
            }
            else if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
            {
                for (int j = 0; j < bitmap.rows; j++)
                {
                    for (int i = 0; i < bitmap.width; i++)
                    {
                        u8c_p pIn = (pSrc + ((j * bitmap.pitch) + (i / 8u)));
                        u8_p pOut = (pDest + (((y + j) * aDestPitch) + ((i + x) * 4u)));

                        u8 val = ((*pIn) & (0x80 >> (i & 7))) ? 255 : 0;
                        pOut[0] = 255;
                        pOut[1] = 255;
                        pOut[2] = 255;
                        pOut[3] = val;
                    }
                }
            }
        }
    }
}
