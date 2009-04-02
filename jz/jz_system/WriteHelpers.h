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
#ifndef _JZ_SYSTEM_WRITE_HELPERS_H_
#define _JZ_SYSTEM_WRITE_HELPERS_H_

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

        class IWriteFile; typedef AutoPtr<IWriteFile> IWriteFilePtr;
     
        void WriteBoolean(IWriteFilePtr& p, bool v);
        void WriteBoundingBox(IWriteFilePtr& p, const BoundingBox& v);
        void WriteBoundingSphere(IWriteFilePtr& p, const BoundingSphere& v);
        void WriteInt32(IWriteFilePtr& p, s32 v);
        void WriteUInt32(IWriteFilePtr& p, u32 v);
        void WriteString(IWriteFilePtr& p, const string& s);
        void WriteMatrix3(IWriteFilePtr& p, const Matrix3& v);
        void WriteMatrix4(IWriteFilePtr& p, const Matrix4& v);
        void WriteSingle(IWriteFilePtr& p, float v);
        void WriteSizeT(IWriteFilePtr& p, size_t v);
        void WriteVector2(IWriteFilePtr& p, const Vector2& v);
        void WriteVector3(IWriteFilePtr& p, const Vector3& v);
        void WriteVector4(IWriteFilePtr& p, const Vector4& v);

        template <typename T>
        void WriteBuffer(IWriteFilePtr& p, const MemoryBuffer<T>& aOut)
        {
            const size_t kSize = aOut.size();
            const size_t kSizeInBytes = aOut.GetSizeInBytes();

            WriteSizeT(p, kSize);
            if (kSize > 0u)
            {
                JZ_E_ON_FAIL(p->Write(aOut.Get(), kSizeInBytes) == kSizeInBytes, "write failed.");
            }
        }

    }
}

#endif
