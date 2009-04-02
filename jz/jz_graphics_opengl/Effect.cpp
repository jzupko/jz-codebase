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
#include <jz_graphics/Effect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Texture.h>
#include <jz_graphics_opengl/CG.h>
#include <jz_graphics_opengl/OpenGL.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>

namespace jz
{
    namespace graphics
    {

        Pass Effect::Begin() const
        {
            Pass ret;
            ret.mEffect = mHandle;
            ret.mHandle = cgGetFirstPass((CGtechnique)mActiveTechnique.mHandle);

            return ret;
        }

        void Effect::End() const
        {}

        void Effect::SetTechnique(Technique aTechnique)
        {
            mActiveTechnique = aTechnique;
        }

        Effect::Effect(const string& aFilename)
            : IObject(aFilename)
        {}
        
        Effect::~Effect()
        {}

        IObject::State Effect::_Lost()
        {
            return (kLost);
        }

        IObject::State Effect::_Reset(natural aWidth, natural aHeight)
        {
            return (kReset);
        }

        IObject::State Effect::_Load()
        {
            CG_ASSERT();

            try
            {
                system::IReadFilePtr pFile = system::Files::GetSingleton().Open(GetFilename().c_str());
                const size_t kSize = pFile->GetSize();

                MemoryBuffer<char> buf(kSize + 1);
                JZ_E_ON_FAIL(pFile->Read(buf.Get(), kSize) == kSize, "read size not expected.");
                buf[kSize] = 0; // add null terminator.

                mHandle = cgCreateEffect(gpCgContext, buf.Get(), null);

                if (!mHandle)
                {
                    const char* pListing = cgGetLastListing(gpCgContext);

                    throw exception(pListing);
                }
            }
            catch (exception& e)
            {
                LogMessage(__FUNCTION__ ": \"" + GetFilename() + "\": " + string(e.what()), Logger::kError);
                return (kErrorDataRead);
            }

            return (kLost);
        }

        IObject::State Effect::_Unload()
        {
            CG_ASSERT();

            CGeffect effect = mHandle.Cast<CGeffect>();
            cgDestroyEffect(effect);
            mHandle.Reset();

            return (kUnloaded);
        }

        Handle Effect::_GetParameterAnnotationByName(voidc_p apObject, const string& aName) const
        {
            return (cgGetNamedParameterAnnotation((CGparameter)apObject, aName.c_str()));
        }

        Handle Effect::_GetTechniqueAnnotationByName(voidc_p apObject, const string& aName) const
        {
            return (cgGetNamedTechniqueAnnotation((CGtechnique)apObject, aName.c_str()));
        }

        void Effect::_GetParameterBySemantic(const string& s, Handle& arEffect, Handle& arHandle) const
        {
            arEffect = mHandle;
            arHandle = (cgGetEffectParameterBySemantic(mHandle.Cast<CGeffect>(), s.c_str()));
        }

        Handle Effect::_GetTechniqueByName(const string& aName) const
        {
            return (cgGetNamedTechnique(mHandle.Cast<CGeffect>(), aName.c_str()));
        }

    }
}
