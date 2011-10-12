#include "cvm.h"
#include "file.h"
#include "icvmerr.h"
#include "icvmsys.h"


// Copyright (C) 2000,2001 Bob Dayley

#ifndef __WIN32__

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <dirent.h>

#endif




void ICVM_read(void){
	R_R0=i_read(
		rSTKs32(0),
		Nptr(rSTKu32(1)),
		rSTKu32(2));
	icvm_returnerr();
}



void ICVM_write(void){
	R_R0=i_write(
		rSTKs32(0),
		Nptr(rSTKu32(1)),
		rSTKu32(2));
	icvm_returnerr();
}


void ICVM_unlink(void){
	R_R0=i_unlink(
		(char *)Nptr(rSTKs32(0))
	);
	icvm_returnerr();
}

void ICVM_rmdir(void){
	R_R0=i_rmdir(
		(char *)Nptr(rSTKs32(0))
	);
	icvm_returnerr();
}

void ICVM_mkdir(void){
	R_R0=i_mkdir(
		(char *)Nptr(rSTKs32(0)),
		rSTKs32(1)
	);
	icvm_returnerr();
}


void ICVM_rename(void){
	R_R0=i_rename(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	);
	icvm_returnerr();
}


void ICVM_link(void){
	R_R0=i_link(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	);
	icvm_returnerr();
}


void ICVM_symlink(void){
	R_R0=i_symlink(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKs32(1))
	);
	icvm_returnerr();
}



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

static void CopyIStat(p32 istat,struct ICVM_stat *st){
	WDs32(istat+ICVM_ist_dev,st->st_dev,0);
	WDs32(istat+ICVM_ist_mode,st->st_mode,0);
	WDu32(istat+ICVM_ist_uid,st->st_uid,0);
	WDu32(istat+ICVM_ist_gid,st->st_gid,0);
	WDs32(istat+ICVM_ist_rdev,st->st_rdev,0);
	WDu32(istat+ICVM_ist_nlink,st->st_nlink,0);
	WDu32(istat+ICVM_ist_ino,st->st_ino,0);
	WDu64(istat+ICVM_ist_size,st->st_size,0);
	WDu32(istat+ICVM_ist_atime,st->st_iatime,0);
	WDu32(istat+ICVM_ist_mtime,st->st_imtime,0);
	WDu32(istat+ICVM_ist_ctime,st->st_ictime,0);
	WDu32(istat+ICVM_ist_atimensec,0,0);
	WDu32(istat+ICVM_ist_atimensec,st->st_iatimensec,0);
	WDu32(istat+ICVM_ist_mtimensec,st->st_imtimensec,0);
	WDu32(istat+ICVM_ist_ctimensec,st->st_ictimensec,0);
	WDu32(istat+ICVM_ist_blksize,st->st_blksize,0);
	WDu64(istat+ICVM_ist_blocks,st->st_blocks,0);
	WDu32(istat+ICVM_ist_flags,0,0);

}


void ICVM_fstat(void){
	struct ICVM_stat st;
	p32 istat;
	if(!i_fstat(rSTKs32(0),&st)){
		R_R0=0;
		istat=rSTKs32(1);
		CopyIStat(istat,&st);
	}else
		R_R0=-1;
	icvm_returnerr();
}


void ICVM_stat(void){
	struct ICVM_stat st;
	p32 istat;
	if(!i_stat(
		(char *)Nptr(rSTKs32(0)),
		&st)
	){
		R_R0=0;
		istat=rSTKs32(1);
		CopyIStat(istat,&st);
	}else
		R_R0=-1;
	icvm_returnerr();
}

void ICVM_lstat(void){
	struct ICVM_stat st;
	p32 istat;
	if(!i_lstat(
		(char *)Nptr(rSTKs32(0)),
		&st)
	){
		R_R0=0;
		istat=rSTKs32(1);
		CopyIStat(istat,&st);
	}else
		R_R0=-1;
	icvm_returnerr();
}



void ICVM_lseek(void){
	iRETURNs64(
		i_lseek(
			rSTKs32(0),
			rSTKs64(1),
			rSTKu32(2)
		)
	);
	icvm_returnerr();
}


void ICVM_isatty(void){
	R_R0=i_isatty(
		rSTKs32(0)
	);
	icvm_returnerr();
}


void ICVM_dup(void){
	R_R0=i_dup(
		rSTKs32(0)
	);
	icvm_returnerr();
}

void ICVM_dup2(void){
	R_R0=i_dup2(
		rSTKs32(0),
		rSTKs32(1)
	);
	icvm_returnerr();
}


void ICVM_chown(void){
	R_R0=i_chown(
		(char *)Nptr(rSTKs32(0)),
		rSTKs32(1),
		rSTKs32(2)
	);
	icvm_returnerr();
}


void ICVM_fchown(void){
	R_R0=i_fchown(
		rSTKs32(0),
		rSTKs32(1),
		rSTKs32(2)
	);
	icvm_returnerr();
}


void ICVM_chmod(void){
	R_R0=i_chmod(
		(char *)Nptr(rSTKs32(0)),
		rSTKs32(1)
	);
	icvm_returnerr();
}


