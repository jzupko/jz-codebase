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
#ifndef _JZ_APP_2D_ENTITY_H_
#define _JZ_APP_2D_ENTITY_H_

#include <jz_core/Auto.h>
#include <jz_engine_2D/Sprite.h>
#include <jz_physics/narrowphase/Body.h>

namespace jz
{
    namespace pathfinding
    {
        class PathGrid;
    }

    namespace app_2D
    {

        class Entity
        {
        public:
            Entity()
                : mPathId(0u), mpPathGrid(null), mReferenceCount(0u)
            {}

            virtual ~Entity() {}

            physics::Body2D* GetBody() const { return (mpBody.Get()); }
            size_t GetPathId() const { return mPathId; }
            pathfinding::PathGrid* GetPathGrid() const { return mpPathGrid; }
            engine_2D::Sprite* GetSprite() const { return (mpSprite.Get()); }

            void SetBody(physics::Body2D* p);
            void SetPathGrid(pathfinding::PathGrid* p);
            void SetSprite(engine_2D::Sprite* p);

            void Update();

        protected:
            friend void jz::__IncrementRefCount<app_2D::Entity>(app_2D::Entity* p);
            friend size_t jz::__GetRefCount<app_2D::Entity>(app_2D::Entity* p);
            friend void jz::__DecrementRefCount<app_2D::Entity>(app_2D::Entity* p);

            size_t mPathId;
            physics::Body2DPtr mpBody;
            pathfinding::PathGrid* mpPathGrid;
            engine_2D::SpritePtr mpSprite;
            
            size_t mReferenceCount;

        private:
            Entity(const Entity&);
            Entity& operator=(const Entity&);
        };

        typedef AutoPtr<Entity> EntityPtr;

    }
}

#endif
