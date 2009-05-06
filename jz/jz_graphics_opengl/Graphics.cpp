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
#include <jz_core/Memory.h>
#include <jz_system/Files.h>
#include <jz_system/System.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/IObject.h>
#include <jz_graphics/VolatileTexture.h>
#include <jz_graphics_opengl/CG.h>
#include <jz_graphics_opengl/OpenGL.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#endif

namespace jz
{
    graphics::Graphics* graphics::Graphics::mspSingleton = null;
    namespace graphics
    {

        static HDC gspDeviceContext = null;
        static HGLRC gspOpenGLContext = null;

        static void CgIncludeHandler(CGcontext p, const char* apFilename)
        {
            CG_ASSERT();

            try
            {
                system::IReadFilePtr pFile = system::Files::GetSingleton().Open(apFilename);
                const size_t kSize = pFile->GetSize();

                MemoryBuffer<char> buf(kSize + 1u);
                if (pFile->Read(buf.Get(), kSize) != kSize)
                {
                    throw exception(__FUNCTION__ ": read failed.");
                }
                
                // add null terminator.
                buf[kSize] = 0;
                cgSetCompilerIncludeString(p, apFilename, buf.Get());
            }
            catch (std::exception&)
            {
                cgSetCompilerIncludeString(p, apFilename, null);
            }
        }

        static CGbool CullCCWSet(CGstateassignment sa)
        {
            int n = 0;
            CGbool const* p = cgGetBoolStateAssignmentValues(sa, &n);

            if (*p)
            {
                glFrontFace(GL_CW);
                glCullFace(GL_BACK);
            }
            else
            {
                glFrontFace(GL_CW);
                glCullFace(GL_FRONT);
            }

            return CG_TRUE;
        }

        static CGbool CullCCWReset(CGstateassignment sa)
        {
            glFrontFace(GL_CW);
            glCullFace(GL_BACK);

            return CG_TRUE;
        }

        static CGbool CullCCWValidate(CGstateassignment sa)
        {
            int n = 0;
            CGbool const* p = cgGetBoolStateAssignmentValues(sa, &n);

            if (!p || n != 1) { return CG_FALSE; }
            else { return CG_TRUE; }
        }

        CGcontext gpCgContext = null;

        static void InitCG()
        {
            CG_ASSERT();

            if (!gpCgContext)
            {
                gpCgContext = cgCreateContext();

#               ifndef NDEBUG
                    cgGLSetDebugMode(CG_TRUE);
#               else
                    cgGLSetDebugMode(CG_FALSE);
#               endif

                cgSetParameterSettingMode(gpCgContext, CG_IMMEDIATE_PARAMETER_SETTING);

                cgGLRegisterStates(gpCgContext);
                cgGLSetManageTextureParameters(gpCgContext, CG_TRUE);

                cgSetCompilerIncludeCallback(gpCgContext, CgIncludeHandler);

                cgSetStateCallbacks(cgCreateState(gpCgContext, "CullCCW", CG_BOOL), CullCCWSet, CullCCWReset, CullCCWValidate);
            }
        }

        static void DeinitCG()
        {
            CG_ASSERT();

            if (gpCgContext)
            {
                cgDestroyContext(gpCgContext);
                gpCgContext = null;
            }
        }

        static LRESULT CALLBACK NullHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWordParam, LPARAM aLongParam)
        {
            return 1;
        }

        static WNDPROC gspGraphicsHandler = NullHandler;