void ICVM_fchmod(void){
	R_R0=i_fchmod(
		rSTKs32(0),
		rSTKs32(1)
	);
	icvm_returnerr();
}

void ICVM_chdir(void){
	R_R0=i_chdir(
		(char *)Nptr(rSTKs32(0))
	);
	icvm_returnerr();
}


void ICVM_fchdir(void){
	R_R0=i_fchdir(
		rSTKs32(0)
	);
	icvm_returnerr();
}

void ICVM_getcwd(void){
	R_R0=Iptr(
		i_getcwd(
			(char *)Nptr(rSTKs32(0)),
			rSTKs32(1)
		)
	);
	icvm_returnerr();
}


void ICVM_access(void){
	R_R0=i_access(
		(char *)Nptr(rSTKs32(0)),
		rSTKs32(1)
	);
	icvm_returnerr();
}



void ICVM_ftruncate(void){
	R_R0=i_ftruncate(
		rSTKs32(0),
		rSTKu64(1)
	);
	icvm_returnerr();
}

void ICVM_truncate(void){
	R_R0=i_truncate(
		(char *)Nptr(rSTKs32(0)),
		rSTKu64(1)
	);
	icvm_returnerr();
}


void ICVM_mknod(void){
// NOT SUPPORTED IN ICVM
	R_R0=-1;
	R_ERRNO1=ICVM_EOPNOTSUPP;
}


void ICVM_chroot(void){
// NOT SUPPORTED IN ICVM
	R_R0=-1;
	R_ERRNO1=ICVM_EOPNOTSUPP;
}

void ICVM_close(void){
	R_R0=i_close(
		rSTKs32(0)
	);
	icvm_returnerr();
}

void ICVM_open(void){
	s32 flags=rSTKu32(1);
	s32 mode=0;
	if(flags & ICVM_O_CREAT)
		mode=rSTKu32(2);
	R_R0=i_open(
		(char *)Nptr(rSTKs32(0)),
		flags,
		mode);
	icvm_returnerr();
}

void ICVM_creat(void){
	s32 mode=rSTKu32(1);
	R_R0=i_open(
		(char *)Nptr(rSTKs32(0)),
		ICVM_O_TRUNC | ICVM_O_CREAT | ICVM_O_WRONLY,
		mode);
	icvm_returnerr();
}

void ICVM_umask(void){
	R_R0=i_umask(rSTKu32(0));
	R_ERRNO1=0;
}


void ICVM_pipe(void){
	icvm_fh fh[2];
	p32 ptr=rSTKs32(0);
	R_R0=i_pipe(fh);
	if(R_R0==0){
		WDs32(ptr,fh[0],0);
		WDs32(ptr+4,fh[1],0);
	}
	icvm_returnerr();
}


void ICVM_opendir(void){
	R_R0=Iptr(i_opendir(
		(char *)Nptr(rSTKs32(0))
	));
	icvm_returnerr();
}

void ICVM_closedir(void){
	R_R0=i_closedir(
		(ICVM_DIR *)Nptr(rSTKs32(0))
	);
	icvm_returnerr();
}

void ICVM_dirfd(void){
	R_R0=((ICVM_DIR *)Nptr(rSTKs32(0)))->fd;
}


void ICVM_telldir(void){
	R_R0=i_telldir(
		(ICVM_DIR *)Nptr(rSTKs32(0))
	);
	icvm_returnerr();
}

void ICVM_rewinddir(void){
	i_rewinddir(
		(ICVM_DIR *)Nptr(rSTKs32(0))
	);
}

void ICVM_seekdir(void){
	i_seekdir(
		(ICVM_DIR *)Nptr(rSTKs32(0)),
		rSTKs32(1)
	);
}



void ICVM_readdir(void){
	p32 di;
	struct ICVM_dirent *d=i_readdir(
		(ICVM_DIR *)Nptr(rSTKs32(0))
	);
	if(d==0){
		R_R0=0;
		icvm_returnerr();
		return;
	}
	di=Iptr(d->icvmdent);
	strcpy((char *)Nptr(di+ICVM_d_name),d->d_name);
	WDu32(di+ICVM_d_type,d->d_type,0);
	WDu32(di+ICVM_d_reclen,d->d_reclen,0);
	WDu32(di+ICVM_d_namlen,d->d_namlen,0);
	WDu32(di+ICVM_d_fileno,d->d_fileno,0);
	R_R0=di;
	
}


#ifndef __WIN32__

