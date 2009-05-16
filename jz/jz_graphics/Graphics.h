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
#ifndef _JZ_GRAPHICS_GRAPHICS_H_
#define _JZ_GRAPHICS_GRAPHICS_H_

#include <jz_core/Auto.h>
#include <jz_core/Color.h>
#include <jz_core/Event.h>
#include <jz_core/Memory.h>
#include <jz_core/Prereqs.h>
#include <jz_core/Utility.h>
#include <jz_graphics/PrimitiveType.h>
#include <jz_graphics/Viewport.h>
#include <map>
#include <list>

namespace jz
{
    namespace graphics
    {

        class IObject; typedef AutoPtr<IObject> ObjectPtr;
        class IVolatileObject; typedef AutoPtr<IVolatileObject> VolatileObjectPtr;
        class Effect; typedef AutoPtr<Effect> EffectPtr;
        class Mesh;
        class Pass;
        class VolatileTexture; typedef AutoPtr<VolatileTexture> VolatileTexturePtr;
        class Graphics : public Singleton<Graphics>
        {
        public:
            JZ_ALIGNED_NEW

            enum ClearOptions
            {
                kColor = (1 << 0),
                kDepth = (1 << 1),
                kStencil = (1 << 2)
            };

            struct Settings
            {
                Settings()
                    : bIsFullscreen(false),
                      bWantsFullscreen(false),
                      FullscreenFormat(0), FullscreenWidth(0), FullscreenHeight(0), FullscreenRefresh(0),
                      WindowX(0), WindowY(0),
                      ViewportWidth(0), ViewportHeight(0),
                      WindowWidth(0), WindowHeight(0),
                      WindowBottomBorder(0), WindowLeftBorder(0),
                      WindowRightBorder(0), WindowTopBorder(0)
                {}
                
                bool bIsFullscreen;
                bool bWantsFullscreen;
                natural FullscreenFormat;
                natural FullscreenWidth;
                natural FullscreenHeight;
                natural FullscreenRefresh;
                natural ViewportWidth;
                natural ViewportHeight;
                natural WindowX;
                natural WindowY;
                natural WindowWidth;
                natural WindowHeight;
                natural WindowBottomBorder;
                natural WindowLeftBorder;
                natural WindowRightBorder;
                natural WindowTopBorder;
            };

            Graphics(bool abStartFullscreen = false);
            ~Graphics();

            bool Begin(const ColorRGBA& aClearColor = ColorRGBA::kBlack, bool abClear = true);
            void End(bool abPresent = true);

            void Clear(unatural aClearOptions, const ColorRGBA& aClearColor = ColorRGBA::kBlack, float aZdepth = 1.0f, u8 aStencil = 0u);

            void DrawPrimitives(PrimitiveType::Enum aType, voidc_p aVertices, size_t aPrimitiveCount, size_t aVertexStrideInBytes);
            void DrawIndexedPrimitives(PrimitiveType::Enum aType, voidc_p aIndices, size_t aPrimitiveCount, voidc_p aVertices, size_t aVertexCount, size_t aVertexStrideInBytes);

            void SetViewport(const Viewport& v);

            natural GetViewportWidth() const;
            natural GetViewportHeight() const;

            template <typename T>
            T* Create(const string& aFilename, typename EnableIf<tr1::is_base_of<IObject, T>::value>::type* pDummy = null)
            {
                return Helper<T>(aFilename);
            }

            template <typename T>
            T* Create(typename EnableIf<tr1::is_base_of<IVolatileObject, T>::value>::type* pDummy = null)
            {
                T* p(new T());
                mVolatileObjects.insert(make_pair(T::SortOrder, p));
                if (mbLoaded) { p->Create(); }
                if (!mbLost) { p->Reset(GetViewportWidth(), GetViewportHeight()); }

                return p;
            }

            template <typename T, typename A1>
            T* Create(A1 a1, typename EnableIf<tr1::is_base_of<IVolatileObject, T>::value>::type* pDummy = null)
            {
                T* p(new T(a1));
                mVolatileObjects.insert(make_pair(T::SortOrder, p));
                if (mbLoaded) { p->Create(); }
                if (!mbLost) { p->Reset(GetViewportWidth(), GetViewportHeight()); }

                return p;
            }

            template <typename T, typename A1, typename A2>
            T* Create(A1 a1, A2 a2, typename EnableIf<tr1::is_base_of<IVolatileObject, T>::value>::type* pDummy = null)
            {
                T* p(new T(a1, a2));
                mVolatileObjects.insert(make_pair(T::SortOrder, p));
                if (mbLoaded) { p->Create(); }
                if (!mbLost) { p->Reset(GetViewportWidth(), GetViewportHeight()); }

                return p;
            }

