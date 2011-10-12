#include "cvm.h"
#include "icvmsys.h"
#include "icvmerr.h"

// Copyright (C) 2000,2001 Bob Dayley


#ifdef __WIN32__
#include <stdlib.h>
#include <float.h>
#endif // __WIN32__

#ifdef UNIXSYSTEM

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/shm.h>


#endif // UNIXSYSTEM

// get icvm base root path - where sub etc directories etc are kept.
// may be overridden by using environment variable ICVMPATH



void ICVM_icvmbasepath(void){
	char *p;
	p=getenv("ICVMPATH");
	if(p==0)
//		p="/usr/local/icvm";
		p="/";
	R_R0=Iptr(p);
};


void ICVM_isbigendian(void){
#ifdef BIGENDIAN
	R_R0=1;
#else
	R_R0=0;
#endif
};




void ICVM_malloc(void){
	R_R0=Iptr(i_malloc(
		rSTKs32(0)
	));
}

void ICVM_realloc(void){
	R_R0=Iptr(i_realloc(
		(void *)Nptr(rSTKs32(0)),
		rSTKs32(1)
	));
}

void ICVM_free(void){
	i_free(
		(void *)Nptr(rSTKs32(0))
	);
}

// icvm timeval offsets 
#define ICVM_tv_sec		0
#define ICVM_tv_usec	4 
#define ICVM_tv_sizeof	8 



void ICVM_gettimeofday(void){
	struct ICVM_timeval ts;
	p32 td=rSTKs32(0);
	R_R0=i_gettimeofday(&ts,(struct ICVM_timezone *)0);
	WDs32(td+ICVM_tv_sec,ts.tv_sec,0);
	WDs32(td+ICVM_tv_usec,ts.tv_usec,0);
	icvm_returnerr();

}



void ICVM_settimeofday(void){
#ifdef NOT_NOW
// disabled for security
	struct timeval ts;
	s8 *td=(s8 *)rSTKs32(0);
	ts.tv_sec=RDu32(td+ICVM_tv_sec);
	ts.tv_usec=RDu32(td+ICVM_tv_usec);
	R_R0=settimeofday(&ts,(struct timezone *)0);
	icvm_SetErr();
#endif
	R_R0=-1;
	R_ERRNO1=ICVM_EOPNOTSUPP;

}


#ifndef __WIN32__

// ***** ICVM MMAP FLAGS, ETC.

// icvm mmap protection bits
#define	ICVM_PROT_READ	0x04	/* pages can be read */
#define	ICVM_PROT_WRITE	0x02	/* pages can be written */
#define	ICVM_PROT_EXEC	0x01	/* pages can be executed */

static int TransMMProtectFromICVM(int inbits){
	int outbits=0;
	if(inbits & ICVM_PROT_READ)
		outbits |= PROT_READ;
	if(inbits & ICVM_PROT_WRITE)
		outbits |= PROT_WRITE;
	if(inbits & ICVM_PROT_EXEC)
		outbits |= PROT_EXEC;
	return outbits;
}


#ifdef NOT_NOW

// icvm mapping types 
#define	ICVM_MAP_FILE	0x0001	/* mapped from a file or device */
#define	ICVM_MAP_ANON	0x0002	/* allocated from memory, swap space */

static int TransMMTypeFromICVM(int inbits){
	int outbits=0;
	if(inbits & ICVM_MAP_FILE)
		outbits |= MAP_FILE;
	if(inbits & ICVM_MAP_ANON)
		outbits |= MAP_ANON;
	return outbits;
}


#endif // NOT_NOW

// icvm mapping flags
#define	ICVM_MAP_FIXED		0x0020	/* "copy" region at mmap time */
#define	ICVM_MAP_SHARED		0x0010	/* share changes */
#define	ICVM_MAP_PRIVATE	0x0000	/* changes are private */
#define	ICVM_MAP_ANON		0x1000	/* annonymous mapping */

static int TransMMShareFromICVM(int inbits){
	int outbits=0;
								
	if(inbits & ICVM_MAP_SHARED) // MUST be very first
		outbits=MAP_SHARED;
	else
		outbits=MAP_PRIVATE;
	if(inbits & ICVM_MAP_FIXED)
		outbits |= MAP_FIXED;	
	if(inbits & ICVM_MAP_ANON)
		outbits |= MAP_ANON;	
	return outbits;
}