        static LRESULT CALLBACK GlobalGraphicsHandler(HWND aWindowHandle, UINT aMessage, WPARAM aWordParam, LPARAM aLongParam)
        {
            Graphics& graphics = Graphics::GetSingleton();

            switch (aMessage)
            {
                case WM_ACTIVATEAPP:
                    if (aWordParam == TRUE)
                    {
                        graphics.SetActive(true);
                        return 0;
                    }
                    else if (aWordParam == FALSE)
                    {
                        graphics.SetActive(false);
                        if (graphics.GetFullscreen())
                        {
                            ShowWindow(system::gpWindowHandle, SW_MINIMIZE);
                        }

                        return 0;
                    }
                case WM_SIZE:
                    RECT rect;
                    GetClientRect(system::gpWindowHandle, &rect);

                    if (aWordParam == SIZE_MINIMIZED || (rect.top == 0 && rect.bottom == 0))
                    {
                        graphics.SetMinimized(true);
                    }
                    else
                    {
                        if (aWordParam == SIZE_RESTORED)
                        {
                            if (graphics.GetMinimized())
                            {
                                graphics.SetMinimized(false);
                            }
                            else if (graphics.GetFullscreen())
                            {
                                graphics.SetFullscreen(false);
                            }
                            else
                            {
                                graphics.Resize(rect.right, rect.bottom);
                            }

                            return 0;
                        }
                        else if (aWordParam == SIZE_MAXIMIZED)
                        {
                            graphics.SetMinimized(false);
                            graphics.SetFullscreen(true);
                            return 0;
                        }
                    }
                break;
                case WM_SYSCOMMAND:
                    // see: http://msdn.microsoft.com/en-us/library/ms646360(VS.85).aspx
                    if (aWordParam == SC_MONITORPOWER)
                    {
                        if (aLongParam == -1)
                        {
                            // Check if this should be GetForegroundWindow()
                            if (GetActiveWindow() == system::gpWindowHandle)
                            {
                                graphics.SetActive(true);
                            }

                            return 0;
                        }
                        else if (aLongParam == 2)
                        {
                            graphics.SetActive(false);
                            return 0;
                        }
                    }
                    else if (aWordParam == SC_SCREENSAVE)
                    {
                        graphics.SetActive(false);
                        return 0;
                    }
                break;
            }
            
            return system::GlobalMessageHandler(aWindowHandle, aMessage, aWordParam, aLongParam);
        }

        void EnableGraphicsHandler()
        {
            gspGraphicsHandler = GlobalGraphicsHandler;
        }
        
        void DisableGraphicsHandler()
        {
            gspGraphicsHandler = NullHandler;
        }

        static const char* gskpAppName = "jz";

        bool Graphics::IsActive() const
        {
            return true;
        }

        natural Graphics::GetViewportWidth() const
        {
            return (mSettings.bIsFullscreen) ? mSettings.FullscreenWidth : mSettings.ViewportWidth;
        }

        natural Graphics::GetViewportHeight() const
        {
            return (mSettings.bIsFullscreen) ? mSettings.FullscreenHeight : mSettings.ViewportHeight;
        }

        void Graphics::Resize(natural width, natural height)
        {
            if (mSettings.bIsFullscreen == mSettings.bWantsFullscreen)
            {
                natural newWidth = Max(width, (natural)1);
                natural newHeight = Max(height, (natural)1);
                
                if (newWidth != mSettings.ViewportWidth || newHeight != mSettings.ViewportHeight)
                {
                    mSettings.ViewportWidth = newWidth;
                    mSettings.ViewportHeight = newHeight;
                    _RecalculateWindowSize();
                    mbNeedsResize = true;
                }
            }
        }

        void Graphics::SetFullscreen(bool abFullscreen)
        {
            if (mSettings.bWantsFullscreen != abFullscreen)
            {
                mSettings.bWantsFullscreen = abFullscreen;
                mbNeedsResize = true;
            }
        }

        void Graphics::SetMinimized(bool abMinimized)
        {
            mbMinimized = abMinimized;
        }

        void Graphics::_Reset()
        {
            for (Objects::iterator I = mObjects.begin(); I != mObjects.end(); I++)
            {
                I->second->Reset(GetViewportWidth(), GetViewportHeight());
            }

            for (VolatileObjects::iterator I = mVolatileObjects.begin(); I != mVolatileObjects.end(); I++)
            {
                I->second->Reset(GetViewportWidth(), GetViewportHeight());
            }

            OnReset(GetViewportWidth(), GetViewportHeight());

            mbNeedsResize = false;
        } 

        static void DefaultTexturePopulate(natural aWidth, natural aHeight, void_p apData, size_t aPitch)
        {
            JZ_ASSERT(aWidth == 1);
            JZ_ASSERT(aHeight == 1);

            u8_p p = (u8_p)apData;

            p[0] = 255; // B
            p[1] = 127; // G
            p[2] = 127; // R
            p[3] = 255; // A
        }

        Graphics::Graphics(bool abStartFullscreen)
            : mbActive(false), mbBegin(false), mbLoaded(false), mbNeedsResize(false),
            mbOwnsWindow(false), mbMinimized(false),
            mpActiveEffect(null), mpActiveMesh(null), mpActivePass(null)
        {
            if (!system::System::GetSingletonExists())
            {
                throw std::exception(__FUNCTION__ ": System must be instantiated before Graphics.");
            }

            _SetDefaultSettings();

            if (abStartFullscreen) { SetFullscreen(true); }
            _InitWindow();
            _ResetDevice();

            mpDefaultTexture = Create<VolatileTexture>(1, 1, VolatileTexture::kA8R8G8B8, DefaultTexturePopulate);
        }

