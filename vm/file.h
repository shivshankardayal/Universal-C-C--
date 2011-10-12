#ifndef ICVM_FILE_H
#define ICVM_FILE_H

#include "sys.h"
#include "id.h"

#include "icvmerr.h"

// Copyright (C) 2000,2001 Bob Dayley


#ifdef __WIN32__

#include <windows.h>


#endif __WIN32__

#ifdef UNIXSYSTEM

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/mount.h>

#endif // UNIXSYSTEM


#ifdef __cplusplus

class ICVM_BaseHandler;

extern ICVM_BaseHandler *basehandlers[];

#else

typedef void *ICVM_BaseHandler;

#endif


#ifdef OPEN_MAX
#define ICVM_MAXFILEHANDLES OPEN_MAX
#else
#define ICVM_MAXFILEHANDLES	256
#endif

typedef s32 icvm_fh; // ICVM file handle type

#ifdef __WIN32__
typedef HANDLE native_fh; // native file handle type
#endif // __WIN32__

#ifdef UNIXSYSTEM
typedef s32 native_fh; // native file handle type
#endif // UNIXSYSTEM

typedef char icvm_pathbuf[1024];

typedef struct ICVM_FileHandle{
#ifdef __cplusplus
private:
#endif
	native_fh  fh; //  filehandle number  - OS side
	int allocated;
	s32 accessflags; // flags this file was opened for
//	ICVM_BaseHandler *handler;
	s32 handlernum;

	char *pathname;

#ifdef __cplusplus
public:
	
	inline void Reset(void){
		handlernum=-1;
		allocated=0;
		accessflags=0;
		if(pathname)
			i_free(pathname);
		pathname=0;
#ifdef UNIXSYSTEM
		fh=-1;
#endif // UNIXSYSTEM
#ifdef __WIN32__
		fh=INVALID_HANDLE_VALUE;
#endif // __WIN32__

	};
	inline int IsFree(void){return allocated==0;};
	inline native_fh FH(void){return fh;};
	inline ICVM_BaseHandler *Handler(void){
		if(handlernum>=0)
			return basehandlers[handlernum];
		return 0;
	};
	inline s32 HandlerNum(void){return handlernum;};
	inline s32 AccessFlags(void){return accessflags;};
	inline char *PathName(void){
		if(pathname)
			return pathname;
		return EMPTYSTRING;
	};
	inline void Allocate(void){allocated=1;};
	inline void Init(native_fh ifh,
			s32 ihandlernum,
			s32 iaccessflags,
			char *ipathname
		){
		Allocate();
		fh=ifh;
		accessflags=iaccessflags;
		handlernum=ihandlernum;
		if(ipathname && strlen(ipathname)){
			pathname=(char *)i_malloc(strlen(ipathname)+1);
			strcpy(pathname,ipathname);
		}
	};



#endif // __cplusplus




} ICVM_FileHandle;


// this is kept in a centralized area

typedef struct filesystemstuff{
	ICVM_FileHandle filehandles[ICVM_MAXFILEHANDLES];
	s32 curumask;
}systemfilestuff;



// icvm open and create mode definitions

#define	ICVM_O_RDONLY	0x0000		/* open for reading only */
#define	ICVM_O_WRONLY	0x0001		/* open for writing only */
#define	ICVM_O_RDWR		0x0002		/* open for reading and writing */
#define	ICVM_O_NONBLOCK	0x0004		/* no delay */
#define	ICVM_O_APPEND	0x0008		/* set append mode */
#define	ICVM_O_ASYNC	0x0040		/* signal pgrp when data ready */
#define	ICVM_O_FSYNC	0x0080		/* synchronous writes */
#define	ICVM_O_CREAT	0x0200		/* create if nonexistant */
#define	ICVM_O_TRUNC	0x0400		/* truncate to zero length */
#define	ICVM_O_EXCL		0x0800		/* error if already exists */




// ICVM file mode definitions
#define	ICVM_S_IRWXU	0000700			/* RWX mask for owner */
#define	ICVM_S_IRUSR	0000400			/* R for owner */
#define	ICVM_S_IWUSR	0000200			/* W for owner */
#define	ICVM_S_IXUSR	0000100			/* X for owner */

#define	ICVM_S_IRWXG	0000070			/* RWX mask for group */
#define	ICVM_S_IRGRP	0000040			/* R for group */
#define	ICVM_S_IWGRP	0000020			/* W for group */
#define	ICVM_S_IXGRP	0000010			/* X for group */

#define	ICVM_S_IRWXO	0000007			/* RWX mask for other */
#define	ICVM_S_IROTH	0000004			/* R for other */
#define	ICVM_S_IWOTH	0000002			/* W for other */
#define	ICVM_S_IXOTH	0000001			/* X for other */

#define	ICVM_S_IFMT	 	 0170000		/* type of file */
#define	ICVM_S_IFIFO	 0010000		/* named pipe (fifo) */
#define	ICVM_S_IFCHR	 0020000		/* character special */
#define	ICVM_S_IFDIR	 0040000		/* directory */
#define	ICVM_S_IFBLK	 0060000		/* block special */
#define	ICVM_S_IFREG	 0100000		/* regular */
#define	ICVM_S_IFLNK	 0120000		/* symbolic link */
#define	ICVM_S_IFSOCK 	 0140000		/* socket */

#define ICVM_S_IFNONE	0170000        /* no file */

#define	ICVM_S_ISDIR(m)		((m & 0170000) == 0040000)	/* directory */
#define	ICVM_S_ISCHR(m)		((m & 0170000) == 0020000)	/* char special */
#define	ICVM_S_ISBLK(m)		((m & 0170000) == 0060000)	/* block special */
#define	ICVM_S_ISREG(m)		((m & 0170000) == 0100000)	/* regular file */
#define	ICVM_S_ISFIFO(m)	((m & 0170000) == 0010000)	/* fifo */
#define	ICVM_S_ISLNK(m)		((m & 0170000) == 0120000)	/* symbolic link */
#define	ICVM_S_ISSOCK(m)	((m & 0170000) == 0140000)	/* socket */


