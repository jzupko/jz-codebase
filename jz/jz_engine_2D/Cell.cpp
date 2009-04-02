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

#include <jz_core/Auto.h>
#include <jz_graphics/Graphics.h>
#include <jz_engine_2D/Cell.h>
#include <jz_engine_2D/Light.h>
#include <jz_engine_2D/Sprite.h>
#include <jz_system/ReadHelpers.h>
#include <jz_system/WriteHelpers.h>
#include <algorithm>

namespace jz
{
    namespace engine_2D
    {

        Cell::Cell()
            : IDrawable(IDrawable::kCell),
              mBounding(BoundingRectangle(Vector2::kNegOne, Vector2::kOne))
        {}

        Cell::~Cell()
        {}

        void Cell::Pose(IDrawable* apDrawable)
        {
            for (vector<IDrawablePtr>::iterator I = mChildren.begin(); I != mChildren.end(); I++)
            {
                if (BoundingRectangle::kNegOneToOne.Intersects((*I)->GetBounding()))
                {
                    (*I)->Pose(this);
                }
            }
        }

        void Cell::Update()
        {
            mBounding = BoundingRectangle::kInvertedMax;

            for (vector<IDrawablePtr>::iterator I = mChildren.begin(); I != mChildren.end(); I++)
            {
                mBounding = BoundingRectangle::Merge(mBounding, (*I)->GetBounding());
            }
        }

        void Cell::AddChild(IDrawable* p)
        {
            mChildren.push_back(p);
        }

        void Cell::RemoveChild(IDrawable* p)
        {
            mChildren.erase(find(mChildren.begin(), mChildren.end(), p));
        }

        IDrawable* Cell::Pick(unatural aType, natural x, natural y)
        {
            graphics::Graphics& graphics = graphics::Graphics::GetSingleton();
            float width = (float)graphics.GetViewportWidth();
            float height = (float)graphics.GetViewportHeight();

            Vector2 u;
            u.X = (((float)x / width) * 2.0f) - 1.0f;
            u.Y = ((1.0f - ((float)y / height)) * 2.0f) - 1.0f;

            float bestZ = Constants<float>::kMax;
            IDrawable* pBest = null;

            for (vector<IDrawablePtr>::iterator I = mChildren.begin(); I != mChildren.end(); I++)
            {
                if (((*I)->GetDrawableType() & aType) != 0)
                {
                    if ((*I)->GetBounding().Intersects(u))
                    {
                        float z = (*I)->GetZdepth();
                        if (z < bestZ)
                        {
                            bestZ = z;
                            pBest = I->Get();
                        }
                    }
                }
            }

            return (pBest);
        }

        enum DrawableType
        {
            kLight = 0,
            kSprite = 1
        };

        void Cell::Serialize(system::IWriteFilePtr& pFile)
        {
            using namespace system;
            size_t sizeSprites = 0u;
            for (vector<IDrawablePtr>::iterator I = mChildren.begin(); I != mChildren.end(); I++)
            {
                if (typeid(*(*I)) == typeid(Light) || typeid(*(*I)) == typeid(Sprite))
                {
                    ISerializeable* p =(ISerializeable*)((*I).Get());

                    if (p->bSerializeable()) { sizeSprites++; }
                }
            }

            WriteSizeT(pFile, sizeSprites);
            for (vector<IDrawablePtr>::iterator I = mChildren.begin(); I != mChildren.end(); I++)
            {
                ISerializeable* p = dynamic_cast<ISerializeable*>((*I).Get());

                if (p && p->bSerializeable())
                {
                    if (typeid(*p) == typeid(Light))
                    {
                        WriteInt32(pFile, kLight);
                        p->Serialize(pFile);
                    }
                    else if (typeid(*p) == typeid(Sprite))
                    {
                        WriteInt32(pFile, kSprite);
                        p->Serialize(pFile);
                    }
                }
            }
        }

        void Cell::Deserialize(system::IReadFilePtr& pFile)
        {
            using namespace system;
            mChildren.clear();

            const size_t kSizeSprites = ReadSizeT(pFile);
            for (size_t i = 0u; i < kSizeSprites; i++)
            {
                DrawableType type = (DrawableType)ReadInt32(pFile);

                switch (type)
                {
                case kLight:
                    {
                        Light* p = new Light();
                        p->Deserialize(pFile);
                        mChildren.push_back(p);
                    }
                    break;
                case kSprite:
                    {
                        Sprite* p = new Sprite();
                        p->Deserialize(pFile);
                        mChildren.push_back(p);
                    }
                    break;
                default:
                    JZ_E_ON_FAIL(false, "unknown drawable type");
                    break;
                }
            }
        }

    }
}