        Graphics::~Graphics()
        {
            _Unload();
            _DestroyDevice();
            _DeinitWindow();
        }

        bool Graphics::Begin(const ColorRGBA& c, bool abClear)
        {
            OPENGL_ASSERT();
            JZ_ASSERT(!mbBegin);

            if (mbActive && !mbMinimized)
            {
                if (mbNeedsResize)
                {
                    _ResetDevice();
                }
                else
                {
                    if (abClear)
                    {
                        glClearColor(c.R, c.G, c.B, c.A);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                    }

                    mbBegin = true;
                }
            }

            return (mbBegin);
        }

#       if JZ_PLATFORM_WINDOWS
            void Graphics::End(bool abSwap)
            {
                OPENGL_ASSERT();
                if (abSwap)
                {
                    SwapBuffers(gspDeviceContext);
                }

                mbBegin = false;
            }
#       endif

        void Graphics::Clear(unatural aClearOptions, const ColorRGBA& aClearColor, float aZdepth, u8 aStencil)
        {
            GLenum clearOptions = 0u;
            clearOptions |= ((aClearOptions & kColor) != 0) ? GL_COLOR_BUFFER_BIT : 0u;
            clearOptions |= ((aClearOptions & kDepth) != 0) ? GL_DEPTH_BUFFER_BIT : 0u;
            clearOptions |= ((aClearOptions & kStencil) != 0) ? GL_STENCIL_BUFFER_BIT : 0u;

            glClearColor(aClearColor.R, aClearColor.G, aClearColor.B, aClearColor.A);
            glClearDepth(aZdepth);
            glClearStencil(aStencil);
            glClear(clearOptions);
        }

        void Graphics::DrawPrimitives(PrimitiveType::Enum aType, voidc_p aVertices, size_t aPrimitiveCount, size_t aVertexStrideInBytes)
        {
            OPENGL_ASSERT();

            // Needs to be implemented - drawing straight primitives in Open GL is more complex than Direct X
            // because it needs the declaration included in the call.
            JZ_ASSERT(false);
        }

        void Graphics::DrawIndexedPrimitives(PrimitiveType::Enum aType, voidc_p aIndices, size_t aPrimitiveCount, voidc_p aVertices, size_t aVertexCount, size_t aVertexStrideInBytes)
        {
            OPENGL_ASSERT();

            // Needs to be implemented - drawing straight primitives in Open GL is more complex than Direct X
            // because it needs the declaration included in the call.
            JZ_ASSERT(false);
        }

        void Graphics::SetViewport(const Viewport& v)
        {
            OPENGL_ASSERT();

            // TODO: implement.
            JZ_ASSERT(false);
        }

        void Graphics::_Load()
        {
            if (!mbLoaded)
            {
                EnableGraphicsHandler();

                for (Objects::iterator I = mObjects.begin(); I != mObjects.end(); I++)
                {
                    I->second->Load();
                }

                for (VolatileObjects::iterator I = mVolatileObjects.begin(); I != mVolatileObjects.end(); I++)
                {
                    I->second->Create();
                }

                OnLoad();
                mbLoaded = true;
            }
        }

        void Graphics::_Unload()
        {
            if (mbLoaded)
            {
                mbLoaded = false;
                OnUnload();

                for (VolatileObjects::reverse_iterator I = mVolatileObjects.rbegin(); I != mVolatileObjects.rend(); I++)
                {
                    I->second->Destroy();
                }

                for (Objects::reverse_iterator I = mObjects.rbegin(); I != mObjects.rend(); I++)
                {
                    I->second->Unload();
                }

                DisableGraphicsHandler();
            }
        }

#       if JZ_PLATFORM_WINDOWS
        void Graphics::_DoFullscreen()
        {
            DEVMODE dm;
            memset(&dm, 0, sizeof(DEVMODE));

            if (!EnumDisplaySettings(null, ENUM_CURRENT_SETTINGS, &dm))
            {
                LogMessage(__FUNCTION__ ": failed enumerating mode settings.", Logger::kError);
                mSettings.bWantsFullscreen = false;
            }
            else
            {
                mSettings.FullscreenHeight = dm.dmPelsHeight;
                mSettings.FullscreenRefresh = dm.dmDisplayFrequency;
                mSettings.FullscreenWidth = dm.dmPelsWidth;

                if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
                {
                    LogMessage(__FUNCTION__ ": change to fullscreen mode failed.", Logger::kError);
                    mSettings.bWantsFullscreen = false;
                }
            }
        }