// icvm madvice
#define	ICVM_MADV_NORMAL	0	/* no further special treatment */
#define	ICVM_MADV_RANDOM	1	/* expect random page references */
#define	ICVM_MADV_SEQUENTIAL	2	/* expect sequential page references */
#define	ICVM_MADV_WILLNEED	3	/* will need these pages */
#define	ICVM_MADV_DONTNEED	4	/* dont need these pages */

// map failure
#define ICVM_MAP_FAILED	-1


void ICVM_mmap(void){
	R_R0=(p32)mmap(
		(void *)rSTKs32(0),
		rSTKu32(1),
		TransMMProtectFromICVM(rSTKu32(2)),
		TransMMShareFromICVM(rSTKu32(3)),
		rSTKu32(4),
		rSTKu32(5));
	icvm_SetErr();
#ifdef MAP_FAILED
	if(R_R0 == (s32)MAP_FAILED)
#else
	if(R_R0 == (s32)-1 || R_R0==0)
#endif
		R_R0=ICVM_MAP_FAILED; 
}

void ICVM_munmap(void){
	R_R0=munmap(
		(void *)rSTKs32(0),
		rSTKu32(1));
	icvm_SetErr();
}


// ********* shared memory functions

#define ICVM_IPC_CREAT	01000		/* Create key if key does not exist. */
#define ICVM_IPC_EXCL	02000		/* Fail if key exists.  */
#define ICVM_IPC_NOWAIT	04000		/* Return error on wait.  */

void ICVM_shmget(void){
	s32 iattr=rSTKs32(2);
    s32 attr=iattr & 0777;
    if(iattr & ICVM_IPC_CREAT)
		attr|=IPC_CREAT;
    if(iattr & ICVM_IPC_EXCL)
		attr|=IPC_EXCL;
    if(iattr & ICVM_IPC_NOWAIT)
		attr|=IPC_NOWAIT;
	R_R0=shmget(rSTKs32(0),
		rSTKu32(1),attr);
	icvm_SetErr();
}

#define	ICVM_SHM_RDONLY	010000	/* Attach read-only (else read-write) */
#define	ICVM_SHM_RND	020000	/* Round attach address to SHMLBA */
	
void ICVM_shmat(void){
	s32 iflag=rSTKs32(2);
	s32 flag=0;
    if(iflag & ICVM_SHM_RDONLY)
		flag|=SHM_RDONLY;
    if(iflag & ICVM_SHM_RND)
		flag|=SHM_RND;
	R_R0=(p32)shmat(rSTKs32(0),
		(void *)rSTKu32(1),flag);
	icvm_SetErr();
}


void ICVM_shmdt(void){
	R_R0=shmdt((void *)rSTKs32(0));
	icvm_SetErr();
}

#define	ICVM_IPC_RMID	0	/* remove identifier */
#define	ICVM_IPC_SET		1	/* set options */
#define	ICVM_IPC_STAT	2	/* get options */

// IPC PERM structure

#define ICVM_ipc_cuid	0	/* creator user id */
#define ICVM_ipc_cgid	2	/* creator group id */
#define ICVM_ipc_uid	4	/* user id */
#define	ICVM_ipc_gid	6	/* group id */
#define	ICVM_ipc_mode	8	/* r/w permission */
#define ICVM_ipc_seq	10	/* sequence # (to generate unique msg/sem/shm
				   id) */
#define ICVM_ipc_key		12		/* user specified msg/sem/shm key */
#define ICVM_ipcperm_size	16

void TranslateIPCPermToICVM(s8 *dst,struct ipc_perm *src){
	memset(dst,0,ICVM_ipcperm_size);
	WDs16(dst+ICVM_ipc_cuid,src->cuid,0);
	WDs16(dst+ICVM_ipc_cgid,src->cgid,0);
	WDs16(dst+ICVM_ipc_uid,src->uid,0);
	WDs16(dst+ICVM_ipc_gid,src->gid,0);
	WDs16(dst+ICVM_ipc_mode,src->mode,0);
};


