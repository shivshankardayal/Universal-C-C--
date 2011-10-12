/*	$NetBSD: unistd.h,v 1.73 1999/03/26 22:23:57 sommerfe Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Klaus Klein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)unistd.h	8.12 (Berkeley) 4/27/95
 */

#ifndef _UNISTD_H_
#define	_UNISTD_H_

#include <sys/cdefs.h>
#include <sys/featuretest.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/syscall.h>


/*
 * IEEE Std 1003.1-90
 */
#define	STDIN_FILENO	0	/* standard input file descriptor */
#define	STDOUT_FILENO	1	/* standard output file descriptor */
#define	STDERR_FILENO	2	/* standard error file descriptor */

#ifndef NULL
#define	NULL		0	/* null pointer constant */
#endif

__BEGIN_DECLS
__dead	 void _exit __P((int)) __attribute__((__noreturn__));
int	 access __P((const char *, int)) ICVMERRCALL(SYS_access);
unsigned int alarm __P((unsigned int));
int	 chdir __P((const char *)) ICVMERRCALL(SYS_chdir);
#if defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
int	chown __P((const char *, uid_t, gid_t)) ICVMERRCALL(SYS_chown);
#else
int	chown __P((const char *, uid_t, gid_t)) ICVMERRCALL(SYS_chown);
#endif /* defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE) */
int	 close __P((int)) ICVMERRCALL(SYS_close);
size_t	 confstr __P((int, char *, size_t));
char	*cuserid __P((char *));	/* obsolete */
int	 dup __P((int)) ICVMERRCALL(SYS_dup);
int	 dup2 __P((int, int))ICVMERRCALL(SYS_dup2);
int	 execl __P((const char *, const char *, ...));
int	 execle __P((const char *, const char *, ...));
int	 execlp __P((const char *, const char *, ...));
int	 execv __P((const char *, char * const *));
int	 execve __P((const char *, char * const *, char * const *)) ICVMERRCALL(SYS_execve);
int	 execvp __P((const char *, char * const *));
pid_t	 fork __P((void)) ICVMERRCALL(SYS_fork);
long	 fpathconf __P((int, int));
char	*getcwd __P((char *, size_t))ICVMERRCALL(SYS_getcwd);
gid_t	 getegid __P((void)) ICVMERRCALL(SYS_getegid);
uid_t	 geteuid __P((void)) ICVMERRCALL(SYS_geteuid);
gid_t	 getgid __P((void)) ICVMERRCALL(SYS_getgid);
int	 getgroups __P((int, gid_t []))ICVMERRCALL(SYS_getgroups);
__aconst char *getlogin __P((void)) ICVMERRCALL(SYS_getlogin);
pid_t	 getpgrp __P((void)) ICVMERRCALL(SYS_getpgrp);
pid_t	 getpid __P((void)) ICVMERRCALL(SYS_getpid);
pid_t	 getppid __P((void)) ICVMERRCALL(SYS_getppid);
uid_t	 getuid __P((void)) ICVMERRCALL(SYS_getuid);
int	 isatty __P((int)) ICVMERRCALL(SYS_isatty);
int	 link __P((const char *, const char *)) ICVMERRCALL(SYS_link);
off_t	 lseek __P((int, off_t, int)) ICVMERRCALL(SYS_lseek);
long	 pathconf __P((const char *, int));
int	 pause __P((void));
int	 pipe __P((int *)) ICVMERRCALL(SYS_pipe);
ssize_t	 read __P((int, void *, size_t)) ICVMERRCALL(SYS_read);
int	 rmdir __P((const char *))ICVMERRCALL(SYS_rmdir);
int	 setgid __P((gid_t)) ICVMERRCALL(SYS_setgid);
int	 setpgid __P((pid_t, pid_t)) ICVMERRCALL(SYS_setpgid);
pid_t	 setsid __P((void))ICVMERRCALL(SYS_setsid);
int	 setuid __P((uid_t)) ICVMERRCALL(SYS_setuid);
unsigned int	 sleep __P((unsigned int));
long	 sysconf __P((int));
pid_t	 tcgetpgrp __P((int));
int	 tcsetpgrp __P((int, pid_t));
__aconst char *ttyname __P((int));
int	 unlink __P((const char *)) ICVMERRCALL(SYS_unlink);
ssize_t	 write __P((int, const void *, size_t)) ICVMERRCALL(SYS_write);


