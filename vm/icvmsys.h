#ifndef ICMVSYS_H
#define ICVMSYS_H

#include "sys.h"

// Copyright (C) 2000,2001 Bob Dayley


// various optimization enable/disable
// these are just for reference - DO NOT USE!!!!
// basic ICVM types

#ifdef DO_NOT_USE_THESE

typedef s8 ICVM_char;
typedef u8 ICVM_uchar;
typedef s16 ICVM_short;
typedef u16 ICVM_ushort;
typedef s32 ICVM_long;
typedef s32 ICVM_int;
typedef u32 ICVM_ulong;


typedef	u8	ICVM_u_char;
typedef	u16	ICVM_u_short;
typedef	s32	ICVM_u_int;
typedef	u32	ICVM_u_long;
typedef s64 ICVM_quad_t; 
typedef u64 ICVM_u_quad_t; 

typedef	p32 ICVM_caddr_t;	
typedef	s32	ICVM_daddr_t;
typedef	s32	ICVM_dev_t;	
typedef	u32	ICVM_ino_t;	
typedef	s64	ICVM_off_t;			
typedef	s32	ICVM_nlink_t;
typedef	s32	ICVM_swblk_t;		
typedef	s32	ICVM_segsz_t;	
typedef	s32	ICVM_uid_t;		
typedef	s32	ICVM_gid_t;	
typedef	s32	ICVM_pid_t;
typedef	s32	ICVM_mode_t;	
typedef u32	ICVM_fixpt_t;	

typedef	u64 ICVM_u_quad;
typedef	s64	ICVM_quad;
typedef	p32	ICVM_qaddr_t;	

typedef u32 ICVM_clock_t;
typedef u32 ICVM_size_t;
typedef s32 ICVM_time_t;

#endif


extern FILE *icvmlogfile; // log file for kprintf - and process indepentent output




// these functions are defined in icvmfile.c

void ICVM_read(void);
void ICVM_write(void);
void ICVM_open(void);
void ICVM_creat(void);
void ICVM_mkfifo(void);
void ICVM_close(void);
void ICVM_link(void);
void ICVM_truncate(void);
void ICVM_ftruncate(void);
void ICVM_symlink(void);
void ICVM_readlink(void);
void ICVM_unlink(void);
void ICVM_mkdir(void);
void ICVM_rmdir(void);
void ICVM_rename(void);
void ICVM_isatty(void);
void ICVM_chdir(void);
void ICVM_fchdir(void);
void ICVM_mknod(void);
void ICVM_flock(void);
void ICVM_chmod(void);
void ICVM_fchmod(void);
void ICVM_chroot(void);
void ICVM_umask(void);
void ICVM_getcwd(void);
void ICVM_chown(void);
void ICVM_fchown(void);
void ICVM_lseek(void);
void ICVM_mount(void);
void ICVM_umount(void);
void ICVM_access(void);
void ICVM_dup(void);
void ICVM_dup2(void);
void ICVM_pipe(void);
void ICVM_fstat(void);
void ICVM_stat(void);
void ICVM_lstat(void);
void ICVM_opendir(void);
void ICVM_closedir(void);
void ICVM_readdir(void);
void ICVM_rewinddir(void);
void ICVM_dirfd(void);
void ICVM_getdtablesize(void);
void ICVM_utimes(void);
void ICVM_telldir(void);
void ICVM_seekdir(void);

// this files are in icvmid.c

void ICVM_getpid(void);
void ICVM_getppid(void);
void ICVM_getuid(void);
void ICVM_geteuid(void);
void ICVM_getegid(void);
void ICVM_setegid(void);
void ICVM_setregid(void);
void ICVM_getgid(void);
void ICVM_setgid(void);
void ICVM_setuid(void);
void ICVM_seteuid(void);
void ICVM_setreuid(void);
void ICVM_gethostid(void);
void ICVM_sethostid(void);

void ICVM_getpriority(void);
void ICVM_setpriority(void);
	
void ICVM_getgrgid(void);
void ICVM_getgrnam(void);
void ICVM_getlogin(void);
void ICVM_getpgrp(void);
void ICVM_getpwuid(void);
void ICVM_getpwnam(void);
void ICVM_getpwent(void);
void ICVM_setpwent(void);
void ICVM_endpwent(void);
void ICVM_getgrent(void);
void ICVM_setgrent(void);
void ICVM_endgrent(void);


// these functions are in icvmstring.c

s32 ICVM_strcat(void);
s32 ICVM_strcmp(void);
s32 ICVM_strcpy(void);
void ICVM_strcspn(void);
s32 ICVM_strlen(void);
s32 ICVM_strncpy(void);
s32 ICVM_strncat(void);
s32 ICVM_strncmp(void);
void ICVM_strpbrk(void);
void ICVM_strspn(void);
void ICVM_strstr(void);
void ICVM_strtod(void);
void ICVM_strtol(void);
void ICVM_strtoul(void);
void ICVM_strtok(void);
void ICVM_strchr(void);
void ICVM_strrchr(void);


char *i_strdup(char *p);

s32 ICVM_memcpy(void);
s32 ICVM_memmove(void);
void ICVM_memccpy(void);
s32 ICVM_memcmp(void);
s32 ICVM_memset(void);
void ICVM_memchr(void);
s32 ICVM_bcmp(void);
void ICVM_bcopy(void);
void ICVM_bzero(void);
void ICVM_swab(void);
void ICVM_strdup(void);
void ICVM_strcasecmp(void);
void ICVM_strncasecmp(void);
void ICVM_ffs(void);
void ICVM_ioctl(void);
void ICVM_fcntl(void);

// these functions are in icvmproc.c or proc.cc

void HandleSignals(void);
void HandleAttn(void);
void ICVM_nanosleep(void);
void ICVM_kill(void);
void ICVM_sigreturn(void);
void ICVM_setsigreturn(void);
void ICVM_sigaction(void);
void ICVM_sigsetjmp(void);
void ICVM_siglongjmp(void);
void ICVM_sigprocmask(void);
void ICVM_sigpending(void);
void ICVM_sigsuspend(void);
void ICVM_wait4(void);
void ICVM_execve(void);

// these functions are in icvmnet.c

void ICVM_socketpair(void);
void ICVM_socket(void);
void ICVM_listen(void);
void ICVM_accept(void);
void ICVM_getsockname(void);
void ICVM_bind(void);
void ICVM_connect(void);
void ICVM_setsockopt(void);
void ICVM_getsockopt(void);


void ICVM_sendto(void);
void ICVM_recvfrom(void);
void ICVM_select(void);
void ICVM_shutdown(void);
void ICVM_getdomainname(void);
void ICVM_setdomainname(void);

void ICVM_gethostbyname(void);
void ICVM_gethostbyaddr(void);

void TranslateTimeValFromICVM(s8 *from,struct timeval *to);

// these functions are in icvmfast.c

s32 ICVM_fixed32mul(void);
u32 ICVM_fixed32mulu(void);
s32 ICVM_fixed32div(void);
u32 ICVM_fixed32divu(void);

#endif



