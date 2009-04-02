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
#ifndef _JZ_SYSTEM_FILES_H_
#define _JZ_SYSTEM_FILES_H_

#include <jz_core/Auto.h>
#include <jz_core/Utility.h>
#include <jz_system/Mutex.h>
#include <list>
#include <string>
#include <vector>

namespace jz
{
    namespace system
    {

        class IReadFile
        {
        public:
            IReadFile()
                : mReferenceCount(0u)
            {}

            virtual ~IReadFile()
            {}

            virtual const char* GetFilename() const = 0;
            virtual natural     GetPosition() const = 0;
            virtual size_t      GetSize()     const = 0;

            virtual size_t Read(void_p apOutBuffer, size_t aSize) = 0;
            virtual bool Seek(natural aPosition, bool abRelative = false) = 0;
            
        protected:
            size_t mReferenceCount;

        private:
            friend void jz::__IncrementRefCount<system::IReadFile>(system::IReadFile* p);
            friend void jz::__DecrementRefCount<system::IReadFile>(system::IReadFile* p);

            IReadFile(const IReadFile&);
            IReadFile& operator=(const IReadFile&);        
        };
        
        typedef AutoPtr<IReadFile> IReadFilePtr;


        class IWriteFile
        {
        public:
            IWriteFile()
                : mReferenceCount(0u)
            {}

            virtual ~IWriteFile()
            {}

            virtual const char* GetFilename() const = 0;
            virtual natural     GetPosition() const = 0;
            virtual size_t      GetSize()     const = 0;

            virtual bool   Seek(natural aPosition, bool abRelative = false) = 0;
            virtual size_t Write(voidc_p apOutBuffer, size_t aSize) = 0;
            
        protected:
            size_t mReferenceCount;

        private:
            friend void jz::__IncrementRefCount<system::IWriteFile>(system::IWriteFile* p);
            friend void jz::__DecrementRefCount<system::IWriteFile>(system::IWriteFile* p);

            IWriteFile(const IWriteFile&);
            IWriteFile& operator=(const IWriteFile&);        
        };
        
        typedef AutoPtr<IWriteFile> IWriteFilePtr;
        

        class MemoryReadFile : public IReadFile
        {
        public:
            MemoryReadFile(const char* apFilename, void_p apData, size_t aSize, bool abOwnMemory = false);
            MemoryReadFile(const string& aFilename, void_p apData, size_t aSize, bool abOwnMemory = false);
            virtual ~MemoryReadFile();

            virtual const char* GetFilename() const
            {
                return mFilename.c_str();
            }
            
            virtual natural GetPosition() const
            {
                return mPosition;
            }
            
            virtual size_t GetSize() const
            {
                return mSize;
            }

            JZ_EXPORT virtual size_t Read(void_p apOutBuffer, size_t aSize);
            JZ_EXPORT virtual bool Seek(natural aPosition, bool abRelative = false);

        private:
            friend void jz::__IncrementRefCount<system::MemoryReadFile>(system::MemoryReadFile* p);
            friend void jz::__DecrementRefCount<system::MemoryReadFile>(system::MemoryReadFile* p);

            MemoryReadFile(const MemoryReadFile&);
            MemoryReadFile& operator=(const MemoryReadFile&);
            
            bool    mbOwnMemory;
            string  mFilename;
            void_p   mpData;
            natural mPosition;
            size_t  mSize;
            Mutex mMutex;

            void Destroy();
        };
            

        class ReadFile : public IReadFile
        {
        public:
            ReadFile(const char* apFilename);
            ReadFile(const string& aFilename);
            virtual ~ReadFile();

            virtual const char* GetFilename() const override
            {
                return mFilename.c_str();
            }
            
            JZ_EXPORT virtual natural GetPosition() const override;
            
            virtual size_t GetSize() const override
            {
                return mSize;
            }

            JZ_EXPORT virtual size_t Read(void_p apOutBuffer, size_t aSize) override;
            JZ_EXPORT virtual bool Seek(natural aPosition, bool abRelative = false) override;

