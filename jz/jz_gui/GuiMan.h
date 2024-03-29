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
#ifndef _JZ_GUI_GUI_MAN_H_
#define _JZ_GUI_GUI_MAN_H_

#include <jz_core/Auto.h>
#include <jz_core/Event.h>
#include <jz_core/Utility.h>
#include <jz_system/InputPrereqs.h>

namespace jz
{
    namespace gui
    {

        class GuiMan : public Singleton<GuiMan>
        {
        public:
            GuiMan();
            ~GuiMan();

            void Pose();

        private:
            GuiMan(const GuiMan&);
            GuiMan& operator=(const GuiMan&);

            Event<bool (system::Mouse::Button, system::Mouse::State)>::Entry mButtonEntry;
            Event<bool (system::Key::Code, system::Key::State)>::Entry mKeyEntry;
            Event<void (natural, natural)>::Entry mMoveEntry;
            Event<void (float)>::Entry mWheelEntry;
            Event<void (natural, natural)>::Entry mResizeEntry;

            void _ResizeHandler(natural width, natural height);
            bool _ButtonHandler(system::Mouse::Button, system::Mouse::State);
            bool _KeyHandler(system::Key::Code, system::Key::State);
            void _MoveHandler(natural, natural);
            void _WheelHandler(float);
        };

    }
}

#endif
