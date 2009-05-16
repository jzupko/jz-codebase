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
#include <jz_core/StringUtility.h>
#include <jz_system/Files.h>

#include <sys/stat.h>
#include <zlib/zlib.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#   include <direct.h>
#   include <io.h>
#endif

namespace jz
{
    system::Files* system::Files::mspSingleton = null;
    namespace system
    {

        MemoryReadFile::MemoryReadFile(const string& aFilename, void_p apData, size_t aSize, bool abOwnMemory)
            : mFilename(aFilename), mpData(apData), mSize(aSize), mPosition(0), mbOwnMemory(abOwnMemory)
        {}
        
        MemoryReadFile::MemoryReadFile(const char* apFilename, void_p apData, size_t aSize, bool abOwnMemory)
            : mFilename(apFilename), mpData(apData), mSize(aSize), mPosition(0), mbOwnMemory(abOwnMemory)
        {}

        MemoryReadFile::~MemoryReadFile()
        {
            if (mbOwnMemory && mpData)
            {
                Free(mpData);
            }
        }

        size_t MemoryReadFile::Read(void_p apOutBuffer, size_t aSize)
        {
            size_t n   = aSize;

            {
                Lock lock(mMutex);
                
                size_t pos = mPosition + n;
                
                if (pos > mSize)
                {
                    n -= (pos - mSize);
                }

                memcpy(apOutBuffer, (byte*)mpData + mPosition, n);

        #       if !NDEBUG
                    long tmp = mPosition;
                    mPosition += (natural)n;
                    JZ_ASSERT(mPosition >= tmp); // overflow
        #       else
                    mPosition += (natural)n;
        #       endif
            }

            return n;
        }

        bool MemoryReadFile::Seek(natural aPosition, bool abRelative)
        {
            if (abRelative)
            {
                Lock lock(mMutex);
                if (((size_t)mPosition + (size_t)aPosition) > mSize)
                {
                    return false;
                }
                else
                {
                    mPosition += aPosition;
                }
            }
            else
            {
                Lock lock(mMutex);
                if ((size_t)aPosition > mSize)
                {
                    return false;
                }
                else
                {
                    mPosition = aPosition;
                }
            }

            return true;
        }

        ReadFile::ReadFile(const char* apFilename)
            : mFilename(apFilename), mSize(0u)
        {
            Load();
        }
        
        ReadFile::ReadFile(const string& aFilename)
            : mFilename(aFilename), mSize(0u)
        {
            Load();
        }

        ReadFile::~ReadFile()
        {
#           if JZ_PLATFORM_WINDOWS
                CloseHandle(StaticCast<HANDLE>(mpFile));
#           else
                fclose(StaticCast<HANDLE>(mpFile));
#           endif
        }
        
        natural ReadFile::GetPosition() const
        {
            Lock lock(const_cast<Mutex&>(mMutex));
            
#           if JZ_PLATFORM_WINDOWS
                return SetFilePointer(StaticCast<HANDLE>(mpFile), 0u, null, FILE_CURRENT);
#           else
                return ftell(StaticCast<FILE>(mpFile));
#           endif
        }

        size_t ReadFile::Read(void_p apOutBuffer, size_t aSize)
        {
            Lock lock(mMutex);

#           if JZ_PLATFORM_WINDOWS
                DWORD read = 0u;
                ::ReadFile(StaticCast<HANDLE>(mpFile), apOutBuffer, aSize, &read, null);

                return (read);
#           else
                return (fread(apOutBuffer, 1, aSize, StaticCast<FILE>(mpFile)));
#           endif
        }
        
        bool ReadFile::Seek(natural aPosition, bool abRelative)
        {
            Lock lock(mMutex);
#           if JZ_PLATFORM_WINDOWS
                DWORD ret = SetFilePointer(StaticCast<HANDLE>(mpFile), aPosition, null, (abRelative) ? FILE_CURRENT : FILE_BEGIN);
                if (ret == INVALID_SET_FILE_POINTER)
                {
                    if (GetLastError() != NO_ERROR) 
                    {
                        return false;
                    }
                }

                return true;
#           else
                return (fseek(StaticCast<FILE>(mpFile), aPosition, (abRelative) ? SEEK_CUR : SEEK_SET));
#           endif
        }