        private:
            friend void jz::__IncrementRefCount<system::ReadFile>(system::ReadFile* p);
            friend void jz::__DecrementRefCount<system::ReadFile>(system::ReadFile* p);

            ReadFile(const ReadFile&);
            ReadFile& operator=(const ReadFile&);
                    
            string mFilename;
            Handle mpFile;
            size_t mSize;
            Mutex mMutex;
            
            void Load();
        };

        class WriteFile : public IWriteFile
        {
        public:
            WriteFile(const char* apFilename);
            WriteFile(const string& aFilename);
            virtual ~WriteFile();

            virtual const char* GetFilename() const
            {
                return mFilename.c_str();
            }
            
            JZ_EXPORT virtual natural GetPosition() const;
            
            virtual size_t GetSize() const
            {
                return mSize;
            }

            JZ_EXPORT virtual bool Seek(natural aPosition, bool abRelative = false);
            JZ_EXPORT virtual size_t Write(voidc_p apOutBuffer, size_t aSize);

        private:
            friend void jz::__IncrementRefCount<system::WriteFile>(system::WriteFile* p);
            friend void jz::__DecrementRefCount<system::WriteFile>(system::WriteFile* p);

            WriteFile(const WriteFile&);
            WriteFile& operator=(const WriteFile&);
                    
            string mFilename;
            FILE*  mpFile;
            size_t mSize;
            Mutex mMutex;
            
            void Load();
        };
        
        class SubReadFile : public IReadFile
        {
        public:
            SubReadFile(const char* apFilename, const AutoPtr<IReadFile>& apFile, natural aStartPos, natural aEndPos);
            SubReadFile(const string& aFilename, const AutoPtr<IReadFile>& apFile, natural aStartPos, natural aEndPos);
            virtual ~SubReadFile();

            virtual const char* GetFilename() const
            {
                return mFilename.c_str();
            }

            virtual natural GetPosition() const
            {
                return mPosition;
            }
            
            virtual size_t GetSize() const
            {
                return mSize;
            }

            virtual JZ_EXPORT size_t Read(void_p apOutBuffer, size_t aSize);
            virtual JZ_EXPORT bool Seek(natural aPosition, bool abRelative = false);

        private:
            friend void jz::__IncrementRefCount<system::SubReadFile>(system::SubReadFile* p);
            friend void jz::__DecrementRefCount<system::SubReadFile>(system::SubReadFile* p);

            SubReadFile(const SubReadFile&);
            SubReadFile& operator=(const SubReadFile&);
                    
            string mFilename;
            AutoPtr<IReadFile> mpFile;
            natural mPosition;
            size_t mSize;
            natural mStart;
            Mutex mMutex;
        };

        class IArchive 
        {
        public:
            IArchive()
                : mReferenceCount(0u)
            {}

            virtual ~IArchive()
            {}
            
            virtual bool GetExists(const char* apFilename) const = 0;
            virtual IReadFile* Open(const char* apFilename) const = 0;

        protected:
            size_t mReferenceCount;

        private:
            friend void jz::__IncrementRefCount<system::IArchive>(system::IArchive* p);
            friend void jz::__DecrementRefCount<system::IArchive>(system::IArchive* p);

            IArchive(const IArchive&);
            IArchive& operator=(const IArchive&);
        };    
        
    #   if defined(_MSC_VER)
    #       pragma pack(push,packing)
    #       pragma pack(1)
    #       define STRUCT_PACK
    #   elif defined(__GNUC__)
    #       define STRUCT_PACK __attribute__((packed))
    #   else
    #       error "Platform not supported."
    #   endif

        struct ZipDataDescriptor
        {
            s32 Crc32;
            u32 CompressedSize;
            u32 UncompressedSize;
        } STRUCT_PACK; 

        struct ZipLocalFileHeader
        {
            s32               Signature;
            s16               VersionNeededToExtract;
            s16               Flags;
            s16               CompressionMethod;
            s16               LastModFileTime;
            s16               LastModFileDate;
            ZipDataDescriptor DataDescriptor;
            u16               FilenameLength;
            u16               ExtraFieldLength;
        } STRUCT_PACK;

