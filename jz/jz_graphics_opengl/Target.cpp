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

#include <jz_graphics/Graphics.h>
#include <jz_graphics/Target.h>
#include <jz_graphics_opengl/OpenGL.h>

namespace jz
{
    namespace graphics
    {

        extern uint gOpenGlFbo;

        struct FormatHelper
        {
            GLenum InternalFormat;
            GLenum Format;
            GLenum Type;
        };

        static const FormatHelper kHelpers[Target::FORMAT_COUNT] =
        {
            { GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE }, // kA8R8G8B8
            { GL_RGB8, GL_BGRA, GL_UNSIGNED_BYTE }, // kX8R8G8B8
            { GL_RGB, GL_BGR, GL_UNSIGNED_SHORT_5_6_5 }, // kR5G6B5
            { GL_RGB, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1 }, // kX1R5G5B5
            { GL_RGBA, GL_BGRA, GL_UNSIGNED_SHORT_5_5_5_1 }, // kA1R5G5B5
            { GL_RGBA, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4 }, // kA4R4G4B4
            { GL_RGB, GL_BGR, GL_UNSIGNED_BYTE_3_3_2 }, // kR3G3B2
            { GL_ALPHA8, GL_ALPHA, GL_UNSIGNED_BYTE }, // kA8
            { GL_RGB, GL_BGRA, GL_UNSIGNED_SHORT_4_4_4_4 }, // kX4R4G4B4
            { GL_RGBA, GL_RGBA, GL_UNSIGNED_INT_10_10_10_2 }, // kA2B10G10R10
            { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE }, // kA8B8G8R8
            { GL_RGB, GL_RGBA, GL_UNSIGNED_BYTE }, // kX8B8G8R8
            { GL_RG16, GL_RG, GL_UNSIGNED_SHORT }, // kG16R16
            { GL_RGBA, GL_BGRA, GL_UNSIGNED_INT_10_10_10_2 }, // kA2R10G10B10
            { GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT }, // kA16B16G16R16
            { GL_R16F, GL_RED, GL_HALF_FLOAT }, // kR16F
            { GL_RG16F, GL_RG, GL_HALF_FLOAT }, // kG16R16F
            { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT }, // kA16B16G16R16F
            { GL_R32F, GL_RED, GL_FLOAT }, // kR32F
            { GL_RG32F, GL_RG, GL_FLOAT }, // kG32R32F
            { GL_RGBA32F, GL_RGBA, GL_FLOAT } // kA32B32G32R32F
        };

        Target::Target(unatural aWidth, unatural aHeight, Format aFormat, float aScaling)
            : mWidth(aWidth), mHeight(aHeight), mFormat(aFormat), mScaling(aScaling)
        {}

        Target::~Target()
        {}

        void Target::SetToDevice(unatural aTargetIndex)
        {
            OPENGL_ASSERT();

            uint handle = mTexture.CastUInt();
            if (GlIdIsValid(gOpenGlFbo) && GlIdIsValid(handle))
            {
                glBindFramebuffer(GL_FRAMEBUFFER, gOpenGlFbo);
                glFramebufferTexture2D(GL_FRAMEBUFFER, (GL_COLOR_ATTACHMENT0 + aTargetIndex), GL_TEXTURE_2D, handle, 0u);
            }
        }

        void Target::ResetTarget(unatural aTargetIndex)
        {
            OPENGL_ASSERT();

            if (aTargetIndex > 0u)
            {
                if (GlIdIsValid(gOpenGlFbo))
                {
                    glBindFramebuffer(GL_FRAMEBUFFER, gOpenGlFbo);
                    glFramebufferTexture2D(GL_FRAMEBUFFER, (GL_COLOR_ATTACHMENT0 + aTargetIndex), GL_TEXTURE_2D, 0u, 0u);
                }
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0u);
            }
        }

        void Target::_Create()
        {}

        void Target::_Destroy()
        {}

        void Target::_Lost()
        {
            OPENGL_ASSERT();
            
            uint handle = mTexture.CastUInt();
            glDeleteTextures(1, &handle);
            mTexture.Reset();
        }
        
        void Target::_Reset(natural aWidth, natural aHeight)
        {
            OPENGL_ASSERT();

            Graphics& graphics = Graphics::GetSingleton();
            unatural width = (mWidth > 0u) ? mWidth : (unatural)(mScaling * graphics.GetViewportWidth());
            unatural height = (mHeight > 0u) ? mHeight : (unatural)(mScaling * graphics.GetViewportHeight());

            uint handle = 0u;
            glGenTextures(1, &handle);
            if (!GlIdIsValid(handle)) { throw exception(__FUNCTION__ ": failed creating target texture."); }

            glBindTexture(GL_TEXTURE_2D, handle);
            glTexImage2D(GL_TEXTURE_2D, 0, kHelpers[mFormat].InternalFormat, width, height, 0, kHelpers[mFormat].Format, kHelpers[mFormat].Type, null);
            glBindTexture(GL_TEXTURE_2D, 0u);
        
            mTexture = handle;
        }

    }
}
