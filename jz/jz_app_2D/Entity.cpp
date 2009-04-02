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

#include <jz_app_2D/Entity.h>
#include <jz_pathfinding/PathGrid.h>

namespace jz
{
    namespace app_2D
    {

        void Entity::SetBody(physics::Body2D* p)
        {
            if (mpPathGrid && mpSprite.IsValid() && mpBody.IsValid())
            {
                mpPathGrid->Remove(mPathId);
            }

            mpBody.Reset(p);

            if (mpPathGrid && mpSprite.IsValid() && mpBody.IsValid())
            {
                mPathId = mpPathGrid->Add(mpSprite->GetBounding());
            }
        }

        void Entity::SetPathGrid(pathfinding::PathGrid* p)
        {
            if (mpPathGrid && mpSprite.IsValid() && mpBody.IsValid())
            {
                mpPathGrid->Remove(mPathId);
            }

            mpPathGrid = p;

            if (mpPathGrid && mpSprite.IsValid() && mpBody.IsValid())
            {
                mPathId = mpPathGrid->Add(mpSprite->GetBounding());
            }
        }

        void Entity::SetSprite(engine_2D::Sprite* p)
        {
            if (mpPathGrid && mpSprite.IsValid() && mpBody.IsValid())
            {
                mpPathGrid->Remove(mPathId);
            }

            mpSprite.Reset(p);

            if (mpPathGrid && mpSprite.IsValid() && mpBody.IsValid())
            {
                mPathId = mpPathGrid->Add(mpSprite->GetBounding());
            }
        }

        void Entity::Update()
        {
            if (mpBody.IsValid() && mpSprite.IsValid())
            {
                mpSprite->SetTranslation(mpBody->GetTranslation());

                if (mpPathGrid)
                {
                    mpPathGrid->Update(mPathId, mpSprite->GetBounding());
                }
            }
        }

    }
}