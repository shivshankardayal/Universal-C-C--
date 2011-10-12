#include "cvm.h"
#include "sys.h"
#include "file.h"
#include "sysstruct.h"

#include "icvmerr.h"
#include "icvmsys.h"

// Copyright (C) 2000,2001 Bob Dayley

static 	ICVM_BaseHandler 		basehandler;
static 	ICVM_FileHandler 		filehandler;
static ICVM_DirectoryHandler 	directoryhandler;

// enumerated handlers so we can get around pointer usages

ICVM_BaseHandler *basehandlers[]={
	&basehandler,
	&filehandler,
	&directoryhandler,
	0
};

void ICVM_BaseHandler::Enumerate(void){
	int cnt=0;
	static int done=0;
	if(done==0){
		done=1;
		while(basehandlers[cnt]){
			basehandlers[cnt]->handlernumber=cnt;
			++cnt;
		}
	}
};



s32 ICVM_BaseHandler::GetCurUMask(void){
	return FILESYSTEMSTUFF.curumask;
};

void ICVM_BaseHandler::SetCurUMask(s32 newmask){
	FILESYSTEMSTUFF.curumask=newmask;
};

ICVM_FileHandle *ICVM_BaseHandler::FileHandles(void){
	return FILESYSTEMSTUFF.filehandles;
};

// THIS IS ONLY TEMPORARY - SHOULD BE GLOBALIZED AND INITIALIZED LATER

extern "C" {

// *********** TO BE OBSOLETE *****************
#ifdef UNIXSYSTEM
	s32 TranslateModeFromICVM(s32 inmode){
		return i_getrootfs()->TranslateModeFromICVM(inmode);
	};

	s32 TranslateModeToICVM(s32 inmode){
		return i_getrootfs()->TranslateModeToICVM(inmode);
	};
// *********** TO BE OBSOLETE *****************

#endif


	ICVM_BaseHandler *i_getrootfs(void){
		return &basehandler;
	};

	ICVM_BaseHandler *i_getfilehandler(char *path){
		return ICVM_BaseHandler::GetFileHandler(path);
	};



	ICVM_FileHandle *i_getnativefh(icvm_fh fh){
		return i_getrootfs()->GetNativeFH(fh);
	};

	char *i_gettmpdir(void){
#ifdef UNIXSYSTEM
		return "/tmp";
#endif // UNIXSYSTEM
#ifdef __WIN32__
		static icvm_pathbuf buf;
		GetTempPath(sizeof(buf)-1,buf);
		return buf;
#endif // __WIN32__
	};

	s32 i_umask(s32 um){
		s32 oldumask=ICVM_BaseHandler::GetCurUMask();
		ICVM_BaseHandler::SetCurUMask(um);
		return oldumask;
	};


	s32 i_getumask(void){
		return ICVM_BaseHandler::GetCurUMask();
	};

	s32 i_initfilesystem(void){
#ifdef __WIN32__
		if(i_thisisfork) // already running
			return 0;
#endif // __WIN32__

		ICVM_BaseHandler::SetCurUMask(022);
		ICVM_BaseHandler::initfilesystem();
		return 0;
	};


	s32 i_read(icvm_fh fh,void *buf,u32 count){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->read(h,buf,count);
		return -1;
	};

	s32 i_write(icvm_fh fh,void *buf,u32 count){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->write(h,buf,count);
		return -1;
	};

	s64 i_lseek(icvm_fh fh,s64 offs,s32 whence){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->lseek(h,offs,whence);
		return -1;
	};

	s32 i_isatty(icvm_fh fh){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->isatty(h);
		return -1;
	};

	s32 i_ftruncate(icvm_fh fh,u64 offs){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->ftruncate(h,offs);
		return -1;
	};


	s32 i_truncate(char *path,u64 offs){
		icvm_fh fh=i_open(path,ICVM_O_WRONLY,0);
		s32 err;
		s32 status;
		if(fh<0)
			return -1;
		status=i_ftruncate(fh,offs);
		err=i_geterr();
		i_close(fh);
		i_seterr(err);
		return status;
	};


	s32 i_fstat(icvm_fh fh,struct ICVM_stat *st){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->fstat(h,st);
		return -1;
	};		

	s32  i_stat(char *path, struct ICVM_stat *st){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->stat(path,st);
		return -1;
	};

	s32  i_lstat(char *path, struct ICVM_stat *st){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->lstat(path,st);
		return -1;
	};


	s32  i_unlink(char *path){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->unlink(path);
		return -1;
	};

	s32 i_pipe(icvm_fh fh[2]){
		return ICVM_BaseHandler::pipe(fh);
	};

	s32  i_rmdir(char *path){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->rmdir(path);
		return -1;
	};

	s32  i_mkdir(char *path,s32 mode){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->mkdir(path,mode);
		return -1;
	};

	s32  i_rename(char *from,char *to){
		ICVM_BaseHandler *h=i_getfilehandler(from);
		if(h)
			return h->rename(from,to);
		return -1;
	};

	s32  i_link(char *from,char *to){
		ICVM_BaseHandler *h=i_getfilehandler(from);
		if(h)
			return h->link(from,to);
		return -1;
	};

	s32  i_symlink(char *from,char *to){
		ICVM_BaseHandler *h=i_getfilehandler(from);
		if(h)
			return h->symlink(from,to);
		return -1;
	};


	s32 i_close(icvm_fh fh){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->close(h);
		return -1;
	};

	s32  i_open(char *path,s32 openmode,s32 filemode){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->open(path,openmode,filemode);
		return -1;
	};


	s32 i_dup(icvm_fh fh){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->dup(h);
		return -1;
	};

	s32 i_dup2(icvm_fh fh1,icvm_fh fh2){
		ICVM_FileHandle *h=i_getnativefh(fh1);
		if(h)
			return h->Handler()->dup2(h,fh2);
		return -1;
	};

	s32  i_chown(char *path,s32 uid,s32 gid){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->chown(path,uid,gid);
		return -1;
	};


	s32  i_fchown(icvm_fh fh,s32 uid,s32 gid){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->fchown(h,uid,gid);
		return -1;
	};


	s32  i_chmod(char *path,s32 mode){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->chmod(path,mode);
		return -1;
	};



	s32  i_fchmod(icvm_fh fh,s32 mode){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->fchmod(h,mode);
		return -1;
	};


	s32  i_access(char *path,s32 amode){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->access(path,amode);
		return -1;
	};




	s32  i_chdir(char *path){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->chdir(path);
		return -1;
	};


	s32  i_fchdir(icvm_fh fh){
		ICVM_FileHandle *h=i_getnativefh(fh);
		if(h)
			return h->Handler()->fchdir(h);
		return -1;
	};

	char  *i_getcwd(char *buf, s32 sz){
		return ICVM_BaseHandler::getcwd(buf,sz);
	};


	ICVM_DIR  *i_opendir(char *path){
		ICVM_BaseHandler *h=i_getfilehandler(path);
		if(h)
			return h->opendir(path);
		return 0;
	};

	s32  i_closedir(ICVM_DIR *d){
		ICVM_FileHandle *h=i_getnativefh(d->fd);
		if(h)
			return h->Handler()->closedir(d);
		return -1;
	};

	void  i_rewinddir(ICVM_DIR *d){
		ICVM_FileHandle *h=i_getnativefh(d->fd);
		if(h)
			h->Handler()->rewinddir(d);
	};

	struct ICVM_dirent  *i_readdir(ICVM_DIR *d){
		ICVM_FileHandle *h=i_getnativefh(d->fd);
		if(h)
			return h->Handler()->readdir(d);
		return 0;
	};


	s32 i_telldir(ICVM_DIR *d){
		ICVM_FileHandle *h=i_getnativefh(d->fd);
		if(h)
			return h->Handler()->telldir(d);
		return -1;
	};

	void i_seekdir(ICVM_DIR *d,s32 offset){
		ICVM_FileHandle *h=i_getnativefh(d->fd);
		if(h)
			h->Handler()->seekdir(d,offset);
	};



};





