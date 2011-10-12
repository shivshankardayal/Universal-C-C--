/* BFD library support routines for architectures.
   Copyright (C) 1990, 91, 92, 93, 94, 95, 97, 1998
   Free Software Foundation, Inc.
   Hacked by Steve Chamberlain of Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

#define N(name, print,d,next)  \
{  32, 32, 8, bfd_arch_icvm, name, "icvm",print,2,d,bfd_default_compatible,bfd_default_scan, next, }

static const bfd_arch_info_type arch_info_struct[] =
{ 
  N(bfd_mach_icvm,"icvm",false,0),
};

const bfd_arch_info_type bfd_icvm_arch =
  N(0,"icvm",true, &arch_info_struct[0]);
