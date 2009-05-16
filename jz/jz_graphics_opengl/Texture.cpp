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

#include <jz_core/Logger.h>
#include <jz_graphics/Texture.h>
#include <jz_graphics_opengl/Dds.h>
#include <jz_graphics_opengl/OpenGL.h>

namespace jz
{
    namespace graphics
    {

        Texture::Texture(const string& aFilename)
            : IObject(aFilename)
        {}
        
        Texture::~Texture()
        {}

        IObject::State Texture::_Load()
        {
            OPENGL_ASSERT();

            try
            {
                uint handle = LoadDdsFile(GetFilename().c_str());
                mHandle = handle;
            }
            catch (exception& e)
            {
                LogMessage(e.what(), Logger::kError);
                return (kErrorDataRead);
            }

            return (kLost);
        }

        IObject::State Texture::_Unload()
        {
            OPENGL_ASSERT();

            uint handle = StaticCast<uint>(mHandle);
            glDeleteTextures(1, &handle);
            mHandle.Reset();

            return (kUnloaded);
        }

    }
}
