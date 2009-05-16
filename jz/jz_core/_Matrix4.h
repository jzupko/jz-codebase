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

#if !defined(JZ_MATRIX4_ASM_OFFSET)
#   error "Please define the necessary macros before including this file."
#endif

    movaps xmm4, [edx + JZ_MATRIX4_ASM_OFFSET]
    movaps xmm5, [edx + JZ_MATRIX4_ASM_OFFSET]
    movaps xmm6, [edx + JZ_MATRIX4_ASM_OFFSET]
    movaps xmm7, [edx + JZ_MATRIX4_ASM_OFFSET]

    shufps xmm4, xmm4, 00000000b
    shufps xmm5, xmm5, 01010101b
    shufps xmm6, xmm6, 10101010b
    shufps xmm7, xmm7, 11111111b

    mulps xmm4, xmm0
    mulps xmm5, xmm1
    mulps xmm6, xmm2
    mulps xmm7, xmm3

    addps xmm4, xmm5
    addps xmm6, xmm7
    addps xmm4, xmm6

    movaps [eax + JZ_MATRIX4_ASM_OFFSET], xmm4

#   undef JZ_MATRIX4_ASM_OFFSET
