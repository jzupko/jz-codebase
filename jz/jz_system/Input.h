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
#ifndef _JZ_SYSTEM_INPUT_H_
#define _JZ_SYSTEM_INPUT_H_

#include <jz_core/Event.h>
#include <jz_core/Utility.h>
#include <jz_system/InputPrereqs.h>

namespace jz
{
    namespace system
    {

        class Input : public Singleton<Input>
        {
        public:
            typedef Event<bool (Mouse::Button, Mouse::State)> ButtonEvent;
            typedef Event<bool (Key::Code, Key::State)> KeyEvent;
            typedef Event<void (natural, natural)> MouseMoveEvent;
            typedef Event<void (float)> MouseWheelEvent;

            Input();
            ~Input();

            bool bMouseVisible() const { return mbMouseVisible; }
            void SetMouseVisible(bool b);

            natural GetMouseX() const;
            natural GetMouseY() const;

            void SetMousePosition(natural x, natural y);

            ButtonEvent OnButton;
            KeyEvent OnKey;

            MouseMoveEvent OnMouseMoveAbsolute;
            MouseMoveEvent OnMouseMoveDelta;

            MouseWheelEvent OnMouseWheel;

            void UpdateMousePosition(natural x, natural y);

        private:
            natural mMousePrevX;
            natural mMousePrevY;

            Input(const Input&);
            Input& operator=(const Input&);

            bool mbMouseVisible;
        };

    }
}

#endif
