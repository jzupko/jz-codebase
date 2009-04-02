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
#include <jz_system/ReadHelpers.h>

namespace jz
{
    namespace system
    {

        bool ReadBoolean(IReadFilePtr& p)
        {
            const size_t kSize = (sizeof(u8));
            u8 val = 0u;

            JZ_E_ON_FAIL(p->Read(&val, kSize) == kSize, "failed reading boolean.");

            bool bReturn = (val == 0u) ? false : true;

            return bReturn;
        }

        BoundingBox ReadBoundingBox(IReadFilePtr& p)
        {
            BoundingBox ret;
            ret.Min = ReadVector3(p);
            ret.Max = ReadVector3(p);

            return ret;
        }

        BoundingSphere ReadBoundingSphere(IReadFilePtr& p)
        {
            BoundingSphere ret;
            ret.Center = ReadVector3(p);
            ret.Radius = ReadSingle(p);

            return ret;
        }

        string ReadString(IReadFilePtr& p)
        {
            u32 size;
            JZ_E_ON_FAIL(p->Read(&size, sizeof(u32)) == sizeof(u32), "failed reading string size.");
            MemoryBuffer<char> buf(size);
            // Includes null terminator.
            JZ_E_ON_FAIL(p->Read(buf.Get(), size) == size, "failed reading string contents.");
            
            return string(buf.Get());
        }

        Matrix3 ReadMatrix3(IReadFilePtr& p)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            size_t size = sizeof(f32) * 9;

            Matrix3 ret;
            JZ_E_ON_FAIL(p->Read(ret.pData, size) == size, "failed reading matrix 3.");

            return ret;
        }

        Matrix4 ReadMatrix4(IReadFilePtr& p)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            size_t size = sizeof(f32) * 16;

            Matrix4 ret;
            JZ_E_ON_FAIL(p->Read(ret.pData, size) == size, "failed reading matrix 4.");

            return ret;
        }

        float ReadSingle(IReadFilePtr& p)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            size_t size = sizeof(f32) * 1;

            float ret;
            JZ_E_ON_FAIL(p->Read(&ret, size) == size, "failed reading single.");

            return ret;
        }

        s32 ReadInt32(IReadFilePtr& p)
        {
            size_t size = sizeof(s32) * 1;

            s32 ret;
            JZ_E_ON_FAIL(p->Read(&ret, size) == size, "failed reading int32.");

            return ret;
        }

        u32 ReadUInt32(IReadFilePtr& p)
        {
            size_t size = (sizeof(u32) * 1);

            u32 ret;
            JZ_E_ON_FAIL(p->Read(&ret, size) == size, "failed reading u32.");

            return ret;
        }

        size_t ReadSizeT(IReadFilePtr& p)
        {
            JZ_STATIC_ASSERT(sizeof(size_t) == sizeof(u32));
            size_t size = sizeof(u32) * 1;

            size_t ret;
            JZ_E_ON_FAIL(p->Read(&ret, size) == size, "failed reading size_t.");

            return ret;
        }

        Vector2 ReadVector2(IReadFilePtr& p)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            size_t size = sizeof(f32) * 2;

            Vector2 ret;
            JZ_E_ON_FAIL(p->Read(ret.pData, size) == size, "failed reading vector 2.");

            return ret;
        }

        Vector3 ReadVector3(IReadFilePtr& p)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            size_t size = sizeof(f32) * 3;

            Vector3 ret;
            JZ_E_ON_FAIL(p->Read(ret.pData, size) == size, "failed reading vector 3.");

            return ret;
        }

        Vector4 ReadVector4(IReadFilePtr& p)
        {
            JZ_STATIC_ASSERT(sizeof(float) == sizeof(f32));
            size_t size = sizeof(f32) * 4;

            Vector4 ret;
            JZ_E_ON_FAIL(p->Read(ret.pData, size) == size, "failed reading vector 4.");

            return ret;
        }

    }
}