#define ICVM_shm_perm		0
#define ICVM_shm_segsz	(ICVM_ipcperm_size+0)	/* size of segment in bytes */
#define ICVM_shm_lpid	(ICVM_shm_segsz+4)	/* process ID of last shm op */
#define ICVM_shm_cpid	(ICVM_shm_lpid+4)	/* process ID of creator */
#define ICVM_shm_nattch	(ICVM_shm_cpid+4)	/* number of current attaches */
#define ICVM_shm_atime	(ICVM_shm_nattch+4)	/* time of last shmat() */
#define ICVM_shm_dtime	(ICVM_shm_atime+4) /* time of last shmdt() */
#define ICVM_shm_ctime	(ICVM_shm_dtime+4) /* time of last change by shmctl() */
#define ICVM_shm_internal (ICVM_shm_ctime+4)/* sysv stupidity */

void ICVM_shmctl(void){
	s32 id=rSTKs32(0);
	s32 icmd=rSTKs32(1);
	s8 *ids=(s8 *)rSTKs32(2);
	struct shmid_ds ds;
	switch(icmd){

		case ICVM_IPC_RMID:
			R_R0=shmctl(id,IPC_RMID,0);
			break;

		case ICVM_IPC_SET:
			if(ids==0)
				goto badparm;
			ds.shm_perm.uid=RDs16(ids+ICVM_ipc_uid,0);
			ds.shm_perm.gid=RDs16(ids+ICVM_ipc_gid,0);
			ds.shm_perm.mode=RDs16(ids+ICVM_ipc_mode,0);
			R_R0=shmctl(id,IPC_SET,&ds);
			break;

		case ICVM_IPC_STAT:
			if(ids==0)
				goto badparm;
			R_R0=shmctl(id,IPC_STAT,&ds);
			if(R_R0<0)
				break;
			TranslateIPCPermToICVM(ids,&ds.shm_perm);
			WDs32(ids+ICVM_shm_segsz,ds.shm_segsz,0);
			WDs32(ids+ICVM_shm_lpid,ds.shm_lpid,0);
			WDs32(ids+ICVM_shm_cpid,ds.shm_cpid,0);
			WDs32(ids+ICVM_shm_atime,ds.shm_atime,0);
			WDs32(ids+ICVM_shm_dtime,ds.shm_dtime,0);
			WDs32(ids+ICVM_shm_ctime,ds.shm_ctime,0);
			WDs32(ids+ICVM_shm_nattch,ds.shm_nattch,0);
			break;
	
		default:
badparm:
			R_R0=-1;
			R_ERRNO1=ICVM_EINVAL;
			return;
	}
	icvm_SetErr();
}






#endif // !__WIN32__

s32 debugparms[4];

void ICVM_setdebug(void){
	debugparms[0]=rSTKs32(0);
	debugparms[1]=rSTKs32(1);
	debugparms[2]=rSTKs32(2);
	debugparms[3]=rSTKs32(3);
	ICVM_SETBREAK(ICVM_DEBUG);
}


// custom debug code
void ICVM_debug(void){
}


extern FILE *icvmlogfile; // log file for kprintf - 

// does not yet understand 64 bits

void ICVM_kprintf(void){
	R_R0=fprintf(icvmlogfile,
		(char *)Nptr(rSTKs32(0)),
		rSTKs32(1),
		rSTKs32(2),
		rSTKs32(3),
		rSTKs32(4),
		rSTKs32(5),
		rSTKs32(6),
		rSTKs32(7),
		rSTKs32(8),
		rSTKs32(9),
		rSTKs32(10),
		rSTKs32(11),
		rSTKs32(12)
	);
	icvm_SetErr();
}

void ICVM_exit(void){ICVM_SETBREAK(ICVM_EXIT);};

void ICVM_ceil(void){
	s32 id;
	R_FP0=ceil(
		rSTKf64(0)
	);
	icvm_SetErr();
}

void ICVM_floor(void){
	s32 id;
	R_FP0=floor(
		rSTKf64(0)
	);
	icvm_SetErr();
}


