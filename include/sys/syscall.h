#ifndef ICVM_SYSCALL_H
#define ICVM_SYSCALL_H

#ifndef NOSYSCALLS
// define attributes for functions for direct system calls

#define ICVMCALL(opcode) \
__attribute__ ((__syscall__(opcode,"","")));

#define ICVMDIRECT(inst) \
__attribute__ ((__syscall__(0,inst,"")));

#define ICVMERRCALL(opcode) \
__attribute__ ((__syscall__(opcode,"","movel (-116,%fp),_errno")));
#else

#define ICVMCALL(opcode) 
#define ICVMERRCALL(opcode) 

#endif

// 0 is reserved and dead
#define	SYS_exit   1
#define	SYS_fork	2 // ***
#define	SYS_read	3 // ***
#define	SYS_write	4 // ***
#define	SYS_open	5 // ***
#define	SYS_close	6 // ***
#define	SYS_wait4	7 // ***
#define	SYS_creat	8 // ***
#define	SYS_link	9 // ***
#define	SYS_unlink	10 // ***
#define SYS_kprintf 11  // debug printf statement
#define	SYS_chdir	12 // ***
#define	SYS_fchdir	13 // ***
#define	SYS_mknod	14 // ***
#define	SYS_chmod	15 // ***
#define	SYS_chown	16 // ***
#define	SYS_break	17
#define	SYS_getfsstat	18
#define	SYS_lseek	19 // ***
#define	SYS_getpid	20 // ***
#define	SYS_mount	21
#define	SYS_unmount	22
#define	SYS_setuid	23 // ***
#define	SYS_getuid	24 // ***
#define	SYS_geteuid	25 // ***
#define	SYS_ptrace	26
#define	SYS_recvmsg	27
#define	SYS_sendmsg	28
#define	SYS_recvfrom	29
#define	SYS_accept	30
#define	SYS_getpeername	31
#define	SYS_getsockname	32
#define	SYS_access	33 // ***
#define SYS_frexp  34
#define SYS_ldexp  35
#define	SYS_sync	36 // ***
#define	SYS_kill	37 // ***
#define	SYS_stat	38 // ***
#define	SYS_getppid	39 // ***
#define	SYS_lstat	40 // ***
#define	SYS_dup	41 // ***
#define	SYS_pipe	42 // ***
#define	SYS_getegid	43 // ***
#define	SYS_getdomainname	44 // ***
#define	SYS_isatty	45 // ***
#define	SYS_sigaction	46 
#define	SYS_getgid	47 // ***
#define	SYS_sigprocmask	48 // ***
#define	SYS_getlogin	49 // ***
#define	SYS_setlogin	50 // ***
#define	SYS_acct	51 // ***
#define	SYS_sigpending	52 // ***
#define	SYS_setdomainname	53 // ***
#define	SYS_ioctl	54
#define	SYS_reboot	55
#define	SYS_revoke	56 // ***
#define	SYS_symlink	57 // ***
#define	SYS_readlink	58 // ***
#define	SYS_execve	59 // ***
#define	SYS_umask	60 // ***
#define	SYS_chroot	61 // ***
#define	SYS_fstat	62 // ***
#define	SYS_setdebug	63
#define	SYS_getpagesize	64 // ***
#define	SYS_msync	65 // ***
#define	SYS_vfork	66 // ***
#define	SYS_malloc	67 // ***
#define	SYS_free	68 // ***
#define	SYS_realloc	69 // ***
#define	SYS_sstk	70
#define	SYS_mmap	71 // ***
#define	SYS_vadvise	72
#define	SYS_munmap	73 // ***
#define	SYS_mprotect	74 // ***
#define	SYS_madvise	75 // ***
 /* 76  old sleep call */
#define	SYS_getcwd	77 // ***

#define	SYS_mincore	78
#define	SYS_getgroups	79 // ***
#define	SYS_setgroups	80 // ***
#define	SYS_getpgrp	81 // ***
#define	SYS_setpgid	82 // ***
#define	SYS_setitimer	83 // ***
#define SYS_geticvmbasepath 84
#define	SYS_swapon	85 
#define	SYS_getitimer	86 // ***
#define	SYS_gethostname	87 // ***
#define	SYS_sethostname	88 // ***
#define	SYS_getdtablesize	89 // ***
#define	SYS_dup2	90 // ***
#define	SYS_fcntl	92 // ***
#define	SYS_select	93 // ***
#define	SYS_fsync	95 // ***
#define	SYS_setpriority	96
#define	SYS_socket	97
#define	SYS_connect	98
#define SYS_nanosleep 99 // ***				
#define	SYS_getpriority	100
				/* 101 is old send */
				/* 102 is old recv */
