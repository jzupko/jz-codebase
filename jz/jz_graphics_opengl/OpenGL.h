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
#ifndef _JZ_GRAPHICS_OPENGL_OPEN_GL_H_
#define _JZ_GRAPHICS_OPENGL_OPEN_GL_H_

#include <jz_core/Prereqs.h>
#include <GL/glew.h>

#if JZ_PLATFORM_WINDOWS
#   pragma comment(lib, "opengl32.lib")
#   pragma comment(lib, "glu32.lib")
#endif

namespace jz
{
    namespace graphics
    {

        __inline bool GlIdIsValid(GLuint id) { return (id != 0u); }
        
        struct _AssertOpenGL
        {
            _AssertOpenGL()
            {
                Assert();
            }
            
            ~_AssertOpenGL()
            {
                Assert();
            }
            
            void Assert()
            {
    #           if _MSC_VER
                    GLenum err = glGetError();

                    if (err != GL_NO_ERROR)
                    {
                        const GLubyte* pString = gluErrorString(err);

                        _asm int 3;
                    }
    #           else
                    assert(glGetError() == GL_NO_ERROR);
    #           endif
            }        
        };

    #   ifndef NDEBUG
    #       define OPENGL_ASSERT() _AssertOpenGL __assert_Object_
    #   else
    #       define OPENGL_ASSERT() {}
    #   endif

    }
}

#endif