void ICVM_sleep(void){
#ifndef __WIN32__
	R_R0=sleep(rSTKs32(0));
#endif // ! __WIN32__
#ifdef __WIN32__
// *********** NEED TO REPLACE WITH REAL POSIX SLEEP !!!!! ********
	_sleep(rSTKs32(0));
	R_R0=0;
#endif //  __WIN32__
}





void ICVM_modf(void){
	double id;
	R_FP0=modf(
		rSTKf64(0),
	&id);
	WDf64(rSTKs32(1),id,0);
	icvm_SetErr();
}

void ICVM_frexp(void){
	s32 id;
	R_FP0=frexp(
		rSTKf64(0),
	&id);
	WDs32(rSTKs32(1),id,0);
	icvm_SetErr();
}

void ICVM_hypot(void){
	double a=rSTKf64(0);
	double b=rSTKf64(1);
	R_FP0=sqrt(a*a+b*b);
	icvm_SetErr();
}

void ICVM_ldexp(void){
	s32 id;
	R_FP0=ldexp(
		rSTKf64(0),
		rSTKs32(1));
	icvm_SetErr();
}

void ICVM_fork(void){
#ifdef __WIN32__
	R_R0=i_artificialfork();
#else // __WIN32__
	R_R0=fork();
	if(R_R0==0)
		i_initchildfork();
	icvm_SetErr();
#endif // __WIN32__

};

void ICVM_vfork(void){
	return ICVM_fork(); // gets explosive when we use real thing
};


void ICVM_isnan(void){
#ifndef __WIN32__
	R_R0=isnan(
		rSTKf64(0)
	);
#endif // ! __WIN32__
#ifdef __WIN32__
	R_R0=_isnan(
		rSTKf64(0)
	);
#endif //  __WIN32__
}

void ICVM_isinf(void){
#ifdef NOT_NOW
	R_R0=isinf(
		rSTKf64(0)
	);
#endif
#ifndef __WIN32__
	R_R0=!finite(
		rSTKf64(0)
	);
#endif // ! __WIN32__
#ifdef __WIN32__
	R_R0=!_finite(
		rSTKf64(0)
	);
#endif //  __WIN32__
}

void ICVM_getpagesize(void){
	R_R0=i_getpagesize();
};


#ifndef __WIN32__

// debug optimizer trace
void ICVMOptTrace(int opcode,int offset){
	fprintf(icvmlogfile,"$%04.4x %d\n",opcode,offset);
};	








void ICVM_sync(void){sync();R_R0=0;};






#define	ICVM_ITIMER_REAL	0
#define	ICVM_ITIMER_VIRTUAL	1
#define	ICVM_ITIMER_PROF	2

// offsets into itimerval from ICVM - NOTE - the two members are timeval structs
#define ICVM_it_interval	0
#define ICVM_it_value		ICVM_tv_sizeof



/* ******* itimerval struct - for reference only
struct	itimerval {
	struct	timeval it_interval;	
	struct	timeval it_value;	
};

********* */

void ICVM_getitimer(void){
	struct itimerval ts;
	int which=rSTKs32(0);
	s8 *td=(s8 *)rSTKs32(1);
	if(which==ICVM_ITIMER_REAL)
		which=ICVM_ITIMER_REAL;
	else if(which==ICVM_ITIMER_VIRTUAL)
		which=ICVM_ITIMER_VIRTUAL;
	else if(which==ICVM_ITIMER_PROF)
		which=ICVM_ITIMER_PROF;
	else{
		R_R0=-1;
		errno=EINVAL;
		goto done;				
	}
	R_R0=getitimer(which,&ts);
	if(R_R0<0)
		goto done;
	WDs32(td+ICVM_it_interval+ICVM_tv_sec,ts.it_interval.tv_sec,0);
	WDs32(td+ICVM_it_interval+ICVM_tv_usec,ts.it_interval.tv_usec,0);
	WDs32(td+ICVM_it_value+ICVM_tv_sec,ts.it_value.tv_sec,0);
	WDs32(td+ICVM_it_value+ICVM_tv_usec,ts.it_value.tv_usec,0);

done:
	icvm_SetErr();
}


