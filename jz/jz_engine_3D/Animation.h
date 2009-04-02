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
#ifndef _JZ_ENGINE_3D_ANIMATION_H_
#define _JZ_ENGINE_3D_ANIMATION_H_

#include <jz_core/Auto.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Memory.h>

namespace jz
{
    namespace engine_3D
    {

        struct KeyFrame
        {
            Matrix4 Key;
            float Time;
        };

        struct Animation
        {
            MemoryBuffer<KeyFrame> KeyFrames;        
        };

        class AnimationControl sealed
        {
        public:
            AnimationControl();
            ~AnimationControl();

            int GetCurrentIndex() const { return mCurrentIndex; }
            void SetCurrentIndex(int v) { mCurrentIndex = v; mbDirty = true; }

            int GetStartIndex() const { return mStartIndex; }
            void SetStartIndex(int v) { mStartIndex = v; mbDirty = true; }

            int GetEndIndex() const { return mEndIndex; }
            void SetEndIndex(int v) { mEndIndex = v; mbDirty = true; }

            bool bPlay() const { return mbPlay; }
            void SetPlay(bool b) { mbPlay = b; mCurrentIndex = mStartIndex; mbDirty = true; }

            bool Tick(const Animation& aAnimation, Matrix4& m);

        protected:
            size_t mReferenceCount;

            int mCurrentIndex;
            int mStartIndex;
            int mEndIndex;
            bool mbPlay;
            
            float mStartTime;
            bool mbDirty;

        private:
            friend void ::jz::__IncrementRefCount<AnimationControl>(AnimationControl*);
            friend void ::jz::__DecrementRefCount<AnimationControl>(AnimationControl*);

            AnimationControl(const AnimationControl&);
            AnimationControl& operator=(const AnimationControl&);
        };

        typedef AutoPtr<AnimationControl> AnimationControlPtr;

    }
}

#endif