#ifdef UNIXSYSTEM


s32 ICVM_BaseHandler::TranslateModeFromICVM(s32 inmode){

	s32 mode=0;
	if(inmode &  ICVM_S_IRUSR)
		mode |= S_IRUSR;
	if(inmode &  ICVM_S_IWUSR)
		mode |= S_IWUSR;
	if(inmode &  ICVM_S_IXUSR)
		mode |= S_IXUSR;
	if(inmode &  ICVM_S_IRGRP)
		mode |= S_IRGRP;
	if(inmode &  ICVM_S_IWGRP)
		mode |= S_IWGRP;
	if(inmode &  ICVM_S_IXGRP)
		mode |= S_IXGRP;
	if(inmode &  ICVM_S_IROTH)
		mode |= S_IROTH;
	if(inmode &  ICVM_S_IWOTH)
		mode |= S_IWOTH;
	if(inmode &  ICVM_S_IXOTH)
		mode |= S_IXOTH;
	if(ICVM_S_ISDIR(inmode))
		mode |= S_IFDIR;
	if(ICVM_S_ISCHR(inmode))
		mode |= S_IFCHR;
	if(ICVM_S_ISBLK(inmode))
		mode |= S_IFBLK;
	if(ICVM_S_ISREG(inmode))
		mode |= S_IFREG;
	if(ICVM_S_ISFIFO(inmode))
		mode |= S_IFIFO;
	if(ICVM_S_ISLNK(inmode))
		mode |= S_IFLNK;
	if(ICVM_S_ISSOCK(inmode))
		mode |= S_IFSOCK;

	return mode;
};



s32 ICVM_BaseHandler::TranslateModeToICVM(s32 inmode){
	s32 mode=0;
	if(inmode &  S_IRUSR)
		mode |= ICVM_S_IRUSR;
	if(inmode &  S_IWUSR)
		mode |= ICVM_S_IWUSR;
	if(inmode &  S_IXUSR)
		mode |= ICVM_S_IXUSR;
	if(inmode &  S_IRGRP)
		mode |= ICVM_S_IRGRP;
	if(inmode &  S_IWGRP)
		mode |= ICVM_S_IWGRP;
	if(inmode &  S_IXGRP)
		mode |= ICVM_S_IXGRP;
	if(inmode &  S_IROTH)
		mode |= ICVM_S_IROTH;
	if(inmode &  S_IWOTH)
		mode |= ICVM_S_IWOTH;
	if(inmode &  S_IXOTH)
		mode |= ICVM_S_IXOTH;
	if(S_ISDIR(inmode))
		mode |= ICVM_S_IFDIR;
	if(S_ISCHR(inmode))
		mode |= ICVM_S_IFCHR;
	if(S_ISBLK(inmode))
		mode |= ICVM_S_IFBLK;
	if(S_ISREG(inmode))
		mode |= ICVM_S_IFREG;
	if(S_ISFIFO(inmode))
		mode |= ICVM_S_IFIFO;
	if(S_ISLNK(inmode))
		mode |= ICVM_S_IFLNK;
	if(S_ISSOCK(inmode))
		mode |= ICVM_S_IFSOCK;
	return mode;

};

s32 ICVM_BaseHandler::TranslateOpenFlagsFromICVM(s32 icvmflags){
	s32 flags=0;
	if((icvmflags & ICVM_O_RDONLY)==ICVM_O_RDONLY)
		flags |= O_RDONLY;
	if(icvmflags & ICVM_O_WRONLY)
		flags |= O_WRONLY;
	if(icvmflags & ICVM_O_RDWR)
		flags |= O_RDWR;
	if(icvmflags & ICVM_O_NONBLOCK)
		flags |= O_NONBLOCK;
	if(icvmflags & ICVM_O_APPEND)
		flags |= O_APPEND;
#ifdef O_ASYNC
	if(icvmflags & ICVM_O_ASYNC)
		flags |= O_ASYNC;
#endif
#ifdef O_FSYNC
	if(icvmflags & ICVM_O_FSYNC)
		flags |= O_FSYNC;
#endif
	if(icvmflags & ICVM_O_CREAT)
		flags |= O_CREAT;
	if(icvmflags & ICVM_O_TRUNC)
		flags |= O_TRUNC;
	if(icvmflags & ICVM_O_EXCL)
		flags |= O_EXCL;
	return flags;
}


