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

#include <jz_graphics/Graphics.h>
#include <jz_graphics/Parameter.h>
#include <jz_graphics/Target.h>
#include <jz_graphics/Texture.h>
#include <jz_graphics/VolatileTexture.h>
#include <jz_graphics_opengl/CG.h>

namespace jz
{
    namespace graphics
    {

        void __SetParameterValue(Handle e, Handle p, bool v)
        {
            CG_ASSERT();

            if (p)
            {
                CGparameter param = StaticCast<CGparameter>(p);
                cgSetParameter1i(param, v);
            }
        }

        void __SetParameterValue(Handle e, Handle p, float v)
        {
            CG_ASSERT();

            if (p)
            {
                CGparameter param = StaticCast<CGparameter>(p);
                cgSetParameter1f(param, v);
            }
        }

        void __SetParameterValue(Handle e, Handle p, voidc_p v, size_t aSizeInBytes)
        {
            CG_ASSERT();
            
            // Temp: can't set arbitrary arrays with cg.
            if (p && (aSizeInBytes % sizeof(float)) == 0)
            {
                CGparameter param = StaticCast<CGparameter>(p);
                cgSetParameterValuefc(param, (aSizeInBytes / sizeof(float)), (float const*)v);
            }
        }

        void __SetParameterValue(Handle e, Handle p, float const* v, size_t aNumberOfSingles)
        {
            CG_ASSERT();
            
            if (p)
            {
                CGparameter param = StaticCast<CGparameter>(p);
                cgSetParameterValuefc(param, aNumberOfSingles, v);
            }
        }

        void __SetParameterValue(Handle e, Handle p, const Matrix4& v)
        {
            CG_ASSERT();
            
            if (p)
            {
                CGparameter param = StaticCast<CGparameter>(p);
                cgSetParameterValuefc(param, (Matrix4::N), v.pData);
            }
        }

        void __SetParameterValue(Handle e, Handle p, Matrix4 const* v, size_t aNumberOfMatrix4)
        {
            CG_ASSERT();
            
            if (p)
            {
                CGparameter param = StaticCast<CGparameter>(p);
                cgSetParameterValuefc(param, (aNumberOfMatrix4 * Matrix4::N), (float const*)v);
            }
        }

        void __SetParameterValue(Handle e, Handle p, Vector4 const* v, size_t aNumberOfVector4)
        {
            CG_ASSERT();
            
            if (p)
            {
                CGparameter param = StaticCast<CGparameter>(p);
                cgSetParameterValuefc(param, (aNumberOfVector4 * Vector4::N), (float const*)v);
            }
        }

        void __SetParameterValue(Handle e, Handle p, Target* v)
        {
            CG_ASSERT();
            
            if (p)
            {
                uint texture = 0u;
                if (v)
                {
                    if (v->IsReset()) { texture = StaticCast<uint>(v->mTexture); }
                    else { texture = StaticCast<uint>(Graphics::GetSingleton().GetDefaultTexture()->mHandle); }
                }

                CGparameter param = StaticCast<CGparameter>(p);
                cgGLSetTextureParameter(param, texture);
            }
        }

        void __SetParameterValue(Handle e, Handle p, Texture* v)
        {
            CG_ASSERT();
            
            if (p)
            {
                uint texture = 0u;
                if (v)
                {
                    if (v->IsReset()) { texture = StaticCast<uint>(v->mHandle); }
                    else { texture = StaticCast<uint>(Graphics::GetSingleton().GetDefaultTexture()->mHandle); }
                }

                CGparameter param = StaticCast<CGparameter>(p);
                cgGLSetTextureParameter(param, texture);
            }
        }

        void __SetParameterValue(Handle e, Handle p, VolatileTexture* v)
        {
            CG_ASSERT();
            
            if (p)
            {
                uint texture = 0u;
                if (v)
                {
                    if (v->IsReset()) { texture = StaticCast<uint>(v->mHandle); }
                    else { texture = StaticCast<uint>(Graphics::GetSingleton().GetDefaultTexture()->mHandle); }
                }

                CGparameter param = StaticCast<CGparameter>(p);
                cgGLSetTextureParameter(param, texture);
            }
        }

    }
}
