/* Definitions for Motorola icvm running Linux-based GNU systems with
   ELF format.
   Copyright (C) 1995, 1996, 1997 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

//#define LINUX_DEFAULT_ELF
#define MOTOROLA		/* Use Motorola syntax */
#define USE_GAS			/* But GAS wants jbsr instead of jsr */

#include <icvm/icvm.h>
//#include <linux.h>		/* some common stuff */

/* How to renumber registers for dbx and gdb. */

#define DBX_REGISTER_NUMBER(r) (r)

/* Do not break .stabs pseudos into continuations.  */
#define DBX_CONTIN_LENGTH 0



/* Provide a LINK_SPEC appropriate for GNU/Linux.  Here we provide support
   for the special GCC options -static and -shared, which allow us to
   link things in one of these three modes by applying the appropriate
   combinations of options at link-time.  We like to support here for
   as many of the other GNU linker options as possible.  But I don't
   have the time to search for those flags.  I am sure how to add
   support for -soname shared_object_name. H.J.

   I took out %{v:%{!V:-V}}.  It is too much :-(.  They can use
   -Wl,-V.

   When the -shared link option is used a final link is not being
   done.  */


#undef	LINK_SPEC


#define LINK_SPEC "-m icvmelf %{shared:-shared} \
  %{!shared: \
   %{!static: \
	%{rdynamic:-export-dynamic} \
	%{!dynamic-linker:-dynamic-linker /lib/ld-icvm.so.1}} \
	%{static:-static}}"



#define CPP_PREDEFINES \
  "-D__ELF__ -Dunix -Dicvm\
	 -DDONT_USE_BUILTIN_SETJMP\
	  -D__STDC__  -DICVM -DINTERNETC -D__ICVM__ -DBSD -D__BSD__\
	 -Asystem(unix) -Asystem(posix) -Acpu(icvm) -Amachine(icvm)"

#undef CPP_SPEC

#ifdef USE_GNULIBC_1

#define CPP_SPEC \
  "%{posix:-D_POSIX_SOURCE}"

#else

#define CPP_SPEC \
  "%{posix:-D_POSIX_SOURCE} %{pthread:-D_REENTRANT}"

#endif



#undef  LIBGCC_SPEC

// kill libgcc
#define LIBGCC_SPEC ""


