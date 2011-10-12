/*-
 * Copyright (c) 1991 The Regents of the University of California.
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
 */



extern char **environ;
static int fd;

extern	unsigned char	etext;
extern	unsigned char	eprol asm ("eprol");
extern			start() asm("start");

extern char *__progname;
extern void __do_global_ctors ();

													
/* in ICVM this environment should be handed to us*/

struct kframe {
	int	kargc;
	char	**kargv;	
	char **kenviron;
};


start(register struct kframe *kfp)
{
	extern int errno;
asm("eprol:");
#ifdef MCRT0
	monstartup(&eprol, &etext);
#endif MCRT0
	environ=kfp->kenviron;
	errno = 0;
	__progname=kfp->kargv[0];
	__do_global_ctors(); // build up constructors for C++ globals
	exit(main(kfp->kargc, kfp->kargv,kfp->kenviron));
}

#ifdef MCRT0
/*ARGSUSED*/
exit(code)
	register int code;
{
	_mcleanup();
	_cleanup();
	_exit(code);
}
#endif MCRT0

#ifdef CRT0
/*
 * null mcount and moncontrol,
 * just in case some routine is compiled for profiling
 */
moncontrol(val)
	int val;
{

}

mcount()
{

}
#endif CRT0

/* reserve 1st long in constructor/destructor arrays for sizeof array */

asm(" .ctors");
asm(" ___CTOR_LIST__:");
asm(" .globl ___CTOR_LIST__");
asm(" .long 0");

asm(" .dtors");
asm(" ___DTOR_LIST__:");
asm(" .globl ___DTOR_LIST__");
asm(" .long 0");

