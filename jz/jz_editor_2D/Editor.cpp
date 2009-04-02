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
#include <jz_core/StringUtility.h>
#include <jz_editor_2D/Editor.h>
#include <jz_engine_2D/Cell.h>
#include <jz_engine_2D/DrawMan.h>
#include <jz_engine_2D/Light.h>
#include <jz_engine_2D/Sprite.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Texture.h>
#include <jz_system/Files.h>
#include <jz_system/Input.h>

extern "C"
{
#   include <lua/lua.h>
#   include <lua/lualib.h>
#   include <lua/lauxlib.h>
}

namespace jz
{
    editor_2D::Editor* editor_2D::Editor::mspSingleton = null;

    namespace editor_2D
    {

        static const int kLuaTop = (-1);

        static void CloseLua(lua_State*& arpState)
        {
            if (arpState)
            {
                lua_close(arpState);
                arpState = null;
            }
        }

        static const char* kLuaEntry = "../media/editor_2D.lua";

        static const char* kDefaultDiffuse = "../media/compiled/default.dds";
        static const char* kDefaultNormal = "../media/compiled/default_NRM.dds";

        static const char* kDiffuseParam = "jz_Diffuse";
        static const char* kNormalDepthParam = "jz_NormalDepth";

        Editor::Editor()
            : mActiveCell(-1), mActiveSprite(-1), mpLua(lua_open())
        {
            if (!engine_2D::DrawMan::GetSingletonExists())
            {
                CloseLua(mpLua);
                throw std::exception(__FUNCTION__ ": DrawMan must be instantiated before Editor.");
            }

            #pragma region Register lua functions
            static const luaL_reg kLuaFunctions[] =
            {
                { "jzCreateCell", Editor::_CreateCell },
                { "jzDeleteCell", Editor::_DeleteCell },
                { "jzGetActiveCell", Editor::_GetActiveCell },
                { "jzSetActiveCell", Editor::_SetActiveCell },

                { "jzCreateSprite", Editor::_CreateSprite },
                { "jzDeleteSprite", Editor::_DeleteSprite },
                { "jzGetActiveSprite", Editor::_GetActiveSprite },
                { "jzSetActiveSprite", Editor::_SetActiveSprite },

                { "jzMoveSpriteScreenSpace", Editor::_MoveSpriteScreenSpace },

                { "jzCreateLight", Editor::_CreateLight },

                { null, null }
            };

            {
                int i = 0;
                while (kLuaFunctions[i].name)
                {
                    lua_register(mpLua, kLuaFunctions[i].name, kLuaFunctions[i].func);
                    i++;
                }
            }
            #pragma endregion

            try
            {
                graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
                mpDefault = graphics.Create<graphics::Material>(string());
                mpDefault->AddParameter(kDiffuseParam, graphics.Create<graphics::Texture>(kDefaultDiffuse));
                mpDefault->AddParameter(kNormalDepthParam, graphics.Create<graphics::Texture>(kDefaultNormal));
            }
            catch (std::exception&)
            {
                CloseLua(mpLua);
                throw;
            }

            try
            {
                system::IReadFilePtr pFile = system::Files::GetSingleton().Open(kLuaEntry);
                MemoryBuffer<char> buf(pFile->GetSize());
                JZ_E_ON_FAIL(pFile->Read(buf, buf.size()) == buf.size(), "failed reading lua script");

                int err = luaL_loadbuffer(mpLua, buf.Get(), buf.size(), kLuaEntry)
                       || lua_pcall(mpLua, 0, 0, 0);
                if (err != 0)
                {
                    throw std::exception(lua_tostring(mpLua, -1));
                }
            }
            catch (std::exception&)
            {
                CloseLua(mpLua);
                throw;
            }

            _CreateCell(mpLua);

            system::Input& input = system::Input::GetSingleton();
            input.OnButton.Add<Editor, &Editor::_ButtonHandler>(this, mMouseButton);
            input.OnMouseMoveAbsolute. Add<Editor, &Editor::_MoveHandler>(this, mMouseMove);
        }

        Editor::~Editor()
        {
            for (Sprites::reverse_iterator I = mSprites.rbegin(); I != mSprites.rend(); I++)
            {
                if ((*I))
                {
                    __DecrementRefCount((*I));
                }
            }

            for (Cells::reverse_iterator I = mCells.rbegin(); I != mCells.rend(); I++)
            {
                if ((*I))
                {
                    __DecrementRefCount((*I));
                }
            }

            CloseLua(mpLua);
        }

        void Editor::Pose()
        {
            #pragma region Pose active cell
            if (mActiveCell >= 0)
            {
                mCells[mActiveCell]->Pose(null);
            }
            #pragma endregion
        }

        void Editor::Update()
        {
            if (mActiveCell >= 0)
            {
                mCells[mActiveCell]->Update();
            }
        }