#if defined(__svr4__) || defined(mklinux)
int ICVM_BaseHandler::TranslateDTToICVM(DIR *dirptr){
	struct stat st;
	int status;
	mode_t md;
	status=fstat(dirfd(dirptr),&st);
	if(status<0)
		return ICVM_DT_UNKNOWN;
	md=st.st_mode;
	if(S_ISFIFO(md))
		return ICVM_DT_FIFO;
	if(S_ISCHR(md))
		return ICVM_DT_CHR;
	if(S_ISDIR(md))
		return ICVM_DT_DIR;
	if(S_ISBLK(md))
		return ICVM_DT_BLK;
	if(S_ISLNK(md))
		return ICVM_DT_LNK;
	if(S_ISSOCK(md))
		return ICVM_DT_SOCK;
	if(S_ISREG(md))
		return ICVM_DT_REG;
	return ICVM_DT_UNKNOWN;
};


#else

int ICVM_BaseHandler::TranslateDTToICVM(int in){
	switch(in){
		case DT_UNKNOWN:
			return ICVM_DT_UNKNOWN;
		case DT_FIFO:
			return ICVM_DT_FIFO;
		case DT_CHR:
			return ICVM_DT_CHR;
		case DT_DIR:
			return ICVM_DT_DIR;
		case DT_BLK:
			return ICVM_DT_BLK;
		case DT_REG:
			return ICVM_DT_REG;
		case DT_LNK:
			return ICVM_DT_LNK;
	}				
	return ICVM_DT_UNKNOWN;
};

#endif

#endif // UNIXSYSTEM


#ifdef __WIN32__
s32 ICVM_BaseHandler::TranslateFileAttributesToICVM(DWORD inmode,int typ){

	s32 mode=ICVM_S_IRUSR | ICVM_S_IRGRP | ICVM_S_IROTH |
			 ICVM_S_IWUSR;

	if(inmode & FILE_ATTRIBUTE_COMPRESSED) 
		mode |= ICVM_S_IXUSR | ICVM_S_IXGRP | ICVM_S_IXOTH;

	if(inmode & FILE_ATTRIBUTE_READONLY)
		mode &= ~(ICVM_S_IWUSR | ICVM_S_IWGRP | ICVM_S_IWOTH);

	if(inmode & FILE_ATTRIBUTE_HIDDEN)
		mode &= ~(ICVM_S_IRUSR | ICVM_S_IRGRP | ICVM_S_IROTH);

	if(inmode & FILE_ATTRIBUTE_DIRECTORY)
		mode |= ICVM_S_IFDIR;

	switch(typ){
		case FILE_TYPE_CHAR:
			mode |= ICVM_S_IFCHR;
			break;

		case FILE_TYPE_DISK:
			mode |= ICVM_S_IFREG;
			break;

		case FILE_TYPE_PIPE:
			mode |= ICVM_S_IFIFO;
			break;
#ifdef NOT_NOW
		if(S_ISBLK(inmode))
			mode |= ICVM_S_IFBLK;
		if(S_ISREG(inmode))
			mode |= ICVM_S_IFREG;
		if(S_ISFIFO(inmode))
			mode |= ICVM_S_IFIFO;
		if(S_ISLNK(inmode))
			mode |= ICVM_S_IFLNK;
		if(S_ISSOCK(inmode))
			mode |= ICVM_S_IFSOCK;
#endif

	};


	return mode;

};


DWORD ICVM_BaseHandler::TranslateFileAttributesFromICVM(s32 inmode, int *typ){

	DWORD mode=FILE_ATTRIBUTE_NORMAL;

	if((inmode & (ICVM_S_IWUSR | ICVM_S_IWGRP | ICVM_S_IWOTH)) ==0)
		mode|=FILE_ATTRIBUTE_READONLY;

	if((inmode & (ICVM_S_IRUSR | ICVM_S_IRGRP | ICVM_S_IROTH)) ==0)
		mode|=FILE_ATTRIBUTE_HIDDEN;

	if((inmode & (ICVM_S_IXUSR | ICVM_S_IXGRP | ICVM_S_IXOTH)) ==0)
		mode|=FILE_ATTRIBUTE_COMPRESSED;

	if(ICVM_S_ISDIR(inmode))
		mode|=FILE_ATTRIBUTE_DIRECTORY;

	if(mode & ~FILE_ATTRIBUTE_NORMAL)
		mode &= FILE_ATTRIBUTE_NORMAL;

	if(typ){
		*typ=FILE_TYPE_UNKNOWN;

		if(ICVM_S_ISCHR(inmode))
			*typ=FILE_TYPE_CHAR;

		if(ICVM_S_ISREG(inmode))
			*typ=FILE_TYPE_DISK;

		if(ICVM_S_ISFIFO(inmode))
			*typ=FILE_TYPE_PIPE;

#ifdef NOT_NOW

	if(ICVM_S_ISBLK(inmode))
		mode |= S_IFBLK;
	if(ICVM_S_ISLNK(inmode))
		mode |= S_IFLNK;
	if(ICVM_S_ISSOCK(inmode))
		mode |= S_IFSOCK;
#endif
	}
	return mode;
};

#endif // __WIN32__


#ifdef NOT_NOW


// this is the base filesystem init call
// mostly gets initial stdin/stdout/stderr running