    #   ifdef _MSC_VER
    #       pragma pack(pop,packing)
    #   endif
    #   undef STRUCT_PACK

        class ZipArchive : public IArchive
        {
        public:
            JZ_EXPORT ZipArchive(const char* apFilename);
            JZ_EXPORT ZipArchive(const string& aFilename);
            JZ_EXPORT virtual ~ZipArchive();

            virtual bool GetExists(const char* apFilename) const;
            virtual IReadFile* Open(const char* apFilename) const;

        private:
            friend void jz::__IncrementRefCount<system::ZipArchive>(system::ZipArchive* p);
            friend void jz::__DecrementRefCount<system::ZipArchive>(system::ZipArchive* p);

            ZipArchive(const ZipArchive&);
            ZipArchive& operator=(const ZipArchive&);
            
            struct ZipFileEntry
            {
                ZipFileEntry()
                    : Offset(0)
                {}
                
                string Filename;
                ZipLocalFileHeader Header;
                natural Offset;
                string Path;
            };

            typedef vector<ZipFileEntry> CompressedFiles;

            CompressedFiles    mFiles;
            AutoPtr<IReadFile> mpZipFile;

            IReadFile* _Open(const string& aFilename) const;
            bool FindLocalHeaders();
            void ProcessFilename(const char* apFilename, ZipFileEntry& arOut);
        };

        class FileArchive : public IArchive
        {
        public:
            JZ_EXPORT FileArchive(const string& aPath);
            JZ_EXPORT virtual ~FileArchive();

            const string& GetPath() const { return mPath; }

            JZ_EXPORT virtual bool GetExists(const char* apFilename) const override;
            JZ_EXPORT virtual IReadFile* Open(const char* apFilename) const override;

        private:
            const string mPath;

            friend void jz::__IncrementRefCount<system::FileArchive>(system::FileArchive* p);
            friend void jz::__DecrementRefCount<system::FileArchive>(system::FileArchive* p);

            FileArchive(const FileArchive&);
            FileArchive& operator=(const FileArchive&);
        };
        
        class Files : public Singleton<Files>
        {
        public:
            Files();
            ~Files();

            JZ_EXPORT void AddArchive(const AutoPtr<IArchive>& apArchive);
            JZ_EXPORT bool GetExists(const char* apFilename) const;
            JZ_EXPORT IReadFile* Open(const char* apFilename) const;
            JZ_EXPORT IWriteFile* OpenWriteable(const char* apFilename) const;

            static const natural kMaximumPathLength = 1024;
		    typedef vector<string> DirectoryList;
            
            static bool IsDirectorySeperator(char c)
            {
                return (c == '/') || (c == '\\');
            }

            JZ_EXPORT static void CleanFilename(string& arFilename);
            JZ_EXPORT static string Combine(const string& aPathLeft, const string& aPathRight);
            JZ_EXPORT static bool Exists(const char* apFilename);
            JZ_EXPORT static string ExtractBase(const string& aFilename);
            JZ_EXPORT static string ExtractExtension(const string& aFilename);
            JZ_EXPORT static string ExtractPath(const string& aFilename);
            JZ_EXPORT static string RemoveExtension(const string& aFilename);
            JZ_EXPORT static void ListDirectories(const string aDirectory, DirectoryList& arOut);
            JZ_EXPORT static void ListFiles(const string& aDirectory, DirectoryList& arOut);
            JZ_EXPORT static bool IsDirectory(const char* apDirectoryName);  
            JZ_EXPORT static bool IsFile(const char* apFilename);
            JZ_EXPORT static string GetCurrentProcessFilename();
            JZ_EXPORT static time_t GetModifiedTime(const char* apFilename);
            JZ_EXPORT static void SetModifiedTime(const char* apFilename, time_t aTime);
            JZ_EXPORT static string GetWorkingDirectory();
            JZ_EXPORT static void SetWorkingDirectory(const string& aPath);
            
        private:
            typedef list<AutoPtr<IArchive>> Archives;
            
            Archives mArchives;
            Mutex mMutex;
        };
 
    }
}

#endif
