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

#include <jz_core/Memory.h>
#include <jz_system/Files.h>
#include <jz_system/WriteHelpers.h>

namespace jz
{
    namespace system
    {

        void WriteBoolean(IWriteFilePtr& p, bool v)
        {
            const size_t kSize = (sizeof(u8));
            u8 val = (v) ? (u8)1u : (u8)0u;

            JZ_E_ON_FAIL(p->Write(&val, kSize) == kSize, "failed writing boolean.");            
        }

        void WriteBoundingBox(IWriteFilePtr& p, const BoundingBox& v)
        {
            WriteVector3(p, v.Min);
            WriteVector3(p, v.Max);
        }

        void WriteBoundingSphere(IWriteFilePtr& p, const BoundingSphere& v)
        {
            WriteVector3(p, v.Center);
            WriteSingle(p, v.Radius);
        }

        void WriteString(IWriteFilePtr& p, const string& v)
        {
            const u32 kSize = (v.size() + 1u);
            JZ_E_ON_FAIL(p->Write(&kSize, sizeof(u32)) == sizeof(u32), "failed writing string size.");
            MemoryBuffer<char> buf(kSize);
            buf.CopyFrom(v.c_str(), v.size());
            buf[v.size()] = 0;

            JZ_E_ON_FAIL(p->Write(buf.Get(), kSize) == kSize, "failed writing string contents.");
        }

        void WriteMatrix3(IWriteFilePtr& p, const Matrix3& v)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            const size_t kSize = sizeof(f32) * 9;

            JZ_E_ON_FAIL(p->Write(v.pData, kSize) == kSize, "failed writing matrix 3.");
        }

        void WriteMatrix4(IWriteFilePtr& p, const Matrix4& v)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            const size_t kSize = sizeof(f32) * 16;

            JZ_E_ON_FAIL(p->Write(v.pData, kSize) == kSize, "failed writing matrix 4.");
        }

        void WriteSingle(IWriteFilePtr& p, float v)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            const size_t kSize = sizeof(f32) * 1;

            JZ_E_ON_FAIL(p->Write(&v, kSize) == kSize, "failed writing single.");
        }

        void WriteInt32(IWriteFilePtr& p, s32 v)
        {
            const size_t kSize = sizeof(s32) * 1;

            JZ_E_ON_FAIL(p->Write(&v, kSize) == kSize, "failed writing single.");
        }

        void WriteUInt32(IWriteFilePtr& p, u32 v)
        {
            const size_t kSize = (sizeof(u32) * 1);

            JZ_E_ON_FAIL(p->Write(&v, kSize) == kSize, "failed writing u32.");
        }

        void WriteSizeT(IWriteFilePtr& p, size_t v)
        {
            JZ_STATIC_ASSERT(sizeof(size_t) == sizeof(u32));
            const size_t kSize = sizeof(u32) * 1;

            JZ_E_ON_FAIL(p->Write(&v, kSize) == kSize, "failed writing single.");
        }

        void WriteVector2(IWriteFilePtr& p, const Vector2& v)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            const size_t kSize = sizeof(f32) * 2;

            JZ_E_ON_FAIL(p->Write(v.pData, kSize) == kSize, "failed writing vector 2.");
        }

        void WriteVector3(IWriteFilePtr& p, const Vector3& v)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            const size_t kSize = sizeof(f32) * 3;

            JZ_E_ON_FAIL(p->Write(v.pData, kSize) == kSize, "failed writing vector 3.");
        }

        void WriteVector4(IWriteFilePtr& p, const Vector4& v)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            const size_t kSize = sizeof(f32) * 4;

            JZ_E_ON_FAIL(p->Write(v.pData, kSize) == kSize, "failed writing vector 4.");
        }

        void WriteTextLine(IWriteFilePtr& p, const string& v)
        {
            const u32 kSize = (v.size() + 1u);
            MemoryBuffer<char> buf(kSize);
            buf.CopyFrom(v.c_str(), v.size());
            buf[v.size()] = '\n';

            JZ_E_ON_FAIL(p->Write(buf.Get(), kSize) == kSize, "failed writing string contents.");
        }

    }
}