#define	SYS_sigreturn	103
#define	SYS_bind	104
#define	SYS_setsockopt	105
#define	SYS_listen	106
				/* 107 is obsolete vtimes */
				/* 108 is old sigvec */
				/* 109 is old sigblock */
				/* 110 is old sigsetmask */
#define	SYS_sigsuspend	111 // ***
#define SYS_modf 112 
#define	SYS_sigsetjmp	113
#define	SYS_siglongjmp	114
#define	SYS_vtrace	115 /* may be obsolete */
#define	SYS_gettimeofday	116 // ***
#define	SYS_getrusage	117
#define	SYS_getsockopt	118
#define	SYS_resuba	119
#define SYS_hypot   120 // ***
// 121
#define	SYS_settimeofday	122 // ***
#define	SYS_fchown	123 // ***
#define	SYS_fchmod	124 // ***
				/* 125 is old recvfrom */
#define	SYS_setreuid	126	/* ** compatibility; still used by libc */
#define	SYS_setregid	127	/* ** compatibility; still used by libc */
#define	SYS_rename	128 // ***
#define	SYS_truncate	129 // ***
#define	SYS_ftruncate	130 // ***
#define	SYS_flock	131 // ***
#define	SYS_mkfifo	132 // ***
#define	SYS_sendto	133
#define	SYS_shutdown	134
#define	SYS_socketpair	135
#define	SYS_mkdir	136 // ***
#define	SYS_rmdir	137 // ***
#define	SYS_utimes	138 // ***
#define	SYS_setsigreturn 139
#define	SYS_adjtime	140 // ***
				/* 141 is old getpeername */
#define	SYS_gethostid	142 // ***
#define	SYS_sethostid	143 // ***
#define	SYS_getrlimit	144
#define	SYS_setrlimit	145
				/* 146 is old killpg */
#define	SYS_setsid	147 // ***
#define	SYS_quotactl	148

#define	SYS_getgrgid	149
#define	SYS_getgrnam	150
#define	SYS_getpwnam	151
#define	SYS_getpwuid	152
#define	SYS_gethostbyname	153
#define	SYS_gethostbyaddr	154
#define	SYS_nfssvc	155 // ***
#define	SYS_getdirentries	156
#define	SYS_statfs	157
#define	SYS_fstatfs	158
#define	SYS_async_daemon	160
#define	SYS_getfh	161
#define	SYS_opendir	162
#define	SYS_readdir	163
#define	SYS_closedir 164
#define	SYS_rewinddir 165
#define	SYS_dirfd 	166
#define SYS_floor  167
#define SYS_ceil   168
#define SYS_seekdir  169
#define SYS_telldir  170
#define	SYS_shmsys	171 
#define	SYS_getpwent 172 
#define	SYS_setpwent 173 
#define	SYS_endpwent 174
#define	SYS_getgrent 175
#define	SYS_setgrent 176
#define	SYS_endgrent 177

#define	SYS_setgid	181 // ***
#define	SYS_setegid	182 // ***
#define	SYS_seteuid	183 // ***

#define SYS_isnan		184 // ***
#define SYS_isinf		185 // ***
#define SYS_memcpy		186 // ***
#define SYS_bigendian 	187 // returns 1 if bigendian, 0 if little endial
#define SYS_memmove		188 // ***
#define SYS_strchr		189 // ***
#define SYS_strrchr		190 // ***


#define SYS_strcat		191 // ***
#define SYS_strcmp		192 // ***
#define SYS_strcpy		193 // ***
#define SYS_strcspn		194 // ***
#define SYS_strlen		195 // ***
#define SYS_strncat		196  // ***
#define SYS_strncmp		197  // ***
#define SYS_strncpy		198 // ***
#define SYS_strpbrk		199 // ***
#define SYS_strspn		200 // ***
#define SYS_strstr		201 // ***
#define SYS_strtok		202 // ***


#define SYS_bcmp		203 // ***
#define SYS_bcopy		204 // ***
#define SYS_bzero		205 // ***
#define SYS_memccpy		206 // ***
#define SYS_memchr		207 // ***
#define SYS_memcmp		208 // ***
#define SYS_memset		209 // ***
/*		210 */
/*		211 */
#define SYS_swab		212 // ***
/* available */
#define SYS_strdup		214
#define SYS_strcasecmp	215 // ***
#define SYS_strncasecmp	216 // ***
#define SYS_ffs			217 // ***
#define SYS_shmctl		218 // ***
#define	SYS_shmdt		219 // ***
#define	SYS_shmat		220 // ***
#define	SYS_shmget		221 // ***


#define SYS_XCALLS		4096	// base of X system calls
#define SYS_FCALLS		8192	// base of FAST system calls


#endif













