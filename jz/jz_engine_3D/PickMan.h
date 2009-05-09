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
#ifndef _JZ_ENGINE_3D_PICKMAN_H_
#define _JZ_ENGINE_3D_PICKMAN_H_

#include <jz_core/Color.h>
#include <jz_core/Ray3D.h>
#include <jz_core/Rectangle.h>
#include <jz_core/Utility.h>
#include <jz_core/Vector3.h>
#include <jz_graphics/RenderNode.h>
#include <jz_graphics/RenderPack.h>
#include <map>

namespace jz
{
    namespace graphics
    {
        class SystemMemorySurface; typedef AutoPtr<SystemMemorySurface> SystemMemorySurfacePtr;
    }

    namespace engine_3D
    {

        class IPickable;
        class PickMan sealed : public Singleton<PickMan>
        {
        public:
            typedef tr1::function<void(IPickable*, float aDepth)> PickCallback;

            PickMan();
            ~PickMan();

            JZ_EXPORT void Pose(const graphics::RenderPack& r, IPickable* apPickable);
            JZ_EXPORT bool Pick(const RectangleU& aRectangle, IPickable*& arpPickable, float& arDepth);

            JZ_EXPORT Ray3D GetPickingRay(natural x, natural y);
            
            JZ_EXPORT Vector3 Project(const Vector3& v);
            JZ_EXPORT Vector3 UnProject(const Vector3& v);

        private:
            PickMan(const PickMan&);
            PickMan& operator=(const PickMan&);

            graphics::SystemMemorySurfacePtr mpPickSurface;
            graphics::RenderNode mRenderPick;

            JZ_STATIC_ASSERT(sizeof(ColorRGBAu) == sizeof(voidc_p));
            union
            {
                ColorRGBAu mPickColor;
                voidc_p mPickColorObject;
            };

            typedef map<ColorRGBAu, IPickable*> PickTable;
            PickTable mPickTable;
        };

    }
}

#endif
