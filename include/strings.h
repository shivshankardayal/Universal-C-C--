/*	$NetBSD: strings.h,v 1.6 1998/05/06 17:12:45 kleink Exp $	*/

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

#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <machine/ansi.h>
#include <sys/featuretest.h>
#include <sys/syscall.h>

#ifdef	_BSD_SIZE_T_
typedef	_BSD_SIZE_T_	size_t;
#undef	_BSD_SIZE_T_
#endif

#ifndef	_XOPEN_SOURCE
#ifndef	NULL
#define	NULL	0
#endif
#endif

#include <sys/cdefs.h>

__BEGIN_DECLS
int	 bcmp __P((const void *, const void *, size_t)) ICVMDIRECT("bcmp");
void	 bcopy __P((const void *, void *, size_t)) ICVMDIRECT("bcopy");
void	 bzero __P((void *, size_t)) ICVMDIRECT("bzero");
int	 ffs __P((int)) ICVMCALL(SYS_ffs);
char	*index __P((const char *, int));
char	*rindex __P((const char *, int));
int	 strcasecmp __P((const char *, const char *)) ICVMCALL(SYS_strcasecmp);
int	 strncasecmp __P((const char *, const char *, size_t))ICVMCALL(SYS_strncasecmp);
__END_DECLS

#if !defined(_XOPEN_SOURCE)
#include <string.h>
#endif

#define stricmp strcasecmp
#define strnicmp strncasecmp


#endif /* !defined(_STRINGS_H_) */
