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
#ifndef _JZ_ENGINE_3D_THREE_POINT_H_
#define _JZ_ENGINE_3D_THREE_POINT_H_

#include <jz_core/Color.h>
#include <jz_core/Vector3.h>

namespace jz
{
    namespace engine_3D
    {

        struct ThreePoint
        {
            static ThreePoint Create()
            {
                ThreePoint ret;
                ret.KeyPosition = Vector3::kZero;
                ret.KeyDiffuse = ColorRGB::kBlack;
                ret.KeySpecular = ColorRGB::kBlack;
                ret.FillDirection = Vector3::kForward;
                ret.KeyToFillRatio = 0.0f;
                ret.BackPosition = Vector3::kZero;
                ret.BackDiffuse = ColorRGB::kBlack;
                ret.BackSpecular = ColorRGB::kBlack;

                return ret;
            }

	        Vector3 KeyPosition;
	        ColorRGB KeyDiffuse;
	        ColorRGB KeySpecular;
	        Vector3 FillDirection;
	        float KeyToFillRatio;
	        Vector3 BackPosition;
	        ColorRGB BackDiffuse;
	        ColorRGB BackSpecular;
        };

    }
}

#endif
