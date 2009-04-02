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
#ifndef _JZ_GRAPHICS_OPENGL_CG_H_
#define _JZ_GRAPHICS_OPENGL_CG_H_

#include <jz_core/Prereqs.h>
#include <jz_graphics_opengl/OpenGL.h>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#if JZ_PLATFORM_WINDOWS
#   pragma comment(lib, "cg.lib")
#   pragma comment(lib, "cgGL.lib")
#endif

namespace jz
{
    namespace graphics
    {

        extern CGcontext gpCgContext;

        struct _AssertCG
        {
            _AssertCG()
            {
                Assert();
            }
            
            ~_AssertCG()
            {
                Assert();
            }
            
            void Assert()
            {
    #           if _MSC_VER
                    CGerror err = cgGetError();

                    if (err != CG_NO_ERROR)
                    {
                        const char* pError = cgGetErrorString(err);
                        const char* pListing = cgGetLastListing(gpCgContext);

                        _asm int 3;
                    }
    #           else
                    assert(err == CG_NO_ERROR);
    #           endif
            }        
        };

    #   ifndef NDEBUG
    #       define CG_ASSERT() _AssertCG __assert_Cg_Object_
    #   else
    #       define CG_ASSERT() {}
    #   endif

    }
}

#endif