static void CopyStat(s8 *istat,struct stat *st){
	WDs32(istat+ICVM_ist_dev,st->st_dev,0);
	WDs32(istat+ICVM_ist_mode,
			TranslateModeToICVM(st->st_mode),0);
	WDu32(istat+ICVM_ist_uid,st->st_uid,0);
	WDu32(istat+ICVM_ist_gid,st->st_gid,0);
	WDs32(istat+ICVM_ist_rdev,st->st_rdev,0);
	WDu32(istat+ICVM_ist_nlink,st->st_nlink,0);
	WDu32(istat+ICVM_ist_ino,st->st_ino,0);
	WDu64(istat+ICVM_ist_size,st->st_size,0);
	WDu32(istat+ICVM_ist_atime,st->st_atime,0);
	WDu32(istat+ICVM_ist_mtime,st->st_mtime,0);
	WDu32(istat+ICVM_ist_ctime,st->st_ctime,0);
	WDu32(istat+ICVM_ist_atimensec,0,0);

#if defined(__NetBSD__)

	WDu32(istat+ICVM_ist_atimensec,st->st_atimensec,0);
	WDu32(istat+ICVM_ist_mtimensec,st->st_mtimensec,0);
	WDu32(istat+ICVM_ist_ctimensec,st->st_ctimensec,0);

#else

	WDs32(istat+ICVM_ist_atimensec,0,0);
	WDs32(istat+ICVM_ist_mtimensec,0,0);
	WDs32(istat+ICVM_ist_ctimensec,0,0);

#endif

	WDu32(istat+ICVM_ist_blksize,st->st_blksize,0);
	WDu64(istat+ICVM_ist_blocks,st->st_blocks,0);
	WDu32(istat+ICVM_ist_flags,0,0);

}


void ICVM_getdtablesize(void){
	R_R0=getdtablesize();
	if(R_R0==-1)
		R_R0=1024; // reasonable number for max open files
	icvm_SetErr();
}




static s32 TranslateOpenFlagsFromICVM(s32 icvmflags){
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


static s32 TranslateOpenFlagsToICVM(s32 icvmflags){
	s32 flags=0;
	if((icvmflags & O_RDONLY)==O_RDONLY)
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







/* lock operations for icvm flock(2) */
#define	ICVM_LOCK_SH		0x01		/* shared file lock */
#define	ICVM_LOCK_EX		0x02		/* exclusive file lock */
#define	ICVM_LOCK_NB		0x04		/* don't block when locking */
#define	ICVM_LOCK_UN		0x08		/* unlock file */

void ICVM_flock(void){
#if defined(BSD) || defined(__FreeBSD__) || defined(__NetBSD__)\
		|| defined(linux) || defined(__OpenBSD__)	
	s32 mode=rSTKu32(1);
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

	R_R0=flock(rSTKs32(0),mode);
#else
	R_R0=-1;
	errno=ICVM_EOPNOTSUPP;
#endif
	icvm_SetErr();
}



void ICVM_mkfifo(void){
	s32 mode=TranslateModeFromICVM(rSTKu32(1));
	R_R0=mkfifo(
		(char *)Nptr(rSTKs32(0)),
		mode);
	icvm_SetErr();
}



void ICVM_readlink(void){
	R_R0=readlink(
		(char *)Nptr(rSTKs32(0)),
		(char *)Nptr(rSTKu32(1)),
		rSTKu32(2));
	icvm_SetErr();
}







void ICVM_utimes(void){
	struct timeval tv;
	s8 *itv=(s8 *)Nptr(rSTKs32(1));
	TranslateTimeValFromICVM(itv,&tv);
	R_R0=utimes(
		(char *)Nptr(rSTKs32(0)),
		&tv);
	icvm_SetErr();
}





// tacky, really bad, ioctl call just passes everything through

void ICVM_ioctl(void){
	R_R0=ioctl(rSTKs32(0),
		rSTKs32(1),
		rSTKs32(2),
		rSTKs32(3),
		rSTKs32(4),
		rSTKs32(5),
		rSTKs32(6),
		rSTKs32(7)
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



void ICVM_fcntl(void){
	int filedes=rSTKs32(0);
	int cmd=rSTKs32(1);
	int arg1=rSTKs32(2);
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
			WDs16(arg1+ICVM_l_type,type,0);
			if(type!=ICVM_F_UNLCK)
				break;
			WDu64(arg1+ICVM_l_start,lock.l_start,0);
			WDu64(arg1+ICVM_l_len,lock.l_len,0);
			WDu32(arg1+ICVM_l_pid,lock.l_pid,0);
			whence=lock.l_whence;
			if(whence==SEEK_END)
				whence=ICVM_SEEK_END;
			else if(whence==SEEK_CUR)
				whence=ICVM_SEEK_CUR;
			else whence=ICVM_SEEK_SET;
			WDs16(arg1+ICVM_l_whence,whence,0);
			break;
		case ICVM_F_SETLK:
			cmd=F_SETLK;
			goto finishsetlk;
		case ICVM_F_SETLKW:
			cmd=F_SETLKW;
finishsetlk:
			lock.l_start=RDu64(arg1+ICVM_l_start,0);
			lock.l_len=RDu64(arg1+ICVM_l_len,0);
			lock.l_pid=RDs32(arg1+ICVM_l_pid,0);
			type=RDs16(arg1+ICVM_l_type,0);
			if(type==ICVM_F_RDLCK)
				type=F_RDLCK;
			else if(type==ICVM_F_WRLCK)
				type=F_WRLCK;
			else if(type==ICVM_F_UNLCK)
				type=F_UNLCK;
			else
				goto inval;
			lock.l_type=type;
			whence=RDs16(arg1+ICVM_l_whence,0);
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


#endif // !__WIN32__

