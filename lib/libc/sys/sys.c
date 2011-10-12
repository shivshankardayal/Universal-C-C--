
#define MAKESYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");

#define MAKEERRSYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-116,%fp),_errno"); \
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");

#define MAKEHERRSYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-116,%fp),_errno"); \
	  asm("movel (-120,%fp),_h_errno"); \
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");

#define MAKESIGHANDLERCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel #.sigreturn,-(%sp)");\
	  asm("syscalll %0": : "i" (SYS_setsigreturn) );\
	  asm("inc4l %sp");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-116,%fp),_errno"); \
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");

#define MAKESIGRETURN\
	  asm(".sigreturn:");\
	  asm("syscalll %0": : "i" (SYS_sigreturn) );

#include <syscall.h>
#include <unistd.h>

char **environ = (char **)0;
char *__progname;

int h_errno;

/* surrounded with a dummy call so we can access system header files.
   these actually turn to jumps into the Host handlers 
   so we just need to compile
   the jump vectors.   */


/* basic operating level system calls. 
*/

static void dummy(void){
	MAKESIGRETURN; // need this to do ANYTHING with signals
	MAKEERRSYSCALL("kprintf",SYS_kprintf);
	MAKESYSCALL("_exit",SYS_exit);
	MAKEERRSYSCALL("fork",SYS_fork);
	MAKEERRSYSCALL("read",SYS_read);
	MAKEERRSYSCALL("write",SYS_write);
	MAKEERRSYSCALL("open",SYS_open);
	MAKEERRSYSCALL("creat",SYS_creat);
	MAKEERRSYSCALL("close",SYS_close);
	MAKEERRSYSCALL("wait4",SYS_wait4);
	MAKEERRSYSCALL("link",SYS_link);
	MAKEERRSYSCALL("unlink",SYS_unlink);
	MAKEERRSYSCALL("chdir",SYS_chdir);
	MAKEERRSYSCALL("fchdir",SYS_fchdir);
	MAKEERRSYSCALL("mknod",SYS_mknod);
	MAKEERRSYSCALL("chmod",SYS_chmod);
	MAKEERRSYSCALL("lchmod",SYS_chmod);
	MAKEERRSYSCALL("chown",SYS_chown);
	MAKEERRSYSCALL("lchown",SYS_chown);
	MAKEERRSYSCALL("getcwd",SYS_getcwd);
	MAKESYSCALL("break",SYS_break);
	MAKESYSCALL("getfsstat",SYS_getfsstat);
	MAKESYSCALL("getpid",SYS_getpid);
	MAKEERRSYSCALL("mount",SYS_mount);
	MAKEERRSYSCALL("unmount",SYS_unmount);
	MAKEERRSYSCALL("setuid",SYS_setuid);
	MAKESYSCALL("getuid",SYS_getuid);
	MAKESYSCALL("geteuid",SYS_geteuid);
	MAKESYSCALL("ptrace",SYS_ptrace);
	MAKESYSCALL("recvmsg",SYS_recvmsg);
	MAKESYSCALL("sendmsg",SYS_sendmsg);
	MAKEERRSYSCALL("recvfrom",SYS_recvfrom);
	MAKEERRSYSCALL("accept",SYS_accept);
	MAKEERRSYSCALL("getdomainname",SYS_getdomainname);
	MAKEERRSYSCALL("setdomainname",SYS_setdomainname);
	MAKESYSCALL("getpeername",SYS_getpeername);
	MAKESYSCALL("getsockname",SYS_getsockname);
	MAKEERRSYSCALL("access",SYS_access);
	MAKESYSCALL("sync",SYS_sync);
	MAKEERRSYSCALL("kill",SYS_kill);
	MAKEERRSYSCALL("stat",SYS_stat);
	MAKESYSCALL("getppid",SYS_getppid);
	MAKEERRSYSCALL("lstat",SYS_lstat);
	MAKEERRSYSCALL("dup",SYS_dup);
	MAKEERRSYSCALL("pipe",SYS_pipe);
	MAKESYSCALL("getegid",SYS_getegid);
	MAKEERRSYSCALL("isatty",SYS_isatty);
	MAKESIGHANDLERCALL("sigaction",SYS_sigaction);
	MAKESYSCALL("getgid",SYS_getgid);
	MAKEERRSYSCALL("sigprocmask",SYS_sigprocmask);
	MAKESYSCALL("getlogin",SYS_getlogin);
	MAKESYSCALL("setlogin",SYS_setlogin);
	MAKESYSCALL("acct",SYS_acct);
	MAKEERRSYSCALL("sigpending",SYS_sigpending);
	MAKEERRSYSCALL("ioctl",SYS_ioctl);
	MAKESYSCALL("reboot",SYS_reboot);
	MAKESYSCALL("revoke",SYS_revoke);
	MAKESYSCALL("symlink",SYS_symlink);
	MAKESYSCALL("readlink",SYS_readlink);
	MAKEERRSYSCALL("execve",SYS_execve);
	MAKEERRSYSCALL("umask",SYS_umask);
	MAKEERRSYSCALL("chroot",SYS_chroot);
	MAKEERRSYSCALL("fstat",SYS_fstat);
//	MAKESYSCALL("setdebug",SYS_setdebug);
	MAKESYSCALL("getpagesize",SYS_getpagesize);
	MAKESYSCALL("msync",SYS_msync);
	MAKESYSCALL("vfork",SYS_vfork);
	MAKESYSCALL("malloc",SYS_malloc);
	MAKESYSCALL("free",SYS_free);
	MAKESYSCALL("realloc",SYS_realloc);
	MAKESYSCALL("sstk",SYS_sstk);
	MAKEERRSYSCALL("mmap",SYS_mmap);
	MAKEERRSYSCALL("shmctl",SYS_shmctl);
	MAKEERRSYSCALL("shmdt",SYS_shmdt);
	MAKEERRSYSCALL("shmat",SYS_shmat);
	MAKEERRSYSCALL("shmget",SYS_shmget);

	MAKESYSCALL("vadvise",SYS_vadvise);
	MAKEERRSYSCALL("munmap",SYS_munmap);
	MAKESYSCALL("mprotect",SYS_mprotect);
	MAKESYSCALL("madvise",SYS_madvise);
	MAKESYSCALL("nanosleep",SYS_nanosleep);
	MAKESYSCALL("mincore",SYS_mincore);
	MAKESYSCALL("getgroups",SYS_getgroups);
	MAKESYSCALL("setgroups",SYS_setgroups);
	MAKESYSCALL("getpgrp",SYS_getpgrp);
	MAKESYSCALL("setpgid",SYS_setpgid);
	MAKESYSCALL("setitimer",SYS_setitimer);
	MAKESYSCALL("swapon",SYS_swapon);
	MAKESYSCALL("getitimer",SYS_getitimer);
	MAKESYSCALL("gethostname",SYS_gethostname);
	MAKESYSCALL("sethostname",SYS_sethostname);
	MAKESYSCALL("getdtablesize",SYS_getdtablesize);
	MAKEERRSYSCALL("dup2",SYS_dup2);
	MAKEERRSYSCALL("fcntl",SYS_fcntl);
	MAKEERRSYSCALL("select",SYS_select);
	MAKESYSCALL("fsync",SYS_fsync);
	MAKEERRSYSCALL("setpriority",SYS_setpriority);
	MAKEERRSYSCALL("socket",SYS_socket);
	MAKEERRSYSCALL("connect",SYS_connect);
	MAKEERRSYSCALL("getpriority",SYS_getpriority);
	MAKESYSCALL("sigreturn",SYS_sigreturn);
	MAKEERRSYSCALL("bind",SYS_bind);
	MAKESYSCALL("setsockopt",SYS_setsockopt);
	MAKEERRSYSCALL("listen",SYS_listen);
	MAKEERRSYSCALL("sigsuspend",SYS_sigsuspend);
	MAKESYSCALL("vtrace",SYS_vtrace);
	MAKEERRSYSCALL("gettimeofday",SYS_gettimeofday);
	MAKESYSCALL("getrusage",SYS_getrusage);
	MAKESYSCALL("getsockopt",SYS_getsockopt);
	MAKESYSCALL("resuba",SYS_resuba);
	MAKEERRSYSCALL("settimeofday",SYS_settimeofday);
	MAKEERRSYSCALL("fchown",SYS_fchown);
	MAKEERRSYSCALL("fchmod",SYS_fchmod);
	MAKEERRSYSCALL("setreuid",SYS_setreuid);
	MAKEERRSYSCALL("setregid",SYS_setregid);
	MAKEERRSYSCALL("rename",SYS_rename);
	MAKEERRSYSCALL("truncate",SYS_truncate);
	MAKEERRSYSCALL("ftruncate",SYS_ftruncate);
	MAKEERRSYSCALL("flock",SYS_flock);
	MAKESYSCALL("mkfifo",SYS_mkfifo);
	MAKEERRSYSCALL("sendto",SYS_sendto);
	MAKEERRSYSCALL("lseek",SYS_lseek);
	MAKEERRSYSCALL("shutdown",SYS_shutdown);
	MAKEERRSYSCALL("socketpair",SYS_socketpair);
	MAKEERRSYSCALL("mkdir",SYS_mkdir);
	MAKEERRSYSCALL("rmdir",SYS_rmdir);
	MAKEERRSYSCALL("utimes",SYS_utimes);
	MAKESYSCALL("lutimes",SYS_utimes);
	MAKESYSCALL("adjtime",SYS_adjtime);
	MAKESYSCALL("gethostid",SYS_gethostid);
	MAKESYSCALL("sethostid",SYS_sethostid);
	MAKESYSCALL("getrlimit",SYS_getrlimit);
	MAKESYSCALL("setrlimit",SYS_setrlimit);
	MAKESYSCALL("setsid",SYS_setsid);
	MAKESYSCALL("quotactl",SYS_quotactl);
	MAKESYSCALL("nfssvc",SYS_nfssvc);
	MAKESYSCALL("getdirentries",SYS_getdirentries);
	MAKESYSCALL("statfs",SYS_statfs);
	MAKESYSCALL("fstatfs",SYS_fstatfs);
	MAKESYSCALL("async_daemon",SYS_async_daemon);
	MAKESYSCALL("getfh",SYS_getfh);
	MAKESYSCALL("shmsys",SYS_shmsys);
	MAKEERRSYSCALL("setgid",SYS_setgid);
	MAKEERRSYSCALL("setegid",SYS_setegid);
	MAKEERRSYSCALL("seteuid",SYS_seteuid);
	MAKESYSCALL("isnan",SYS_isnan);
	MAKESYSCALL("isinf",SYS_isinf);
	MAKEERRSYSCALL("opendir",SYS_opendir);
	MAKEERRSYSCALL("readdir",SYS_readdir);
	MAKEERRSYSCALL("closedir",SYS_closedir);
	MAKEERRSYSCALL("rewinddir",SYS_rewinddir);
	MAKEERRSYSCALL("dirfd",SYS_dirfd);
	MAKEERRSYSCALL("seekdir",SYS_seekdir);
	MAKEERRSYSCALL("telldir",SYS_telldir);
	MAKEERRSYSCALL("getgrgid",SYS_getgrgid);
	MAKEERRSYSCALL("getgrnam",SYS_getgrnam);
	MAKEERRSYSCALL("getpwnam",SYS_getpwnam);
	MAKEERRSYSCALL("getpwuid",SYS_getpwuid);
	MAKEERRSYSCALL("getpwent",SYS_getpwent);
	MAKEERRSYSCALL("setpwent",SYS_setpwent);
	MAKEERRSYSCALL("endpwent",SYS_endpwent);
	MAKEERRSYSCALL("getgrent",SYS_getgrent);
	MAKEERRSYSCALL("setgrent",SYS_setgrent);
	MAKEERRSYSCALL("endgrent",SYS_endgrent);
	MAKESYSCALL("memcpy",SYS_memcpy);
	MAKESYSCALL("__isbigendian",SYS_bigendian);
	MAKESYSCALL("memmove",SYS_memmove);
	MAKESYSCALL("strchr",SYS_strchr);
	MAKESYSCALL("strrchr",SYS_strrchr);
	MAKESYSCALL("strcat",SYS_strcat);
	MAKESYSCALL("strcmp",SYS_strcmp);
	MAKESYSCALL("strcpy",SYS_strcpy);
	MAKESYSCALL("strcspn",SYS_strcspn);
	MAKESYSCALL("strlen",SYS_strlen);
	MAKESYSCALL("strncat",SYS_strncat);
	MAKESYSCALL("strncmp",SYS_strncmp);
	MAKESYSCALL("strncpy",SYS_strncpy);
	MAKESYSCALL("strpbrk",SYS_strpbrk);
	MAKESYSCALL("strspn",SYS_strspn);
	MAKESYSCALL("strstr",SYS_strstr);
	MAKESYSCALL("strtok",SYS_strtok);
	MAKESYSCALL("bcmp",SYS_bcmp);
	MAKESYSCALL("memccpy",SYS_memccpy);
	MAKESYSCALL("memchr",SYS_memchr);
	MAKESYSCALL("memcmp",SYS_memcmp);
	MAKESYSCALL("memset",SYS_memset);
	MAKESYSCALL("index",SYS_strchr);
	MAKESYSCALL("rindex",SYS_strrchr);
	MAKESYSCALL("swab",SYS_swab);
	MAKESYSCALL("ffs",SYS_ffs);
	MAKEERRSYSCALL("strdup",SYS_strdup);
	MAKESYSCALL("strcasecmp",SYS_strcasecmp);
	MAKESYSCALL("strncasecmp",SYS_strncasecmp);


// setjmp/longjmp cluges

	  asm("\n_setjmp:");
	  asm(".globl _setjmp");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("movel (%sp)+,(-120,%fp)");
	  asm("movewl #1,-(%sp)");

	  asm("\nsetjmpcluge:");
	  asm("movel (-120,%fp),-(%sp)");
	  asm("movel (-112,%fp),-(%sp)");

// NOTE THIS CALL MUST IMMEDIATELY FOLLOW THE SETJMP LABELS ABOVE!!!
	MAKESYSCALL("sigsetjmp",SYS_sigsetjmp);

	  asm("\n__setjmp:");
	  asm(".globl __setjmp");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("movel (%sp)+,(-120,%fp)");
	  asm("movewl #0,-(%sp)");
	  asm("bral setjmpcluge");
	

	  asm("\n_longjmp:");
	  asm(".globl _longjmp");
	  asm("\n__longjmp:");
	  asm(".globl __longjmp");
// NOTE THIS CALL MUST IMMEDIATELY FOLLOW THE LONGJMP LABELS ABOVE!!!
	MAKESYSCALL("siglongjmp",SYS_siglongjmp);



	MAKESYSCALL("__geticvmbasepath",SYS_geticvmbasepath);

//	MAKEHERRSYSCALL("gethostbyname",SYS_gethostbyname);
//	MAKEHERRSYSCALL("gethostbyaddr",SYS_gethostbyaddr);

	MAKESYSCALL("frexp",SYS_frexp);
	MAKESYSCALL("ldexp",SYS_ldexp);
	MAKESYSCALL("modf",SYS_modf);

#ifdef NOT_NOW

// alloca call
	  asm("\n_alloca:");
	  asm(".globl _alloca");
	  asm("\n___builtin_alloca:");
	  asm(".globl ___builtin_alloca");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("movel (%sp)+,(-120,%fp)");
	  asm("movel %sp,%r0");
	  asm("addwl   #7,(-120,%fp)");  // align
	  asm("andwl   #-8,(-120,%fp)");
	  asm("subl   (-120,%fp),%r0");
	  asm("movel  %r0,%sp");
	  asm("movel  (-120,%fp),-(%sp)");
	  asm("jmpl @(-112,%fp)");
#endif

// fabs call
	  asm("\n_fabs:");
	  asm(".globl _fabs");
	  asm("\n__builtin_fabs:");
	  asm(".globl __builtin_fabs");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("absd (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");


// absf call
	  asm("\n_absf:");
	  asm(".globl _absf");
	  asm("\n__builtin_absf:");
	  asm(".globl __builtin_absf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("absf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");



}