void ICVM_BaseHandler::InitFS(int stdinfh,int stdoutfh,int stderrfh){
// THESE CALLS MUST BE VERY FIRST OPENS MADE
	GetRootDir()->open("/dev/stdin",O_RDONLY);
	GetRootDir()->open("/dev/stdout",O_WRONLY);
	GetRootDir()->open("/dev/stderr",O_WRONLY);
#ifdef NOT_NOW
	ICVM_FileHandle *fhp;
	fhp=GetRootDir()->AllocFHPtr(0);
	fhp->fh=stdinfh;
	fhp->accessflags = ICVM_O_ACCESSREAD;
	fhp=GetRootDir()->AllocFHPtr(1);
	fhp->fh=stdoutfh;
	fhp->accessflags = ICVM_O_ACCESSWRITE;
	fhp=GetRootDir()->AllocFHPtr(2);
	fhp->fh=stderrfh;
	fhp->accessflags = ICVM_O_ACCESSWRITE;
#endif
};



ICVM_BaseHandler 	*ICVM_BaseHandler::FindHandler(char *path){
	return GetRootDir();
};

																	
ICVM_FileHandle 	*ICVM_BaseHandler::NewFHPtr(int tthisfh){
	return new ICVM_FileHandle(tthisfh,this);
};

void ICVM_BaseHandler::FreeFHPtr(ICVM_FileHandle *fhp){
	filehandles[fhp->thisfh]=0;
	delete fhp;
};


int  ICVM_BaseHandler::FindFreeFHPtr(int wantfh){
	int cnt=0;
	if(wantfh>=0){
		if(wantfh>=ICVM_MAXFILEHANDLES || filehandles[wantfh]){
			icvmerrno=ICVM_EBADF;
			return -1;
		}
		cnt=wantfh;
	}else{
		while(cnt<ICVM_MAXFILEHANDLES && filehandles[cnt])
			++cnt;
		if(cnt>=ICVM_MAXFILEHANDLES){
			icvmerrno=ICVM_EMFILE;
			return -1;
		}
	}
	return cnt;
}

ICVM_FileHandle  *ICVM_BaseHandler::AllocFHPtr(int wantfh){
	ICVM_FileHandle *fhp;
	int fh=FindFreeFHPtr(wantfh);
	if(fh<0)
		return 0;
	fhp=NewFHPtr(fh);
	if(fhp==0)
		return 0;
	filehandles[fh]=fhp;
	return fhp;
};


ICVM_FileHandle 	*ICVM_BaseHandler::GetFHPtr(int fh){
	if(fh<0 || fh>=ICVM_MAXFILEHANDLES){
		icvmerrno=ICVM_EBADF;
		return 0;
	}
	if(filehandles[fh]==0)
		icvmerrno=ICVM_EBADF;
	return filehandles[fh];
}


int ICVM_FileHandle::close(void){
	return handler->do_close(this);
};

int ICVM_FileHandle::fstat(struct ICVM_stat *ist){
	return handler->do_stat(this,ist);
};


ssize_t ICVM_FileHandle::read(void *buf,size_t count){
	int status;
	if((accessflags & ICVM_O_ACCESSREAD)==0){
		icvmerrno=ICVM_EPERM;
		return -1;
	}
	return handler->do_read(this,buf,count);
}

ssize_t ICVM_FileHandle::write(void *buf,size_t count){
	int status;
	if((accessflags & ICVM_O_ACCESSWRITE)==0){
		icvmerrno=ICVM_EPERM;
		return -1;
	}
	return handler->do_write(this,buf,count);
}


off_t ICVM_FileHandle::lseek(off_t offset,int whence){
	return handler->do_lseek(this,offset,whence);
}

int ICVM_FileHandle::ftruncate(off_t length){
	if((accessflags & ICVM_O_ACCESSWRITE)==0){
		icvmerrno=ICVM_EPERM;
		return -1;
	}
	return handler->do_ftruncate(this,length);
}


int ICVM_FileHandle::dup(ICVM_FileHandle *newfh){
	newfh->DupFrom(this);
	return handler->do_dup(this,newfh);
}




