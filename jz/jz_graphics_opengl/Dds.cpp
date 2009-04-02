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
#include <jz_graphics_opengl/Dds.h>
#include <jz_graphics_opengl/OpenGL.h>

namespace jz
{
    namespace graphics
    {

        namespace DdsConstants
        {
            static const u32 kMagic = 0x20534444;

            namespace Flags
            {
                static const u32 kCaps = 0x00000001;
                static const u32 kHeight = 0x00000002; 
                static const u32 kWidth = 0x00000004;
                static const u32 kPitch = 0x00000008;
                static const u32 kPixelFormat = 0x00001000;
                static const u32 kMipmapCount = 0x00020000; 
                static const u32 kLinearSize = 0x00080000;
                static const u32 kDepth = 0x00800000;
            }

            namespace PfFlags
            {
                static const u32 kAlphaPixels = 0x00000001;
                static const u32 kFourCC = 0x00000004;
                static const u32 kIndexed = 0x00000020;
                static const u32 kRgb = 0x00000040;
            }

            namespace Caps1
            {
                static const u32 kComplex = 0x00000008;
                static const u32 kTexture = 0x00001000;
                static const u32 kMipmap = 0x00400000;
            }

            namespace Caps2
            {
                static const u32 kCubemap = 0x00000200;
                static const u32 kCubemapPositiveX = 0x00000400; 
                static const u32 kCubemapNegativeX = 0x00000800; 
                static const u32 kCubemapPositiveY = 0x00001000;
                static const u32 kCubemapNegativeY = 0x00002000;
                static const u32 kCubemapPositiveZ = 0x00004000; 
                static const u32 kCubemapNegativeZ = 0x00008000;
                static const u32 kVolume = 0x00200000;
            }

            namespace Format
            {
                static const u32 kDxt1 = '1TXD';
                static const u32 kDxt2 = '2TXD';
                static const u32 kDxt3 = '3TXD';
                static const u32 kDxt4 = '4TXD';
                static const u32 kDxt5 = '5TXD';
            }
        }

    #   if JZ_PLATFORM_WINDOWS
    #       pragma pack(push, 1)
    #   else
    #       pragma pack(1)
    #   endif

        struct DdsPixelFormat
        {
            u32 Size;
            u32 Flags;
            u32 FourCC;
            u32 RGBBitCount;
            u32 RBitMask;
            u32 GBitMask;
            u32 BBitMask;
            u32 RGBAlphaBitMask;
        };

        struct DdsCaps2
        {
            u32 Caps1;
            u32 Caps2;
            u32 Reserved[2];
        };

        struct DdsSurface
        {
            u32 Size;
            u32 Flags;
            u32 Height;
            u32 Width;
            u32 PitchOrLinearSize;
            u32 Depth;
            u32 MipMapCount;
            u32 Reserved1[11];
            DdsPixelFormat PixelFormat;
            DdsCaps2 Caps;
            u32 Reserved2;
        };

        struct DdsHeader
        {
            u32 Magic;
            DdsSurface Surface;
        };

    #   if JZ_PLATFORM_WINDOWS
    #	    pragma pack(pop)
    #   else
    #	    pragma pack()
    #   endif

        struct Helper
        {
            bool bCompressed;
            uint BytesPerChunk;
            uint Denominator;
            GLenum Format;
        };

        static Helper Get(const DdsSurface& s)
        {
            static const Helper kDxt1 = { true, 8,  4, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT };
            static const Helper kDxt3 = { true, 16, 4, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT };
            static const Helper kDxt5 = { true, 16, 4, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT };

            if ((s.PixelFormat.Flags & DdsConstants::PfFlags::kFourCC) != 0)
            {
                if      (s.PixelFormat.FourCC == DdsConstants::Format::kDxt1) { return kDxt1; }
                else if (s.PixelFormat.FourCC == DdsConstants::Format::kDxt3) { return kDxt3; }
                else if (s.PixelFormat.FourCC == DdsConstants::Format::kDxt5) { return kDxt5; }
            }

            JZ_E_ON_FAIL(false, "unsupported DDS format.");
        }

        static size_t GetSize(size_t width, size_t height, const Helper& h)
        {
            size_t ret = 
                (Max(h.Denominator, width) / h.Denominator) *
                (Max(h.Denominator, height) / h.Denominator) * h.BytesPerChunk;

            return ret;
        }

        uint LoadDdsFile(const char* apFilename)
        {
            OPENGL_ASSERT();

            system::IReadFilePtr pFile = system::Files::GetSingleton().Open(apFilename);

            DdsHeader header;
            JZ_E_ON_FAIL(pFile->Read(&header, sizeof(DdsHeader)) == sizeof(DdsHeader), "failed reading header.");
            JZ_E_ON_FAIL(header.Magic == DdsConstants::kMagic && header.Surface.Size == sizeof(DdsSurface), "invalid header.");

            const DdsSurface& surface = header.Surface;
            JZ_E_ON_FAIL((surface.Flags & (DdsConstants::Flags::kPixelFormat | DdsConstants::Flags::kCaps)) != 0, "invalid DDS flags.");

            Helper helper = Get(surface);

            size_t levels = ((surface.Flags & DdsConstants::Flags::kMipmapCount) != 0) ? surface.MipMapCount : 1u;

            GLuint ret = 0u;
            glGenTextures(1, &ret);
            glBindTexture(GL_TEXTURE_2D, ret);
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, (levels <= 1) ? GL_TRUE : GL_FALSE);

            if (helper.bCompressed)
            {
                size_t width = surface.Width;
                size_t height = surface.Height;

                size_t size = GetSize(width, height, helper);

                ByteBuffer buf(size);

                for (size_t i = 0u; i < levels; i++)
                {
                    JZ_E_ON_FAIL(pFile->Read(buf, size) == size, "read failed.");

                    glCompressedTexImage2D(GL_TEXTURE_2D, i, helper.Format, width, height, 0, size, buf);

                    width  = ((width + 1) >> 1);
                    height = ((height + 1) >> 1);

                    size = GetSize(width, height, helper);
                }
                glBindTexture(GL_TEXTURE_2D, 0u);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, 0u);
                JZ_E_ON_FAIL(false, "unsupported DDS file format.");
            }

            return ret;
        }

    }
}