        void Graphics::_UndoFullscreen()
        {
            ChangeDisplaySettings(null, 0);
        }

        void Graphics::_InitWindow()
        {
            if (!system::gpWindowHandle && system::gpWindowInstance)
            {
                if (mSettings.bWantsFullscreen)
                {
                    _DoFullscreen();
                }

                WNDCLASS windowClass;

                memset(&windowClass, 0, sizeof(WNDCLASS));
                windowClass.style         = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;
                windowClass.lpfnWndProc   = GlobalGraphicsHandler;
                windowClass.cbClsExtra    = 0;
                windowClass.cbWndExtra    = 0;
                windowClass.hInstance     = system::gpWindowInstance;
                windowClass.hIcon         = LoadIcon(system::gpWindowInstance, JZ_IDI_JZ_ICON);
                windowClass.hCursor       = LoadCursor(0, IDC_ARROW);
                windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
                windowClass.lpszMenuName  = 0;
                windowClass.lpszClassName = gskpAppName;
                JZ_E_ON_FAIL(RegisterClass(&windowClass), "failed to register window class.");

                if (mSettings.bWantsFullscreen)
                {
                    system::gpWindowHandle = CreateWindow(gskpAppName, gskpAppName, WS_POPUP, 0, 0, mSettings.FullscreenWidth, mSettings.FullscreenHeight, 0, 0, system::gpWindowInstance, 0);
                }
                else
                {
                    system::gpWindowHandle = CreateWindow(gskpAppName, gskpAppName, WS_OVERLAPPEDWINDOW, mSettings.WindowX, mSettings.WindowY, mSettings.WindowWidth, mSettings.WindowHeight, 0, 0, system::gpWindowInstance, 0);
                }
                
                JZ_E_ON_FAIL(system::gpWindowHandle, "failed to create window.");

                SetFocus(system::gpWindowHandle);
                SetForegroundWindow(system::gpWindowHandle);
                ShowWindow(system::gpWindowHandle, SW_SHOW);

                mbOwnsWindow = true;
                mSettings.bIsFullscreen = mSettings.bWantsFullscreen;
            }

            JZ_E_ON_FAIL(system::gpWindowHandle != null, "no window.");

            mbActive = true;
        }

        void Graphics::_DeinitWindow()
        {
            _UndoFullscreen();

            if (mbOwnsWindow)
            {
                mbOwnsWindow = false;
                DestroyWindow(system::gpWindowHandle);
                system::gpWindowHandle = null;
                UnregisterClass(gskpAppName, system::gpWindowInstance);
            }
        }

        uint gOpenGlFbo = 0u;

        static void DestroyOpenGLContext()
        {
            DeinitCG();

            glDeleteFramebuffers(1, &gOpenGlFbo);
            gOpenGlFbo = 0u;

            if (gspOpenGLContext)
            {
                wglMakeCurrent(null, null);
                wglDeleteContext(gspOpenGLContext);
                gspOpenGLContext = null;
            }
        }

        static void ReleaseDeviceContext()
        {
            if (gspDeviceContext)
            {
                ReleaseDC(system::gpWindowHandle, gspDeviceContext);
                gspDeviceContext = null;
            }
        }