/*
 * IEEE Std 1003.2-92, adopted in X/Open Portability Guide Issue 4 and later
 */
#if (!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
    (_POSIX_C_SOURCE - 0) >= 2 || (_XOPEN_SOURCE - 0) >= 4
int	 getopt __P((int, char * const *, const char *));

extern	 char *optarg;			/* getopt(3) external variables */
extern	 int opterr;
extern	 int optind;
extern	 int optopt;
#endif


/*
 * IEEE Std 1003.1b-93,
 * also found in X/Open Portability Guide >= Issue 4 Verion 2
 */
#if (!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
    (_POSIX_C_SOURCE - 0) >= 199309L || \
    (defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED)) || \
    (_XOPEN_SOURCE - 0) >= 500
int	 ftruncate __P((int, off_t)) ICVMERRCALL(SYS_ftruncate);
#endif


/*
 * IEEE Std 1003.1b-93, adopted in X/Open CAE Specification Issue 5 Version 2
 */
#if (!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
    (_POSIX_C_SOURCE - 0) >= 199309L || (_XOPEN_SOURCE - 0) >= 500
int	 fdatasync __P((int));
int	 fsync __P((int)) ICVMERRCALL(SYS_fsync);
#endif


/*
 * X/Open Portability Guide, all issues
 */
#if !defined(_POSIX_C_SOURCE) || defined(_XOPEN_SOURCE)
int	 chroot __P((const char *)) ICVMERRCALL(SYS_chroot);
int	 nice __P((int));
#endif


/*
 * X/Open Portability Guide <= Issue 3
 */
#if defined(_XOPEN_SOURCE) && (_XOPEN_SOURCE - 0) <= 3
int	 rename __P((const char *, const char *)) ICVMERRCALL(SYS_rename);
#endif


/*
 * X/Open Portability Guide >= Issue 4
 */
#if (!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
    (_XOPEN_SOURCE - 0) >= 4
__aconst char *crypt __P((const char *, const char *));
int	 encrypt __P((char *, int));
char	*getpass __P((const char *));
pid_t	 getsid __P((pid_t));
#endif


/*
 * X/Open Portability Guide >= Issue 4 Version 2
 */
#if (!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
    (defined(_XOPEN_SOURCE) && defined(_XOPEN_SOURCE_EXTENDED)) || \
    (_XOPEN_SOURCE - 0) >= 500
#define F_ULOCK		0
#define F_LOCK		1
#define F_TLOCK		2
#define F_TEST		3

int	 brk __P((void *));
int	 fchdir __P((int))ICVMERRCALL(SYS_fchdir);
#if defined(_XOPEN_SOURCE)
int	 fchown __P((int, uid_t, gid_t))ICVMERRCALL(SYS_fchown);
#else
int	 fchown __P((int, uid_t, gid_t))ICVMERRCALL(SYS_fchown);
#endif
int	 getdtablesize __P((void)) ICVMERRCALL(SYS_getdtablesize);
long	 gethostid __P((void))ICVMERRCALL(SYS_gethostid);
int	 gethostname __P((char *, size_t))ICVMERRCALL(SYS_gethostname);
__pure int
	 getpagesize __P((void)) ICVMCALL(SYS_getpagesize);		/* legacy */
pid_t	 getpgid __P((pid_t));
#if defined(_XOPEN_SOURCE)
int	 lchown __P((const char *, uid_t, gid_t));
#else
int	 lchown __P((const char *, uid_t, gid_t));
#endif
int	 lockf __P((int, int, off_t));
int	 readlink __P((const char *, char *, size_t))ICVMERRCALL(SYS_readlink); 
char	*sbrk __P((int));
/* XXX prototype wrong! */
int	 setpgrp __P((pid_t pid, pid_t pgrp));	/* obsoleted by setpgid() */
int	 setregid __P((gid_t, gid_t)) ICVMERRCALL(SYS_setregid);
int	 setreuid __P((uid_t, uid_t))ICVMERRCALL(SYS_setreuid);
void	 swab __P((const void *, void *, size_t))ICVMCALL(SYS_swab);
int	 symlink __P((const char *, const char *))ICVMERRCALL(SYS_symlink);
void	 sync __P((void))ICVMCALL(SYS_sync);
int	 truncate __P((const char *, off_t))ICVMERRCALL(SYS_truncate);
useconds_t ualarm __P((useconds_t, useconds_t));
int	 usleep __P((useconds_t));
pid_t	 vfork __P((void) ICVMERRCALL(SYS_vfork));