void ICVM_setitimer(void){
	struct itimerval ts;
	struct itimerval ots;
	int which=rSTKs32(0);
	s8 *td=(s8 *)rSTKs32(1);
	s8 *otd=(s8 *)rSTKs32(2);
	if(which==ICVM_ITIMER_REAL)
		which=ICVM_ITIMER_REAL;
	else if(which==ICVM_ITIMER_VIRTUAL)
		which=ICVM_ITIMER_VIRTUAL;
	else if(which==ICVM_ITIMER_PROF)
		which=ICVM_ITIMER_PROF;
	else{
		R_R0=-1;
		errno=EINVAL;
		goto done;				
	}
	ts.it_interval.tv_sec=RDs32(td+ICVM_it_interval+ICVM_tv_sec,0);
	ts.it_interval.tv_usec=RDs32(td+ICVM_it_interval+ICVM_tv_usec,0);
	ts.it_value.tv_sec=RDs32(td+ICVM_it_value+ICVM_tv_sec,0);
	ts.it_value.tv_usec=RDs32(td+ICVM_it_value+ICVM_tv_usec,0);
	R_R0=setitimer(which,&ts,&ots);
	if(R_R0<0)
		goto done;

	if(otd){
		WDs32(otd+ICVM_it_interval+ICVM_tv_sec,ots.it_interval.tv_sec,0);
		WDs32(otd+ICVM_it_interval+ICVM_tv_usec,ots.it_interval.tv_usec,0);
		WDs32(otd+ICVM_it_value+ICVM_tv_sec,ots.it_value.tv_sec,0);
		WDs32(otd+ICVM_it_value+ICVM_tv_usec,ots.it_value.tv_usec,0);
	}

done:
	icvm_SetErr();
}


	



#endif // !__WIN32__

#ifdef __WIN32__
#define I_SYSCALL(call) 0
#else
#define I_SYSCALL(call) call
#endif

// base system calls always hardwired into ICVM


