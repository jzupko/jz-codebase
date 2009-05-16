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
#ifndef _JZ_GRAPHICS_IOBJECT_H_
#define _JZ_GRAPHICS_IOBJECT_H_

#include <jz_core/Auto.h>
#include <string>

namespace jz
{
    namespace graphics
    {

        class Graphics;
        class IObject abstract
        {
        public:
            JZ_ALIGNED_NEW

            enum State
            {
                kUnloaded = 0,
                kLost = 1,
                kReset = 2,
                kLoading = 3,
                kErrorFileNotFound = 4,
                kErrorDataRead = 5,
                kErrorGraphics = 6
            };

            void ClearError() { (mInternalState = kUnloaded); }
            const string& GetFilename() const { return mFilename; }

            virtual bool IsLoadable() const;

            bool IsUnloaded() const { return (mInternalState == kUnloaded); }
            bool IsLoaded() const { return (mInternalState == kLost || mInternalState == kReset); }
            bool IsLost() const { return (mInternalState == kLost); }
            bool IsReset() const { return (mInternalState == kReset); }
            bool IsLoading() const { return (mInternalState == kLoading); }
            bool IsError() const { return (mInternalState >= kErrorFileNotFound); }

            void Load();
            void Unload();
            void Lost();        
            void Reset(natural aWidth, natural aHeight);

        protected:
            IObject(const string& aFilename)
                : mInternalState(kUnloaded), mReferenceCount(0u), mFilename(aFilename)
            {}

            State mInternalState;
            size_t mReferenceCount;

            virtual ~IObject()
            {
                JZ_ASSERT(IsLoading() || IsUnloaded() || IsError());
            }

            virtual State _Load() = 0;
            virtual State _Unload() = 0;
            virtual State _Lost() { return (kLost); }
            virtual State _Reset(natural aWidth, natural aHeight) { return (kReset); }

        private:
            friend class Graphics;

            friend void jz::__IncrementRefCount<graphics::IObject>(graphics::IObject* p);
            friend size_t jz::__GetRefCount<graphics::IObject>(graphics::IObject* p);
            friend void jz::__DecrementRefCount<graphics::IObject>(graphics::IObject* p);

            IObject(const IObject&);
            IObject& operator=(const IObject&);

            const string mFilename;
        };
        typedef AutoPtr<IObject> ObjectPtr;

        class IVolatileObject abstract
        {
        public:
            JZ_ALIGNED_NEW

            enum State
            {
                kDestroyed = 0,
                kLost = 1,
                kReset = 2,
                kError = 3
            };

            bool IsDestroyed() const { return (mInternalState == kDestroyed); }
            bool IsCreated() const { return (mInternalState == kLost || mInternalState == kReset); }
            bool IsLost() const { return (mInternalState == kLost); }
            bool IsReset() const { return (mInternalState == kReset); }
            bool IsError() const { return (mInternalState == kError); }
            
            void Create();
            void Destroy();
            void Lost();        
            void Reset(natural aWidth, natural aHeight);

        protected:
            IVolatileObject()
                : mInternalState(kDestroyed), mReferenceCount(0u)
            {}

            virtual ~IVolatileObject() { }

            size_t mReferenceCount;

            virtual void _Create() = 0;
            virtual void _Destroy() = 0;
            virtual void _Lost() {}
            virtual void _Reset(natural aWidth, natural aHeight) {}

        private:
            friend void jz::__IncrementRefCount<IVolatileObject>(IVolatileObject* p);
            friend size_t jz::__GetRefCount<IVolatileObject>(IVolatileObject* p);
            friend void jz::__DecrementRefCount<IVolatileObject>(IVolatileObject* p);

            State mInternalState;
            
            IVolatileObject(const IVolatileObject&);
            IVolatileObject& operator=(const IVolatileObject&);           
        };
        typedef AutoPtr<IVolatileObject> VolatileObjectPtr;

    }
}

#endif

