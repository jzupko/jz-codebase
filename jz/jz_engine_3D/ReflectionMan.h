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
#ifndef _JZ_ENGINE_3D_REFLECTION_MAN_H_
#define _JZ_ENGINE_3D_REFLECTION_MAN_H_

#include <jz_core/Auto.h>
#include <jz_core/Utility.h>
#include <jz_core/Vector4.h>
#include <vector>

namespace jz
{
    namespace graphics
    {
        class Target; typedef AutoPtr<Target> TargetPtr;
    }

    namespace engine_3D
    {

        namespace ReflectionManConstants
        {
            static const int kPerDimension = 2;
        }

        class ReflectionMan sealed : public Singleton<ReflectionMan>
        {
        public:
            ReflectionMan();
            ~ReflectionMan();

            void Pre();
            void SetActive(int aHandle);
            void Post();

            graphics::Target* GetReflectionTarget() const;
            Vector4 GetScaleShift(int aHandle) const;

            int Grab();
            void Release(int& aHandle);

        private:
            bool mbAfterPre;

            ReflectionMan(const ReflectionMan&);
            ReflectionMan& operator=(const ReflectionMan&);

            graphics::TargetPtr mpReflectionTarget;
            vector<int> mFreeList;
        };

    }
}

#endif
