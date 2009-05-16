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
#include <jz_graphics/Font.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

        Font::Font(u16 aWidth, u16 aHeight, u32 aCreateFlags, const char* apTypefaceName)
            : mWidth(aWidth), mHeight(aHeight), mCreateFlags(aCreateFlags), mTypefaceName(apTypefaceName)
        {}

        Font::~Font()
        {}

        void Font::RenderText(const char* apString,
            natural x, natural y, natural width, natural height,
            u32 aDrawFlags, const ColorRGBA& aColor)
        {
            if (mpFont)
            {
                RECT rect;
                rect.left = x;
                rect.top = y;
                rect.right = (x + width);
                rect.bottom = (y + height);

                JZ_DEBUG_DX_FAIL(StaticCast<ID3DXFont*>(mpFont)->DrawText(
                    null, apString, -1, &rect, aDrawFlags, 
                    D3DCOLOR_COLORVALUE(aColor.R, aColor.G, aColor.B, aColor.A)));
            }
        }

        void Font::_Create()
        {
            ID3DXFont* pFont = null;

            JZ_E_ON_DX_FAIL(D3DXCreateFont(
                gpD3dDevice9,
                mHeight,
                mWidth,
                ((mCreateFlags & kBold) != 0) ? FW_BOLD : 0,
                0,
                ((mCreateFlags & kItalic) != 0) ? TRUE : FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE,
                mTypefaceName.c_str(),
                &pFont));

            mpFont = pFont;
        }

        void Font::_Destroy()
        {
            SafeRelease<ID3DXFont>(mpFont);
        }

        void Font::_Lost()
        {
            JZ_DEBUG_DX_FAIL(StaticCast<ID3DXFont*>(mpFont)->OnLostDevice());
        }
        
        void Font::_Reset(natural aWidth, natural aHeight)
        {
            JZ_DEBUG_DX_FAIL(StaticCast<ID3DXFont*>(mpFont)->OnResetDevice());
        }

    }
}
