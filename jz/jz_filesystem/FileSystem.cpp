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

#include <jz_core/CRC32.h>
#include <jz_core/StringUtility.h>
#include <jz_filesystem/FileSystem.h>
#include <sys/stat.h>
#include <zlib/zlib.h>
#include <zlib/zutil.h>
#include <ctime>

namespace jz
{
    namespace filesystem
    {

        static unatural _Deflate(const u8* pIn, size_t aInSize, u8* pOut, size_t aAvailableOut)
        {
            z_stream stream;
            memset(&stream, 0, sizeof(z_stream));
            stream.next_in = (Bytef*)pIn;
            stream.avail_in = aInSize;
            stream.next_out = pOut;
            stream.avail_out = aAvailableOut;
            stream.zalloc = (alloc_func)null;
            stream.zfree = (alloc_func)null;
            
            int ret = deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY);
            
            if (ret == Z_OK)
            {
                ret = deflate(&stream, Z_FINISH);
                deflateEnd(&stream);
                
                if (ret == Z_STREAM_END)
                {
                    ret = Z_OK;
                }
                
                deflateEnd(&stream);
            }
            
            JZ_E_ON_FAIL(ret == Z_OK, "deflation failed.");
            
            return stream.total_in;
        }

        static unatural _Inflate(const u8* pIn, size_t aInSize, u8* pOut, size_t aAvailableOut)
        {
            z_stream stream;
            memset(&stream, 0, sizeof(z_stream));
            stream.next_in = (Bytef*)pIn;
            stream.avail_in = aInSize;
            stream.next_out = pOut;
            stream.avail_out = aAvailableOut;
            stream.zalloc = (alloc_func)null;
            stream.zfree = (alloc_func)null;

            int ret = inflateInit2(&stream, -MAX_WBITS);
            
            if (ret == Z_OK)
            {
                ret = inflate(&stream, Z_FINISH);
                inflateEnd(&stream);
                
                if (ret == Z_STREAM_END)
                {
                    ret = Z_OK;
                }
                
                inflateEnd(&stream);
            }
            
            JZ_E_ON_FAIL(ret == Z_OK, "inflation failed.");
            
            return stream.total_out;
        }
        
        FileSystem::FileSystem(const char* apFileSystemFilename)
            : mpFileSystemHandle(null), mFileSystemSize(0u), mCurrentPos(0u)
        {
            _Open(apFileSystemFilename);
        }
        
        FileSystem::~FileSystem()
        {
            _Destroy();
        }
        
        void FileSystem::_Destroy()
        {
            try
            {
                Commit();
            }
            catch (...)
            {}

            mFileSystemSize = 0u;     
            mCurrentPos = 0u;

            memset((void_p)&mFileSystemHeader, 0, sizeof(FileSystemHeader));

            if (mpFileSystemHandle)
            {
                fclose(mpFileSystemHandle);
                mpFileSystemHandle = null;
            }
        }
        
        time_t FileSystem::GetModifiedTime(const FileTag& aTag) const
        {
            u32 index;
            if (!_FindFileIndex(aTag, index))
            {
                return -1;
            }
            else
            {
                const FileTableEntry& entry = mFileTable[index];
                const time_t kRet = gskBaseTime + ((time_t)entry.ModifiedTime);
                
                return kRet;
            }
        }

    #   if JZ_LITTLE_ENDIAN
            void FileSystem::Commit()
            {
                _WriteFileSystemHeader();
                _WriteFileTable();
                _Flush();
            }
            
            bool FileSystem::GetExists(const FileTag& aTag) const
            {
                const size_t kFileTableSize = mFileTable.size();
                size_t index = _GetIndex(aTag);
                
                if (mFileTable[index].Hash == 0u && mFileTable[index].ModifiedTime == 0u)
                {
                    return false;
                }
                
                for (; index < kFileTableSize; index++)
                {
                    const FileTableEntry& entry = mFileTable[index];
                    if (entry.Hash == aTag.GetHash())
                    {
                        return true;
                    }
                }
                
                return false;
            }

