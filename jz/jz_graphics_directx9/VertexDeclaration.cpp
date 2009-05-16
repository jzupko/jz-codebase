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
#include <jz_graphics/Graphics.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_graphics_directx9/DirectX9.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>

namespace jz
{
    namespace graphics
    {

        VertexDeclaration::VertexDeclaration(const string& aFilename)
            : IObject(aFilename)
        {}
        
        VertexDeclaration::~VertexDeclaration()
        {}

        void VertexDeclaration::SetToDevice() const
        {
            JZ_DEBUG_DX_FAIL(gpD3dDevice9->SetVertexDeclaration(StaticCast<IDirect3DVertexDeclaration9*>(mHandle)));
        }

        IObject::State VertexDeclaration::_Load()
        {
            using namespace system;

            JZ_ASSERT(!mHandle);

            static const D3DVERTEXELEMENT9 kNullTerminator = D3DDECL_END();

            IReadFilePtr pFile;
            try
            {
                pFile = Files::GetSingleton().Open(GetFilename().c_str());
            }
            catch (std::exception&)
            {
                return (kErrorFileNotFound);
            }

            MemoryBuffer<D3DVERTEXELEMENT9> buf;
            try
            {
                ReadBuffer(pFile, buf);

                // add the D3DVERTEXELEMENT9 "null" terminator.
                buf.resize(buf.size() + 1u); 
                buf[buf.size() - 1] = kNullTerminator;
            }
            catch (std::exception&)
            {
                return (kErrorDataRead); 
            }

            IDirect3DVertexDeclaration9* p;
            HRESULT hr = gpD3dDevice9->CreateVertexDeclaration(buf.Get(), &p);
            if (!SUCCEEDED(hr)) { return (kErrorGraphics); }

            mHandle = p;

            return (kLost);
        }

        IObject::State VertexDeclaration::_Unload()
        {
            SafeRelease<IDirect3DVertexDeclaration9>(mHandle);

            return (kUnloaded);
        }

    }
}
