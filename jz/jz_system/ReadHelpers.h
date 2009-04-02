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
#ifndef _JZ_SYSTEM_READ_HELPERS_H_
#define _JZ_SYSTEM_READ_HELPERS_H_

#include <jz_core/Auto.h>
#include <jz_core/BoundingBox.h>
#include <jz_core/BoundingSphere.h>
#include <jz_core/Matrix4.h>
#include <jz_core/Vector2.h>
#include <jz_core/Vector3.h>
#include <jz_core/Vector4.h>
#include <string>

namespace jz
{
    namespace system
    {

        class IReadFile; typedef AutoPtr<IReadFile> IReadFilePtr;
     
        bool ReadBoolean(IReadFilePtr& p);
        BoundingBox ReadBoundingBox(IReadFilePtr& p);
        BoundingSphere ReadBoundingSphere(IReadFilePtr& p);
        s32 ReadInt32(IReadFilePtr& p);
        u32 ReadUInt32(IReadFilePtr& p);
        string ReadString(IReadFilePtr& p);
        Matrix3 ReadMatrix3(IReadFilePtr& p);
        Matrix4 ReadMatrix4(IReadFilePtr& p);
        float ReadSingle(IReadFilePtr& p);
        size_t ReadSizeT(IReadFilePtr& p);
        Vector2 ReadVector2(IReadFilePtr& p);
        Vector3 ReadVector3(IReadFilePtr& p);
        Vector4 ReadVector4(IReadFilePtr& p);

        template <typename T>
        void ReadBuffer(IReadFilePtr& p, MemoryBuffer<T>& arOut)
        {
            const size_t kSize = ReadSizeT(p);
            const size_t kSizeInBytes = (kSize * sizeof(T));

            arOut.resize(kSize);

            if (kSize > 0u)
            {
                JZ_E_ON_FAIL(p->Read(arOut.Get(), kSizeInBytes) == kSizeInBytes, "read failed.");
            }
        }

    }
}

#endif