            void FileSystem::Delete(const FileTag& aTag)
            {
                u32 index;
                if (_FindFileIndex(aTag, index))
                {
                    mFileTable[index].Hash = 0u;
                    mFileTable[index].ModifiedTime = 0u;
                    
                    Commit();
                }
            }

            void FileSystem::Read(const FileTag& aTag, ByteBuffer& arBuffer)
            {
                u32 index;
                JZ_E_ON_FAIL(_FindFileIndex(aTag, index), "file not found.");
                const FileTableEntry& entry = mFileTable[index];

                const u32 kFileSize = entry.FileSize;
        
                arBuffer.resize(kFileSize);
                
                u64 filePos = entry.FirstBlockPosition;
                size_t bufferPos = 0u;

                const BlockHeader& header = _GetBlockHeader();            
                const size_t kBlockBufferSize = mBlockBuffer.GetSizeInBytes();
                u8* const kpBlockData = (mBlockBuffer.Get() + sizeof(BlockHeader));

                while (bufferPos < kFileSize)
                {
                    _ReadBlockAt(filePos);
                    JZ_E_ON_FAIL(Crc32(kpBlockData, header.DataSize) == header.CRC, "CRC check failed.");

                    size_t bytesRead = 0u;
                
                    if (header.Descriptor == Block::kDescriptorDeflated)
                    {
                        bytesRead = _Inflate(kpBlockData, header.DataSize, (arBuffer.Get() + bufferPos), kFileSize - bufferPos);
                    }
                    else
                    {
                        bytesRead = header.DataSize;
                        memcpy((void_p)(arBuffer.Get() + bufferPos), kpBlockData, bytesRead);
                    }
                    
                    bufferPos += bytesRead;
                    filePos = header.NextBlock;
                }
                
                JZ_ASSERT(bufferPos == kFileSize);
            }
              
            void FileSystem::Write(const FileTag& aTag, const ByteBuffer& aBuffer)
            {
                FileTableEntry& entry = _AddFileEntry(aTag);

                size_t bufferPos = 0u;
                u64 filePos = (entry.FirstBlockPosition == 0u) ? mFileSystemSize : entry.FirstBlockPosition;
                const size_t kBufferSize = aBuffer.GetSizeInBytes();
                const size_t kBlockBufferSize = mBlockBuffer.GetSizeInBytes();
                const size_t kBlockDataSize = (kBlockBufferSize - sizeof(BlockHeader));
                u8* const kpBlockData = (mBlockBuffer.Get() + sizeof(BlockHeader));
                BlockHeader& header = _GetBlockHeader();

                entry.FileSize = kBufferSize;
                entry.FirstBlockPosition = filePos;

                // Write over free space.
                while (filePos > 0u && filePos < mFileSystemSize)
                {
                    const size_t kDataSize = Min(kBlockDataSize, kBufferSize - bufferPos);
                    
                    memcpy(kpBlockData, (aBuffer.Get() + bufferPos), kDataSize);
                    bufferPos += kDataSize;
                    
                    _ReadBlockHeaderAt(filePos);
                    
                    header.CRC = Crc32(kpBlockData, kDataSize);
                    header.Hash = aTag.GetHash();
                    header.Descriptor = Block::kDescriptorUncompressed;
                    header.DataSize = kDataSize;

                    if (header.NextBlock == 0u && bufferPos < kBufferSize)
                    {
                        header.NextBlock = mFileSystemSize;
                    }

                    _WriteBlockAt(filePos);
                    filePos = header.NextBlock;
                }
                 
                // Write at end.
                while (bufferPos < kBufferSize)
                {
                    const size_t kDataSize = Min(kBlockDataSize, kBufferSize - bufferPos);

                    memcpy(kpBlockData, (aBuffer.Get() + bufferPos), kDataSize);
                    bufferPos += kDataSize;
                    
                    header.CRC = Crc32(kpBlockData, kDataSize);
                    header.Hash = aTag.GetHash();
                    header.Descriptor = Block::kDescriptorUncompressed;
                    header.DataSize = kDataSize;
                    
                    if (bufferPos < kBufferSize) { header.NextBlock = (mFileSystemSize + kBlockBufferSize); }
                    else { header.NextBlock = 0u; }
                    
                    _WriteBlockAt(filePos);
                    filePos = header.NextBlock;
                    
                    JZ_ASSERT(filePos == mCurrentPos || filePos == 0u);
                }
                
                Commit();
            }
      
