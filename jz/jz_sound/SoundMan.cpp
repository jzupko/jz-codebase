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

#include <jz_core/Logger.h>
#include <jz_sound/Sound.h>
#include <jz_sound/SoundMan.h>
#include <jz_system/Files.h>
#include <fmod/fmod.h>

#pragma comment(lib, "../fmod/fmodvc.lib")

namespace jz
{
    sound::SoundMan* sound::SoundMan::mspSingleton = null;

    namespace sound
    {

        static const float kMinimumFmodVersion = 3.75f;
        static const int kFmodFrequency = 44100;
        static const int kFmodChannels = 32;

        static void_p __stdcall OpenCallback(const char* apFilename)
        {
            system::IReadFile* p = null;
            try
            {
                p = system::Files::GetSingleton().Open(apFilename);
                __IncrementRefCount(p);
            }
            catch (std::exception& e)
            {
                LogMessage(e.what(), Logger::kError);
                return null;
            }

            return (p);
        }

        static void __stdcall CloseCallback(void_p apHandle)
        {
            system::IReadFile* p = static_cast<system::IReadFile*>(apHandle);
            __DecrementRefCount(p);
        }

        static int __stdcall ReadCallback(void_p buffer, int aSize, void_p apHandle)
        {
            system::IReadFile* p = static_cast<system::IReadFile*>(apHandle);
            return ((int)p->Read(buffer, (size_t)aSize));
        }

        static int __stdcall SeekCallback(void_p apHandle, int aPos, signed char aMode)
        {
            system::IReadFile* p = static_cast<system::IReadFile*>(apHandle);

            if (aMode == SEEK_SET)
            {
                p->Seek(aPos, false);
            }
            else if (aMode == SEEK_CUR)
            {
                p->Seek(aPos, true);
            }
            else if (aMode == SEEK_END)
            {
                p->Seek((p->GetSize() + aPos), false);
            }
            else
            {
                JZ_ASSERT(false);
            }

            return (p->GetPosition());
        }

        static int __stdcall TellCallback(void_p apHandle)
        {
            system::IReadFile* p = static_cast<system::IReadFile*>(apHandle);
            return (p->GetPosition());
        }

        SoundMan::SoundMan()
        {
            JZ_E_ON_FAIL(FSOUND_GetVersion() >= kMinimumFmodVersion, "FMOD less than minimum required version.");
            FSOUND_File_SetCallbacks(OpenCallback, CloseCallback, ReadCallback, SeekCallback, TellCallback);
            JZ_E_ON_FAIL(FSOUND_Init(kFmodFrequency, kFmodChannels, 0), "FMOD init failed.");
        }

        SoundMan::~SoundMan()
        {
            FSOUND_Close();
        }

        Sound* SoundMan::Create() const
        {
            return (new Sound());
        }

        Sound* SoundMan::Create(const char* apFilename, Sound::Mode aMode) const
        {
            return (new Sound(apFilename, aMode));
        }

    }
}
