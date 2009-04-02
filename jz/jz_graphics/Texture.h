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
#ifndef _JZ_GRAPHICS_TEXTURE_H_
#define _JZ_GRAPHICS_TEXTURE_H_

#include <jz_graphics/IObject.h>
#include <string>

namespace jz
{
    namespace graphics
    {

        class Effect;
        class Graphics;
        class TextureLoader;
        class Texture : public IObject
        {
        public:
            static const int SortOrder = 2;

        protected:
            JZ_EXPORT virtual ~Texture();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;

        private:
            friend void __SetParameterValue(Handle e, Handle p, Texture* v);
            friend void jz::__IncrementRefCount<graphics::Texture>(graphics::Texture* p);
            friend void jz::__DecrementRefCount<graphics::Texture>(graphics::Texture* p);

            friend class Effect;
            friend class Graphics;
            friend class TextureLoader;

            Texture(const string& aFilename);

            Texture(const Texture&);
            Texture& operator=(const Texture&);

            Handle mHandle;
        };

        typedef AutoPtr<Texture> TexturePtr;

    }
}

#endif
