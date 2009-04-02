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
#include <jz_core/StringUtility.h>
#include <jz_graphics/VertexDeclaration.h>
#include <jz_graphics_opengl/OpenGL.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>

namespace jz
{
    namespace graphics
    {

        struct Element
        {
            u16 Stream;
            u16 Offset;
            u8 Type;
            u8 Method;
            u8 Usage;
            u8 UsageIndex;
        };

        struct VertexDeclarationData
        {
            VertexDeclarationData()
                : VertexStrideInBytes(0u)
            {}

            MemoryBuffer<Element> Elements;
            size_t VertexStrideInBytes;
        };

        VertexDeclaration::VertexDeclaration(const string& aFilename)
            : IObject(aFilename)
        {}
        
        VertexDeclaration::~VertexDeclaration()
        {}

        static const size_t kTotalAttributes = 16;

        // POSITION = 0
        // BLENDWEIGHT = 1
        // BLENDINDICES = 2
        // NORMAL = 3
        // PSIZE = 4
        // TEXCOORD = 5
        // TANGENT = 6
        // BINORMAL = 7
        // TESSFACTOR = 8
        // POSITIONT = 9
        // COLOR = 10
        // FOG = 11
        // DEPTH = 12
        // SAMPLE = 13
        static GLuint GetAttribIndex(const Element& v)
        {
            switch (v.Usage)
            {
            case 0: return 0; break;
            case 1: return 1; break;
            case 2: return 7; break;
            case 3: return 2; break;
            case 4: return 6; break;
            case 5: return (8 + Clamp(v.UsageIndex, (u8)0, (u8)7)); break;
            case 6: return 14; break;
            case 7: return 15; break;
            case 8: return 5; break;
            // 9, POSITIONT, not supported.
            case 10: return (3 + Clamp(v.UsageIndex, (u8)0, (u8)1)); break;
            case 11: return 5; break;
            // 12, DEPTH, not supported.
            // 13, SAMPLE, not supported.
            default:
                LogMessage("Invalid vertex attrib index: " + StringUtility::ToString(v.Usage));
                return 0;
            }
        }

        static GLint GetAttribSize(const Element& v)
        {
            switch (v.Type)
            {
                case 0: return 1; break; // FLOAT1
                case 1: return 2; break; // FLOAT2
                case 2: return 3; break; // FLOAT3
                case 3: return 4; break; // FLOAT4
                case 4: return 4; break; // COLOR (4 bytes)
                case 5: return 4; break; // UBYTE4
                case 6: return 2; break; // SHORT2
                case 7: return 4; break; // SHORT4
                case 8: return 4; break; // UBYTE4N
                case 9: return 2; break; // SHORT2N
                case 10: return 4; break; // SHORT4N
                case 11: return 2; break; // USHORT2N
                case 12: return 4; break; // USHORT4N
                default:
                    LogMessage("Invalid vertex type: " + StringUtility::ToString(v.Type));
                    return 0;
            }
        }

        static unatural GetAttribSizeInBytes(const Element& v)
        {
            switch (v.Type)
            {
                case 0: return (1 * sizeof(f32)); break; // FLOAT1
                case 1: return (2 * sizeof(f32)); break; // FLOAT2
                case 2: return (3 * sizeof(f32)); break; // FLOAT3
                case 3: return (4 * sizeof(f32)); break; // FLOAT4
                case 4: return (4 * sizeof(u8)); break; // COLOR (4 bytes)
                case 5: return (4 * sizeof(u8)); break; // UBYTE4
                case 6: return (2 * sizeof(s16)); break; // SHORT2
                case 7: return (4 * sizeof(s16)); break; // SHORT4
                case 8: return (4 * sizeof(u8)); break; // UBYTE4N
                case 9: return (2 * sizeof(s16)); break; // SHORT2N
                case 10: return (4 * sizeof(s16)); break; // SHORT4N
                case 11: return (2 * sizeof(u16)); break; // USHORT2N
                case 12: return (4 * sizeof(u16)); break; // USHORT4N
                default:
                    LogMessage("Invalid vertex type: " + StringUtility::ToString(v.Type));
                    return 0;
            }
        }

