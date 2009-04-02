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
#ifndef _JZ_GRAPHICS_VERTEX_DECLARATION_H_
#define _JZ_GRAPHICS_VERTEX_DECLARATION_H_

#include <jz_graphics/IObject.h>
#include <string>

namespace jz
{
    namespace graphics
    {

        class Graphics;
        class Mesh;
        class VertexDeclaration : public IObject
        {
        public:
            static const int SortOrder = 0;

            void SetToDevice() const;
            
        protected:
            JZ_EXPORT virtual ~VertexDeclaration();
            JZ_EXPORT virtual State _Load() override;
            JZ_EXPORT virtual State _Unload() override;

        private:
            friend class Graphics;
            friend class Mesh;
            friend void jz::__IncrementRefCount<graphics::VertexDeclaration>(graphics::VertexDeclaration* p);
            friend void jz::__DecrementRefCount<graphics::VertexDeclaration>(graphics::VertexDeclaration* p);

            VertexDeclaration(const string& aFilename);

            VertexDeclaration(const VertexDeclaration&);
            VertexDeclaration& operator=(const VertexDeclaration&);

            Handle mHandle;
        };

        typedef AutoPtr<VertexDeclaration> VertexDeclarationPtr;

    }
}

#endif