        void Editor::Call(const char* apFunction)
        {
            lua_getglobal(mpLua, apFunction);
            if (!lua_isfunction(mpLua, kLuaTop))
            {
                LogMessage("Editor: could not execute " + string(apFunction), Logger::kError);
                return;
            }

            int err = lua_pcall(mpLua, 0, 0, 0);
            if (err != 0)
            {
                LogMessage(lua_tostring(mpLua, kLuaTop), Logger::kError);
                return;
            }
        }

        void Editor::Call(const char* apFunction, float a0)
        {
            lua_getglobal(mpLua, apFunction);
            if (!lua_isfunction(mpLua, kLuaTop))
            {
                LogMessage("Editor: could not execute " + string(apFunction), Logger::kError);
                return;
            }

            lua_pushnumber(mpLua, a0);
            int err = lua_pcall(mpLua, 1, 0, 0);
            if (err != 0)
            {
                LogMessage(lua_tostring(mpLua, kLuaTop), Logger::kError);
                return;
            }
        }

        bool Editor::_PickActiveSprite(natural x, natural y)
        {
            JZ_ASSERT(mActiveCell >= 0);

            engine_2D::IDrawable* p = mCells[mActiveCell]->Pick(engine_2D::IDrawable::kSprite, x, y);

            if (!p) { mActiveSprite = -1; }
            else
            {
                engine_2D::Sprite* pSprite = dynamic_cast<engine_2D::Sprite*>(p);
                if (!pSprite) { mActiveSprite = -1; }
                else
                {
                    // Temp: linear search is going to get very slow.
                    const size_t kSize = mSprites.size();
                    for (size_t i = 0u; i < kSize; i++)
                    {
                        if (mSprites[i] == pSprite)
                        {
                            mActiveSprite = (natural)i;

                            LogMessage("Editor: sprite " + StringUtility::ToString(mActiveSprite) + " is now active.");
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        static const char* kLuaOnMousePressed = "OnMousePressed";
        static const char* kLuaOnMouseReleased = "OnMouseReleased";
        static const char* kLuaOnMouseMove = "OnMouseMove";

        bool Editor::_ButtonHandler(system::Mouse::Button aButton, system::Mouse::State aState)
        {
            if (aButton == system::Mouse::kLeft)
            {
                const char* kFunction = (aState == system::Mouse::kPressed) ? kLuaOnMousePressed : kLuaOnMouseReleased;
            
                lua_getglobal(mpLua, kFunction);
                if (!lua_isfunction(mpLua, kLuaTop))
                {
                    return false;
                }

                natural x = system::Input::GetSingleton().GetMouseX();
                natural y = system::Input::GetSingleton().GetMouseY();

                lua_pushinteger(mpLua, x);
                lua_pushinteger(mpLua, y);
                int err = lua_pcall(mpLua, 2, 1, 0);
                if (err != 0)
                {
                    LogMessage(lua_tostring(mpLua, kLuaTop), Logger::kError);
                    return false;
                }

                if (lua_gettop(mpLua) > 0 && lua_isboolean(mpLua, kLuaTop))
                {
                    bool ret = (lua_toboolean(mpLua, kLuaTop) != 0);
                    return ret;
                }
            }

	        return false;            
        }

        void Editor::_MoveHandler(natural x, natural y)
        {
            lua_getglobal(mpLua, kLuaOnMouseMove);
            if (!lua_isfunction(mpLua, kLuaTop)) { return; }

            lua_pushinteger(mpLua, x);
            lua_pushinteger(mpLua, y);
            int err = lua_pcall(mpLua, 2, 0, 0);
            if (err != 0)
            {
                LogMessage(lua_tostring(mpLua, kLuaTop), Logger::kError);
                return;
            }
        }

        int Editor::_CreateCell(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            natural v = -1;
            if (e.mFreeCells.size() > 0u)
            {
                v = e.mFreeCells.back();
                e.mFreeCells.pop_back();
            }
            else
            {
                v = (natural)(e.mCells.size());
                e.mCells.push_back(null);
            }

            e.mCells[v] = new engine_2D::Cell();
            __IncrementRefCount(e.mCells[v]);
            e.mActiveCell = v;

            LogMessage("Editor: created cell " + StringUtility::ToString(v) + ", this cell is now active.");

            lua_pushboolean(apState, true);
            return 1;
        }

        int Editor::_DeleteCell(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            natural v = (e.mActiveCell);

            if (v >= 0 && v < (natural)(e.mCells.size()))
            {
                if (e.mCells[v])
                {
                    e.mActiveCell = -1;
                    // Temp: destroy sprites?
                    
                    __DecrementRefCount(e.mCells[v]);
                    e.mCells[v] = null;
                    e.mFreeCells.push_back(v);

                    LogMessage("Editor: destroyed cell " + StringUtility::ToString(v) + ".");
                    lua_pushboolean(apState, true);
                    return 1;
                }
            }

            LogMessage("Editor: could not destroy cell " + StringUtility::ToString(v) + ".");
            lua_pushboolean(apState, false);
            return 1;
        }

        int Editor::_GetActiveCell(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            lua_pushinteger(apState, (e.mActiveCell));
            return 1;
        }

        int Editor::_SetActiveCell(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            bool bReturn = false;
            if (lua_gettop(apState) > 0 && lua_isnumber(apState, 1))
            {
                natural v = (natural)lua_tointeger(apState, 1);

                e.mActiveCell = v;
                bReturn = true;
                
                LogMessage("Editor: cell " + StringUtility::ToString(v) + " is now active.");
            }

            lua_pushboolean(apState, bReturn);
            return 1;
        }

        int Editor::_CreateSprite(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            natural v = -1;
            if (e.mFreeSprites.size() > 0u)
            {
                v = e.mFreeSprites.back();
                e.mFreeSprites.pop_back();
            }
            else
            {
                v = (natural)(e.mSprites.size());
                e.mSprites.push_back(null);
            }

            e.mSprites[v] = new engine_2D::Sprite();
            __IncrementRefCount(e.mSprites[v]);
            e.mActiveSprite = v;

            e.mSprites[v]->SetMaterial(e.mpDefault.Get());
            e.mSprites[v]->SetScale(0.1f, 0.1f);
            e.mSprites[v]->SetTranslation(0.0f, 0.0f);
            e.mSprites[v]->SetZdepth(0.5f);

            if (e.mActiveCell >= 0)
            {
                e.mCells[e.mActiveCell]->AddChild(e.mSprites[v]);
            }

            LogMessage("Editor: created sprite " + StringUtility::ToString(v) + ", this sprite is now active.");

            lua_pushboolean(apState, true);
            return 1;
        }

        int Editor::_DeleteSprite(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            natural v = (e.mActiveSprite);

            if (v >= 0 && v < (natural)(e.mSprites.size()))
            {
                if (e.mSprites[v])
                {
                    e.mActiveSprite = -1;
                    if (e.mActiveCell >= 0)
                    {
                        e.mCells[e.mActiveCell]->RemoveChild(e.mSprites[v]);
                    }
                    
                    __DecrementRefCount(e.mSprites[v]);
                    e.mSprites[v] = null;
                    e.mFreeSprites.push_back(v);

                    LogMessage("Editor: destroyed sprite " + StringUtility::ToString(v) + ".");
                    lua_pushboolean(apState, true);
                    return 1;
                }
            }

            LogMessage("Editor: could not destroy sprite " + StringUtility::ToString(v) + ".");
            lua_pushboolean(apState, false);
            return 1;
        }

        int Editor::_GetActiveSprite(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            lua_pushinteger(apState, (e.mActiveSprite));
            return 1;
        }

        int Editor::_SetActiveSprite(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            lua_Number narg = lua_gettop(apState);

            bool bReturn = false;
            if (narg == 1)
            {
                if (lua_isnumber(apState, 1))
                {
                    natural v = (natural)lua_tointeger(apState, 1);

                    LogMessage("Editor: sprite " + StringUtility::ToString(v) + " is now active.");
                    e.mActiveSprite = v;
                    bReturn = true;
                }
            }
            else if (narg > 1)
            {
                if (e.mActiveCell >= 0)
                {
                    if (lua_isnumber(apState, 1))
                    {
                        natural x = (natural)lua_tointeger(apState, 1);
                        if (lua_isnumber(apState, 2))
                        {
                            natural y = (natural)lua_tointeger(apState, 2);

                            bReturn = e._PickActiveSprite(x, y);
                        }
                    }
                }
            }

            lua_pushboolean(apState, bReturn);
            return 1;
        }

        int Editor::_CreateLight(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            if (e.mActiveCell >= 0)
            {
                engine_2D::Light* pLight = new engine_2D::Light();
                pLight->SetShadow(true);
                pLight->SetColor(ColorRGB::kWhite);
                pLight->SetPosition(0, 0, -1);
                e.mCells[e.mActiveCell]->AddChild(pLight);

                LogMessage("Editor: created light.");
                lua_pushboolean(apState, true);
                return 1;
            }

            lua_pushboolean(apState, false);
            return 1;
        }

        int Editor::_MoveSpriteScreenSpace(lua_State* apState)
        {
            Editor& e = Editor::GetSingleton();

            if (e.mActiveSprite >= 0 && lua_gettop(apState) > 1)
            {
                if (lua_isnumber(apState, 1))
                {
                    natural x = (natural)lua_tointeger(apState, 1);
                    if (lua_isnumber(apState, 2))
                    {
                        natural y = (natural)lua_tointeger(apState, 2);
                    
                        graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
                        float width = (float)graphics.GetViewportWidth();
                        float height = (float)graphics.GetViewportHeight();

                        float dx = ((float)x / width) * 2.0f;
                        float dy = -((float)y / height) * 2.0f;

                        if (width < height) { dy *= (height / width); }
                        else if (height < width) { dx *= (width / height); }

                        engine_2D::Sprite* p = e.mSprites[e.mActiveSprite];
                        p->SetTranslation(p->GetTranslation() + Vector2(dx, dy));
                    }
                }
            }

            return 0;
        }

    }
}
