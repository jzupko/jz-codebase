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

#include <jz_system/Input.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#endif

namespace jz
{
    system::Input* system::Input::mspSingleton = 0;
    namespace system
    {

        Input::Input()
            : mbMouseVisible(true),
            mMousePrevX(0), mMousePrevY(0)
        { }

        Input::~Input()
        {}

    #   if JZ_PLATFORM_WINDOWS
            static HCURSOR gsCursor = null;

            void Input::SetMouseVisible(bool b)
            {
                if (b != mbMouseVisible)
                {
                    mbMouseVisible = b;

                    if (!mbMouseVisible)
                    {
                        gsCursor = GetCursor();
                        SetCursor(null);
                    }
                    else
                    {
                        SetCursor(gsCursor);
                        gsCursor = null;
                    }
                }
            }
    #   endif

    #   if JZ_PLATFORM_WINDOWS
            extern HWND gpWindowHandle;
            natural Input::GetMouseX() const
            {
                JZ_ASSERT(gpWindowHandle != null);

                POINT point;
                GetCursorPos(&point);

                ScreenToClient(gpWindowHandle, &point);

                return point.x;
            }

            natural Input::GetMouseY() const
            {
                JZ_ASSERT(gpWindowHandle != null);

                POINT point;
                GetCursorPos(&point);

                ScreenToClient(gpWindowHandle, &point);

                return point.y;
            }

            void Input::SetMousePosition(natural x, natural y)
            {
                JZ_ASSERT(gpWindowHandle != null);

                POINT point;
                point.x = x;
                point.y = y;

                if (ClientToScreen(gpWindowHandle, &point))
                {
                    SetCursorPos(point.x, point.y);
                }
            }

            void Input::UpdateMousePosition(natural x, natural y)
            {
                if (x != mMousePrevX || y != mMousePrevY)
                {
                    OnMouseMoveAbsolute(x, y);
                    OnMouseMoveDelta(x - mMousePrevX, y - mMousePrevY);

                    mMousePrevX = x;
                    mMousePrevY = y;
                }
            }
    #   endif

    }   
}

