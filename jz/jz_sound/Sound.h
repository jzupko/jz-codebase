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
#ifndef _JZ_SOUND_SOUND_H_
#define _JZ_SOUND_SOUND_H_

#include <jz_core/Auto.h>

namespace jz
{
    namespace sound
    {

        class SoundMan;
        class Sound sealed
        {
        public:
            enum Mode
            {
                kNone,
                kLoadCompletely,
                kStreaming,
            };

            void Load(const char* apFilename, Mode aMode = kLoadCompletely);

            void Play(bool abLoop = false);
            void Stop();

        protected:
            ~Sound();

        private:
            friend class SoundMan;
            size_t mReferenceCount;

            int mChannel;
            Mode mMode;
            Handle mpHandle;

            friend void jz::__IncrementRefCount<sound::Sound>(sound::Sound* p);
            friend size_t jz::__GetRefCount<sound::Sound>(sound::Sound* p);
            friend void jz::__DecrementRefCount<sound::Sound>(sound::Sound* p);

            Sound();
            Sound(const char* apFilename, Mode aMode = kLoadCompletely);
            Sound(const Sound&);
            Sound& operator=(const Sound&);

            void _Destroy();
        };
        typedef AutoPtr<Sound> SoundPtr;

    }
}

#endif