        void ReadFile::Load()
        {
#           if JZ_PLATFORM_WINDOWS
                mpFile = CreateFile(mFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null);
                if (mpFile)
                {
                    mSize = GetFileSize(StaticCast<HANDLE>(mpFile), null);
                }
                else
                {
                    throw std::exception(__FUNCTION__ ": load failed.");
                }
#           else
                mpFile = fopen(mFilename.c_str(), "rb");
            
                if (mpFile)
                {
                    fseek(mpFile, 0, SEEK_END);
                    mSize = ftell(mpFile);
                    fseek(mpFile, 0, SEEK_SET);
                }
                else
                {
                    throw std::exception(__FUNCTION__ ": load failed.");
                }        
#           endif
        }

        WriteFile::WriteFile(const char* apFilename)
            : mFilename(apFilename), mpFile(null), mSize(0u)
        {
            Load();
        }
        
        WriteFile::WriteFile(const string& aFilename)
            : mFilename(aFilename), mpFile(null), mSize(0u)
        {
            Load();
        }

        WriteFile::~WriteFile()
        {
            fclose(mpFile);
        }
        
        natural WriteFile::GetPosition() const
        {
            Lock lock(const_cast<Mutex&>(mMutex));
            return (ftell(mpFile));
        }

        bool WriteFile::Seek(natural aPosition, bool abRelative)
        {
            Lock lock(mMutex);
            return (fseek(mpFile, aPosition, abRelative ? SEEK_CUR : SEEK_SET) == 0);
        }

        size_t WriteFile::Write(voidc_p apOutBuffer, size_t aSize)
        {
            Lock lock(mMutex);
            return (fwrite(apOutBuffer, 1, aSize, mpFile));
        }

        void WriteFile::Load()
        {
    #       if JZ_PLATFORM_WINDOWS
                fopen_s(&mpFile, mFilename.c_str(), "wb");
    #       else
                mpFile = fopen(mFilename.c_str(), "wb");
    #       endif            
            
            if (mpFile)
            {
                fseek(mpFile, 0, SEEK_END);
                mSize = ftell(mpFile);
                fseek(mpFile, 0, SEEK_SET);
            }
            else
            {
                throw std::exception(__FUNCTION__ ": load failed.");
            }        
        }

        SubReadFile::SubReadFile(const char* apFilename, const AutoPtr<IReadFile>& apFile, natural aStartPos, natural aEndPos)
            : mFilename(apFilename), mpFile(apFile), mPosition(0), mSize(aEndPos-aStartPos+1), mStart(aStartPos)
        {}
        
        SubReadFile::SubReadFile(const string& aFilename, const AutoPtr<IReadFile>& apFile, natural aStartPos, natural aEndPos)
            : mFilename(aFilename), mpFile(apFile), mPosition(0), mSize(aEndPos-aStartPos+1), mStart(aStartPos)
        {}    

        SubReadFile::~SubReadFile()
        {}

        size_t SubReadFile::Read(void_p apOutBuffer, size_t aSize)
        {
            size_t ret = 0u;

            {
                Lock lock(mMutex);

                size_t  n    = aSize;
                natural orig = mpFile->GetPosition();
                
                mpFile->Seek(mPosition + mStart, false);

                size_t pos = mPosition + n;

                if (pos > mSize)
                {
                    n -= (pos - mSize);
                }

                ret = mpFile->Read(apOutBuffer, n);

        #       if !NDEBUG
                    long tmp = mPosition;
                    mPosition += (natural)n;
                    JZ_ASSERT(mPosition >= tmp); // overflow
        #       else
                    mPosition += (natural)n;
        #       endif

                mpFile->Seek(orig, false);
            }

            return ret;
        }