        void Graphics::_ResetDevice()
        {
            if (mbNeedsResize && (mSettings.bWantsFullscreen != mSettings.bIsFullscreen))
            {
                // Temp: used when the window needs to be destroyed and completely recreated, eventually.
                if (false)
                {
                    _Unload();
                    _DestroyDevice();
                    _DeinitWindow();

                    try
                    {
                        _InitWindow();
                    }
                    catch (exception& e)
                    {
                        LogMessage(e.what());
                        mSettings.bWantsFullscreen = false;
                        mSettings.bIsFullscreen = false;
                        return;
                    }
                }
                else
                {
                    if (mSettings.bWantsFullscreen)
                    {
                        _DoFullscreen();
                        SetWindowPos(system::gpWindowHandle, HWND_NOTOPMOST, 0, 0, mSettings.FullscreenWidth, mSettings.FullscreenHeight, SWP_NOCOPYBITS);
                        SetWindowLongPtr(system::gpWindowHandle, GWL_STYLE, WS_POPUP);
                        // see http://msdn.microsoft.com/en-us/library/ms633545(VS.85).aspx for what this does.
                        SetWindowPos(system::gpWindowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
                    }
                    else
                    {
                        _UndoFullscreen();
                        SetWindowPos(system::gpWindowHandle, HWND_NOTOPMOST, mSettings.WindowX, mSettings.WindowY, mSettings.WindowWidth, mSettings.WindowHeight, SWP_NOCOPYBITS);
                        SetWindowLongPtr(system::gpWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                        // see http://msdn.microsoft.com/en-us/library/ms633545(VS.85).aspx for what this does.
                        SetWindowPos(system::gpWindowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
                    }
                }

                mSettings.bIsFullscreen = mSettings.bWantsFullscreen;
            }

            if (!system::gpWindowHandle)
            {
                try
                {
                    _InitWindow();
                }
                catch (exception& e)
                {
                    LogMessage(e.what());
                    mbActive = false;
                    return;
                }
            }

            if (!gspDeviceContext)
            {
                GLuint pixelFormat;
                PIXELFORMATDESCRIPTOR pixelFormatDescriptor;

                memset(&pixelFormatDescriptor, 0, sizeof(PIXELFORMATDESCRIPTOR));
                pixelFormatDescriptor.cAlphaBits   = 8;
                pixelFormatDescriptor.cColorBits   = 24;
                pixelFormatDescriptor.cDepthBits   = 24;
                pixelFormatDescriptor.cStencilBits = 8;
                pixelFormatDescriptor.dwFlags      = PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_GENERIC_ACCELERATED | PFD_SUPPORT_OPENGL;
                pixelFormatDescriptor.iLayerType   = PFD_MAIN_PLANE;
                pixelFormatDescriptor.iPixelType   = PFD_TYPE_RGBA;
                pixelFormatDescriptor.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
                pixelFormatDescriptor.nVersion     = 1;

                gspDeviceContext = GetDC(system::gpWindowHandle);
                pixelFormat = ChoosePixelFormat(gspDeviceContext, &pixelFormatDescriptor);

                if (!SetPixelFormat(gspDeviceContext, pixelFormat, &pixelFormatDescriptor))
                {
                    ReleaseDeviceContext();
                    LogMessage(__FUNCTION__ ": failed setting pixel format.", Logger::kError);
                    Sleep(100);
                    return;
                }
            }

            if (!gspOpenGLContext)
            {
                gspOpenGLContext = wglCreateContext(gspDeviceContext);
                if (!wglMakeCurrent(gspDeviceContext, gspOpenGLContext))
                {
                    DestroyOpenGLContext();
                    LogMessage(__FUNCTION__ ": failed making open gl context current.", Logger::kError);
                    Sleep(100);
                    return;
                }

                glewInit();

                glGenFramebuffers(1, &gOpenGlFbo);

                InitCG();

                _Load();
            }

            if (mbNeedsResize)
            {
                glViewport(0, 0, GetViewportWidth(), GetViewportHeight());

                _Reset();
            }
        }

        void Graphics::_DestroyDevice()
        {
            DestroyOpenGLContext();
            ReleaseDeviceContext();
        }

        void Graphics::_RecalculateViewportSize()
        {
            mSettings.ViewportHeight = mSettings.WindowHeight - (mSettings.WindowBottomBorder + mSettings.WindowTopBorder);
            mSettings.ViewportWidth  = mSettings.WindowWidth  - (mSettings.WindowLeftBorder   + mSettings.WindowRightBorder);
        }
        
        void Graphics::_RecalculateWindowSize()
        {
            mSettings.WindowHeight = mSettings.ViewportHeight + (mSettings.WindowBottomBorder + mSettings.WindowTopBorder);
            mSettings.WindowWidth  = mSettings.ViewportWidth  + (mSettings.WindowLeftBorder   + mSettings.WindowRightBorder);
        }

        void Graphics::_SetDefaultSettings()
        {
            mSettings.bIsFullscreen      = false;
            mSettings.bWantsFullscreen   = false;
            
            mSettings.FullscreenWidth    = 640;
            mSettings.FullscreenHeight   = 480;
            mSettings.FullscreenRefresh  = 0;

            mSettings.WindowX            = 0;
            mSettings.WindowY            = 0;

            mSettings.WindowLeftBorder   = GetSystemMetrics(SM_CXSIZEFRAME);
            mSettings.WindowRightBorder  = mSettings.WindowLeftBorder;

            mSettings.WindowBottomBorder = GetSystemMetrics(SM_CYSIZEFRAME);
            mSettings.WindowTopBorder    = mSettings.WindowBottomBorder + GetSystemMetrics(SM_CYCAPTION);

            mSettings.WindowWidth        = GetSystemMetrics(SM_CXFULLSCREEN);
            mSettings.WindowHeight       = GetSystemMetrics(SM_CYFULLSCREEN);            

            _RecalculateViewportSize();
        }
    #   endif

    }
}
