/* This file is needed by libio to define various configuration parameters.
   These are always the same in the GNU C library.  */

#ifndef _G_config_h
#define _G_config_h 1

/* Define types for libio in terms of the standard internal type names.  */
#include <stddef.h>
#include <sys/types.h>

#define _G_size_t	size_t
#define _G_fpos_t	off_t
#define _G_fpos64_t	off_t
#define _G_ssize_t	ssize_t
#define _G_off_t	off_t
#define _G_off64_t	off_t
#define	_G_pid_t	pid_t
#define	_G_uid_t 	uid_t
#define _G_wchar_t	wchar_t
#define _G_wint_t	int
#define _G_stat64		stat

typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));

#define _G_HAVE_BOOL 1


/* These library features are always available in the GNU C library.  */
#define _G_HAVE_ATEXIT 1
#define _G_HAVE_SYS_CDEFS 1
#define _G_HAVE_SYS_WAIT 1
#define _G_NEED_STDARG_H 1
#define _G_va_list _BSD_VA_LIST_

//#define _G_HAVE_PRINTF_FP 1
#define _G_HAVE_MMAP 1
//#define _G_HAVE_LONG_DOUBLE_IO 1
#define _G_HAVE_IO_FILE_OPEN 0
#define _G_HAVE_IO_GETLINE_INFO 1

//#define _G_IO_IO_FILE_VERSION 0x20001

#define _G_OPEN64	open
#define _G_LSEEK64	lseek
#define _G_FSTAT64(fd,buf) fstat(fd, buf)

/* This is defined by <bits/stat.h> if `st_blksize' exists.  */
#define _G_HAVE_ST_BLKSIZE 1 

#define _G_BUFSIZ 8192

/* These are the vtbl details for ELF.  */
#define _G_NAMES_HAVE_UNDERSCORE 1
#define _G_VTABLE_LABEL_HAS_LENGTH 1
#define _G_USING_THUNKS	0
#define _G_VTABLE_LABEL_PREFIX "__vt$"
#define _G_VTABLE_LABEL_PREFIX_ID __vt$


#if defined __cplusplus || defined __STDC__
# define _G_ARGS(ARGLIST) ARGLIST
#else
# define _G_ARGS(ARGLIST) ()
#endif

#endif	/* _G_config.h */