        bool SubReadFile::Seek(natural aPosition, bool abRelative)
        {
            if (abRelative)
            {
                Lock lock(mMutex);
                if ((size_t)mPosition + (size_t)aPosition > mSize)
                {
                    return false;
                }
                else
                {
                    mPosition += aPosition;
                }
            }
            else
            {
                Lock lock(mMutex);
                if ((size_t)aPosition > mSize)
                {
                    return false;
                }
                else
                {
                    mPosition = aPosition;
                }
            }

            return true;
        }

        enum
        {
            gkZipEncrypted            = 1 << 0,
            gkZipCompressionBit1      = 1 << 1,
            gkZipCompressionBit2      = 1 << 2,
            gkZipUseDataDescriptor    = 1 << 3,
            gkZipStrongEncrypted      = 1 << 6,
            gkZipUseUTF8              = 1 << 11,
            gkLocalHeaderSignature    = 0x04034b50,
        };
        
        enum
        {
            gkZipCompressionStore     = 0,
            gkZipCompressionShrunk    = 1,
            gkZipCompressionReduced1  = 2,
            gkZipCompressionReduced2  = 3,
            gkZipCompressionReduced3  = 4,
            gkZipCompressionReduced4  = 5,
            gkZipCompressionImploded  = 6,
            gkZipCompressionDeflated  = 8,
            gkZipCompressionDeflate64 = 9,
            gkZipCompressionBZip2     = 12
        };

        ZipArchive::ZipArchive(const char* apFilename)
            : mpZipFile(Files::GetSingleton().Open(apFilename))
        {
            while (FindLocalHeaders());
        }

        ZipArchive::ZipArchive(const string& aFilename)
            : mpZipFile(Files::GetSingleton().Open(aFilename.c_str()))
        {
            while (FindLocalHeaders());
        }

        ZipArchive::~ZipArchive()
        {}

        bool ZipArchive::GetExists(const char* apFilename) const
        {
            string filename(Files::CleanFilename(apFilename));
            
            if (mFiles.find(filename) != mFiles.end()) { return true; }
            else { return false; }
        }

        IReadFile* ZipArchive::Open(const char* apFilename) const
        {
            string filename(Files::CleanFilename(apFilename));
            
            return _Open(filename);
        }

        IReadFile* ZipArchive::_Open(const string& aFilename) const
        {
            const ZipFileEntry* pEntry(0);
            
            CompressedFiles::const_iterator I = mFiles.find(aFilename);
            JZ_E_ON_FAIL(I != mFiles.end(), "file not found.");

            pEntry = &(I->second);
           
            switch (pEntry->Header.CompressionMethod)
            {
                case gkZipCompressionStore:
                    return new SubReadFile(pEntry->FullFilename, mpZipFile, pEntry->Offset, pEntry->Offset + pEntry->Header.DataDescriptor.UncompressedSize);
                break;
                case gkZipCompressionDeflated:
                    {
                        u32 cSize = pEntry->Header.DataDescriptor.CompressedSize;
                        u32 uSize = pEntry->Header.DataDescriptor.UncompressedSize;

                        byte* pcBuf = (byte*)Malloc(cSize, JZ_ALIGN_OF(byte));
                        byte* puBuf = (byte*)Malloc(uSize, JZ_ALIGN_OF(byte));

                        const_cast< AutoPtr<IReadFile>& >(mpZipFile)->Seek(pEntry->Offset, false);
                        const_cast< AutoPtr<IReadFile>& >(mpZipFile)->Read(pcBuf, cSize);
                        
                        z_stream stream;
                        int ret(Z_ERRNO);    			

                        stream.next_in   = (Bytef*)pcBuf;
                        stream.avail_in  = (uInt)cSize;
                        stream.next_out  = (Bytef*)puBuf;
                        stream.avail_out = (uInt)uSize;
                        stream.zalloc    = (alloc_func)0;
                        stream.zfree     = (free_func)0;

                        ret = inflateInit2(&stream, -MAX_WBITS);
                        
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

                        Free(pcBuf);
                        pcBuf = 0;
                        
                        if (ret != Z_OK)
                        {
                            Free(puBuf);
                            JZ_E_ON_FAIL(false, "inflation failed.");
                        }
                        else
                        {
                            return new MemoryReadFile(pEntry->FullFilename, puBuf, uSize, true);
                        }
                    }
                break;
            }
            
            JZ_E_ON_FAIL(false, "file not found.");
        }

