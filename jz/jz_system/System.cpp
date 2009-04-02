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
#include <jz_system/Input.h>
#include <jz_system/Files.h>
#include <jz_system/System.h>
#include <jz_system/Time.h>
#include <list>

namespace jz
{
    system::System* system::System::mspSingleton = null;
    namespace system
    {

        static Files* gspFiles = null;
        static Input* gspInput = null;
        static Time* gspTime = null;

    #   if JZ_PLATFORM_WINDOWS
            HINSTANCE gpWindowInstance = null;
            HWND gpWindowHandle = null;

            static LRESULT CALLBACK NullHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWordParam, LPARAM aLongParam)
            {
                return 1;
            }

            LRESULT CALLBACK GlobalInputHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWordParam, LPARAM aLongParam);

            static WNDPROC gspClientMessageHandler = NullHandler;
            static WNDPROC gspInputHandler = NullHandler;

            static void Deinit()
            {
                gpWindowHandle = null;
                gpWindowInstance = null;

                gspInputHandler = NullHandler;
                SafeDelete(gspInput);

                SafeDelete(gspFiles);
                SafeDelete(gspTime);

                gspClientMessageHandler = NullHandler;
            }

            System::System(HINSTANCE apWindowInstance, HWND apWindowHandle, WNDPROC apMessageHandler)
            {
                if (apMessageHandler)
                {
                    gspClientMessageHandler = apMessageHandler;
                }

                if (!gspTime)
                {
                    try
                    {
                        gspTime = new Time();
                    }
                    catch (std::exception&)
                    {
                        Deinit();
                        throw;
                    }
                }
                
                if (!gspFiles)
                {
                    try
                    {
                        gspFiles = new Files();
                    }
                    catch (std::exception&)
                    {
                        Deinit();
                        throw;
                    }
                }
            
                if (!gspInput)
                {
                    try
                    {
                        gspInput = new Input();
                    }
                    catch (std::exception&)
                    {
                        Deinit();
                        throw;
                    }

                    gspInputHandler = GlobalInputHandler;
                }

                gpWindowInstance = (apWindowInstance) ? apWindowInstance : GetModuleHandle(null);
                gpWindowHandle = apWindowHandle;
            }

            System::~System()
            {
                Deinit();
            }

            void EnableInputHandler()
            {
                gspInputHandler = GlobalInputHandler;
            }
            
            void DisableInputHandler()
            {
                gspInputHandler = NullHandler;
            }      

            LRESULT CALLBACK GlobalMessageHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWordParam, LPARAM aLongParam)
            {
                LRESULT res = 1;

                switch (aMessage)
                {
                case WM_CLOSE: PostQuitMessage(0); res = 0; break;
                }

                if (res != 0) { res = gspInputHandler(aWindowHandle, aMessage, aWordParam, aLongParam); }
                if (res != 0) { res = gspClientMessageHandler(aWindowHandle, aMessage, aWordParam, aLongParam); }
                if (res != 0) { res = DefWindowProc(aWindowHandle, aMessage, aWordParam, aLongParam); }

                return res;
            }

            LRESULT CALLBACK GlobalInputHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWordParam, LPARAM aLongParam)
            {
                // TODO: what was I smoking when I made this a list?
                static list<WPARAM> sKeys;
                
                static bool sbLeftDown = false;
                static bool sbMiddleDown = false;
                static bool sbRightDown = false;
                static bool sbX1Down = false;
                static bool sbX2Down = false;

                Input& input = *gspInput;

                switch (aMessage)
                {
                    case WM_CAPTURECHANGED:
                        if (aLongParam != 0)
                        {
                            if (sbLeftDown)
                            {
                                sbLeftDown = false;
                                try
                                {
                                    input.OnButton(Mouse::kLeft, Mouse::kReleased);
                                }
                                catch (std::exception& e)
                                {
                                    LogMessage(e.what(), Logger::kError);
                                }
                            }
                            
                            if (sbMiddleDown)
                            {
                                sbMiddleDown = false;
                                try
                                {
                                    input.OnButton(Mouse::kMiddle, Mouse::kReleased);
                                }
                                catch (std::exception& e)
                                {
                                    LogMessage(e.what(), Logger::kError);
                                }
                            }
                            
                            if (sbRightDown)
                            {
                                sbRightDown = false;
                                try
                                {
                                    input.OnButton(Mouse::kRight, Mouse::kReleased);
                                }
                                catch (std::exception& e)
                                {
                                    LogMessage(e.what(), Logger::kError);
                                }
                            }

                            if (sbX1Down)
                            {
                                sbX1Down = false;
                                try
                                {
                                    input.OnButton(Mouse::kX1, Mouse::kReleased);
                                }
                                catch (std::exception& e)
                                {
                                    LogMessage(e.what(), Logger::kError);
                                }
                            }

                           if (sbX2Down)
                            {
                                sbX2Down = false;
                                try
                                {
                                    input.OnButton(Mouse::kX2, Mouse::kReleased);
                                }
                                catch (std::exception& e)
                                {
                                    LogMessage(e.what(), Logger::kError);
                                }
                            }
                        }
                        return 0;
                    break;
                    case WM_KILLFOCUS:
                        for (list<WPARAM>::iterator I = sKeys.begin(); I != sKeys.end(); I++)
                        {
                            try
                            {
                                input.OnKey((Key::Code)*I, Key::kReleased);
                            }
                            catch (std::exception& e)
                            {
                                LogMessage(e.what(), Logger::kError);
                            }
                        }
                        
                        sKeys.clear();
                        return 0;
                    break;
	                case WM_KEYDOWN:
                        try
                        {
                            input.OnKey((Key::Code)aWordParam, Key::kPressed);
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        sKeys.push_back(aWordParam);
                        return 0;
                    break;
	                case WM_KEYUP:
                        sKeys.remove(aWordParam);
                        try
                        {
                            input.OnKey((Key::Code)aWordParam, Key::kReleased);    
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        return 0;
                    break;
	                case WM_XBUTTONDOWN:
                        {
                            u32 xparam = GET_XBUTTON_WPARAM(aWordParam);
                            try
                            {
                                input.OnButton((xparam == XBUTTON1) ? Mouse::kX1 : Mouse::kX2, Mouse::kPressed);
                            }
                            catch (std::exception& e)
                            {
                                LogMessage(e.what(), Logger::kError);
                            }

                            SetCapture(aWindowHandle);
                            if (xparam == XBUTTON1) { sbX1Down = true; }
                            else { sbX2Down = true; }
                            return 0;
                        }
                    break;
	                case WM_XBUTTONUP:
                        {
                            u32 xparam = GET_XBUTTON_WPARAM(aWordParam);
                            if (xparam == XBUTTON1) { sbX1Down = false; }
                            else { sbX2Down = false; }

                            if (!sbLeftDown && !sbMiddleDown && !sbRightDown)
                            {
                                ReleaseCapture();
                            }
                            
                            try
                            {
                                input.OnButton((xparam == XBUTTON1) ? Mouse::kX1 : Mouse::kX2, Mouse::kReleased);
                            }
                            catch (std::exception& e)
                            {
                                LogMessage(e.what(), Logger::kError);
                            }

                            return 0;
                        }
                    break;
	                case WM_LBUTTONDOWN:
                        try
                        {
                            input.OnButton(Mouse::kLeft, Mouse::kPressed);
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        SetCapture(aWindowHandle);
                        sbLeftDown = true;
                        return 0;
                    break;
	                case WM_LBUTTONUP:
                        sbLeftDown = false;

                        if (!sbMiddleDown && !sbRightDown && !sbX1Down && !sbX2Down)
                        {
                            ReleaseCapture();
                        }
                        
                        try
                        {
                            input.OnButton(Mouse::kLeft, Mouse::kReleased);
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        return 0;
                    break;
	                case WM_MBUTTONDOWN:
                        try
                        {
                            input.OnButton(Mouse::kMiddle, Mouse::kPressed);
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        SetCapture(aWindowHandle);
                        sbMiddleDown = true;

                        return 0;
                    break;
	                case WM_MBUTTONUP:
                        sbMiddleDown = false;

                        if (!sbLeftDown && !sbRightDown && !sbX1Down && !sbX2Down)
                        {
                            ReleaseCapture();
                        }
                        
                        try
                        {
                            input.OnButton(Mouse::kMiddle, Mouse::kReleased);
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        return 0;
                    break;
	                case WM_RBUTTONDOWN:
                        try
                        {
                            input.OnButton(Mouse::kRight, Mouse::kPressed);
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        SetCapture(aWindowHandle);
                        sbRightDown = true;

                        return 0;
                    break;
	                case WM_RBUTTONUP:
                        sbRightDown = false;

                        if (!sbLeftDown && !sbMiddleDown && !sbX1Down && !sbX2Down)
                        {
                            ReleaseCapture();
                        }

                        try
                        {
                            input.OnButton(Mouse::kRight, Mouse::kReleased);
                        }
                        catch (std::exception& e)
                        {
                            LogMessage(e.what(), Logger::kError);
                        }

                        return 0;
                    break;
                    case WM_MOUSEMOVE:
                        {
                            input.UpdateMousePosition(GET_X_LPARAM(aLongParam), GET_Y_LPARAM(aLongParam));
                        }

                        return 0;
                    break;
	                case WM_MOUSEWHEEL:
                        {
                            float wheel = float((short)HIWORD(aWordParam)) / float(WHEEL_DELTA);

                            if (input.OnMouseWheel)
                            {
                                try
                                {
                                    (input.OnMouseWheel)(wheel);
                                }
                                catch (std::exception& e)
                                {
                                    LogMessage(e.what(), Logger::kError);
                                }
                            }
                        }
                        return 0;
                    break;
	            }

                return 1;
            }
    #   endif

    }
}
