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
#ifndef _JZ_GRAPHICS_EFFECT_H_
#define _JZ_GRAPHICS_EFFECT_H_

#include <jz_graphics/Annotation.h>
#include <jz_graphics/IObject.h>
#include <jz_graphics/Parameter.h>
#include <jz_graphics/Pass.h>
#include <jz_graphics/Technique.h>

#include <string>

namespace jz
{
    namespace graphics
    {

        class Effect : public IObject
        {
        public:
            static const int SortOrder = 0;

            template <typename T>
            void GetParameterBySemantic(const string& s, Parameter<T>& arParam) const
            {
                _GetParameterBySemantic(s, arParam.mEffect, arParam.mHandle);
            }

            template <typename T>
            Annotation GetAnnotationByName(const Parameter<T>& aParameter, const string& aName) const
            {
                Annotation ret;
                ret.mHandle = _GetParameterAnnotationByName(aParameter.mHandle, aName);

                return ret;
            }

            Annotation GetAnnotationByName(Technique aTechnique, const string& aName) const
            {
                Annotation ret;
                ret.mHandle = _GetTechniqueAnnotationByName(aTechnique.mHandle, aName);

                return ret;
            }
            
            Technique GetTechniqueByName(const string& aName) const
            {
                Technique ret;
                ret.mHandle = _GetTechniqueByName(aName);

                return ret;
            }

            Pass Begin() const;
            void End() const;

            void SetTechnique(Technique aTechnique);

        protected:
            Effect(const string& aFilename);

            JZ_EXPORT virtual ~Effect();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;
            JZ_EXPORT virtual State _Lost() override;
            JZ_EXPORT virtual State _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class Graphics;
            friend void jz::__IncrementRefCount<graphics::Effect>(graphics::Effect* p);
            friend void jz::__DecrementRefCount<graphics::Effect>(graphics::Effect* p);

            Effect(const Effect&);
            Effect& operator=(const Effect&);

            Handle _GetParameterAnnotationByName(voidc_p apObject, const string& aName) const;
            Handle _GetTechniqueAnnotationByName(voidc_p apObject, const string& aName) const;
            void _GetParameterBySemantic(const string& s, Handle& arEffect, Handle& arHandle) const;
            Handle _GetTechniqueByName(const string& aName) const;

            Technique mActiveTechnique;
            Handle mHandle;
        };

        typedef AutoPtr<Effect> EffectPtr;

    }
}

#endif