#ifndef __AUDIT__
char	*getwd __P((char *));			/* obsoleted by getcwd() */
#endif

/* FIXME: this should go to <sys/time.h>! */
#ifdef __STDC__
struct timeval;				/* select(2) XXX */
#endif
int	 select __P((int, fd_set *, fd_set *, fd_set *, struct timeval *))ICVMERRCALL(SYS_select);
#endif /* (!defined(_POSIX_SOURCE) && !defined(_XOPEN_SOURCE)) || ... */


/*
 * X/Open CAE Specification Issue 5 Version 2
 */
#if (!defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)) || \
    (_XOPEN_SOURCE - 0) >= 500
ssize_t	 pread __P((int, void *, size_t, off_t));
ssize_t	 pwrite __P((int, const void *, size_t, off_t));
#endif


/*
 * Implementation-defined extensions
 */
#if !defined(_POSIX_C_SOURCE) && !defined(_XOPEN_SOURCE)
int	 acct __P((const char *)) ICVMERRCALL(SYS_acct);
int	 des_cipher __P((const char *, char *, long, int));
int	 des_setkey __P((const char *key));
void	 endusershell __P((void));
int	 exect __P((const char *, char * const *, char * const *));
int	 fchroot __P((int));
int	 getdomainname __P((char *, size_t)) ICVMERRCALL(SYS_getdomainname);
int	 getgrouplist __P((const char *, gid_t, gid_t *, int *));
mode_t	 getmode __P((const void *, mode_t));
int	 getsubopt __P((char **, char * const *, char **));
__aconst char *getusershell __P((void));
int	 initgroups __P((const char *, gid_t));
int	 iruserok __P((u_int32_t, int, const char *, const char *));
int	 nfssvc __P((int, void *)) ICVMERRCALL(SYS_nfssvc);
int	 profil __P((char *, size_t, u_long, u_int));
void	 psignal __P((unsigned int, const char *));
int	 rcmd __P((char **, int, const char *,
	    const char *, const char *, int *));
int	 reboot __P((int, char *));
int	 revoke __P((const char *)) ICVMERRCALL(SYS_revoke);
int	 rresvport __P((int *));
int	 ruserok __P((const char *, int, const char *, const char *));
int	 setdomainname __P((const char *, size_t)) ICVMERRCALL(SYS_setdomainname);
int	 setegid __P((gid_t))ICVMERRCALL(SYS_setegid);
int	 seteuid __P((uid_t))ICVMERRCALL(SYS_seteuid);
int	 setgroups __P((int, const gid_t *))ICVMERRCALL(SYS_setgroups);
int	 sethostid __P((long))ICVMERRCALL(SYS_sethostid);
int	 sethostname __P((const char *, size_t))ICVMERRCALL(SYS_sethostname);
int	 setlogin __P((const char *))ICVMERRCALL(SYS_setlogin);
void	*setmode __P((const char *));
int	 setrgid __P((gid_t));
int	 setruid __P((uid_t));
void	 setusershell __P((void));
void	 strmode __P((mode_t, char *));
__aconst char *strsignal __P((int));
int	 swapctl __P((int, const void *, int));
int	 swapon __P((const char *));		/* obsoleted by swapctl() */
int	 syscall __P((int, ...));
quad_t	 __syscall __P((quad_t, ...));
int	 undelete __P((const char *));

extern char *sys_siglist[];
extern	 int optreset;		/* getopt(3) external variable */
extern	 char *suboptarg;	/* getsubopt(3) external variable */
#endif

__END_DECLS

#endif /* !_UNISTD_H_ */