SysCallFunction *BaseSysCalls[1024]={
	0, 	//  0 - always dead
	ICVM_exit,		//  1
	ICVM_fork,		//  2
	ICVM_read,		//  3
	ICVM_write,		//  4
	ICVM_open,		//  5
	ICVM_close,		//  6
	I_SYSCALL(ICVM_wait4), // wait4  7
	ICVM_creat,		//  8
	ICVM_link,		//  9
	ICVM_unlink,	//  10

	ICVM_kprintf, 	//  11

	ICVM_chdir,		//  12
	ICVM_fchdir,	//  13
	ICVM_mknod,		// 14
	ICVM_chmod,		// 15
	ICVM_chown,		//  16
	0, // break		//  17
	0, // getfsstat	//  18
	ICVM_lseek,		//  19
	ICVM_getpid,	// 	20 
	0, // NO LONGER SUPPORTED I_SYSCALL(ICVM_mount),		//  21
	0, // NO LONGER SUPPORTED I_SYSCALL(ICVM_umount),	// 	22
	I_SYSCALL(ICVM_setuid),	//  23
	ICVM_getuid,	//  24
	I_SYSCALL(ICVM_geteuid),	//  25
	0, // ptrace	//  26
	0, // recvmsg	//  27
	0, // sendmsg	//  28
	I_SYSCALL(ICVM_recvfrom), // recvfrom	//  29
	I_SYSCALL(ICVM_accept), // accept	//  30
	0, // getpeername // 31
	I_SYSCALL(ICVM_getsockname), // getsockname // 32
	ICVM_access,	//   33
	ICVM_frexp, // frexp	  34
	ICVM_ldexp, // ldexp  35
	I_SYSCALL(ICVM_sync),		//   36
	ICVM_kill,		//   37
	ICVM_stat, // stat),		//   38
	ICVM_getppid,	//   39
	ICVM_lstat, // lstat),	//   40
	ICVM_dup,		//   41
	ICVM_pipe,		//   42
	I_SYSCALL(ICVM_getegid),	//   43
	I_SYSCALL(ICVM_getdomainname), // getdomainname),	//   44
	ICVM_isatty, // isatty),	//   45
	ICVM_sigaction, // sigaction), //  46
	I_SYSCALL(ICVM_getgid),	//   47
	ICVM_sigprocmask, // sigprocmask), // 48 
	I_SYSCALL(ICVM_getlogin), // getlogin),	//   49
	0, // setlogin),	//   50
	0, // acct),		//   51
	ICVM_sigpending, // sigpending), // 52
	I_SYSCALL(ICVM_setdomainname), // 53
	I_SYSCALL(ICVM_ioctl), // ioctl), //  54
	0, // reboot), //  55
	0, // revoke), //  56
	ICVM_symlink,		//  57
	I_SYSCALL(ICVM_readlink), // readlink), //  58
	I_SYSCALL(ICVM_execve), // execve), //  59
	ICVM_umask, // umask), //  60
	ICVM_chroot, // chroot), //  61
	ICVM_fstat, // fstat), //  62
	ICVM_setdebug, // setdebug //  63
	ICVM_getpagesize, // getpagesize  64
	0, // msync //  65
	ICVM_vfork, // vfork //  66
	ICVM_malloc, // malloc  //  67
	ICVM_free, // free  //  68
	ICVM_realloc, // realloc //  69
	0, // sstk //  70
	I_SYSCALL(ICVM_mmap), // mmap //  71
	0, // vadvise //  72
	I_SYSCALL(ICVM_munmap), // munmap //  73
	0, // protect //  74
	0, // madvise //  75
	ICVM_sleep, // sleep //  76
	ICVM_getcwd, // getcwd  //  77
	0,//mincore	78
	0,//getgroups	79
	0,//setgroups	80
	ICVM_getpgrp,//getpgrp	81
	0,//setpgid	82
	I_SYSCALL(ICVM_setitimer),//setitimer	83
	ICVM_icvmbasepath, // 84
	0,//swapon	85
	I_SYSCALL(ICVM_getitimer),//getitimer	86
	I_SYSCALL(ICVM_gethostid),//gethostname	87
	I_SYSCALL(ICVM_sethostid),//sethostname	88
	I_SYSCALL(ICVM_getdtablesize),//getdtablesize	89
	ICVM_dup2,//dup2	90
	0,			/* 91 */
	I_SYSCALL(ICVM_fcntl),//fcntl	92
	I_SYSCALL(ICVM_select),//select	93
	0,			/* 94 */
	0,//fsync	95
	I_SYSCALL(ICVM_setpriority),//setpriority	96
	I_SYSCALL(ICVM_socket),//socket	97
	I_SYSCALL(ICVM_connect),//connect	98
	ICVM_nanosleep,			/* 99 nanosleep */
	I_SYSCALL(ICVM_getpriority),//getpriority	100
	0,			/* 101 is old send */
	0,			/* 102 is old recv */
	ICVM_sigreturn,//sigreturn	103
	I_SYSCALL(ICVM_bind), //bind	104
	I_SYSCALL(ICVM_setsockopt),//setsockopt	105
	I_SYSCALL(ICVM_listen),//listen	106
	0,			/* 107 is obsolete vtimes */
	0,			/* 108 is old sigvec */
	0,			/* 109 is old sigblock */
	0,			/* 110 is old sigsetmask */
	I_SYSCALL(ICVM_sigsuspend),//sigsuspend	111
	ICVM_modf,	// modf	112
	ICVM_sigsetjmp, /* 113 */ 
	ICVM_siglongjmp,/* 114  */
	0,//vtrace	115
	ICVM_gettimeofday,//gettimeofday	116
	0,//getrusage	117
	I_SYSCALL(ICVM_getsockopt),//getsockopt	118
	0,//resuba	119
	ICVM_hypot,//	120
	0,//	121
	ICVM_settimeofday,//settimeofday	122
	ICVM_fchown,//fchown	123
	ICVM_fchmod,//fchmod	124
	0,			/* 125 is old recvfrom */
	I_SYSCALL(ICVM_setreuid),//setreuid	126	/* compatibility; still used by libc */
	I_SYSCALL(ICVM_setregid),//setregid	127	/* compatibility; still used by libc */
	ICVM_rename,	//rename	128
	ICVM_truncate,//truncate	129
	ICVM_ftruncate,//ftruncate	130
	I_SYSCALL(ICVM_flock),//flock	131
	I_SYSCALL(ICVM_mkfifo),//mkfifo	132
	I_SYSCALL(ICVM_sendto),//sendto	133
	I_SYSCALL(ICVM_shutdown),//shutdown	134
	I_SYSCALL(ICVM_socketpair),//socketpair	135
	ICVM_mkdir,//mkdir	136
	ICVM_rmdir,//rmdir	137
	I_SYSCALL(ICVM_utimes),//utimes	138
	ICVM_setsigreturn, // 139		
	0,//adjtime	140
	0,			/* 141 is old getpeername */
	0,//gethostid	142
	0,//sethostid	143
	0,//getrlimit	144
	0,//setrlimit	145
	0,			/* 146 is old killpg */
	0,//setsid	147
	0,//quotactl	148
	ICVM_getgrgid,	/* 149 */
	I_SYSCALL(ICVM_getgrnam),	/* 150 */
	I_SYSCALL(ICVM_getpwnam),	/* 151  */
	ICVM_getpwuid,	/* 152  */
	I_SYSCALL(ICVM_gethostbyname),	/* 153  */
	I_SYSCALL(ICVM_gethostbyaddr),	/* 154  */
	0,//nfssvc	155
	0,//getdirentries	156
	0,//statfs	157
	0,//fstatfs	158
	0,			/* 159  */
	0,//async_daemon	160
	0,//getfh	161
	ICVM_opendir,/* 162  */
	ICVM_readdir,/* 163  */
	ICVM_closedir,/* 164  */
	ICVM_rewinddir,/* 165  */
	ICVM_dirfd,	/* 166  */
	ICVM_floor,	/* 167  */
	ICVM_ceil,	/* 168  */
	ICVM_seekdir,	/* 169  */
	ICVM_telldir,	/* 170  */
	0,//shmsys	171
	I_SYSCALL(ICVM_getpwent),			/* 172  */
	I_SYSCALL(ICVM_setpwent),			/* 173  */
	I_SYSCALL(ICVM_endpwent),			/* 174  */
	I_SYSCALL(ICVM_getgrent),			/* 175  */
	I_SYSCALL(ICVM_setgrent),			/* 176  */
	I_SYSCALL(ICVM_endgrent),			/* 177  */
	0,			/* 178  */
	0,			/* 179  */
	0,			/* 180  */
	I_SYSCALL(ICVM_setgid),//setgid	181
	I_SYSCALL(ICVM_setegid),//setegid	182
	I_SYSCALL(ICVM_seteuid),//seteuid	183
	ICVM_isnan,//isnan	184
	ICVM_isinf,//isinf	185
	ICVM_memcpy,//	186
	ICVM_isbigendian,//	187
	ICVM_memmove,//	188
	ICVM_strchr,//	189
	ICVM_strrchr,//	190
	ICVM_strcat,//	191
	ICVM_strcmp,//	192
	ICVM_strcpy,//	193
	ICVM_strcspn,//	194
	ICVM_strlen,//	195
	ICVM_strncat,//	196
	ICVM_strncmp,//	197
	ICVM_strncpy,//	198
	ICVM_strpbrk,//	199
	ICVM_strspn,//	200
	ICVM_strstr,//	201
	ICVM_strtok,//	202
	ICVM_bcmp,//	203
	ICVM_bcopy,//	204
	ICVM_bzero,//	205
	ICVM_memccpy,//	206
	ICVM_memchr,//	207
	ICVM_memcmp,//	208
	ICVM_memset,//	209
	0,//	210
	0,//	211
	ICVM_swab,//	212
	0,//	213
	ICVM_strdup,//	214
	ICVM_strcasecmp,//	215
	ICVM_strncasecmp,//	216
	ICVM_ffs,//	217
	I_SYSCALL(ICVM_shmctl), // 218
	I_SYSCALL(ICVM_shmdt), // 219
	I_SYSCALL(ICVM_shmat), // 220
	I_SYSCALL(ICVM_shmget), // 221

};