// dirent types

#define	ICVM_DT_UNKNOWN	 	0
#define	ICVM_DT_FIFO		1
#define	ICVM_DT_CHR			 2
#define	ICVM_DT_DIR			 4
#define	ICVM_DT_BLK			 6
#define	ICVM_DT_REG			 8
#define	ICVM_DT_LNK			10
#define	ICVM_DT_SOCK		12
#define	ICVM_DT_WHT			14

#define	ICVM_IFTODT(mode)	(((mode) & 0170000) >> 12)


// accessflags in fh definitions

#define ICVM_O_ACCESSREAD	0x0001 
#define ICVM_O_ACCESSWRITE	0x0002 


// seek modes

#define	ICVM_SEEK_SET	0	/* set file offset to offset */
#define	ICVM_SEEK_CUR	1	/* set file offset to current plus offset */
#define	ICVM_SEEK_END	2	/* set file offset to EOF plus offset */

/* access function */
#define	ICVM_F_OK		0	/* test for existence of file */
#define	ICVM_X_OK		0x01	/* test for execute or search permission */
#define	ICVM_W_OK		0x02	/* test for write permission */
#define	ICVM_R_OK		0x04	/* test for read permission */



struct ICVM_stat {
	s32	  st_dev;		/* inode's device */
	s32	  st_mode;		/* inode protection mode */
	s32	  st_uid;		/* user ID of the file's owner */
	s32	  st_gid;		/* group ID of the file's group */
	s32	  st_rdev;		/* device type */
	s32	  st_nlink;		/* number of hard links */
	s64	  st_size;		/* file size, in bytes */
	s32	  st_ino;		/* inode's number */
	s32	  st_iatime;		/* time of last access */
	s32	  st_imtime;		/* time of last data modification */
	s32	  st_ictime;		/* time of last file status change */
	s32	  st_iatimensec;		/* time of last access - nanosecs */
	s32	  st_imtimensec;		/* time of last data modification - nanosecs */
	s32	  st_ictimensec;		/* time of last file status change - nanosecs */
	s64   st_blocks;		/* blocks allocated for file */
	s32	  st_blksize;
	s32   st_flags;		/* user defined flags for file */
};


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




struct ICVM_dirent {
	u32 	d_fileno;		/* file number of entry */
	u32 	d_reclen;		/* length of this record */
	u32  	d_type; 		/* file type, see below */
	u32  	d_namlen;		/* length of string in d_name */
	char	d_name[256];	/* name must be no longer than this */
	struct_ICVM_dirent icvmdent;    /* to be passed back to ICVM */
};

/* structure describing an open directory. */
typedef struct {
	icvm_fh	fd;		// file descriptor associated with directory 
#ifdef UNIXSYSTEM
	DIR *dir; // host DIR pointer
#endif
#ifdef __WIN32__
	icvm_pathbuf truepath; // so we can do rewinds etc.
#endif
	struct ICVM_dirent dent;
	u32 pos;
} ICVM_DIR;




#ifdef __cplusplus



class ICVM_BaseHandler;

ICVM_BaseHandler *GetRootDir(void);



extern "C"{

#endif

	char *icvm_getcurwd(void); // get current working directory


//	s32 TranslateOpenFlagsFromICVM(s32 icvmflags);
//	s32 TranslateOpenFlagsToICVM(s32 icvmflags);

	ICVM_FileHandle *i_getnativefh(icvm_fh fh);
	ICVM_BaseHandler *i_getrootfs(void);
	ICVM_BaseHandler *i_getfilehandler(char *path);

	s32 i_initfilesystem(void);

	s32 i_read(icvm_fh fh,void *buf,u32 count);
	s32 i_write(icvm_fh fh,void *buf,u32 count);
	s64 i_lseek(icvm_fh fh,s64 offs,s32 whence);
	s32 i_isatty(icvm_fh fh);
	s32 i_fstat(icvm_fh fh,struct ICVM_stat *st);
	s32 i_stat(char *path,struct ICVM_stat *st);
	s32 i_lstat(char *path,struct ICVM_stat *st);
	s32  i_unlink(char *path);
	s32  i_rmdir(char *path);
	s32  i_mkdir(char *path,s32 mode);
	s32  i_rename(char *from,char *to);
	s32  i_link(char *from,char *to);
	s32  i_symlink(char *from,char *to);
	s32 i_close(icvm_fh fh);
	s32 i_open(char *name,s32 openmode,s32 filemode);
	s32 i_dup(icvm_fh fh);
	s32 i_dup2(icvm_fh fh1,icvm_fh fh2);
	s32 i_ftruncate(icvm_fh fh,u64 offs);
	s32 i_truncate(char *path,u64 offs);
	s32 i_chown(char *path,s32 uid,s32 gid);
	s32 i_fchown(icvm_fh fh,s32 uid,s32 gid);
	s32 i_chmod(char *path,s32 mode);
	s32 i_fchmod(icvm_fh fh,s32 mode);
	s32 i_access(char *path,s32 mode);
	s32 i_umask(s32 um);
	s32 i_getumask(void);
	s32 i_chdir(char *path);
	s32 i_fchdir(icvm_fh fh);
	char *i_getcwd(char *buf,s32 sz);

// *** gettmpdir returns ABSOLUTE pointer - not good for returning to ICVM directly
	char *i_gettmpdir(void);	

	s32 i_pipe(icvm_fh fh[2]);
	ICVM_DIR *i_opendir(char *name);
	s32 i_closedir(ICVM_DIR *d);
	void i_rewinddir(ICVM_DIR *d);
	struct ICVM_dirent *i_readdir(ICVM_DIR *d);
	s32 i_telldir(ICVM_DIR *d);
	void i_seekdir(ICVM_DIR *d,s32 offset);
	
#ifdef __cplusplus
};
#endif

