#include "vfs_posix.h"
#include "memory_stream.h"
#include <iostream>
#include <cstdio>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <sys/stat.h>
#endif
#include "../ghl_ref_counter_impl.h"
#include "../ghl_log_impl.h"
#include <ghl_data.h>
#ifdef _MSC_VER
#define snprintf _snprintf
#include <windows.h>
#endif
namespace GHL {
    
    static const char* MODULE = "VFS";

    class PosixFileStream : public RefCounterImpl<DataStream> {
    private:
        FILE* m_file;
        bool m_eof;
    public:
        explicit PosixFileStream(FILE* file) {
            m_file = file;
        }
        virtual ~PosixFileStream() {
            fclose(m_file);
        }
        /// read data
        virtual UInt32 GHL_CALL Read(Byte* dest,UInt32 bytes) {
            return fread(dest,1,bytes,m_file);
        }
        /// write data
        virtual UInt32 GHL_CALL Write(const Byte* src,UInt32 bytes) {
            return fwrite(src,1,bytes,m_file);
        }
        /// tell
        virtual UInt32 GHL_CALL Tell() const {
               return ftell(m_file);
        }
        /// seek
        virtual	bool GHL_CALL Seek(Int32 offset,FileSeekType st) {
           int dir = 0;
           switch (st)
           {
           case F_SEEK_CURRENT :
               dir = SEEK_CUR;
               break;
           case F_SEEK_END :
               dir = SEEK_END;
               break;
           case F_SEEK_BEGIN :
               dir = SEEK_SET;
               break;
           default: return false;
           }
           return fseek(m_file,offset,dir)==0;
        }
        /// End of file
        virtual bool GHL_CALL Eof() const {
                return feof(m_file) != 0;
        }
    };

    VFSPosixImpl::VFSPosixImpl(const char* dat,const char* docs) {
        m_data_dir = dat;
        m_docs_dir = docs;
    }

    VFSPosixImpl::~VFSPosixImpl() {
    }

    /// get dir
    const char* GHL_CALL VFSPosixImpl::GetDir(DirType dt) const {
        if (dt==DIR_TYPE_DATA) {
            return m_data_dir.c_str();
        }
        if (dt==DIR_TYPE_USER_PROFILE) {
            return m_docs_dir.c_str();
        }
        return "/";
    }
    /// attach package
    void GHL_CALL VFSPosixImpl::AttachPack(DataStream* /*ds*/) {
    }
    /// file is exists
    bool GHL_CALL VFSPosixImpl::IsFileExists(const char* file) const {
		typedef struct stat stat_t;
		stat_t s;
		if (::stat(file, (stat_t*)(&s)) == 0) {
			return (s.st_mode&S_IFREG)!=0;
        }
        return false;
    }
    /// remove file
    bool GHL_CALL VFSPosixImpl::DoRemoveFile(const char* /*file*/) {
        return false;
    }
    /// copy file
    bool GHL_CALL VFSPosixImpl::DoCopyFile(const char* /*from*/,const char* /*to*/) {
        return false;
    }
    /// create dir
    bool GHL_CALL VFSPosixImpl::DoCreateDir(const char* path) {
#ifdef _MSC_VER
		return CreateDirectory(path, NULL) == 0;
#else
		return mkdir(path, 0777) == 0;
#endif
    }
    /// open file
    DataStream* GHL_CALL VFSPosixImpl::OpenFile(const char* _file){
        (void)MODULE;
        LOG_VERBOSE("try open file '" << _file << "'");
        if (!_file) return 0;
        if (_file[0]==0) return 0;
        if (!IsFileExists(_file)) return 0;
        FILE* f = fopen(_file, "rb"  );
        if (f)
            return new PosixFileStream(f);
        return 0;
    }
    
    /// write file
    bool GHL_CALL VFSPosixImpl::WriteFile(const char* file, const Data* data) {
        FILE* f = fopen(file, "wb" );
        if (!f)
            return false;
        size_t writen = fwrite(data->GetData(), data->GetSize(),1,  f);
        fclose(f);
        return writen == 1;
    }
}


GHL_API GHL::VFS* GHL_CALL GHL_CreateVFSPosix(const char* data,const char* docs) {
    return new GHL::VFSPosixImpl(data,docs);
}

GHL_API void GHL_CALL GHL_DestroyVFSPosix(GHL::VFS* vfs) {
    delete reinterpret_cast<GHL::VFSPosixImpl*>(vfs);
}