extern "C" {

char *icvm_getcurwd(void){ return icvmcurcontext->getcwd();}; // get current working directory


int icvmerrno;

// C links into ICVM filesystem. These calls may be used freely by
// C and C++ routines to implement functions, just as their 
// C libary counterparts would be.

void icvm_initfilesystem(int stdinfh,int stdoutfh,int stderrfh){
	ICVM_BaseHandler::InitFS(stdinfh,stdoutfh,stderrfh);
}


int icvm_close(int fh){
	return GetRootDir()->close(fh);
}

ssize_t icvm_read(int fh,void *buf,size_t count){
	return GetRootDir()->read(fh,buf,count);
};

ssize_t icvm_write(int fh,void *buf,size_t count){
	return GetRootDir()->write(fh,buf,count);
};


int icvm_dup(int fh){
	return GetRootDir()->dup(fh);
}

int icvm_dup2(int fh,int wantfh){
	return GetRootDir()->dup2(fh,wantfh);
}


off_t icvm_lseek(int fh,off_t offset,int whence){
	return GetRootDir()->lseek(fh,offset,whence);
};


int icvm_ftruncate(int fh,off_t length){
	return GetRootDir()->ftruncate(fh,length);
};

int icvm_fstat(int fh,struct ICVM_stat *st){
	return GetRootDir()->fstat(fh,st);
};

int icvm_stat(char *path,struct ICVM_stat *st){
	return GetRootDir()->stat(path,st);
};


int icvm_lstat(char *path,struct ICVM_stat *st){
	return GetRootDir()->lstat(path,st);
};


int icvm_truncate(char *name,off_t length){
	int fh=icvm_open(name,ICVM_O_RDWR);
	int status;
	if(fh<0)
		return fh;
	status=icvm_ftruncate(fh,length);
	icvm_close(fh);
	return status;
};


int icvm_creat(char *filename,mode_t mode){
	return ICVM_FINDFSH(filename)->creat(filename,mode);
};

int icvm_open(char *filename,int flags,...){
	mode_t *mp=(mode_t *)&flags;
	++mp;
	if(flags & ICVM_O_CREAT)
		return ICVM_FINDFSH(filename)->open(filename,flags,*mp);
	else
		return ICVM_FINDFSH(filename)->open(filename,flags);
};



// ICVM offset access to stat buffer

#define ICVM_ist_dev		0
#define ICVM_ist_mode		4
#define ICVM_ist_uid		8
#define ICVM_ist_gid		12
#define ICVM_ist_rdev		16
#define ICVM_ist_nlink		20
#define ICVM_ist_size		24
#define ICVM_ist_ino		32
#define ICVM_ist_blksize	36
#define ICVM_ist_atime		40
#define ICVM_ist_atimensec	44
#define ICVM_ist_mtime		48
#define ICVM_ist_mtimensec	52
#define ICVM_ist_ctime		56
#define ICVM_ist_ctimensec	60
#define ICVM_ist_blocks		64
#define ICVM_ist_flags		72
#define ICVM_ist_gen		76



#ifdef __svr4__
#define d_fileno d_ino
#define dirfd(x) ((x)->dd_fd)
#endif


// icvm dirent offsets

#define	ICVM_d_fileno	0
#define	ICVM_d_reclen	4
#define	ICVM_d_type		8
#define ICVM_d_namlen	12
#define ICVM_d_name		16
#define ICVM_d_sizeof	(264+16)

typedef s8 struct_ICVM_dirent[ICVM_d_sizeof];






// icvm d_type s

#define	ICVM_DT_UNKNOWN	 0
#define	ICVM_DT_FIFO	1
#define	ICVM_DT_CHR		 2
#define	ICVM_DT_DIR		 4
#define	ICVM_DT_BLK		 6
#define	ICVM_DT_REG		 8
#define	ICVM_DT_LNK		10
#define	ICVM_DT_SOCK	12
#define	ICVM_DT_WHT		14

#ifdef __svr4__
static int TranslateDTToICVM(DIR *dirptr){
	struct stat st;
	int status;
	mode_t md;
	status=fstat(dirfd(dirptr),&st);
	if(status<0)
		return ICVM_DT_UNKNOWN;
	md=st.st_mode;
	if(S_ISFIFO(md))
		return ICVM_DT_FIFO;
	if(S_ISCHR(md))
		return ICVM_DT_CHR;
	if(S_ISDIR(md))
		return ICVM_DT_DIR;
	if(S_ISBLK(md))
		return ICVM_DT_BLK;
	if(S_ISLNK(md))
		return ICVM_DT_LNK;
	if(S_ISSOCK(md))
		return ICVM_DT_SOCK;
	if(S_ISREG(md))
		return ICVM_DT_REG;
	return ICVM_DT_UNKNOWN;
};


#else

static int TranslateDTToICVM(int in){
	switch(in){
		case DT_UNKNOWN:
			return ICVM_DT_UNKNOWN;
		case DT_FIFO:
			return ICVM_DT_FIFO;
		case DT_CHR:
			return ICVM_DT_CHR;
		case DT_DIR:
			return ICVM_DT_DIR;
		case DT_BLK:
			return ICVM_DT_BLK;
		case DT_REG:
			return ICVM_DT_REG;
		case DT_LNK:
			return ICVM_DT_LNK;
	}				
	return ICVM_DT_UNKNOWN;
};

#endif



s32 TranslateOpenFlagsFromICVM(s32 icvmflags){
	s32 flags=0;
	if(icvmflags & ICVM_O_RDONLY)
		flags |= O_RDONLY;
	if(icvmflags & ICVM_O_WRONLY)
		flags |= O_WRONLY;
	if(icvmflags & ICVM_O_RDWR)
		flags |= O_RDWR;
	if(icvmflags & ICVM_O_NONBLOCK)
		flags |= O_NONBLOCK;
	if(icvmflags & ICVM_O_APPEND)
		flags |= O_APPEND;
#ifdef O_ASYNC
	if(icvmflags & ICVM_O_ASYNC)
		flags |= O_ASYNC;
#endif
#ifdef O_FSYNC
	if(icvmflags & ICVM_O_FSYNC)
		flags |= O_FSYNC;
#endif
	if(icvmflags & ICVM_O_CREAT)
		flags |= O_CREAT;
	if(icvmflags & ICVM_O_TRUNC)
		flags |= O_TRUNC;
	if(icvmflags & ICVM_O_EXCL)
		flags |= O_EXCL;
	return flags;
}


s32 TranslateOpenFlagsToICVM(s32 icvmflags){
	s32 flags=0;
	if(icvmflags & O_RDONLY)
		flags |= ICVM_O_RDONLY;
	if(icvmflags & O_WRONLY)
		flags |= ICVM_O_WRONLY;
	if(icvmflags & O_RDWR)
		flags |= ICVM_O_RDWR;
	if(icvmflags & O_NONBLOCK)
		flags |= ICVM_O_NONBLOCK;
	if(icvmflags & O_APPEND)
		flags |= ICVM_O_APPEND;
#ifdef O_ASYNC
	if(icvmflags & O_ASYNC)
		flags |= ICVM_O_ASYNC;
#endif
#ifdef O_FSYNC
	if(icvmflags & O_FSYNC)
		flags |= ICVM_O_FSYNC;
#endif
	if(icvmflags & O_CREAT)
		flags |= ICVM_O_CREAT;
	if(icvmflags & O_TRUNC)
		flags |= ICVM_O_TRUNC;
	if(icvmflags & O_EXCL)
		flags |= ICVM_O_EXCL;
	return flags;
}


void ICVM_read(s32 *stack){
	R_R0=icvm_read(
		GETDATAMEM_s32(&stack[0]),
		(void *)GETDATAMEM_u32(&stack[1]),
		GETDATAMEM_u32(&stack[2]));
	icvm_SetR1Err();
}


void ICVM_write(s32 *stack){
	R_R0=icvm_write(
		GETDATAMEM_s32(&stack[0]),
		(void *)GETDATAMEM_u32(&stack[1]),
		GETDATAMEM_u32(&stack[2]));
	icvm_SetR1Err();
}





void ICVM_creat(s32 *stack){
	R_R0=icvm_creat(
		(char *)GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_s32(&stack[1]));
	icvm_SetR1Err();
}

void ICVM_open(s32 *stack){
	R_R0=icvm_open(
		(char *)GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_s32(&stack[1]),
		GETDATAMEM_s32(&stack[2]));
	icvm_SetR1Err();
}



void ICVM_close(s32 *stack){
	R_R0=icvm_close(GETDATAMEM_s32(&stack[0]));
	icvm_SetR1Err();
}



void ICVM_lseek(s32 *stack){
	R_Q0=icvm_lseek(
		GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u64(&stack[1]),
		GETDATAMEM_s32(&stack[2]));
	icvm_SetR1Err();
}

void ICVM_ftruncate(s32 *stack){
	R_R0=icvm_ftruncate(
		GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u64(&stack[1]));
	icvm_SetR1Err();
}


void ICVM_chroot(s32 *stack){

// NOT SUPPORTED IN ICVM

	R_R0=-1;
	R_ERRNO1=ICVM_EPERM;
#ifdef NOT_NOW
	R_R0=chroot(
		(char *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
#endif


}


void ICVM_truncate(s32 *stack){
	R_R0=icvm_truncate(
		(char *)GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u64(&stack[1]));
	icvm_SetR1Err();
}




void ICVM_dup(s32 *stack){
	R_R0=icvm_dup(GETDATAMEM_s32(&stack[0]));
	icvm_SetR1Err();
}




void ICVM_dup2(s32 *stack){
	R_R0=icvm_dup2(GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_s32(&stack[1]));
	icvm_SetR1Err();
}


void ICVM_chmod(s32 *stack){
	s32 mode=TranslateModeFromICVM(GETDATAMEM_u32(&stack[1]));
	R_R0=chmod(
		(char *)GETDATAMEM_s32(&stack[0]),mode);
	icvm_SetErr();
}

void ICVM_fchmod(s32 *stack){
	s32 mode=TranslateModeFromICVM(GETDATAMEM_u32(&stack[1]));
	R_R0=fchmod(
		GETDATAMEM_s32(&stack[0]),mode);
	icvm_SetErr();
}

/* lock operations for icvm flock(2) */
#define	ICVM_LOCK_SH		0x01		/* shared file lock */
#define	ICVM_LOCK_EX		0x02		/* exclusive file lock */
#define	ICVM_LOCK_NB		0x04		/* don't block when locking */
#define	ICVM_LOCK_UN		0x08		/* unlock file */

void ICVM_flock(s32 *stack){
#ifdef NOT_NOW
#if defined(BSD) || defined(__FREEBSD__) || defined(__NetBSD__)\
		|| defined(linux) || defined(__OpenBSD__)	
	s32 mode=GETDATAMEM_u32(&stack[1]);
	switch(mode){
		case ICVM_LOCK_SH:
			mode=LOCK_SH;
			break;
			
		case ICVM_LOCK_EX:
			mode=LOCK_EX;
			break;
			
		case ICVM_LOCK_NB:
			mode=LOCK_NB;
			break;
			
		case ICVM_LOCK_UN:
			mode=LOCK_UN;
			break;
			
		default:
			R_ERRNO1=ICVM_EINVAL;
			return;
	}

	R_R0=flock(GETDATAMEM_s32(&stack[0]),mode);
#else
	R_R0=-1;
	errno=ICVM_EOPNOTSUPP;
#endif
	icvm_SetErr();
#endif
}

void ICVM_mkdir(s32 *stack){
	s32 mode=TranslateModeFromICVM(GETDATAMEM_u32(&stack[1]));
	R_R0=mkdir(
		(char *)GETDATAMEM_s32(&stack[0]),mode);
	icvm_SetErr();
}

void ICVM_umask(s32 *stack){
	s32 mode=TranslateModeFromICVM(GETDATAMEM_u32(&stack[0]));
	mode=umask(mode);
	R_R0=TranslateModeToICVM(mode);
	icvm_SetErr();
}


void ICVM_mkfifo(s32 *stack){
	s32 mode=TranslateModeFromICVM(GETDATAMEM_u32(&stack[1]));
	R_R0=mkfifo(
		(char *)GETDATAMEM_s32(&stack[0]),
		mode);
	icvm_SetErr();
}


void ICVM_link(s32 *stack){
	R_R0=link(
		(char *)GETDATAMEM_s32(&stack[0]),
		(char *)GETDATAMEM_u32(&stack[1]));
	icvm_SetErr();
}


void ICVM_symlink(s32 *stack){
	R_R0=symlink(
		(char *)GETDATAMEM_s32(&stack[0]),
		(char *)GETDATAMEM_u32(&stack[1]));
	icvm_SetErr();
}

void ICVM_readlink(s32 *stack){
	R_R0=readlink(
		(char *)GETDATAMEM_s32(&stack[0]),
		(char *)GETDATAMEM_u32(&stack[1]),
		GETDATAMEM_u32(&stack[2]));
	icvm_SetErr();
}

void ICVM_rename(s32 *stack){
	R_R0=rename(
		(char *)GETDATAMEM_s32(&stack[0]),
		(char *)GETDATAMEM_u32(&stack[1]));
	icvm_SetErr();
}

void ICVM_rmdir(s32 *stack){
	R_R0=rmdir((char *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}

void ICVM_isatty(s32 *stack){
	R_R0=isatty(GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}

void ICVM_chdir(s32 *stack){
	R_R0=chdir((char *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}

void ICVM_fchdir(s32 *stack){
	R_R0=fchdir(GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}

void ICVM_mknod(s32 *stack){
	R_R0=mknod(
		(char *)GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u32(&stack[1]),
		GETDATAMEM_u32(&stack[2]));
	icvm_SetErr();
}


void ICVM_getcwd(s32 *stack){
	R_R0=(p32)getcwd(
		(char *)GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u32(&stack[1]));
	icvm_SetErr();
}

void ICVM_chown(s32 *stack){
	R_R0=chown(
		(char *)GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u32(&stack[1]),
		GETDATAMEM_u32(&stack[2]));
	icvm_SetErr();
}

void ICVM_fchown(s32 *stack){
	R_R0=fchown(
		GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u32(&stack[1]),
		GETDATAMEM_u32(&stack[2]));
	icvm_SetErr();
}


void ICVM_mount(s32 *stack){
	R_R0=mount(
		(char *)GETDATAMEM_s32(&stack[0]),
		(char *)GETDATAMEM_u32(&stack[1]),
		(char *)GETDATAMEM_u32(&stack[2]),
		GETDATAMEM_u32(&stack[3]),
		(char *)GETDATAMEM_u32(&stack[4]));
	icvm_SetErr();
}

void ICVM_umount(s32 *stack){
	R_R0=umount((char *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}


void ICVM_access(s32 *stack){
	R_R0=access(
		(char *)GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_u32(&stack[1]));
	icvm_SetErr();
}

void ICVM_getdtablesize(s32 *stack){
	R_R0=getdtablesize();
	if(R_R0==-1)
		R_R0=256; // reasonable number for max open files
	icvm_SetErr();
}


void ICVM_pipe(s32 *stack){
	R_R0=pipe((int *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}



void ICVM_utimes(s32 *stack){
	struct timeval tv;
	s8 *itv=(s8 *)GETDATAMEM_s32(&stack[1]);
	TranslateTimeValFromICVM(itv,&tv);
	R_R0=utimes(
		(char *)GETDATAMEM_s32(&stack[0]),
		&tv);
	icvm_SetErr();
}



static void CopyStat(s8 *istat,struct ICVM_stat *st){
	PUTDATAMEM_s32(istat+ICVM_ist_dev,st->st_dev);
	PUTDATAMEM_s32(istat+ICVM_ist_mode,st->st_mode);
	PUTDATAMEM_u32(istat+ICVM_ist_uid,st->st_uid);
	PUTDATAMEM_u32(istat+ICVM_ist_gid,st->st_gid);
	PUTDATAMEM_s32(istat+ICVM_ist_rdev,st->st_rdev);
	PUTDATAMEM_u32(istat+ICVM_ist_nlink,st->st_nlink);
	PUTDATAMEM_u32(istat+ICVM_ist_ino,st->st_ino);
	PUTDATAMEM_u64(istat+ICVM_ist_size,st->st_size);
	PUTDATAMEM_u32(istat+ICVM_ist_atime,st->st_atime);
	PUTDATAMEM_u32(istat+ICVM_ist_mtime,st->st_mtime);
	PUTDATAMEM_u32(istat+ICVM_ist_ctime,st->st_ctime);
	PUTDATAMEM_u32(istat+ICVM_ist_atimensec,0);
	PUTDATAMEM_u32(istat+ICVM_ist_atimensec,st->st_atimensec);
	PUTDATAMEM_u32(istat+ICVM_ist_mtimensec,st->st_mtimensec);
	PUTDATAMEM_u32(istat+ICVM_ist_ctimensec,st->st_ctimensec);
	PUTDATAMEM_u32(istat+ICVM_ist_blksize,st->st_blksize);
	PUTDATAMEM_u64(istat+ICVM_ist_blocks,st->st_blocks);
	PUTDATAMEM_u32(istat+ICVM_ist_flags,0);

}

void ICVM_fstat(s32 *stack){
	struct ICVM_stat st;
	s8 *istat;
	if(!icvm_fstat(GETDATAMEM_s32(&stack[0]),&st)){
		R_R0=0;
		istat=(s8 *)GETDATAMEM_s32(&stack[1]);
		CopyStat(istat,&st);
	}else
		R_R0=-1;
	icvm_SetErr();
}

void ICVM_stat(s32 *stack){
	struct ICVM_stat st;
	s8 *istat;
	if(!icvm_stat((char *)GETDATAMEM_s32(&stack[0]),&st)){
		R_R0=0;
		istat=(s8 *)GETDATAMEM_s32(&stack[1]);
		CopyStat(istat,&st);
	}else
		R_R0=-1;
	icvm_SetErr();
}

void ICVM_lstat(s32 *stack){
	struct ICVM_stat st;
	s8 *istat;
	if(!icvm_lstat((char *)GETDATAMEM_s32(&stack[0]),&st)){
		R_R0=0;
		istat=(s8 *)GETDATAMEM_s32(&stack[1]);
		CopyStat(istat,&st);
	}else
		R_R0=-1;
	icvm_SetErr();
}



void ICVM_opendir(s32 *stack){
	R_R0=(p32)opendir((char *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}

void ICVM_closedir(s32 *stack){
	R_R0=closedir((DIR *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}


void ICVM_dirfd(s32 *stack){
	R_R0=dirfd((DIR *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}



void ICVM_readdir(s32 *stack){
	static struct_ICVM_dirent _di;
	s8 *di=(s8 *)&_di;
	struct dirent *d;
	DIR *dirptr=(DIR *)GETDATAMEM_s32(&stack[0]);
	d=readdir(dirptr);
	if(d){
		strncpy(di+ICVM_d_name,d->d_name,255);
		PUTDATAMEM_u32(di+ICVM_d_fileno,d->d_fileno);
#ifdef __svr4__
		PUTDATAMEM_u32(di+ICVM_d_type,
			TranslateDTToICVM(dirptr)
		);

#else
		PUTDATAMEM_u32(di+ICVM_d_type,
			TranslateDTToICVM(d->d_type)
		);
#endif
		PUTDATAMEM_u32(di+ICVM_d_reclen,sizeof(di));
		PUTDATAMEM_u32(di+ICVM_d_namlen,strlen(di+ICVM_d_name));
		R_R0=(p32)di;
	}else
		R_R0=0;
	icvm_SetErr();
}


void ICVM_rewinddir(s32 *stack){
	rewinddir((DIR *)GETDATAMEM_s32(&stack[0]));
	icvm_SetErr();
}


// tacky, really bad, ioctl call just passes everything through

void ICVM_ioctl(s32 *stack){
	R_R0=ioctl(GETDATAMEM_s32(&stack[0]),
		GETDATAMEM_s32(&stack[1]),
		GETDATAMEM_s32(&stack[2]),
		GETDATAMEM_s32(&stack[3]),
		GETDATAMEM_s32(&stack[4]),
		GETDATAMEM_s32(&stack[5]),
		GETDATAMEM_s32(&stack[6]),
		GETDATAMEM_s32(&stack[7])
	);
	icvm_SetErr();
}


// icvm fcntl stuff

#define	ICVM_F_DUPFD		0		/* duplicate file descriptor */
#define	ICVM_F_GETFD		1		/* get file descriptor flags */
#define	ICVM_F_SETFD		2		/* set file descriptor flags */
#define	ICVM_F_GETFL		3		/* get file status flags */
#define	ICVM_F_SETFL		4		/* set file status flags */
#define	ICVM_F_GETOWN		5		/* get SIGIO/SIGURG proc/pgrp */
#define ICVM_F_SETOWN		6		/* set SIGIO/SIGURG proc/pgrp */
#define	ICVM_F_GETLK		7		/* get record locking information */
#define	ICVM_F_SETLK		8		/* set record locking information */
#define	ICVM_F_SETLKW		9		/* F_SETLK; wait if blocked */

/* file descriptor flags (F_GETFD, F_SETFD) */
#define	ICVM_FD_CLOEXEC	1		/* close-on-exec flag */

/* record locking flags (F_GETLK, F_SETLK, F_SETLKW) */
#define	ICVM_F_RDLCK		1		/* shared or read lock */
#define	ICVM_F_UNLCK		2		/* unlock */
#define	ICVM_F_WRLCK		3		/* exclusive or write lock */


// icvm flock offsets
#define ICVM_l_start	0
#define ICVM_l_len		8
#define ICVM_l_pid		16
#define ICVM_l_type		20
#define ICVM_l_whence	22
#define ICVM_l_sizeof	24

#ifdef NOT_NOW
struct flock {
	off_t	l_start;	/* starting offset */
	off_t	l_len;		/* len = 0 means until end of file */
	pid_t	l_pid;		/* lock owner */
	short	l_type;		/* lock type: read/write, etc. */
	short	l_whence;	/* type of l_start */
};


#endif


void ICVM_fcntl(s32 *stack){
	int filedes=GETDATAMEM_s32(&stack[0]);
	int cmd=GETDATAMEM_s32(&stack[1]);
	int arg1=GETDATAMEM_s32(&stack[2]);
	int type;
	int whence;
	struct flock lock;
	switch(cmd){
		case ICVM_F_DUPFD:
			R_R0=fcntl(filedes,F_DUPFD,arg1);
			break;
		case ICVM_F_GETFD:
			R_R0=fcntl(filedes,F_GETFD)!=0;
			break;
		case ICVM_F_SETFD:
			R_R0=fcntl(filedes,F_SETFD,arg1 ? FD_CLOEXEC : 0);
			break;
		case ICVM_F_GETFL:
			R_R0=TranslateOpenFlagsToICVM(fcntl(filedes,F_GETFL,arg1));
			break;
		case ICVM_F_SETFL:
			arg1=TranslateOpenFlagsFromICVM(arg1);
			R_R0=fcntl(filedes,F_SETFL,arg1);
			break;
		case ICVM_F_GETOWN:
			R_R0=fcntl(filedes,F_GETOWN);
			break;
		case ICVM_F_SETOWN:
			R_R0=fcntl(filedes,F_SETOWN,arg1);
			break;
		case ICVM_F_GETLK:
			R_R0=fcntl(filedes,F_GETLK,&lock);
			if(R_R0==-1)
				goto err;
			type=lock.l_type;
			if(type==F_RDLCK)
				type=ICVM_F_RDLCK;
			else 
				type=ICVM_F_WRLCK;
			if(type==F_UNLCK)
				type=ICVM_F_UNLCK;
			PUTDATAMEM_s16(arg1+ICVM_l_type,type);
			if(type!=ICVM_F_UNLCK)
				break;
			PUTDATAMEM_u64(arg1+ICVM_l_start,lock.l_start);
			PUTDATAMEM_u64(arg1+ICVM_l_len,lock.l_len);
			PUTDATAMEM_u32(arg1+ICVM_l_pid,lock.l_pid);
			whence=lock.l_whence;
			if(whence==SEEK_END)
				whence=ICVM_SEEK_END;
			else if(whence==SEEK_CUR)
				whence=ICVM_SEEK_CUR;
			else whence=ICVM_SEEK_SET;
			PUTDATAMEM_s16(arg1+ICVM_l_whence,whence);
			break;
		case ICVM_F_SETLK:
			cmd=F_SETLK;
			goto finishsetlk;
		case ICVM_F_SETLKW:
			cmd=F_SETLKW;
finishsetlk:
			lock.l_start=GETDATAMEM_u64(arg1+ICVM_l_start);
			lock.l_len=GETDATAMEM_u64(arg1+ICVM_l_len);
			lock.l_pid=GETDATAMEM_s32(arg1+ICVM_l_pid);
			type=GETDATAMEM_s16(arg1+ICVM_l_type);
			if(type==ICVM_F_RDLCK)
				type=F_RDLCK;
			else if(type==ICVM_F_WRLCK)
				type=F_WRLCK;
			else if(type==ICVM_F_UNLCK)
				type=F_UNLCK;
			else
				goto inval;
			lock.l_type=type;
			whence=GETDATAMEM_s16(arg1+ICVM_l_whence);
			if(whence==ICVM_SEEK_END)
				whence=SEEK_END;
			else if(whence==ICVM_SEEK_CUR)
				whence=SEEK_CUR;
			else if(whence==ICVM_SEEK_SET)
				whence=SEEK_SET;
			else{
inval:
				errno=EINVAL;
				goto err;
			}
			lock.l_whence=whence;
			R_R0=fcntl(filedes,cmd,&lock);
			break;

		default:
			errno=ENOSYS;
err:
			R_R0=-1;
	}
	icvm_SetErr();
}


}


#endif

