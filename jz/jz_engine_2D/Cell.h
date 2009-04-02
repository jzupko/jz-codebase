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
#ifndef _JZ_ENGINE_2D_DRAWABLE_LAYER_H_
#define _JZ_ENGINE_2D_DRAWABLE_LAYER_H_

#include <jz_engine_2D/IDrawable.h>
#include <jz_engine_2D/ISerializeable.h>
#include <vector>

namespace jz
{
    namespace engine_2D
    {

        class Sprite; typedef AutoPtr<Sprite> SpritePtr;
        class Cell : public IDrawable, public ISerializeable
        {
        public:
            Cell();
            virtual ~Cell();

            virtual BoundingRectangle GetBounding() const override { return mBounding; }
            virtual float GetZdepth() const override { return (0.0f); }

            virtual void Pose(IDrawable* apDrawable) override;
            virtual void Update();

            virtual void AddChild(IDrawable* p);
            virtual void RemoveChild(IDrawable* p);

            IDrawable* Pick(unatural aType, natural x, natural y);

            virtual bool bSerializeable() const override { return true; }
            virtual void Serialize(system::IWriteFilePtr& p) override;
            virtual void Deserialize(system::IReadFilePtr& p) override;

            vector<IDrawablePtr>::const_iterator begin() const { return mChildren.begin(); }
            vector<IDrawablePtr>::const_iterator end() const { return mChildren.end(); }

            vector<IDrawablePtr>::iterator begin() { return mChildren.begin(); }
            vector<IDrawablePtr>::iterator end() { return mChildren.end(); }

        protected:
            friend void jz::__IncrementRefCount<Cell>(Cell*);
            friend size_t jz::__GetRefCount<Cell>(Cell*);
            friend void jz::__DecrementRefCount<Cell>(Cell*);

            BoundingRectangle mBounding;
            vector<IDrawablePtr> mChildren;

        private:
            Cell(const Cell&);
            Cell& operator=(const Cell&);

        };

        typedef AutoPtr<Cell> CellPtr;

    }
}

#endif