            void FileSystem::_CalculateFileSystemSize()
            {
                JZ_E_ON_FAIL(fseek(mpFileSystemHandle, 0, SEEK_END) == 0, "seek failed.");
                mFileSystemSize = _Tell();
                mCurrentPos = mFileSystemSize;
            }
      
            void FileSystem::_Flush()
            {
                JZ_E_ON_FAIL(fflush(mpFileSystemHandle) == 0, "flush failed.");
            }
      
            void FileSystem::_PeekBlockHeader(u64 aPos)
            {
                _ReadBlockHeaderAt(aPos);
                _SeekAbsolute(aPos);
            }
      
            void FileSystem::_ReadAt(void_p ap, size_t aSize, u64 aFilePos)
            {
                _SeekAbsolute(aFilePos);
                
                JZ_E_ON_FAIL(fread(ap, 1, aSize, mpFileSystemHandle) == aSize, "read failed.");
                mCurrentPos += aSize;
            }

            void FileSystem::_ReadBlockAt(u64 aPos)
            {
                _ReadAt(mBlockBuffer.Get(), mBlockBuffer.GetSizeInBytes(), aPos);
            }
            
            void FileSystem::_ReadBlockHeaderAt(u64 aPos)
            {
                _ReadAt(mBlockBuffer.Get(), sizeof(BlockHeader), aPos);
            }

            void FileSystem::_SeekAbsolute(u64 aPos)
            {
                if (mCurrentPos != aPos)
                {
                    JZ_E_ON_FAIL(_fseeki64(mpFileSystemHandle, aPos, SEEK_SET) == 0, "absolute seek failed.");
                    mCurrentPos = aPos;
                }
                
                JZ_ASSERT(_ftelli64(mpFileSystemHandle) == mCurrentPos);
            }
            
            u64 FileSystem::_Tell()
            {
                s64 ret = _ftelli64(mpFileSystemHandle);
                JZ_E_ON_FAIL(ret >= 0, "tell failed.");
                
                return (u64)ret;
            }
            
            void FileSystem::_WriteAt(const void_p ap, size_t aSize, u64 aFilePos)
            {
                _SeekAbsolute(aFilePos);
                JZ_E_ON_FAIL(fwrite(ap, 1, aSize, mpFileSystemHandle) == aSize, "write failed.");
                mCurrentPos += aSize;
                
                if (mCurrentPos > mFileSystemSize)
                {
                    mFileSystemSize = mCurrentPos;
                }
            }

            void FileSystem::_WriteBlockAt(u64 aPos)
            {
                _WriteAt((const void_p)mBlockBuffer.Get(), mBlockBuffer.GetSizeInBytes(), aPos);
            }
            
            void FileSystem::_WriteBlockHeaderAt(u64 aPos)
            {
                _WriteAt((const void_p)mBlockBuffer.Get(), sizeof(BlockHeader), aPos);
            }
            
            void FileSystem::_WriteFileSystemHeader()
            {
                _WriteAt((const void_p)&mFileSystemHeader, sizeof(FileSystemHeader), 0u);
            }
            