#ifdef __cplusplus



#ifndef __WIN32__


#endif


// ******************** NOTE - NEED TO MOVE FH POINTER STUFF UP
// **************** AND CHANGE ERROR REFERENCES TO i_seterr


class ICVM_BaseHandler{
	static ICVM_FileHandle *FileHandles(void);
	static inline ICVM_FileHandle *FileHandles(s32 n){return FileHandles()+n;};
	static void Enumerate(void);
	int handlernumber; // initialized by Enumerate above
public:
// this constructor normally runs only on static declarations.
// things such as enumerations should be run always, including artifical forks
	ICVM_BaseHandler(void){
		Enumerate();
	};

	inline s32 HandlerNum(void){return handlernumber;};

	static s32 GetCurUMask(void);
	static void SetCurUMask(s32 newmask);

// general purpose not-supported error

	static s32 NotSupported(void){
		i_seterr(ICVM_EOPNOTSUPP);
		return -1;
	};

	static void *NotSupportedPtr(void){
		i_seterr(ICVM_EOPNOTSUPP);
		return 0;
	};

// attempt to validate a system filehandle into an ICVM_FileHandle
// return 0 if error else return a pointer to the filehandle


	static ICVM_FileHandle *ValidateFH(icvm_fh fh,ICVM_BaseHandler *handler){
		if(fh<0 || fh>=ICVM_MAXFILEHANDLES
#ifdef __WIN32__
			|| IsFreeFH(fh)
#endif // __WIN32__
		){
			i_seterr(ICVM_EBADF);
			return 0;
		}


#ifdef UNIXSYSTEM
		if(IsFreeFH(fh)){
fprintf(stderr,"Stray fh found=%d\n",fh);
			FileHandles(fh)->Init(fh,handler->HandlerNum(),0,EMPTYSTRING);
		}
#endif // UNIXSYSTEM

		return FileHandles(fh);

	};


// translate ICVM fh to a native filehandler structure
// return pointer if successfull, 0 if fail.


	static ICVM_FileHandle *GetNativeFH(icvm_fh fh){
		return ValidateFH(fh,i_getrootfs()); // FOR NOW SET ALL FUNCTIONS
										// TO DEFAULT
										// - CHANGE LATER
	};				


// find handler for file based on name

	static  ICVM_BaseHandler *GetFileHandler(char *path){
		return i_getrootfs(); // for now
	};

// resolve pathname to native platform
// return 0 if successful, -1 if not and icvm error set
	static s32 ParsePath(char *inpath,char *outpath){
		if(inpath==0){
			i_seterr(ICVM_EINVAL);
			return -1;
		}
		strcpy(outpath,inpath);
		return 0;
	};

// see if filehandle is free -- return true if so, false if not
// NOTE - NO ERROR CHECKING DONE HERE FOR RANGE

	static inline s32 IsFreeFH(s32 fh){return FileHandles(fh)->IsFree();};
	static inline s32 IsFreeFH(ICVM_FileHandle *fh){return fh->IsFree();};

// get a free file handler, if available. return fh number if successful
// set error code and return -1 if not available

	static s32 AllocFH(void){
		int cnt=ICVM_MAXFILEHANDLES-1;
		while(cnt>=0){
			if(IsFreeFH(cnt)){
				FileHandles(cnt)->Allocate();
				return cnt;
			}
			--cnt;
		}
		i_seterr(ICVM_EMFILE);
		return 0;
	};

// free a file handle
	static inline void FreeFH(s32 fh){
		if(fh<0 || fh>=ICVM_MAXFILEHANDLES)
			return;
		FileHandles(fh)->Reset();
	};

	static inline void FreeFH(ICVM_FileHandle *fh){fh->Reset();};

// first call to initialize and set up the filesystem

	static void initfilesystem(void){
		int cnt=0;
		while(cnt<ICVM_MAXFILEHANDLES){
			FreeFH(cnt);
			++cnt;
		}
#ifdef UNIXSYSTEM
		FileHandles(0)->Init(0,i_getrootfs()->HandlerNum(),0,"$STDIN");		
		FileHandles(1)->Init(1,i_getrootfs()->HandlerNum(),0,"$STDOUT");		
		FileHandles(2)->Init(2,i_getrootfs()->HandlerNum(),0,"$STDERR");		
#endif // UNIXSYSTEM

#ifdef __WIN32__
// ******** NOTE --- these are just console based now -- need to refine later

		FileHandles(0)->Init(GetStdHandle(STD_INPUT_HANDLE),
			i_getrootfs()->HandlerNum(),0,"$STDIN");		
		FileHandles(1)->Init(GetStdHandle(STD_OUTPUT_HANDLE),
			i_getrootfs()->HandlerNum(),0,"$STDOUT");		
		FileHandles(2)->Init(GetStdHandle(STD_ERROR_HANDLE),
			i_getrootfs()->HandlerNum(),0,"$STDERR");		

#endif // __WIN32__

	};

	


#ifdef __WIN32__
	static DWORD TranslateFileAttributesFromICVM(s32 inmode,int *typ);
	static s32   TranslateFileAttributesToICVM(DWORD inmode,int typ);

// change backspaces to forwardspaces
	static inline void DumpBackSpaces(char *s){
		while(*s){
			if(*s=='\\')
				*s='/';
			++s;
		}
	};

#endif

#ifdef UNIXSYSTEM
	virtual s32 TranslateModeFromICVM(s32 inmode);
	virtual s32 TranslateModeToICVM(s32 inmode);
	virtual s32 TranslateOpenFlagsFromICVM(s32 icvmflags);

#if defined(__svr4__) || defined(mklinux)
	virtual int TranslateDTToICVM(DIR *dirptr);
#else
	virtual int TranslateDTToICVM(int in);

#endif

#endif // UNIXSYSTEM



// **********************************************
// stat support functions
// **********************************************


// copy a native stat buffer to an ICVM stat buffer

#ifdef UNIXSYSTEM


