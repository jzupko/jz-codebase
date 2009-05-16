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
#ifndef _JZ_GUI_IGUI_ELEMENT_H_
#define _JZ_GUI_IGUI_ELEMENT_H_

#include <jz_core/Auto.h>
#include <jz_core/Vector4.h>

namespace jz
{
    namespace gui
    {

        static const float kGuiElementDepthStep = 1e-5f;

        class IGuiElement abstract
        {
        public:
            virtual void Pose(const Vector4& aParentScaleShift = Vector4(1, 1, 0, 0), float aParentZ = 0.0f) = 0;

        protected:
            virtual ~IGuiElement() {}

            friend void ::jz::__IncrementRefCount<IGuiElement>(IGuiElement*);
            friend void ::jz::__DecrementRefCount<IGuiElement>(IGuiElement*);

            size_t mReferenceCount;

            IGuiElement()
                : mReferenceCount(0u)
            {}

        private:
            IGuiElement(const IGuiElement&);
            IGuiElement& operator=(const IGuiElement&);
        };

        typedef AutoPtr<IGuiElement> IGuiElementPtr;

    }
}

#endif
