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
#ifndef JZ_FILESYSTEM_FILE_SYSTEM_H_
#define JZ_FILESYSTEM_FILE_SYSTEM_H_

#include <jz_filesystem/FileTag.h>
#include <ctime>
#include <queue>

namespace jz
{
    namespace filesystem
    {

        namespace Block
        {
            enum Descriptor
            {
                kDescriptorUncompressed = 0,
                kDescriptorDeflated = 1
            };
            
            enum Type
            {
                kTypeNone = 0,
                kTypeStart = 1,
                kTypeInternal = 2,
                kTypeEnd = 3
            };
        }
        
    #   if defined(_MSC_VER)
    #       pragma pack(push, packing)
    #       pragma pack(1)
    #       define PACK_STRUCT
    #   elif defined(__GNUC__)
    #       define PACK_STRUCT __attribute__((packed))
    #   else
    #       error "Platform not supported."
    #   endif

        struct FileSystemHeader
        {
            u32 Signature;
            u16 BlockSizePower;
            u16 FileTableSizePower;
        } PACK_STRUCT;

        struct BlockHeader
        {
            u64 Hash;
            u32 CRC;
            u16 Descriptor;
            u16 DataSize;
            u64 NextBlock;
        } PACK_STRUCT;

        struct FileTableEntry
        {
            u64 Hash;
            u64 FirstBlockPosition;
            u32 FileSize;
            u32 ModifiedTime;
        } PACK_STRUCT;
        
    #   ifdef _MSC_VER
    #       pragma pack(pop, packing)
    #   endif
    #   undef PACK_STRUCT

        static const size_t gskFileSystemOpsPerTick = 8;
        static const u32 gskSignature = 0xA18BC5B7;
        static const u16 gskMaximumBlockSizePower = 14;
        static const u32 gskMaximumFileTableSizePower = 20;
        static const u16 gskDefaultBlockSizePower = 12;
        static const u32 gskDefaultFileTableSizePower = 18;
        static const FileSystemHeader gskNewFileSystemHeader = 
            { gskSignature, 
              gskDefaultBlockSizePower, 
              gskDefaultFileTableSizePower, 
              };
              
        static const FileTableEntry gskEmptyFileEntry =
            { 0u,
              0u,
              0u, 
              0u 
              };

        // 00:00:00, 1st January 2008
        static struct tm gskBaseTimeT = { 0, 0, 0, 1, 0, 108 };
        static const time_t gskBaseTime = mktime(&gskBaseTimeT);
                
        class FileSystem
        {
            public:
                FileSystem(const char* apFileSystemFilename);
                ~FileSystem();

                void Commit();
                bool GetExists(const FileTag& aTag) const;
                u16 GetFileTableSizePower() const { return mFileSystemHeader.FileTableSizePower; }
                time_t GetModifiedTime(const FileTag& aTag) const;

                void Delete(const FileTag& aTag);
                void Read(const FileTag& aTag, ByteBuffer& arBuffer);
                void Write(const FileTag& aTag, const ByteBuffer& aBuffer);
                
            private:
                FileSystem(const FileSystem&);
                FileSystem& operator=(const FileSystem&);
                
                FILE* mpFileSystemHandle;
                u64 mFileSystemSize;
                u64 mCurrentPos;
                
                FileSystemHeader mFileSystemHeader;
                ByteBuffer mBlockBuffer;
                MemoryBuffer<FileTableEntry> mFileTable;
                
                const BlockHeader& _GetBlockHeader() const
                {
                    return *reinterpret_cast<const BlockHeader*>(mBlockBuffer.Get());
                }
                
                BlockHeader& _GetBlockHeader()
                {
                    return *reinterpret_cast<BlockHeader*>(mBlockBuffer.Get());
                }

                void _CalculateFileSystemSize();
                void _Destroy();
                void _Flush();
                void _PeekBlockHeader(u64 aPos);
                void _ReadAt(void_p ap, size_t aSize, u64 aFilePos);
                void _ReadBlockAt(u64 aPos);            
                void _ReadBlockHeaderAt(u64 aPos);
                void _SeekAbsolute(u64 aPos);
                u64 _Tell();
                void _WriteAt(const void_p ap, size_t aSize, u64 aFilePos);
                void _WriteBlockAt(u64 aPos);
                void _WriteBlockHeaderAt(u64 aPos);

                void _WriteFileSystemHeader();
                void _WriteFileTable();
                
                FileTableEntry& _AddFileEntry(const FileTag& aTag);
                bool _FindFileIndex(const FileTag& aTag, u32& arIndex) const;
                void _Open(const char* apFileSystemFilename);
                
                u32 _GetIndex(const FileTag& aTag) const;
        };

    }
}

#endif
