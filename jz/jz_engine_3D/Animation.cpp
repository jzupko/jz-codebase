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

#include <jz_engine_3D/Animation.h>
#include <jz_system/Time.h>

namespace jz
{
    namespace engine_3D
    {

        AnimationControl::AnimationControl()
            : mReferenceCount(0u),
            mStartTime(0.0f),
            mCurrentIndex(0),
            mStartIndex(0),
            mEndIndex(0),
            mbPlay(false),
            mbDirty(true)
        {}

        AnimationControl::~AnimationControl()
        {
        }

        bool AnimationControl::Tick(const Animation& aAnimation, Matrix4& m)
        {
            if (mbPlay || mbDirty)
            {
                bool bOk = (mStartIndex >= 0 &&
                            mStartIndex < mEndIndex &&
                            mEndIndex < (int)aAnimation.KeyFrames.size());
                
                mCurrentIndex = jz::Clamp(mCurrentIndex, mStartIndex, mEndIndex);

                float currentTime = system::Time::GetSingleton().GetSeconds();

                if (bOk && mbDirty)
                {
                    mStartTime = (currentTime - aAnimation.KeyFrames[mCurrentIndex].TimeAndPadding.X);
                    mbDirty = false;
                }

                float relTime = (currentTime - mStartTime);

                if (bOk)
                {
                    while (relTime > aAnimation.KeyFrames[mCurrentIndex + 1].TimeAndPadding.X)
                    {
                        mCurrentIndex++;

                        if (mCurrentIndex >= mEndIndex)
                        {
                            mCurrentIndex = mStartIndex;
                            mStartTime += (aAnimation.KeyFrames[mEndIndex].TimeAndPadding.X - aAnimation.KeyFrames[mStartIndex].TimeAndPadding.X);
                            relTime = (currentTime - mStartTime);
                        }
                    }

                    float lerp = jz::Clamp((relTime - aAnimation.KeyFrames[mCurrentIndex].TimeAndPadding.X) / (aAnimation.KeyFrames[mCurrentIndex + 1].TimeAndPadding.X - aAnimation.KeyFrames[mCurrentIndex].TimeAndPadding.X), 0.0f, 1.0f);
                    m = Matrix4::Lerp(aAnimation.KeyFrames[mCurrentIndex].Key, aAnimation.KeyFrames[mCurrentIndex + 1].Key, lerp);

                    return true;
                }
                else if (aAnimation.KeyFrames.size() > 0)
                {
                    m = aAnimation.KeyFrames[0].Key;
                    return true;
                }
            }

            return false;
        }

    }
}