	inline int GetICVMStat(char *statname,char *lstatname,
				native_fh fh,struct ICVM_stat *d){
		struct stat lst;
		struct stat *s=&lst;
		if(statname){
			if(::stat(statname,s)){
				i_settranslatederr();
				return -1;
			}
		}else if(lstatname){
			if(::lstat(lstatname,s)){
				i_settranslatederr();
				return -1;
			}
		}else if(::fstat(fh,s)){
			i_settranslatederr();
			return -1;
		}
		d->st_dev=0; // NOT USED FOR NOW
		d->st_rdev=0; // NOT USED FOR NOW
		d->st_nlink=1; // NOT USED FOR NOW
		d->st_ino=0; // NOT USED FOR NOW
		d->st_flags=0; // NOT USED FOR NOW
		d->st_mode=TranslateModeToICVM(s->st_mode);
		d->st_uid=s->st_uid;
		d->st_gid=s->st_gid;
		d->st_size=s->st_size;
		d->st_iatime=s->st_atime;
		d->st_iatimensec=0;
		d->st_imtime=s->st_mtime;
		d->st_imtimensec=0;
		d->st_ictime=s->st_ctime;
		d->st_ictimensec=0;
		d->st_blksize=1024;
		d->st_blocks=(d->st_size+1023)/1024;
		return 0;
	};


#endif

#ifdef __WIN32__


#define WFTtoUFT(fth,ftl) NSECTOSEC(FILETIMETONSEC(ftl,fth))

	inline u64 WinFTtoT(FILETIME *in){
		FILETIME out;
		FileTimeToLocalFileTime(in,&out);
		return WFTtoUFT(
				out.dwHighDateTime,
				out.dwLowDateTime);
	};		

// adaption to get a file type from Windows
	inline int GetFileType(char *path){
		int ftp=FILE_TYPE_UNKNOWN;;
		HANDLE h=CreateFile(path,0,FILE_SHARE_READ,0,OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,0);
		if(h){
			ftp=::GetFileType(h);
			CloseHandle(h);
		}
		return ftp;
	};

	inline int GetICVMStat(char *statname,char *lstatname,
			native_fh fh,struct ICVM_stat *d){
		BY_HANDLE_FILE_INFORMATION wst;
		WIN32_FILE_ATTRIBUTE_DATA ast;

//******** NOTE - SOFT LINKS NOT HANLDED YET!!!! ***************
//********** ALSO LSTAT and STAT are not fully pulling filetype yet

		if(statname){
fprintf(stderr,"Broken for now - needs replace for GetFileAttributesEx\n");
exit(3);
#ifdef NOT_NOW
			if(!GetFileAttributesEx(statname,
					GetFileExInfoStandard,&ast))
#endif
			{ // ERROR
				i_settranslatederr();
				return -1;
			}		
			
		}else if(lstatname){
fprintf(stderr,"Broken for now - needs replace for GetFileAttributesEx\n");
exit(3);
#ifdef NOT_NOW
			if(!GetFileAttributesEx(lstatname,
					GetFileExInfoStandard,&ast))
#endif
			{ // ERROR
				i_settranslatederr();
				return -1;
			}		

		}else if(!GetFileInformationByHandle(fh,&wst)){ // ERROR
			i_settranslatederr();
			return -1;
		}		

		if(statname || lstatname){
			if(statname)
				d->st_mode=TranslateFileAttributesToICVM(
					ast.dwFileAttributes,GetFileType(statname));
		else
				d->st_mode=TranslateFileAttributesToICVM(
					ast.dwFileAttributes,GetFileType(lstatname));
			d->st_size=
				(((s64)ast.nFileSizeLow) & 0xffffffffLL) +
				(((s64)ast.nFileSizeHigh) << 32);
			d->st_atime=WinFTtoT(&ast.ftLastAccessTime);
			d->st_mtime=WinFTtoT(&ast.ftLastWriteTime);
			d->st_ctime=WinFTtoT(&ast.ftCreationTime);

		}else{
			d->st_mode=TranslateFileAttributesToICVM(
				wst.dwFileAttributes,::GetFileType(fh));
			d->st_size=
				(((s64)wst.nFileSizeLow) & 0xffffffffLL) +
				(((s64)wst.nFileSizeHigh) << 32);
			d->st_atime=WinFTtoT(&wst.ftLastAccessTime);
			d->st_mtime=WinFTtoT(&wst.ftLastWriteTime);
			d->st_ctime=WinFTtoT(&wst.ftCreationTime);
		}
		d->st_dev=0; // NOT USED FOR NOW
		d->st_rdev=0; // NOT USED FOR NOW
		d->st_nlink=1; // NOT USED FOR NOW
		d->st_ino=0; // NOT USED FOR NOW
		d->st_flags=0; // NOT USED FOR NOW
		d->st_uid=DEFAULT_UID; // ??
		d->st_gid=DEFAULT_GID;// ??
		d->st_atimensec=0;
		d->st_mtimensec=0;
		d->st_ctimensec=0;
		d->st_blksize=1024;
		d->st_blocks=(d->st_size+1023)/1024;
		return 0;
	};


#endif

// *******************************************************
// these are the various local handlers for different general file
// ops - these are to be overridden by superclasses of ICVM_BaseHandler
// on this level most will just return errors.
// *******************************************************