    #   if JZ_LITTLE_ENDIAN
            bool ZipArchive::FindLocalHeaders()
            {
                char buffer[Files::kMaximumPathLength];

	            ZipFileEntry entry;

                JZ_E_ON_FAIL(mpZipFile->Read(&entry.Header, sizeof(ZipLocalFileHeader)) == sizeof(ZipLocalFileHeader), "failed reading local header.");
                
                // todo: verify that this is correct procedure - basically, we stop when we get to 
                // the end of the zip file, which has different information than the local headers
                // and will not have a "correct" signature.
                if (entry.Header.Signature != gkLocalHeaderSignature)
                {
                    return false;
                }

                JZ_E_ON_FAIL(entry.Header.FilenameLength <= Files::kMaximumPathLength, "filename length too big.");
                JZ_E_ON_FAIL(mpZipFile->Read(buffer, entry.Header.FilenameLength) == entry.Header.FilenameLength, "failed reading filename.");
                
                buffer[entry.Header.FilenameLength] = 0; // add null terminator
                ProcessFilename(buffer, entry);

                // skip the extra field
                if (entry.Header.ExtraFieldLength > 0)
                {
                    JZ_E_ON_FAIL(mpZipFile->Seek(entry.Header.ExtraFieldLength, true), "failed skipping extra field.");
                }

                // todo: according to the standard, this is after the compressed data?
                // but how do we know the size in this case? Is the descriptor in the header
                // still valid?
                if (entry.Header.Flags & gkZipUseDataDescriptor)
                {
                    JZ_E_ON_FAIL(mpZipFile->Read(&entry.Header.DataDescriptor, sizeof(ZipDataDescriptor)) == sizeof(ZipDataDescriptor), "failed reading data descriptor.");
	            }
    	        
	            entry.Offset = mpZipFile->GetPosition();

                JZ_E_ON_FAIL(mpZipFile->Seek(entry.Header.DataDescriptor.CompressedSize, true), "failed skipping compressed data.");

                // THis is not general. Two folders deep will fail.
                mFiles.insert(make_pair(entry.FullFilename, entry));

	            return true;
            }
    #   endif        

        void ZipArchive::ProcessFilename(const char* apFilename, ZipFileEntry& arOut)
        {
            arOut.Path = Files::CleanFilename(Files::ExtractPath(apFilename));
            arOut.ShortFilename = Files::CleanFilename(Files::ExtractBase(apFilename) + "." + Files::ExtractExtension(apFilename));
            arOut.FullFilename = Files::CleanFilename(apFilename);
        }

        
        FileArchive::FileArchive(const string& aPath)
            : mPath(aPath)
        {
            JZ_E_ON_FAIL(Files::IsDirectory(mPath.c_str()), "path is not valid.");
        }

        FileArchive::~FileArchive()
        {
        }

        bool FileArchive::GetExists(const char* apFilename) const
        {
            string fullPath = Files::Combine(mPath, apFilename);

            bool bReturn = (Files::Exists(fullPath.c_str()));
            bReturn = bReturn && Files::IsFile(fullPath.c_str());
            
            return bReturn;
        }

        IReadFile* FileArchive::Open(const char* apFilename) const
        {
            string fullPath = Files::Combine(mPath, apFilename);

            IReadFile* pReturn = new ReadFile(fullPath.c_str());

            return pReturn;
        }

        Files::Files()
        {}
            
        Files::~Files()
        {}

        string Files::CleanFilename(const string& aFilename)
        {
            size_t size = aFilename.length();

            string ret;
            ret.resize(size);

            for (size_t i = 0u; i < size; i++)
            {
#               if JZ_PLATFORM_WINDOWS
                    if (aFilename[i] == '/') { ret[i] = '\\'; }
#               else
                    if (aFilename[i] == '\\') { ret[i] = '/'; }
#               endif
                    else
                    {
                        ret[i] = tolower(aFilename[i]);
                    }
            }

            return ret;
        }
           
