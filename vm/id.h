#ifndef ID_H
#define ID_H

#include "sys.h"


// Copyright (C) 2000,2001 Bob Dayley

/* defaults for systems that do not support GID, UID */

#define DEFAULT_UID		0xffff
#define DEFAULT_GID		0xffff
#define _DEFAULT_UNAME	"owner"
#define _DEFAULT_GNAME	"group"


#ifdef __WIN32__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

char *i_getdefaultuname(void);
char *i_getdefaultgname(void);

#ifdef __cplusplus
};
#endif // __cplusplus


#define DEFAULT_UNAME	i_getdefaultuname()
#define DEFAULT_GNAME	i_getdefaultgname()

#else // __WIN32__

#define DEFAULT_UNAME	_DEFAULT_UNAME
#define DEFAULT_GNAME	_DEFAULT_GNAME

#endif // __WIN32__





#endif

