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
#include <jz_core/Memory.h>
#include <jz_system/Files.h>
#include <jz_system/Loader.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Texture.h>
#include <jz_graphics_directx9/DirectX9.h>

namespace jz
{
    namespace graphics
    {

#       if JZ_MULTITHREADED
            class TextureLoader : public system::ILoaderEntry
            {
            public:
                enum Stages
                {
                    kLoadData = 0,
                    kCreateTexture = 1,
                    kDone = 2
                };

                virtual NextAction GetNextAction() const override
                {
                    switch (mStage)
                    {
                    case kLoadData: return (kLoaderThread); break;
                    case kCreateTexture: return (kMainThread); break;
                    default:
                        return (kComplete);
                    break;
                    }
                }

                virtual NextAction MainThreadAction() override
                {
                    switch (mStage)
                    {
                    case kCreateTexture:
                        _CreateTexture();
                        break;
                    default:
                        _Error(IObject::kErrorDataRead);
                        break;
                    }

                    return (GetNextAction());
                }

                virtual NextAction LoaderThreadAction() override
                {
                    switch (mStage)
                    {
                    case kLoadData:
                        _LoadData();
                        break;
                    default:
                        _Error(IObject::kErrorDataRead);
                        break;
                    }

                    return (GetNextAction());
                }

            private:
                friend class Texture;

                friend void jz::__IncrementRefCount<graphics::TextureLoader>(graphics::TextureLoader* p);
                friend size_t jz::__GetRefCount<graphics::TextureLoader>(graphics::TextureLoader* p);
                friend void jz::__DecrementRefCount<graphics::TextureLoader>(graphics::TextureLoader* p);

                TextureLoader(Texture* apTexture)
                    : mStage(0u), mpTexture(apTexture)
                {}
                TextureLoader(const TextureLoader&);
                TextureLoader& operator=(const TextureLoader&);

                ByteBuffer mData;
                natural mStage;
                TexturePtr mpTexture;

                void _LoadData()
                {
                    system::IReadFilePtr pFile;
                    try
                    {
                        pFile = system::Files::GetSingleton().Open(mpTexture->GetFilename().c_str());
                    }
                    catch (std::exception&)
                    {
                        _Error(IObject::kErrorFileNotFound);
                        return;
                    }

                    mData.resize(pFile->GetSize());
                    if (pFile->Read(mData.Get(), mData.size()) != mData.size())
                    {
                        _Error(IObject::kErrorDataRead);
                        return;
                    }

                    mStage = (kCreateTexture);
                }

                void _CreateTexture()
                {
                    Graphics& graphics = Graphics::GetSingleton();

                    if (graphics.IsLoaded())
                    {
                        IDirect3DTexture9* p;
                        HRESULT hr = D3DXCreateTextureFromFileInMemory(gpD3dDevice9, mData.Get(), mData.GetSizeInBytes(), &p);
                        if (!SUCCEEDED(hr))
                        {
                            _Error(IObject::kErrorGraphics);
                            return;
                        }

                        mpTexture->mHandle = p;
                        mpTexture->mInternalState = (IObject::kLost);
                    }

                    if (!graphics.IsLost())
                    {
                        mpTexture->mInternalState = (mpTexture->_Reset(graphics.GetViewportWidth(), graphics.GetViewportHeight()));
                    }

                    mStage = (kDone);
                }

                void _Error(IObject::State err)
                {
                    mStage = kDone;
                    mpTexture->mInternalState = err;
                }
            };
#       endif

        Texture::Texture(const string& aFilename)
            : IObject(aFilename)
        {}
        
        Texture::~Texture()
        {}

        IObject::State Texture::_Load()
        {
#       if JZ_MULTITHREADED
            if (!system::Loader::GetSingletonExists())
            {
#       endif
                JZ_ASSERT(!mHandle);

                system::IReadFilePtr pFile;
                try
                {
                    pFile = (system::Files::GetSingleton().Open(GetFilename().c_str()));
                }
                catch (std::exception&)
                {
                    return (kErrorFileNotFound);
                }
                const size_t kSize = pFile->GetSize();

                MemoryBuffer<u8> buf(kSize);
                if (pFile->Read(buf.Get(), kSize) != kSize) { return (kErrorDataRead); }

                IDirect3DTexture9* p;
                HRESULT hr = D3DXCreateTextureFromFileInMemory(
                    gpD3dDevice9, buf.Get(), kSize, &p);
                if (!SUCCEEDED(hr)) { return (kErrorGraphics); }

                mHandle = p;

                return (kLost);
#       if JZ_MULTITHREADED
            }
            else
            {
                system::Loader::GetSingleton().Add(new TextureLoader(this));
                return (kLoading);
            }
#       endif
        }

        IObject::State Texture::_Unload()
        {
            SafeRelease<IDirect3DTexture9>(mHandle);
            return (kUnloaded);
        }

    }
}