	virtual s32 read(ICVM_FileHandle *h,void *buf,u32 count){

#ifdef __WIN32__

		DWORD status;
		BOOL  success;

//**** need to look at access types for async reads etc....
// also look into OVERLAPPED stuff

		success=ReadFile(h->FH(),(LPVOID)buf,count,&status,NULL);
		i_settranslatederr();
		if(!success){
			i_settranslatederr();
			return -1;
		}
		return status;

#endif // __WIN32__

#ifdef UNIXSYSTEM
		ssize_t status=::read(h->FH(),buf,count);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	virtual s32 write(ICVM_FileHandle *h,void *buf,u32 count){
#ifdef __WIN32__
		DWORD status;
		BOOL  success;

//**** need to look at access types for async reads etc....
// also look into OVERLAPPED stuff

		success=WriteFile(h->FH(),(LPVOID)buf,count,&status,NULL);
		if(!success){
			i_settranslatederr();
			return -1;
		}
		return status;

#endif //  __WIN32__

#ifdef UNIXSYSTEM

		s32 status=::write(h->FH(),buf,count);
		i_settranslatederr();
		return status;

#endif // UNIXSYSTEM

	};

	virtual s32 fstat(ICVM_FileHandle *h,struct ICVM_stat *st){
		return GetICVMStat(0,0,h->FH(),st);	
	};


	virtual s32 stat(char *path,struct ICVM_stat *st){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;
		return GetICVMStat(p,0,0,st);	
	};

	virtual s32 lstat(char *path,struct ICVM_stat *st){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;
		return GetICVMStat(0,p,0,st);	
	};



	virtual s64 lseek(ICVM_FileHandle *h,s64 offset,s32 whence){

#ifdef __WIN32__
		s64 status;
		LONG hipos;
		LONG   lowpos;
		hipos=(offset>>32);

		switch(whence){
			case ICVM_SEEK_SET:
				whence=FILE_BEGIN;
				break;
	
			case ICVM_SEEK_CUR:
				whence=FILE_CURRENT;
				break;

			case ICVM_SEEK_END:
				whence=FILE_END;
				break;
	
			default:
				i_seterr(ICVM_EINVAL);
				return -1;
		}

		lowpos=SetFilePointer(h->FH(),(LONG) offset,&hipos,whence);
		status=lowpos & 0xffffffff;
		status|=((u64)hipos)<<32;
		i_settranslatederr();
		return status;
#endif // __WIN32__

#ifdef UNIXSYSTEM

		off_t status;
		switch(whence){
			case ICVM_SEEK_SET:
				whence=SEEK_SET;
				break;
	
			case ICVM_SEEK_CUR:
				whence=SEEK_CUR;
				break;

			case ICVM_SEEK_END:
				whence=SEEK_END;
				break;
	
			default:
				i_seterr(ICVM_EINVAL);
				return -1;
		}

		status=::lseek(h->FH(),offset,whence);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	virtual s32 ftruncate(ICVM_FileHandle *h,u64 offs){
#ifdef __WIN32__
		s64 	status;
		LONG 	hipos;
		LONG   lowpos;
		LONG   oldhipos;
		LONG   oldlowpos;

		oldlowpos=SetFilePointer(h->FH(),0,&oldhipos,FILE_CURRENT);
		if(oldlowpos==0xffffffff){
			i_settranslatederr();
			return -1;
		}
		hipos=(offs>>32);
		lowpos=SetFilePointer(h->FH(),(LONG) offs,&hipos,FILE_BEGIN);
		status=lowpos & 0xffffffff;
		status|=((u64)hipos)<<32;
		if(status!=offs){
			i_settranslatederr();
			return -1;
		}
		if(!SetEndOfFile(h->FH())){
			i_settranslatederr();
			return -1;
		}
		oldlowpos=SetFilePointer(h->FH(),0,&oldhipos,FILE_BEGIN);
		if(oldlowpos==0xffffffff){
			i_settranslatederr();
			return -1;
		}
		return 0;

#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::ftruncate(h->FH(),offs);
		i_settranslatederr();
		return status;

#endif // UNIXSYSTEM

	};

	virtual s32 isatty(ICVM_FileHandle *h){

#ifdef __WIN32__
// ****************** NEED WINDOWS IMPLEMENTATION *****************
		return 0; // not implemented for now
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::isatty(h->FH());
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	virtual s32 dup(ICVM_FileHandle *h){

#ifdef __WIN32__
		s32 fh=AllocFH();
		HANDLE wh;
		if(fh<0){
			return -1;
		}
		if(!DuplicateHandle(
			GetCurrentProcess(),
			h->FH(),
			GetCurrentProcess(),
			&wh,0,1,DUPLICATE_SAME_ACCESS
		)){
			i_settranslatederr();
			FreeFH(fh);
			return -1;
		}
		FileHandles(fh)->Init(wh,h->HandlerNum(),h->AccessFlags(),h->PathName());
		return fh;


#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 fh=AllocFH();
		s32 status;
		if(fh<0){
			return -1;
		}
		status=::dup(h->FH());
		if(status<0){
			i_settranslatederr();
			FreeFH(fh);
			return -1;
		}
		
		FileHandles(fh)->Init(status,h->HandlerNum(),h->AccessFlags(),h->PathName());
		return fh;


#endif // UNIXSYSTEM

	};

	virtual s32 dup2(ICVM_FileHandle *h,s32 fh){
		if(fh<0 || fh>=ICVM_MAXFILEHANDLES){
			i_seterr(ICVM_EBADF);
			return 0;
		}

#ifdef __WIN32__
		HANDLE wh;
		if(!IsFreeFH(fh))
			i_close(fh);
		if(!DuplicateHandle(
			GetCurrentProcess(),
			h->FH(),
			GetCurrentProcess(),
			&wh,0,1,DUPLICATE_SAME_ACCESS
		)){
			i_settranslatederr();
			FreeFH(fh);
			return -1;
		}
		FileHandles(fh)->Init(wh,h->HandlerNum(),h->AccessFlags(),h->PathName());
		return fh;

#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status;
		if(!IsFreeFH(fh))
			i_close(fh);
		status=::dup(h->FH());
		if(status<0){
			i_settranslatederr();
			FreeFH(fh);
			return -1;
		}
		
		FileHandles(fh)->Init(status,h->HandlerNum(),h->AccessFlags(),h->PathName());
		return fh;

#endif // UNIXSYSTEM

	};


	virtual s32 unlink(char *path){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;

#ifdef __WIN32__
		s32 status=DeleteFile(p);
		i_settranslatederr();
		if(status)
			return 0;
		return -1;
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::unlink(p);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 rmdir(char *path){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;

#ifdef __WIN32__
		s32 status=RemoveDirectory(p);
		i_settranslatederr();
		if(status)
			return 0;
		return -1;
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::rmdir(p);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 mkdir(char *path,s32 mode){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;

#ifdef __WIN32__
		s32 status=CreateDirectory(p,0);
		i_settranslatederr();
		if(status){
			if(!SetFileAttributes(p,
					TranslateFileAttributesFromICVM(mode,0))){
				i_settranslatederr();
				return -1;
			}
			return 0;
		}
		return -1;
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::mkdir(p,TranslateModeFromICVM(mode));
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 rename(char *from,char *to){
		icvm_pathbuf pf,pt;
		if(ParsePath(from,pf))
			return -1;
		if(ParsePath(to,pt))
			return -1;

#ifdef __WIN32__
		int status=MoveFile(pf,pt);
		i_settranslatederr();
		if(status)
			return 0;
		return -1;
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::rename(pf,pt);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 link(char *from,char *to){
		icvm_pathbuf pf,pt;
		if(ParsePath(from,pf))
			return -1;
		if(ParsePath(to,pt))
			return -1;

#ifdef __WIN32__
		return NotSupported();
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::link(pf,pt);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 symlink(char *from,char *to){
		icvm_pathbuf p;
		if(ParsePath(from,p))
			return -1;

#ifdef __WIN32__
		return NotSupported();
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::symlink(from,p);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 chown(char *path,s32 uid,s32 gid){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;
#ifdef __WIN32__
		ICVM_stat st;
		return i_stat(p,&st); // for windoze, just stat it and return error
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::chown(p,uid,gid);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 fchown(ICVM_FileHandle *fh,s32 uid,s32 gid){

#ifdef __WIN32__
		ICVM_stat st;
		return fh->Handler()->fstat(fh,&st); // for windoze, just stat it and return error
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::fchown(fh->FH(),uid,gid);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	virtual s32 chmod(char *path,s32 mode){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;
#ifdef __WIN32__
		
		if(!SetFileAttributes(p,
				TranslateFileAttributesFromICVM(mode,0))){
			i_settranslatederr();
			return -1;
		}
		return 0;		
		
#endif // __WIN32__

#ifdef UNIXSYSTEM

		s32 status=::chmod(p,TranslateModeFromICVM(mode));
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	virtual s32 fchmod(ICVM_FileHandle *fh,s32 mode){

#ifdef __WIN32__
		return i_chmod(fh->PathName(),mode); // windoze lack of functionality
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::fchmod(fh->FH(),TranslateModeFromICVM(mode));
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};

	virtual s32 chdir(char *path){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;

#ifdef __WIN32__
		
		if(!SetCurrentDirectory(p)){
			i_settranslatederr();
			return -1;
		}
		return 0;		
		
#endif // __WIN32__

#ifdef UNIXSYSTEM

		s32 status=::chdir(p);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	virtual s32 fchdir(ICVM_FileHandle *fh){

#ifdef __WIN32__
		return i_chdir(fh->PathName()); // windoze lack of functionality
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::fchdir(fh->FH());
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	virtual s32 access(char *path,s32 mode){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;
#ifdef __WIN32__
		ICVM_stat st;
		if(i_stat(path,&st)<0)
			return -1;

		if(mode & ICVM_X_OK && (st.st_mode & ICVM_S_IXUSR)==0){
			i_seterr(ICVM_EACCES);
			return -1;
		}

		if(mode & ICVM_R_OK && (st.st_mode & ICVM_S_IRUSR)==0){
			i_seterr(ICVM_EACCES);
			return -1;
		}

		if(mode & ICVM_W_OK && (st.st_mode & ICVM_S_IWUSR)==0){
			i_seterr(ICVM_EACCES);
			return -1;
		}

		return 0;
		
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status;
		s32 amode=0;
		if(mode & ICVM_F_OK)
			amode|=F_OK;
		if(mode & ICVM_X_OK)
			amode|=X_OK;
		if(mode & ICVM_W_OK)
			amode|=W_OK;
		if(mode & ICVM_R_OK)
			amode|=R_OK;
		status=::access(p,amode);
		i_settranslatederr();
		return status;
#endif // UNIXSYSTEM

	};


	static char  *getcwd(char *buf,s32 sz){

#ifdef __WIN32__
		icvm_pathbuf tmpbuf;
		char *st;
		char *s;
		int status;
		if(buf==0){ // malloc a buffer
			status=GetCurrentDirectory(sizeof(tmpbuf)-1,tmpbuf);
			if(status==0 || status>=sizeof(tmpbuf)){
				i_settranslatederr();
				return 0;
			}
			st=(char *)i_malloc(strlen(s)+1);
			if(st==0){
				i_seterr(ICVM_ENOMEM);
				return 0;
			}
			strcpy(st,s);
			DumpBackSpaces(st);
			return st;
		}
		status=GetCurrentDirectory(sz,buf);
		if(status==0 || status>sz){
			if(status>sz)
				i_seterr(ICVM_ERANGE);
			else
				i_settranslatederr();
			return 0;
		}
		DumpBackSpaces(buf);
		return buf;
		
#endif // __WIN32__

#ifdef UNIXSYSTEM
		icvm_pathbuf tmpbuf;
		char *st;
		char *s;
		if(buf==0){ // malloc a buffer
			s=::getcwd(tmpbuf,sizeof(tmpbuf));
			if(s==0){
				i_settranslatederr();
				return 0;
			}
			st=(char *)i_malloc(strlen(s)+1);
			if(st==0){
				i_seterr(ICVM_ENOMEM);
				return 0;
			}
			strcpy(st,s);
			return st;
		}
		s=::getcwd(buf,sz);
		i_settranslatederr();
		return s;
		

#endif // UNIXSYSTEM

	};


	virtual s32 open(char *path,s32 openmode,s32 filemode){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return -1;
#ifdef __WIN32__
		s32 fh;
		SECURITY_ATTRIBUTES se;

		filemode&=~i_getumask();
		DWORD dwAccess=0;
		DWORD dwCreate=0;
		DWORD dwAttrsAndFlags=FILE_FLAG_POSIX_SEMANTICS | FILE_ATTRIBUTE_NORMAL;
		native_fh nfh;

		fh=AllocFH();
		if(fh<0){
			return -1;
		}
		if(openmode & ICVM_O_CREAT){
			if(openmode & ICVM_O_EXCL)
				dwCreate=CREATE_NEW;
			else{
				dwCreate=OPEN_ALWAYS;
				dwAttrsAndFlags|=TranslateFileAttributesFromICVM(filemode,0);
			}
		}else
			dwCreate=OPEN_EXISTING;

		if(openmode & ICVM_O_RDWR)
			dwAccess|=GENERIC_READ | GENERIC_WRITE;
		else if(openmode & ICVM_O_WRONLY)
			dwAccess|=GENERIC_WRITE;
		else  if((openmode & ICVM_O_RDONLY)==ICVM_O_RDONLY)  /* no bits to check ... */
			dwAccess|=GENERIC_READ;
		se.nLength=sizeof(se);
		se.lpSecurityDescriptor=0;
		se.bInheritHandle=TRUE;

		nfh=CreateFile(p,dwAccess,
			FILE_SHARE_READ | FILE_SHARE_WRITE,&se,
			dwCreate,dwAttrsAndFlags,0);

		if(nfh==INVALID_HANDLE_VALUE){
			i_settranslatederr();
			FreeFH(fh);
			return -1;
		}
		
	
// truncate if necessary

		if(openmode & ICVM_O_TRUNC)
			SetEndOfFile(nfh);

// append if necessary
		if(openmode & ICVM_O_APPEND){
			if(dwAccess & GENERIC_WRITE)
				SetFilePointer(nfh,0,0,FILE_END);
		}

		FileHandles(fh)->Init(nfh,HandlerNum(),openmode,path);
		return fh;
										
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 fh;
		s32 status;
		s32 topenmode;
		s32 tfilemode;
		filemode&=~i_getumask();
		fh=AllocFH();
		if(fh<0)
			return -1;
		tfilemode=TranslateModeFromICVM(filemode);
		topenmode=TranslateOpenFlagsFromICVM(openmode);
		status=::open(p,topenmode,tfilemode);
		if(status<0){
			i_settranslatederr();
			FreeFH(fh);
			return -1;
		}
		FileHandles(fh)->Init(status,HandlerNum(),openmode,path);
		return fh;
#endif // UNIXSYSTEM
	};



	virtual s32 close(ICVM_FileHandle *h){

#ifdef __WIN32__
		s32 status=CloseHandle(h->FH());
		i_settranslatederr();
		FreeFH(h);
		if(status)
			return 0;
		return -1;
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::close(h->FH());
		i_settranslatederr();
		FreeFH(h);
		return status;
#endif // UNIXSYSTEM
	};


	static s32 pipe(icvm_fh fh[2]){
		s32 fh1,fh2;
		fh1=AllocFH();
		if(fh1==0)
			return -1;
		fh2=AllocFH();
		if(fh2==0){
			FreeFH(fh1);
			return -1;
		}

#ifdef __WIN32__
		HANDLE filedes[2];
		SECURITY_ATTRIBUTES se;
		se.nLength=sizeof(se);
		se.lpSecurityDescriptor=0;
		se.bInheritHandle=TRUE;
		if(!CreatePipe(&filedes[0],&filedes[1],&se,0)){
			i_settranslatederr();
			FreeFH(fh1);
			FreeFH(fh2);
			return -1;
		}
		FileHandles(fh1)->Init(filedes[0],i_getrootfs()->HandlerNum(),0,"$PIPEIN");
		FileHandles(fh2)->Init(filedes[1],i_getrootfs()->HandlerNum(),0,"$PIPEOUT");
		fh[0]=fh1;
		fh[1]=fh2;
		return 0;
#endif // __WIN32__

#ifdef UNIXSYSTEM
		int filedes[2];
		if(::pipe(filedes)<0){
			i_settranslatederr();
			FreeFH(fh1);
			FreeFH(fh2);
			return -1;
		}
		FileHandles(fh1)->Init(filedes[0],i_getrootfs()->HandlerNum(),0,"$PIPEIN");
		FileHandles(fh2)->Init(filedes[1],i_getrootfs()->HandlerNum(),0,"$PIPEOUT");
		fh[0]=fh1;
		fh[1]=fh2;
		return 0;
#endif // UNIXSYSTEM

	};


	virtual ICVM_DIR *opendir(char *path){
		icvm_pathbuf p;
		if(ParsePath(path,p))
			return 0;
		ICVM_DIR *d=(ICVM_DIR *)i_malloc(sizeof(ICVM_DIR));
		s32 fh;
		if(d==0){
			i_seterr(ICVM_ENOMEM);
			return 0;
		}

#ifdef __WIN32__
		HANDLE h;
		WIN32_FIND_DATA wd;
		char buf[4096];
		fh=AllocFH();
		if(fh<0){
			delete d;
			return 0;
		}
		strncpy(d->truepath,p,sizeof(d->truepath)-3);

		if(d->truepath[strlen(d->truepath)-1]!='/')
			strcat(d->truepath,"/");
		strcat(d->truepath,"*");

		h=FindFirstFile(d->truepath,&wd);
		if(h!=INVALID_HANDLE_VALUE){

			d->fd=fh;
			FileHandles(fh)->Init(h,HandlerNum(),0,path);
			d->pos=0;

	// have first entry, fill out what we can
			strncpy(d->dent.d_name,wd.cFileName,256);

			strcpy(buf,d->truepath);
			buf[strlen(d->truepath)-1]!=0;
			strcat(buf,wd.cFileName);

			d->dent.d_type=
				ICVM_IFTODT(TranslateFileAttributesToICVM(
					wd.dwFileAttributes,GetFileType(buf)));

			return d;
		}

		i_settranslatederr();
		FreeFH(fh);
		delete d;
		return 0;
										
#endif // __WIN32__

#ifdef UNIXSYSTEM
		DIR *nd;
		fh=AllocFH();
		if(fh<0){
			delete d;
			return 0;
		}
		nd=::opendir(p);
		if(nd==0){
			i_settranslatederr();
			FreeFH(fh);
			delete d;
			return 0;
		}

		FileHandles(fh)->Init(dirfd(nd),HandlerNum(),0,path);
		d->fd=fh;
		d->dir=nd;
		d->pos=0;
		return d;
#endif // UNIXSYSTEM
	};

	virtual s32 closedir(ICVM_DIR *d){
#ifdef __WIN32__
		FindClose(FileHandles(d->fd)->FH());
		FreeFH(d->fd);
		i_free(d);
		return 0;		
#endif // __WIN32__

#ifdef UNIXSYSTEM
		s32 status=::closedir(d->dir);
		i_settranslatederr();
		FreeFH(d->fd);
		i_free(d);
		return status;
#endif // UNIXSYSTEM
	};

	virtual void rewinddir(ICVM_DIR *d){
#ifdef __WIN32__

		HANDLE h;
		WIN32_FIND_DATA wd;
		char buf[4096];

		h=FindFirstFile(d->truepath,&wd);
		if(h!=INVALID_HANDLE_VALUE){

			FileHandles(d->fd)->Init(h,HandlerNum(),0,FileHandles(d->fd)->PathName());
			d->pos=0;

	// have first entry, fill out what we can
			strncpy(d->dent.d_name,wd.cFileName,256);

			strcpy(buf,d->truepath);
			buf[strlen(d->truepath)-1]!=0;
			strcat(buf,wd.cFileName);

			d->dent.d_type=
				ICVM_IFTODT(TranslateFileAttributesToICVM(
					wd.dwFileAttributes,GetFileType(buf)));

		}

										
#endif // __WIN32__

#ifdef UNIXSYSTEM
		::rewinddir(d->dir);
		d->pos=0;
#endif // UNIXSYSTEM
	};


	virtual ICVM_dirent *readdir(ICVM_DIR *d){

#ifdef __WIN32__
		WIN32_FIND_DATA wd;
		char buf[4096];
// entry 0 already read in				
		if(d->pos!=0){
			if(!FindNextFile(FileHandles(d->fd)->FH(),&wd)){
				i_settranslatederr();
				return 0;
			}
			strncpy(d->dent.d_name,wd.cFileName,256);

			strcpy(buf,d->truepath);
			buf[strlen(d->truepath)-1]!=0;
			strcat(buf,wd.cFileName);
			d->dent.d_type=
				ICVM_IFTODT(TranslateFileAttributesToICVM(
					wd.dwFileAttributes,GetFileType(buf)));
		}			
		d->dent.d_fileno=d->pos;
		d->pos++;
		d->dent.d_namlen=strlen(d->dent.d_name);

		d->dent.d_reclen=1;
										
#endif // __WIN32__

#ifdef UNIXSYSTEM
		struct dirent *de;
		de=::readdir(d->dir);
		if(de==0){
			i_settranslatederr();
			return 0;
		}
		d->dent.d_fileno=d->pos;
		d->pos++;
		strncpy(d->dent.d_name,de->d_name,256);
		d->dent.d_namlen=strlen(d->dent.d_name);
#if defined(__svr4__) || defined(mklinux) 
		d->dent.d_type=TranslateDTToICVM(de);
#else
		d->dent.d_type=TranslateDTToICVM(de->d_type);
#endif
		d->dent.d_reclen=1;
#endif // UNIXSYSTEM
		return &d->dent;
	};

	virtual s32 telldir(ICVM_DIR *d){
		return d->pos;
	};

	virtual void seekdir(ICVM_DIR *d,s32 offset){
		u32 off=offset;
		rewinddir(d);
		while(d->pos<offset){
			if(readdir(d)==0)
				return;
		}
	};


};

class ICVM_FileHandler: public ICVM_BaseHandler{
public:
	virtual s32 rmdir(char *path){ return NotSupported();};
	virtual s32 mkdir(char *path,s32 mode){ return NotSupported();};
	virtual s32 fchdir(ICVM_FileHandle *fh){return NotSupported();};
	virtual ICVM_DIR *opendir(char *path){return (ICVM_DIR *)NotSupportedPtr();};
	virtual s32 closedir(ICVM_DIR *d){return NotSupported();};
	virtual ICVM_dirent *readdir(ICVM_DIR *d){return (ICVM_dirent *)NotSupportedPtr();};
	virtual s32 telldir(ICVM_DIR *d){return NotSupported();};
};



class ICVM_DirectoryHandler : public ICVM_BaseHandler{
public:
	virtual s32 read(ICVM_FileHandle *h,void *buf,u32 count)
		{return NotSupported();};
	virtual s32 write(ICVM_FileHandle *h,void *buf,u32 count)
		{return NotSupported();};
	virtual s32 fstat(ICVM_FileHandle *h,struct ICVM_stat *st)
		{return i_stat(h->PathName(),st);};
	virtual s64 lseek(ICVM_FileHandle *h,s64 offset,s32 whence)
		{return NotSupported();};
	virtual s32 ftruncate(ICVM_FileHandle *h,u64 offs)
		{return NotSupported();};
	virtual s32 isatty(ICVM_FileHandle *h){return 0;};
	virtual s32 unlink(char *path){return NotSupported();};
	virtual s32 fchdir(ICVM_FileHandle *fh){return i_chdir(fh->PathName());};
	virtual s32 open(char *path,s32 openmode,s32 filemode)
		{return NotSupported();};
	virtual s32 close(ICVM_FileHandle *h){return NotSupported();};

};


#endif // __cplusplus


#endif


