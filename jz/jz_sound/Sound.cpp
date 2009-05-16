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

#include <jz_core/Memory.h>
#include <jz_sound/Sound.h>
#include <jz_system/Files.h>
#include <fmod/fmod.h>

namespace jz
{
    namespace sound
    {

        static const int kInvalidChannel = -1;

        Sound::Sound()
            : mReferenceCount(0u), mMode(kNone), mChannel(kInvalidChannel)
        {}

        Sound::Sound(const char* apFilename, Mode aMode)
            : mReferenceCount(0u), mMode(kNone), mChannel(kInvalidChannel)
        {
            Load(apFilename, aMode);
        }

        Sound::~Sound()
        {
            _Destroy();
        }

        void Sound::Load(const char* apFilename, Mode aMode)
        {
            _Destroy();
            mMode = aMode;

            if (mMode == kLoadCompletely)
            {
                mpHandle = FSOUND_Sample_Load(FSOUND_FREE, apFilename, 0u, 0, 0);
            }
            else if (mMode == kStreaming)
            {
                mpHandle = FSOUND_Stream_Open(apFilename, 0u, 0, 0);
            }
        }

        void Sound::Play(bool abLoop)
        {
            if (mpHandle)
            {
                if (mMode == kLoadCompletely)
                {
                    FSOUND_Sample_SetMode(StaticCast<FSOUND_SAMPLE*>(mpHandle), (abLoop) ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
                    mChannel = FSOUND_PlaySound(FSOUND_FREE, StaticCast<FSOUND_SAMPLE*>(mpHandle));            
                }
                else if (mMode == kStreaming)
                {
                    FSOUND_Stream_SetMode(StaticCast<FSOUND_STREAM*>(mpHandle), (abLoop) ? FSOUND_LOOP_NORMAL : FSOUND_LOOP_OFF);
                    mChannel = FSOUND_Stream_Play(FSOUND_FREE, StaticCast<FSOUND_STREAM*>(mpHandle));
                }

                if (mChannel != kInvalidChannel)
                {
                    FSOUND_SetMute(mChannel, false);
                }
            }
        }

        void Sound::Stop()
        {
            if (mpHandle)
            {
                if (mChannel != kInvalidChannel)
                {
                    FSOUND_SetMute(mChannel, true);

                    if (mMode == kLoadCompletely)
                    {
                        FSOUND_StopSound(mChannel);
                    }
                    else if (mMode == kStreaming)
                    {
                        FSOUND_Stream_Stop(StaticCast<FSOUND_STREAM*>(mpHandle));
                    }
                    mChannel = kInvalidChannel;
                }
            }
        }

        void Sound::_Destroy()
        {
            if (mpHandle)
            {
                Stop();

                if (mMode == kLoadCompletely)
                {
                    FSOUND_Sample_Free(StaticCast<FSOUND_SAMPLE*>(mpHandle));
                }
                else if (mMode == kStreaming)
                {
                    FSOUND_Stream_Close(StaticCast<FSOUND_STREAM*>(mpHandle));
                }

                mMode = kNone;
                mpHandle = Handle();
            }
        }
    }
}