        void Files::AddArchive(const AutoPtr<IArchive>& apArchive)
        {
            Lock lock(mMutex);
            mArchives.push_back(apArchive);
        }

        bool Files::GetExists(const char* apFilename) const
        {
            if (Files::Exists(apFilename)) { return true; }
            
            string cleanedFilename(CleanFilename(apFilename));

            if (Files::Exists(cleanedFilename.c_str())) { return true; }
            {
                Lock lock(const_cast<Mutex&>(mMutex));
                for (Archives::const_iterator I = mArchives.begin(); I != mArchives.end(); I++)
                {
                    if ((*I)->GetExists(cleanedFilename.c_str())) { return true; }
                }
            }

            return false;
        }

        IReadFile* Files::Open(const char* apFilename) const
        {
            if (Files::Exists(apFilename))
            {
                return new ReadFile(apFilename);
            }
            
            string cleanedFilename(CleanFilename(apFilename));

            if (Files::Exists(cleanedFilename.c_str()))
            {
                return new ReadFile(cleanedFilename);
            }
                
            {
                Lock lock(const_cast<Mutex&>(mMutex));
                for (Archives::const_iterator I = mArchives.begin(); I != mArchives.end(); I++)
                {
                    if ((*I)->GetExists(cleanedFilename.c_str()))
                    {
                        return (*I)->Open(cleanedFilename.c_str());
                    }
                }
            }

            string msg = __FUNCTION__ ": load failed, \"" + string(apFilename) + "\".";
            throw std::exception(msg.c_str());
        }

        IWriteFile* Files::OpenWriteable(const char* apFilename) const
        {
            return new WriteFile(apFilename);
        }

        string Files::Combine(const string& aPathLeft, const string& aPathRight)
        {
            string pathLeft(aPathLeft);
            string pathRight(aPathRight);

            StringUtility::TrimWhitespace(pathLeft);
            StringUtility::TrimWhitespace(pathRight);

            if (pathLeft.length() == 0) { return pathRight; }
            else if (pathRight.length() == 0) { return pathLeft; }
            else if (
                IsDirectorySeperator(pathLeft[pathLeft.length() - 1]) 
                || IsDirectorySeperator(pathRight[0]))
            {
                return (pathLeft + pathRight);
            }
            else
            {
#               if JZ_PLATFORM_WINDOWS
                    return (pathLeft + "\\" + pathRight);
#               else
                    return (pathLeft + "/" + pathRight);
#               endif
            }
        }

        bool Files::Exists(const char* apFilename)
        {
            struct stat fileInfo;

            if (stat(apFilename, &fileInfo) == 0)
            {
                return true;
            }
            else
            {
                return false;
            }    
        }

        string Files::ExtractBase(const string& aFilename)
        {
            const natural kSize = (natural)aFilename.length();
            natural lPos = -1;
            natural rPos = -1;

            for (natural i = kSize - 1; i >= 0; i--)
            {
                if (rPos < 0 && aFilename[i] == '.')
                {
                    rPos = i - 1;
                }

                if (lPos < 0 && (aFilename[i] == '/' || aFilename[i] == '\\'))
                {
                    lPos = i + 1;
                }

                if (lPos >= 0 && rPos >= 0)
                {
                    break;
                }
            }

            if (lPos < 0) { lPos = 0; }

            string ret = aFilename.substr(lPos, (rPos - lPos) + 1);
            return ret;
        }

        string Files::ExtractExtension(const string& aFilename)
        {
            const natural kSize = (natural)aFilename.length();

            for (natural i = kSize - 1; i >= 0; i--)
            {
                if (aFilename[i] == '.')
                {
                    natural pos = i + 1;

                    string ret = aFilename.substr(pos, kSize - pos);
                    return ret;
                }
            }

            return string();
        }

        string Files::ExtractPath(const string& aFilename)
        {
            const natural kSize = (natural)aFilename.length();

            for (natural i = kSize - 1; i >= 0; i--)
            {
                if ((aFilename[i] == '/' || aFilename[i] == '\\'))
                {
                    string ret = aFilename.substr(0, i);
                    return ret;
                }
            }

            return string();
        }