            void FileSystem::_WriteFileTable()
            {
                _WriteAt((const void_p)mFileTable.Get(), mFileTable.GetSizeInBytes(), (1 << mFileSystemHeader.BlockSizePower));
            }
            
            bool FileSystem::_FindFileIndex(const FileTag& aTag, u32& arIndex) const
            {
                u32 index = _GetIndex(aTag);
                if (mFileTable[index].Hash == 0u && mFileTable[index].ModifiedTime == 0u)
                {
                    return false;
                }
                
                const size_t kFileTableSize = mFileTable.size();
                for (; index < kFileTableSize; index++)
                {
                    const FileTableEntry& entry = mFileTable[index];
                    if (!(entry.Hash == 0u && entry.ModifiedTime == 0u) && entry.Hash == aTag.GetHash())
                    {
                        arIndex = index;
                        return true;
                    }
                }
                
                return false;
            }

            FileTableEntry& FileSystem::_AddFileEntry(const FileTag& aTag)
            {
                const size_t kFileTableSize = mFileTable.size();
                u32 index = _GetIndex(aTag);

                for (; index < kFileTableSize; index++)
                {
                    FileTableEntry& entry = mFileTable[index];
                    
                    if (entry.Hash == 0u && entry.ModifiedTime == 0u)
                    {
                        entry.Hash = aTag.GetHash();
                        
                        const time_t kCurrentTime = time(null);
                        const u32 kModTime = (u32)(kCurrentTime - gskBaseTime);
                        entry.ModifiedTime = kModTime;
                        
                        return entry;
                    }
                }
                
                JZ_E_ON_FAIL(false, "could not find entry for write, file table is full.");
            }
            
            void FileSystem::_Open(const char* apFileSystemFilename)
            {
                struct stat fileInfo;
                const bool kbExists = (stat(apFileSystemFilename, &fileInfo) == 0) ? true : false;

    #           if JZ_PLATFORM_WINDOWS
                    fopen_s(&mpFileSystemHandle, apFileSystemFilename, (kbExists) ? "rb+" : "wb+");
    #           else
                    mpFileSystemHandle = fopen(apFileSystemFilename, (kbExists) ? "rb+" : "wb+");
    #           endif

                JZ_E_ON_FAIL(mpFileSystemHandle, "failed opening archive.");
                
                try
                {
                    if (!kbExists)
                    {
                        mFileSystemHeader = gskNewFileSystemHeader;  
                        mBlockBuffer.resize((1 << mFileSystemHeader.BlockSizePower));
                        mFileTable.resize((1 << mFileSystemHeader.FileTableSizePower));
                        mFileTable.Initialize();
                        _WriteFileSystemHeader();
                        _WriteFileTable();
                    }
                    else
                    {
                        _ReadAt((void_p)&mFileSystemHeader, sizeof(FileSystemHeader), 0u);
                        JZ_E_ON_FAIL(mFileSystemHeader.BlockSizePower <= gskMaximumBlockSizePower, "chunk size is too big.");
                        JZ_E_ON_FAIL(mFileSystemHeader.FileTableSizePower <= gskMaximumFileTableSizePower, "file table size is too big.");
                        mBlockBuffer.resize((1 << mFileSystemHeader.BlockSizePower));
                        mFileTable.resize((1 << mFileSystemHeader.FileTableSizePower));
                        _ReadAt((void_p)mFileTable.Get(), mFileTable.GetSizeInBytes(), (1 << mFileSystemHeader.BlockSizePower));
                        _CalculateFileSystemSize();
                    }
                }
                catch (std::exception&)
                {
                    _Destroy();
                    throw;
                }
            }
                                    
            u32 FileSystem::_GetIndex(const FileTag& aTag) const
            {
                const u32 kMask = (1 << mFileSystemHeader.FileTableSizePower) - 1u;
                const u32 kRet = aTag.GetIndex() & kMask;
                
                return kRet;
            }
    #   endif

    }
}

#undef JZ_HASH
