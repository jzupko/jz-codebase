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
#include <jz_graphics/Graphics.h>
#include <jz_gui/GuiMan.h>
#include <jz_system/Input.h>
#include <jz_system/Time.h>

namespace jz
{
    gui::GuiMan* gui::GuiMan::mspSingleton = null;

    namespace gui
    {

        GuiMan::GuiMan()
        {
            using namespace graphics;
            using namespace system;
          
            if (Input::GetSingletonExists())
            {
                Input& input = Input::GetSingleton();
                input.OnMouseWheel.Add<GuiMan, &GuiMan::_WheelHandler>(this, mWheelEntry);
                input.OnMouseMoveAbsolute.Add<GuiMan, &GuiMan::_MoveHandler>(this, mMoveEntry);
                input.OnKey.Add<GuiMan, &GuiMan::_KeyHandler>(this, mKeyEntry);
                input.OnButton.Add<GuiMan, &GuiMan::_ButtonHandler>(this, mButtonEntry);
            }

            if (Graphics::GetSingletonExists())
            {
                Graphics& graphics = Graphics::GetSingleton();
                graphics.OnReset.Add<GuiMan, &GuiMan::_ResizeHandler>(this, mResizeEntry);
            }
        }

        GuiMan::~GuiMan()
        {
            mResizeEntry.Reset();
            mButtonEntry.Reset();
            mKeyEntry.Reset();
            mMoveEntry.Reset();
            mWheelEntry.Reset();
        }

        void GuiMan::Pose()
        {
        }

        bool GuiMan::_ButtonHandler(system::Mouse::Button aButton, system::Mouse::State aState)
        {
            return false;
        }

        bool GuiMan::_KeyHandler(system::Key::Code aCode, system::Key::State aState)
        {
            return false;
        }

        void GuiMan::_MoveHandler(natural aX, natural aY)
        {
        }

        void GuiMan::_WheelHandler(float aWheel)
        {
        }

        void GuiMan::_ResizeHandler(natural width, natural height)
        {
        }

    }
}
