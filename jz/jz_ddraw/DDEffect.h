// 
// Copyright (C) 2009 Joe Zupko. All Rights Reserved.
//   (jaz147@psu.edu, http://www.personal.psu.edu/~jaz147/)
//

#ifndef _JZ_DD_EFFECT_H_
#define _JZ_DD_EFFECT_H_

#include <jz_core/Rect.h>
#include <jz_graphics/Effect.h>
#include <string>

namespace jz
{
    struct Matrix4;
    struct Vector2;
    struct Vector3;
    namespace ddraw
    {

        class DirectDrawPalette;
        class DDEffect : public graphics::Effect
        {
        public:
            void Set(DirectDrawPalette* p);

            void DrawWithPalette(Handle t, const Rect& src, const Rect& dest);
            void DrawWithoutPalette(Handle t, const Rect& src, const Rect& dest);

        protected:
            virtual ~DDEffect();
            virtual State _Load() override;
            virtual State _Unload() override;

            graphics::Parameter<graphics::Texture> mPalette;
            graphics::Parameter<graphics::Texture> mFbTexture;

            Handle mpPalettedTechnique;
            Handle mpUnpalettedTechnique;

        private:
            friend class graphics::Graphics;
            friend void jz::__IncrementRefCount<ddraw::DDEffect>(ddraw::DDEffect* p);
            friend void jz::__DecrementRefCount<ddraw::DDEffect>(ddraw::DDEffect* p);

            void _Clear();

            DDEffect(const string& aFilename);

            DDEffect(const DDEffect&);
            DDEffect& operator=(const DDEffect&);
        };

        typedef AutoPtr<DDEffect> DDEffectPtr;

    }
}

#endif
