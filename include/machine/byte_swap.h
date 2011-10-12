/*	$NetBSD: byte_swap.h,v 1.1 1999/01/15 13:31:24 bouyer Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
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
 * Copyright (c) 1987, 1991 Regents of the University of California.
 * All rights reserved.
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
 *	@(#)endian.h	7.8 (Berkeley) 4/3/91
 */

#ifndef  BYTE_SWAP_H_
#define	BYTE_SWAP_H_


// swap macros

#define __byte_swap_word(x) \
     (__extension__							      \
      ({ register unsigned short int __v;				      \
	   __asm__ __volatile__ ("bswapw %w0"				      \
				 : "=d" (__v)				      \
				 : "0" ((unsigned short int) (x))	      \
				 : "cc");				      \
	 __v; }))

#define __byte_swap_long(x) \
     (__extension__							      \
      ({ register unsigned long int __v;				      \
	   __asm__ __volatile__ ("bswapl %w0"				      \
				 : "=d" (__v)				      \
				 : "0" ((unsigned long int) (x))	      \
				 : "cc");				      \
	 __v; }))

#define __byte_swap_quad(x) \
     (__extension__							      \
      ({ register unsigned long long int __v;				      \
	   __asm__ __volatile__ ("bswapq %w0"				      \
				 : "=g" (__v)				      \
				 : "0" ((unsigned long long int) (x))	      \
				 : "cc");				      \
	 __v; }))


// swap macros only swap if bigendian

#define __byte_swap_wordBE(x) \
     (__extension__							      \
      ({ register unsigned short int __v;				      \
	   __asm__ __volatile__ ("bswapbew %w0"				      \
				 : "=d" (__v)				      \
				 : "0" ((unsigned short int) (x))	      \
				 : "cc");				      \
	 __v; }))

#define __byte_swap_longBE(x) \
     (__extension__							      \
      ({ register unsigned long int __v;				      \
	   __asm__ __volatile__ ("bswapbel %w0"				      \
				 : "=d" (__v)				      \
				 : "0" ((unsigned long int) (x))	      \
				 : "cc");				      \
	 __v; }))

#define __byte_swap_quadBE(x) \
     (__extension__							      \
      ({ register unsigned long long int __v;				      \
	   __asm__ __volatile__ ("bswapbeq %w0"				      \
				 : "=g" (__v)				      \
				 : "0" ((unsigned long long int) (x))	      \
				 : "cc");				      \
	 __v; }))


// swap macros only swap if littleendian

#define __byte_swap_wordLE(x) \
     (__extension__							      \
      ({ register unsigned short int __v;				      \
	   __asm__ __volatile__ ("bswaplew %w0"				      \
				 : "=d" (__v)				      \
				 : "0" ((unsigned short int) (x))	      \
				 : "cc");				      \
	 __v; }))

#define __byte_swap_longLE(x) \
     (__extension__							      \
      ({ register unsigned long int __v;				      \
	   __asm__ __volatile__ ("bswaplel %w0"				      \
				 : "=d" (__v)				      \
				 : "0" ((unsigned long int) (x))	      \
				 : "cc");				      \
	 __v; }))

#define __byte_swap_quadLE(x) \
     (__extension__							      \
      ({ register unsigned long long int __v;				      \
	   __asm__ __volatile__ ("bswapleq %w0"				      \
				 : "=g" (__v)				      \
				 : "0" ((unsigned long long int) (x))	      \
				 : "cc");				      \
	 __v; }))



#ifndef INTERNETC

#define	__byte_swap_long(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))
#define	__byte_swap_word(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))

#endif

#endif 

