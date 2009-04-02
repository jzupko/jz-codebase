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
#ifndef _JZ_GRAPHICS_OCCLUSION_QUERY_H_
#define _JZ_GRAPHICS_OCCLUSION_QUERY_H_

#include <jz_graphics/IObject.h>

namespace jz
{
    struct ColorRGBA;
    namespace graphics
    {

        class Graphics;
        class OcclusionQuery sealed : public IVolatileObject
        {
        public:
            static const int SortOrder = 0;

            bool Begin();
            void End();

            bool IsComplete() const;
            unatural GetPixelCount() const;

        protected:
            JZ_EXPORT virtual ~OcclusionQuery();
            JZ_EXPORT virtual void _Create() override;
            JZ_EXPORT virtual void _Destroy() override;
            JZ_EXPORT virtual void _Lost() override;
            JZ_EXPORT virtual void _Reset(natural aWidth, natural aHeight) override;

        private:
            friend class Graphics;
            friend void jz::__IncrementRefCount<graphics::OcclusionQuery>(graphics::OcclusionQuery* p);
            friend void jz::__DecrementRefCount<graphics::OcclusionQuery>(graphics::OcclusionQuery* p);

            OcclusionQuery();
            OcclusionQuery(const OcclusionQuery&);
            OcclusionQuery& operator=(const OcclusionQuery&);

            unatural mLastPixelCount;
            Handle mpQuery;
        };

        typedef AutoPtr<OcclusionQuery> OcclusionQueryPtr;

    }        
}

#endif
