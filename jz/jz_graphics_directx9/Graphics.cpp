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

#define PERF_HUD 1

#include <jz_core/Prereqs.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_core/Logger.h>
#   include <jz_core/Memory.h>
#   include <jz_graphics/Effect.h>
#   include <jz_graphics/Graphics.h>
#   include <jz_graphics/VolatileTexture.h>
#   include <jz_graphics_directx9/DirectX9.h>
#   include <jz_graphics_directx9/DxEffectStateManager.h>
#   include <jz_system/Files.h>
#   include <jz_system/System.h>
#   include <jz_system/Thread.h>
#   include <jz_system/Win32.h>

namespace jz
{
    graphics::Graphics* graphics::Graphics::mspSingleton = null;
    namespace graphics
    {

        static IDirect3D9* gspD3D9 = null;
        struct IDirect3DDevice9* gpD3dDevice9 = null;

        ID3DXEffectPool* gpEffectPool = null;
        DxEffectStateManager* gpEffectStateManager = null;

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
            case WM_ACTIVATE:
                if (aWordParam == WA_ACTIVE || aWordParam == WA_CLICKACTIVE)
                {
                    graphics.SetActive(true);
                    return 0;
                }
                else if (aWordParam == WA_INACTIVE)
                {
                    graphics.SetActive(false);
                    if (graphics.GetFullscreen())
                    {
                        ShowWindow(system::gpWindowHandle, SW_MINIMIZE);
                    }

                    return 0;
                }
            break;
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
            break;
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
                    }
                    else if (aWordParam == SIZE_MAXIMIZED)
                    {
                        graphics.SetMinimized(false);
                        graphics.SetFullscreen(true);
                    }
                }

                return 0;
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
            if (abMinimized != mbMinimized)
            {
                mbMinimized = abMinimized;
                if (mSettings.bIsFullscreen)
                {
                    _ResetDevice();
                }
            }
        }

        void Graphics::_Reset()
        {
            if (mbLost)
            {
                gpEffectStateManager->Reset();

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
                mbLost = false;
            }
        }

        void Graphics::_Lost()
        {
            if (!mbLost)
            {
                mbLost = true;
                OnLost();

                for (VolatileObjects::reverse_iterator I = mVolatileObjects.rbegin(); I != mVolatileObjects.rend(); I++)
                {
                    I->second->Lost();
                }

                for (Objects::reverse_iterator I = mObjects.rbegin(); I != mObjects.rend(); I++)
                {
                    I->second->Lost();
                }
            }
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
            : mbActive(false), mbBegin(false), mbLost(true), mbLoaded(false), mbNeedsResize(false),
            mbOwnsWindow(false), mbMinimized(false),
            mpActiveEffect(null), mpActiveMesh(null), mpActivePass(null)
        {
            if (!system::System::GetSingletonExists())
            {
                throw std::exception(__FUNCTION__ ": System must be instantiated before Graphics.");
            }

            _SetDefaultSettings();

            gspD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
            
            if (!gspD3D9) { throw exception(__FUNCTION__ ": failed creating d3d9."); }

            if (abStartFullscreen) { SetFullscreen(true); }
            _InitWindow();
            _ResetDevice();

            mpDefaultTexture = Create<VolatileTexture>(1, 1, VolatileTexture::kA8R8G8B8, DefaultTexturePopulate);
        }

        Graphics::~Graphics()
        {
            _Lost();
            _Unload();
            
            _DestroyDevice();
            _DeinitWindow();
            SafeRelease(gspD3D9);
        }

        bool Graphics::Begin(const ColorRGBA& c, bool abClear)
        {
            JZ_ASSERT(!mbBegin);

            if (mbActive && !mbMinimized)
            {
                if (mbLost || mbNeedsResize)
                {
                    if (!gpD3dDevice9) { _ResetDevice(); }
                    else
                    {
                        HRESULT hr = gpD3dDevice9->TestCooperativeLevel();
                        if (hr == D3D_OK || hr == D3DERR_DEVICENOTRESET)
                        {
                            _ResetDevice();
                        }
                    }

                    return false;
                }
                else if (gpD3dDevice9)
                {
                    if (abClear)
                    {
                        JZ_DEBUG_DX_FAIL(gpD3dDevice9->Clear(
                            0, 
                            null, 
                            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
                            D3DCOLOR_COLORVALUE(c.R, c.G, c.B, c.A),
                            1.0f,
                            0));
                    }

                    mbBegin = SUCCEEDED(gpD3dDevice9->BeginScene());

                    if (mbBegin)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        void Graphics::End(bool abPresent)
        {
            if (gpD3dDevice9 && mbBegin)
            {
                JZ_DEBUG_DX_FAIL(gpD3dDevice9->EndScene());

                if (abPresent)
                {
                    HRESULT hr = gpD3dDevice9->Present(null, null, null, null);

                    if (FAILED(hr))
                    {
                        if (hr == D3DERR_DEVICELOST || hr == D3DERR_DRIVERINTERNALERROR)
                        {
                            _Lost();
                        }
                        else
                        {
                            JZ_DEBUG_DX_FAIL(hr);
                        }
                    }
                }
            }

            mbBegin = false;
        }

        void Graphics::Clear(unatural aClearOptions, const ColorRGBA& aClearColor, float aZdepth, u8 aStencil)
        {
            DWORD clearOptions = 0u;
            clearOptions |= ((aClearOptions & kColor) != 0) ? D3DCLEAR_TARGET : 0u;
            clearOptions |= ((aClearOptions & kDepth) != 0) ? D3DCLEAR_ZBUFFER : 0u;
            clearOptions |= ((aClearOptions & kStencil) != 0) ? D3DCLEAR_STENCIL : 0u;

            JZ_DEBUG_DX_FAIL(gpD3dDevice9->Clear(
                0,
                null,
                clearOptions,
                D3DCOLOR_COLORVALUE(aClearColor.R, aClearColor.G, aClearColor.B, aClearColor.A),
                aZdepth,
                aStencil));
        }

        void Graphics::DrawPrimitives(PrimitiveType::Enum aType, voidc_p aVertices, size_t aPrimitiveCount, size_t aVertexStrideInBytes)
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->DrawPrimitiveUP(Convert(aType), aPrimitiveCount, aVertices, aVertexStrideInBytes));
        }

        void Graphics::DrawIndexedPrimitives(PrimitiveType::Enum aType, voidc_p aIndices, size_t aPrimitiveCount, voidc_p aVertices, size_t aVertexCount, size_t aVertexStrideInBytes)
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->DrawIndexedPrimitiveUP(Convert(aType), 0u, aVertexCount, aPrimitiveCount, aIndices, D3DFMT_INDEX16, aVertices, aVertexStrideInBytes)); 
        }

        void Graphics::SetViewport(const Viewport& v)
        {
            D3DVIEWPORT9 vp;
            vp.X = v.X;
            vp.Y = v.Y;
            vp.Width = v.Width;
            vp.Height = v.Height;
            vp.MinZ = v.MinZ;
            vp.MaxZ = v.MaxZ;

            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetViewport(&vp));
        }

        void Graphics::EnableClipPlane(unatural aIndex)
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetRenderState(D3DRS_CLIPPLANEENABLE, (1 << aIndex)));
        }

        void Graphics::DisableClipPlanes()
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetRenderState(D3DRS_CLIPPLANEENABLE, 0u));
        }

        void Graphics::SetClipPlane(unatural aIndex, const Vector4& v)
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetClipPlane(aIndex, v.pData));
        }

        void Graphics::BeginGraphicsEventMark(const string& s)
        {
            D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 255, 255, 255), StringUtility::Convert(s).c_str());
        }

        void Graphics::EndGraphicsEventMark()
        {
            D3DPERF_EndEvent();
        }

        inline DWORD Convert(float v)
        {
            return *((DWORD*)&v);
        }

        static void _Initialize(DWORD rs[DxEffectStateManager::kMaxRenderStates], bool mask[DxEffectStateManager::kMaxRenderStates])
        {
            memset(rs, 0, sizeof(DWORD) * DxEffectStateManager::kMaxRenderStates);
            memset(mask, 0, sizeof(bool) * DxEffectStateManager::kMaxRenderStates);

    #       define JZ_HELPER(a, b) \
                rs[a] = b; \
                mask[a] = true;

            JZ_HELPER(D3DRS_ZENABLE, D3DZB_TRUE);
            JZ_HELPER(D3DRS_FILLMODE, D3DFILL_SOLID);
            JZ_HELPER(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
            JZ_HELPER(D3DRS_ZWRITEENABLE, TRUE);
            JZ_HELPER(D3DRS_ALPHATESTENABLE, FALSE);
            JZ_HELPER(D3DRS_LASTPIXEL, TRUE);
            JZ_HELPER(D3DRS_SRCBLEND, D3DBLEND_ONE);
            JZ_HELPER(D3DRS_DESTBLEND, D3DBLEND_ZERO);
            JZ_HELPER(D3DRS_CULLMODE, D3DCULL_CCW);
            JZ_HELPER(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
            JZ_HELPER(D3DRS_ALPHAREF, 127);
            JZ_HELPER(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
            JZ_HELPER(D3DRS_DITHERENABLE, FALSE);
            JZ_HELPER(D3DRS_ALPHABLENDENABLE, FALSE);
            JZ_HELPER(D3DRS_FOGENABLE, FALSE);
            JZ_HELPER(D3DRS_SPECULARENABLE, FALSE);
            JZ_HELPER(D3DRS_FOGCOLOR, D3DCOLOR_COLORVALUE(0, 0, 0, 1.0f));
            JZ_HELPER(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
            JZ_HELPER(D3DRS_FOGSTART, Convert(0.0f));
            JZ_HELPER(D3DRS_FOGEND, Convert(1.0f));
            JZ_HELPER(D3DRS_FOGDENSITY, Convert(1.0f));
            JZ_HELPER(D3DRS_RANGEFOGENABLE, FALSE);
            JZ_HELPER(D3DRS_STENCILENABLE, FALSE);
            JZ_HELPER(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
            JZ_HELPER(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
            JZ_HELPER(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
            JZ_HELPER(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
            JZ_HELPER(D3DRS_STENCILREF, 0);
            JZ_HELPER(D3DRS_STENCILMASK, 0xFFFFFFFF);
            JZ_HELPER(D3DRS_STENCILWRITEMASK, 0xFFFFFFFF);
            JZ_HELPER(D3DRS_TEXTUREFACTOR,  0xFFFFFFFF);
            JZ_HELPER(D3DRS_WRAP0, 0);
            JZ_HELPER(D3DRS_WRAP1, 0);
            JZ_HELPER(D3DRS_WRAP2, 0);
            JZ_HELPER(D3DRS_WRAP3, 0);
            JZ_HELPER(D3DRS_WRAP4, 0);
            JZ_HELPER(D3DRS_WRAP5, 0);
            JZ_HELPER(D3DRS_WRAP6, 0);
            JZ_HELPER(D3DRS_WRAP7, 0);
            JZ_HELPER(D3DRS_CLIPPING, TRUE);
            JZ_HELPER(D3DRS_LIGHTING, FALSE);
            JZ_HELPER(D3DRS_AMBIENT, D3DCOLOR_COLORVALUE(0, 0, 0, 1.0f));
            JZ_HELPER(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
            JZ_HELPER(D3DRS_COLORVERTEX, FALSE);
            JZ_HELPER(D3DRS_LOCALVIEWER, FALSE);
            JZ_HELPER(D3DRS_NORMALIZENORMALS, FALSE);
            JZ_HELPER(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
            JZ_HELPER(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
            JZ_HELPER(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
            JZ_HELPER(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
            JZ_HELPER(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
            JZ_HELPER(D3DRS_POINTSIZE, Convert(0.0f));
            JZ_HELPER(D3DRS_POINTSIZE_MIN, Convert(1.0f));
            JZ_HELPER(D3DRS_POINTSPRITEENABLE, FALSE);
            JZ_HELPER(D3DRS_POINTSCALEENABLE, FALSE);
            JZ_HELPER(D3DRS_POINTSCALE_A, Convert(1.0f));
            JZ_HELPER(D3DRS_POINTSCALE_B, Convert(0.0f));
            JZ_HELPER(D3DRS_POINTSCALE_C, Convert(0.0f));
            JZ_HELPER(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
            JZ_HELPER(D3DRS_MULTISAMPLEMASK, 0xFFFFFFFF);
            JZ_HELPER(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);
            JZ_HELPER(D3DRS_DEBUGMONITORTOKEN, D3DDMT_ENABLE);
            JZ_HELPER(D3DRS_POINTSIZE_MAX, Convert(64.0f));
            JZ_HELPER(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
            JZ_HELPER(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_ALPHA);
            JZ_HELPER(D3DRS_TWEENFACTOR, Convert(0.0f));
            JZ_HELPER(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            JZ_HELPER(D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC);
            JZ_HELPER(D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR);
            JZ_HELPER(D3DRS_SCISSORTESTENABLE, FALSE);
            JZ_HELPER(D3DRS_SLOPESCALEDEPTHBIAS, 0);
            JZ_HELPER(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
            JZ_HELPER(D3DRS_MINTESSELLATIONLEVEL, Convert(1.0f));
            JZ_HELPER(D3DRS_MAXTESSELLATIONLEVEL, Convert(1.0f));
            JZ_HELPER(D3DRS_ADAPTIVETESS_X, Convert(0.0f));
            JZ_HELPER(D3DRS_ADAPTIVETESS_Z, Convert(0.0f));
            JZ_HELPER(D3DRS_ADAPTIVETESS_W, Convert(0.0f));
            JZ_HELPER(D3DRS_ENABLEADAPTIVETESSELLATION, FALSE);
            JZ_HELPER(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
            JZ_HELPER(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
            JZ_HELPER(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
            JZ_HELPER(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP);
            JZ_HELPER(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
            JZ_HELPER(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
            JZ_HELPER(D3DRS_COLORWRITEENABLE1, 0);
            JZ_HELPER(D3DRS_COLORWRITEENABLE2, 0);
            JZ_HELPER(D3DRS_COLORWRITEENABLE3, 0);
            JZ_HELPER(D3DRS_BLENDFACTOR, 0xFFFFFFFF);
            JZ_HELPER(D3DRS_SRGBWRITEENABLE, FALSE);
            JZ_HELPER(D3DRS_DEPTHBIAS, 0);
            JZ_HELPER(D3DRS_WRAP8, 0);
            JZ_HELPER(D3DRS_WRAP9, 0);
            JZ_HELPER(D3DRS_WRAP10, 0);
            JZ_HELPER(D3DRS_WRAP11, 0);
            JZ_HELPER(D3DRS_WRAP12, 0);
            JZ_HELPER(D3DRS_WRAP13, 0);
            JZ_HELPER(D3DRS_WRAP14, 0);
            JZ_HELPER(D3DRS_WRAP15, 0);
            JZ_HELPER(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
            JZ_HELPER(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
            JZ_HELPER(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
            JZ_HELPER(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);

    #       undef JZ_HELPER
        }

        void Graphics::_Load()
        {
            if (!mbLoaded)
            {
                JZ_DEBUG_DX_FAIL(D3DXCreateEffectPool(&gpEffectPool));

                DWORD initialRS[DxEffectStateManager::kMaxRenderStates];
                bool mask[DxEffectStateManager::kMaxRenderStates];
                _Initialize(initialRS, mask);
                gpEffectStateManager = new DxEffectStateManager(initialRS, mask);

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

                SafeRelease(gpEffectStateManager);
                SafeRelease(gpEffectPool);
            }
        }

        void Graphics::_DoFullscreen()
        {
            D3DDISPLAYMODE mode;
            mode.Format = D3DFMT_UNKNOWN;
            mode.Width = mSettings.ViewportWidth;
            mode.Height = mSettings.ViewportHeight;
            mode.RefreshRate = 0;
            
            JZ_ASSERT(gspD3D9 != null);
            JZ_DEBUG_DX_FAIL(gspD3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode));

            mSettings.FullscreenFormat = mode.Format;
            mSettings.FullscreenWidth = mode.Width;
            mSettings.FullscreenHeight = mode.Height;
            mSettings.FullscreenRefresh = mode.RefreshRate;
        }

        void Graphics::_UndoFullscreen()
        {

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
            if (mbOwnsWindow)
            {
                mbOwnsWindow = false;
                DestroyWindow(system::gpWindowHandle);
                system::gpWindowHandle = null;
                UnregisterClass(gskpAppName, system::gpWindowInstance);
            }
        }

        void Graphics::_DestroyDevice()
        {
            SafeRelease(gpD3dDevice9);
        }

        static void GetParameters(Graphics::Settings& settings, D3DPRESENT_PARAMETERS& d3dpp, bool abMinimized)
        {
            bool bFullscreen = (settings.bIsFullscreen && !abMinimized);

            memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
            d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
            d3dpp.BackBufferCount = 0;
            d3dpp.BackBufferFormat = (bFullscreen) ? (D3DFORMAT)settings.FullscreenFormat : D3DFMT_UNKNOWN;
            d3dpp.BackBufferHeight = (bFullscreen) ? settings.FullscreenHeight : settings.ViewportHeight;
            d3dpp.BackBufferWidth = (bFullscreen) ? settings.FullscreenWidth : settings.ViewportWidth;
            d3dpp.EnableAutoDepthStencil = TRUE;
            d3dpp.Flags = 0;
            d3dpp.FullScreen_RefreshRateInHz = (bFullscreen) ? settings.FullscreenRefresh : 0;
            d3dpp.hDeviceWindow = system::gpWindowHandle;
            d3dpp.MultiSampleQuality = 0;
            d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
            d3dpp.Windowed = (bFullscreen) ? FALSE : TRUE;
        }

        void Graphics::_ResetDevice()
        {
            if (mbNeedsResize && (mSettings.bWantsFullscreen != mSettings.bIsFullscreen))
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

            D3DPRESENT_PARAMETERS d3dpp;
            GetParameters(mSettings, d3dpp, mbMinimized);

            if (gpD3dDevice9)
            {
                _Lost();
                HRESULT hr = gpD3dDevice9->Reset(&d3dpp);

                if (FAILED(hr))
                {
                    if (hr != D3DERR_DEVICELOST)
                    {
                        _Unload();
                        SafeRelease(gpD3dDevice9);
                        JZ_DEBUG_DX_FAIL(hr);
                        return;
                    }
                }
                else
                {
                    _Reset();
                }
            }
            else
            {
                UINT adpt = D3DADAPTER_DEFAULT;
                D3DDEVTYPE dt = D3DDEVTYPE_HAL;

#              ifndef PERF_HUD
#                   ifndef NDEBUG
                        HRESULT hr = gspD3D9->CreateDevice(adpt, dt, system::gpWindowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &gpD3dDevice9);
#                   else
                        HRESULT hr = gspD3D9->CreateDevice(adpt, dt, system::gpWindowHandle, D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &gpD3dDevice9);
#                   endif
#              else
                    adpt = D3DADAPTER_DEFAULT;
                    dt = D3DDEVTYPE_HAL;

                    UINT count = gspD3D9->GetAdapterCount();
                    for (UINT i = 0u; i < count; i++)
                    {
                        D3DADAPTER_IDENTIFIER9 id;
                        if (SUCCEEDED(gspD3D9->GetAdapterIdentifier(i, 0, &id)))
                        {
                            if (strstr(id.Description, "PerfHUD"))
                            {
                                adpt = i;
                                dt = D3DDEVTYPE_REF;
                                break;
                            }
                        }
                    }

#                   ifndef NDEBUG
                        HRESULT hr = gspD3D9->CreateDevice(adpt, dt, system::gpWindowHandle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &gpD3dDevice9);
#                   else
                        HRESULT hr = gspD3D9->CreateDevice(adpt, dt, system::gpWindowHandle, D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &gpD3dDevice9);
#                   endif
#               endif

                if (FAILED(hr))
                {
                    if (hr != D3DERR_DEVICELOST)
                    {
                        JZ_DEBUG_DX_FAIL(hr);
                        return;
                    }
                }
                else
                {
                    _Load();
                    _Reset();
                }
            }
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
            mSettings.FullscreenFormat   = D3DFMT_UNKNOWN;
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

    }
}

#endif