        string Files::RemoveExtension(const string& aFilename)
        {
            const natural kSize = (natural)aFilename.length();

            for (natural i = kSize - 1; i >= 0; i--)
            {
                if (aFilename[i] == '.')
                {
                    return aFilename.substr(0, i);
                }
            }

            return aFilename;
        }

    #   if JZ_PLATFORM_WINDOWS
        void Files::ListDirectories(const string aDirectory, DirectoryList& arOut)
        {
            struct _finddata_t dirInfo;
            intptr_t h = _findfirst(aDirectory.c_str(), &dirInfo);

            for (int res = (int)h; res != -1; res = _findnext(h, &dirInfo))
            {
                if ((dirInfo.attrib & _A_SUBDIR) != 0)
                {
                    arOut.push_back(dirInfo.name);
                }
            }

            _findclose(h);        
        }

        void Files::ListFiles(const string& aDirectory, DirectoryList& arOut)
        {
            struct _finddata_t fileInfo;
            intptr_t h = _findfirst(aDirectory.c_str(), &fileInfo);

            for (int res = (int)h; res != -1; res = _findnext(h, &fileInfo))
            {
                if ((fileInfo.attrib & _A_SUBDIR) == 0)
                {
                    arOut.push_back(fileInfo.name);
                }
            }

            _findclose(h);
        }
    #   endif

        bool Files::IsDirectory(const char* apDirectoryName)
        {
            struct stat fileInfo;
            string name(apDirectoryName);
            if (IsDirectorySeperator(name[name.length() - 1])) { name.erase(name.length() - 1); }

            if (stat(name.c_str(), &fileInfo) == 0)
            {
                if ((fileInfo.st_mode & S_IFDIR) != 0)
                {
                    return true;
                }
            }

            return false;
        }

        bool Files::IsFile(const char* apFilename)
        {
            struct stat fileInfo;

            if (stat(apFilename, &fileInfo) == 0)
            {
                if ((fileInfo.st_mode & S_IFREG) != 0)
                {
                    return true;
                }
            }

            return false;    
        }

        time_t Files::GetModifiedTime(const char* apFilename)
        {
            struct stat fileInfo;

            if (stat(apFilename, &fileInfo) == 0)
            {
                return (fileInfo.st_mtime);
            }
            else
            {
                return -1;
            }
        }

        string Files::GetCurrentProcessFilename()
        {
            TCHAR fullPath[kMaximumPathLength + 1u];
            GetModuleFileName(null, fullPath, kMaximumPathLength + 1u);

            return string(fullPath);
        }

    #if JZ_PLATFORM_WINDOWS
        FILETIME Convert(time_t aTime)
        {
            s64 temp = aTime;
            temp *= 10000000;
            temp += 116444736000000000LL;

            FILETIME ret;
            ret.dwLowDateTime = static_cast<DWORD>(temp);
            ret.dwHighDateTime = static_cast<DWORD>(temp >> 32);

            return ret;
        }

        void Files::SetModifiedTime(const char* apFilename, time_t aTime)
        {
            FILETIME time = Convert(aTime);

            HANDLE h = CreateFile(apFilename, FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

            bool bSucceeded = true;

            bSucceeded = bSucceeded && (h != INVALID_HANDLE_VALUE);
            bSucceeded = bSucceeded && SUCCEEDED(::SetFileTime(h, 0, 0, &time));

            CloseHandle(h);

            JZ_E_ON_FAIL(bSucceeded, "failed writing file time.");
        }

        string Files::GetWorkingDirectory()
        {
            char buf[kMaximumPathLength];

            JZ_E_ON_FAIL(_getcwd(buf, kMaximumPathLength) == buf, "failed getting working directory.");

            return string(buf);
        }

        void Files::SetWorkingDirectory(const string& aPath)
        {
            JZ_E_ON_FAIL(_chdir(aPath.c_str()) == 0, "failed setting working directory.");
        }
    #endif

    }
}
