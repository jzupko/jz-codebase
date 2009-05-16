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

#include <jz_graphics/DepthStencilSurface.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics_opengl/OpenGL.h>

namespace jz
{
    namespace graphics
    {

        extern uint gOpenGlFbo;

        struct FormatHelper
        {
            GLenum Format;
        };

        static const FormatHelper kHelpers[DepthStencilSurface::FORMAT_COUNT] =
        {
            { GL_DEPTH_COMPONENT16 }, // kD16_LOCKABLE
            { GL_DEPTH_COMPONENT32 }, // kD32,
            { GL_DEPTH_COMPONENT16 }, // kD15S1
            { GL_DEPTH24_STENCIL8 }, // kD24S8
            { GL_DEPTH24_STENCIL8 }, // kD24X8
            { GL_DEPTH24_STENCIL8 }, // kD24X4S4
            { GL_DEPTH_COMPONENT16 } // kD16
        };

        DepthStencilSurface::DepthStencilSurface(unatural aWidth, unatural aHeight, Format aFormat)
            : mWidth(aWidth), mHeight(aHeight), mFormat(aFormat)
        {}

        DepthStencilSurface::~DepthStencilSurface()
        {}

        void DepthStencilSurface::SetToDevice()
        {
            OPENGL_ASSERT();

            uint handle = StaticCast<uint>(mHandle);
            if (GlIdIsValid(gOpenGlFbo) && GlIdIsValid(handle))
            {
                glBindFramebuffer(GL_FRAMEBUFFER, gOpenGlFbo);
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, handle);
                glFramebufferRenderbufferEXT(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, handle);
            }
        }

        void DepthStencilSurface::RestorePrevToDevice()
        {}

        void DepthStencilSurface::_Create()
        {}

        void DepthStencilSurface::_Destroy()
        {}

        void DepthStencilSurface::_Lost()
        {
            OPENGL_ASSERT();
            
            uint handle = StaticCast<uint>(mHandle);
            glDeleteRenderbuffers(1, &handle);
            mHandle.Reset();
        }
        
        void DepthStencilSurface::_Reset(natural aWidth, natural aHeight)
        {
            OPENGL_ASSERT();

            Graphics& graphics = Graphics::GetSingleton();
            unatural width = (mWidth > 0u) ? mWidth : (graphics.GetViewportWidth());
            unatural height = (mHeight > 0u) ? mHeight : (graphics.GetViewportHeight());

            uint handle = 0u;
            glGenRenderbuffers(1, &handle);
            if (!GlIdIsValid(handle)) { throw exception(__FUNCTION__ ": failed creating render buffer."); }

            glBindRenderbuffer(GL_FRAMEBUFFER, handle);
            glRenderbufferStorage(GL_FRAMEBUFFER, kHelpers[mFormat].Format, width, height);
            glBindRenderbuffer(GL_FRAMEBUFFER, 0u);

            mHandle = handle;
        }

    }
}
