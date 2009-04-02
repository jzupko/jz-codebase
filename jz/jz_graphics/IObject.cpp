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
#include <jz_core/StringUtility.h>
#include <jz_graphics/IObject.h>
#include <jz_graphics/Graphics.h>
#include <jz_system/Files.h>
#include <exception>

namespace jz
{
    namespace graphics
    {

        bool IObject::IsLoadable() const
        {
            if (IsError() || !system::Files::GetSingleton().GetExists(mFilename.c_str())) { return false; }
            else { return true; }
        }

#       define JZ_ERROR_HELPER() \
            if (mInternalState == kErrorFileNotFound) \
            { \
                LogMessage(__FUNCTION__ ": " + mFilename + " was not found or could not be opened."); \
                return; \
            } \
            else if (mInternalState == kErrorDataRead) \
            { \
                LogMessage(__FUNCTION__ ": " + mFilename + " data read exception."); \
            } \
            else if (mInternalState == kErrorGraphics) \
            { \
                LogMessage(__FUNCTION__ ": " + mFilename + " graphics error."); \
                return; \
            }

        void IObject::Load()
        {
            if (IsUnloaded())
            {
                if (IsLoadable())
                {
                    mInternalState = _Load();
                    JZ_ERROR_HELPER();
                }
                else
                {
                    LogMessage(__FUNCTION__ ": \"" + mFilename + "\" is not loadable.", Logger::kWarning);
                }
            }
        }

        void IObject::Unload()
        {
            if (IsLost())
            {
                mInternalState = _Unload();
                JZ_ERROR_HELPER();
            }
        }

        void IObject::Lost()
        {
            if (IsReset())
            {
                mInternalState = _Lost();
                JZ_ERROR_HELPER();
            }
        }
        
        void IObject::Reset(natural aWidth, natural aHeight)
        {
            if (IsLost())
            {
                mInternalState = _Reset(aWidth, aHeight);
                JZ_ERROR_HELPER();
            }
        }

#       undef JZ_ERROR_HELPER

        void IVolatileObject::Create()
        {
            if (IsDestroyed())
            {
                try
                {
                    _Create();
                    mInternalState = kLost;
                }
                catch (std::exception& e)
                {
                    LogMessage(__FUNCTION__ ": of " + StringUtility::ToString(this) + " failed, " + string(e.what()), Logger::kError);
                    mInternalState = kError;
                }
            }
        }

        void IVolatileObject::Destroy()
        {
            if (IsLost())
            {
                try
                {
                    _Destroy();
                    mInternalState = kDestroyed;
                }
                catch (std::exception& e)
                {
                    LogMessage(__FUNCTION__ ": of " + StringUtility::ToString(this) + " failed, " + string(e.what()), Logger::kError);
                    mInternalState = kError;
                }
            }
        }

        void IVolatileObject::Lost()
        {
            if (IsReset())
            {
                try
                {
                    _Lost();
                    mInternalState = kLost;
                }
                catch (std::exception& e)
                {
                    LogMessage(__FUNCTION__ ": of " + StringUtility::ToString(this) + " failed, " + string(e.what()), Logger::kError);
                    mInternalState = kError;
                }
            }
        }
        
        void IVolatileObject::Reset(natural aWidth, natural aHeight)
        {
            if (IsLost())
            {
                try
                {
                    _Reset(aWidth, aHeight);
                    mInternalState = kReset;
                }
                catch (std::exception& e)
                {
                    LogMessage(__FUNCTION__ ": of " + StringUtility::ToString(this) + " failed, " + string(e.what()), Logger::kError);
                    mInternalState = kError;
                }
            }
        }

    }
}