            template <typename T, typename A1, typename A2, typename A3>
            T* Create(A1 a1, A2 a2, A3 a3, typename EnableIf<tr1::is_base_of<IVolatileObject, T>::value>::type* pDummy = null)
            {
                T* p(new T(a1, a2, a3));
                mVolatileObjects.insert(make_pair(T::SortOrder, p));
                if (mbLoaded) { p->Create(); }
                if (!mbLost) { p->Reset(GetViewportWidth(), GetViewportHeight()); }

                return p;
            }

            template <typename T, typename A1, typename A2, typename A3, typename A4>
            T* Create(A1 a1, A2 a2, A3 a3, A4 a4, typename EnableIf<tr1::is_base_of<IVolatileObject, T>::value>::type* pDummy = null)
            {
                T* p(new T(a1, a2, a3, a4));
                mVolatileObjects.insert(make_pair(T::SortOrder, p));
                if (mbLoaded) { p->Create(); }
                if (!mbLost) { p->Reset(GetViewportWidth(), GetViewportHeight()); }

                return p;
            }

            template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
            T* Create(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, typename EnableIf<tr1::is_base_of<IVolatileObject, T>::value>::type* pDummy = null)
            {
                T* p(new T(a1, a2, a3, a4, a5));
                mVolatileObjects.insert(make_pair(T::SortOrder, p));
                if (mbLoaded) { p->Create(); }
                if (!mbLost) { p->Reset(GetViewportWidth(), GetViewportHeight()); }

                return p;
            }

            void SetActiveEffect(Effect* p) { mpActiveEffect = p; }
            Effect* GetActiveEffect() const { return mpActiveEffect; }

            void SetActiveMesh(Mesh* p) { mpActiveMesh = p; }
            Mesh* GetActiveMesh() const { return mpActiveMesh; }

            void SetActivePass(Pass* p) { mpActivePass = p; }
            Pass* GetActivePass() const { return mpActivePass; }

            VolatileTexture* GetDefaultTexture() const { return (mpDefaultTexture.Get()); }

            bool IsActive() const;
            bool IsLoaded() const { return mbLoaded; }
            bool IsLost() const { return mbLost; }

            Event<void()> OnLost;
            Event<void(natural, natural)> OnReset;
            Event<void()> OnLoad;
            Event<void()> OnUnload;

            bool GetMinimized() const { return (mbMinimized); }
            bool GetFullscreen() const { return (mSettings.bIsFullscreen); }
            void Resize(natural width, natural height);
            void SetActive(bool abActive) { mbActive = abActive; }
            void SetFullscreen(bool abFullscreen);
            void SetMinimized(bool abMinimized);

            void EnableClipPlane(unatural aIndex);
            void DisableClipPlanes();
            void SetClipPlane(unatural aIndex, const Vector4& v);

            void BeginGraphicsEventMark(const string& s);
            void EndGraphicsEventMark();

            uint GetMaxTextureDimension() const;

        private:
            Graphics(const Graphics& n);
            Graphics& operator=(const Graphics& n); 

            Settings mSettings;
            bool mbActive;
            bool mbBegin;
            bool mbLoaded;
            bool mbLost;
            bool mbMinimized;
            bool mbNeedsResize;
            bool mbOwnsWindow;

            Effect* mpActiveEffect;
            Mesh* mpActiveMesh;
            Pass* mpActivePass;
            VolatileTexturePtr mpDefaultTexture;

            typedef pair<int, string> Key;
            typedef map<Key, ObjectPtr> Objects;
            Objects mObjects;

            typedef multimap<int, VolatileObjectPtr> VolatileObjects;
            VolatileObjects mVolatileObjects;

            void _InitWindow();
            void _DeinitWindow();
            void _Load();
            void _Unload();
            void _Lost();
            void _Reset();
            void _DestroyDevice();
            void _ResetDevice();
            void _Resize();
            void _RecalculateViewportSize();
            void _RecalculateWindowSize();
            void _SetDefaultSettings();
            void _DoFullscreen();
            void _UndoFullscreen();

            template <typename T>
            T* Helper(const string& aFilename)
            {
                string name(system::Files::CleanFilename(aFilename));

                Key key(T::SortOrder, name);

                Objects::iterator I = mObjects.find(key);

                T* p;

                if (I != mObjects.end())
                {
                    p = dynamic_cast<T*>(I->second.Get());
                }
                else
                {
                    p = (new T(aFilename));
                    mObjects.insert(make_pair(key, p));
                    
                    if (mbLoaded) { p->Load(); }
                    if (!mbLost) { p->Reset(GetViewportWidth(), GetViewportHeight()); }
                }

                return p;
            }
        };

    }
}

#endif