        static GLenum GetAttribType(const Element& v)
        {
            switch (v.Type)
            {
                case 0: return GL_FLOAT; break; // FLOAT1
                case 1: return GL_FLOAT; break; // FLOAT2
                case 2: return GL_FLOAT; break; // FLOAT3
                case 3: return GL_FLOAT; break; // FLOAT4
                case 4: return GL_UNSIGNED_BYTE; break; // COLOR (4 bytes)
                case 5: return GL_UNSIGNED_BYTE; break; // UBYTE4
                case 6: return GL_SHORT; break; // SHORT2
                case 7: return GL_SHORT; break; // SHORT4
                case 8: return GL_UNSIGNED_BYTE; break; // UBYTE4N
                case 9: return GL_SHORT; break; // SHORT2N
                case 10: return GL_SHORT; break; // SHORT4N
                case 11: return GL_UNSIGNED_SHORT; break; // USHORT2N
                case 12: return GL_UNSIGNED_SHORT; break; // USHORT4N
                default:
                    LogMessage("Invalid vertex type: " + StringUtility::ToString(v.Type));
                    return 0;
            }
        }

        static bool GetAttribNormalize(const Element& v)
        {
            switch (v.Type)
            {
                case 0: return false; break; // FLOAT1
                case 1: return false; break; // FLOAT2
                case 2: return false; break; // FLOAT3
                case 3: return false; break; // FLOAT4
                case 4: return false; break; // COLOR (4 bytes)
                case 5: return false; break; // UBYTE4
                case 6: return false; break; // SHORT2
                case 7: return false; break; // SHORT4
                case 8: return true; break; // UBYTE4N
                case 9: return true; break; // SHORT2N
                case 10: return true; break; // SHORT4N
                case 11: return true; break; // USHORT2N
                case 12: return true; break; // USHORT4N
                default:
                    LogMessage("Invalid vertex type: " + StringUtility::ToString(v.Type));
                    return 0;
            }
        }

        void VertexDeclaration::SetToDevice() const
        {
            OPENGL_ASSERT();

            bool attributes[kTotalAttributes];
            size_t kCheck = sizeof(attributes);
            memset(attributes, 0, sizeof(attributes));

            VertexDeclarationData* pData = mHandle.Cast<VertexDeclarationData>();
            size_t size = pData->Elements.size();

            for (size_t i = 0u; i < size; i++)
            {
                GLuint index = GetAttribIndex(pData->Elements[i]);
                GLint size = GetAttribSize(pData->Elements[i]);
                GLenum type = GetAttribType(pData->Elements[i]);
                bool bNormalize = GetAttribNormalize(pData->Elements[i]);
                GLsizei stride = (pData->VertexStrideInBytes);// - GetAttribSizeInBytes(mElements[i]));
                u8_p offset = ((u8*)0 + pData->Elements[i].Offset);

                glVertexAttribPointer(index, size, type, bNormalize, stride, offset);
                glEnableVertexAttribArray(index);
                attributes[index] = true;
            }

            for (size_t i = 0u; i < kTotalAttributes; i++)
            {
                if (!attributes[i])
                {
                    glDisableVertexAttribArray(i);
                }
            }
        }

        IObject::State VertexDeclaration::_Load()
        {
            VertexDeclarationData* pData = null;
            try
            {
                pData = new VertexDeclarationData();
                system::IReadFilePtr pFile = system::Files::GetSingleton().Open(GetFilename().c_str());

                system::ReadBuffer(pFile, pData->Elements);
                const size_t kSize = pData->Elements.size();

                if (pData->Elements.size() > 0u)
                {
                    const Element& e = (pData->Elements[pData->Elements.size() - 1u]);
                    pData->VertexStrideInBytes = e.Offset + GetAttribSizeInBytes(e);
                }
                else
                {
                    pData->VertexStrideInBytes = 0u;
                }
            }
            catch (exception& e)
            {
                SafeDelete(pData);
                
                LogMessage(e.what(), Logger::kError);
                return (kErrorDataRead);
            }

            mHandle = pData;

            return (kLost);
        }

        IObject::State VertexDeclaration::_Unload()
        {
            VertexDeclarationData* pData = mHandle.Cast<VertexDeclarationData>();
            SafeDelete(pData);
            mHandle.Reset();

            return (kUnloaded);
        }

    }
}
