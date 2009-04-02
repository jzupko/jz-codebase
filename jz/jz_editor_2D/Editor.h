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
#ifndef _JZ_EDITOR_2D_H_
#define _JZ_EDITOR_2D_H_

#include <jz_core/Auto.h>
#include <jz_core/Event.h>
#include <jz_core/Utility.h>
#include <jz_system/InputPrereqs.h>
#include <vector>

extern "C" struct lua_State;
namespace jz
{

    namespace graphics
    {
        class Material; typedef AutoPtr<Material> MaterialPtr;
    }

    namespace engine_2D
    {
        class Cell;
        class Sprite;
    }

    namespace editor_2D
    {

        class Editor sealed : public Singleton<Editor>
        {
        public:
            Editor();
            ~Editor();

            void Pose();
            void Update();

            void Call(const char* apFunction);
            void Call(const char* apFunction, float a0);

        private:
            Editor(const Editor&);
            Editor& operator=(const Editor&);

            lua_State* mpLua;
            
            typedef vector<natural> FreeList;
            typedef vector<engine_2D::Cell*> Cells;
            typedef vector<engine_2D::Sprite*> Sprites;

            graphics::MaterialPtr mpDefault;

            natural mActiveCell;
            Cells mCells;
            FreeList mFreeCells;

            natural mActiveSprite;
            Sprites mSprites;
            FreeList mFreeSprites;

            Event<void (natural, natural)>::Entry mMouseMove;
            Event<bool (system::Mouse::Button, system::Mouse::State)>::Entry mMouseButton;

            bool _ButtonHandler(system::Mouse::Button aButton, system::Mouse::State aState);
            void _MoveHandler(natural x, natural y);

            bool _PickActiveSprite(natural x, natural y);

            static int _CreateCell(lua_State* apState);
            static int _DeleteCell(lua_State* apState);
            static int _GetActiveCell(lua_State* apState);
            static int _SetActiveCell(lua_State* apState);

            static int _CreateSprite(lua_State* apState);
            static int _DeleteSprite(lua_State* apState);
            static int _GetActiveSprite(lua_State* apState);
            static int _SetActiveSprite(lua_State* apState);

            static int _CreateLight(lua_State* apState);

            static int _MoveSpriteScreenSpace(lua_State* apState);
        };

    }
}

#endif
