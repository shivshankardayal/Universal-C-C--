/* 
   Copyright 1989, 91, 92, 93, 94, 95, 96, 1997 Free Software Foundation.

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */


/* The structure used to hold information for an opcode.  */


#ifdef NOT_NOW
struct icvm_opcode
{
	unsigned short a;
	unsigned short b;
	unsigned short c;
	unsigned short d;
	unsigned short e;
	unsigned short f;
	unsigned short g;
	unsigned short h;

};

#endif

struct icvm_opcode
{
	unsigned long opc;
	unsigned long msk;
	unsigned short opcode;
	unsigned short operand;
};


extern const char *icvmstrpool[];

/* The structure used to hold information for an opcode alias.  */

struct icvm_opcode_alias
{
  /* The alias name.  */
  const char *alias;
  /* The instruction for which this is an alias.  */
  const char *primary;
};


extern const struct icvm_opcode *icvm_opcodes;
extern const struct icvm_opcode_alias icvm_opcode_aliases[];

extern const int icvm_numopcodes, icvm_numaliases;

extern int icvm_genregsbits;
extern int icvm_numgenregs;
extern int icvm_genregsmask;
extern int icvm_fltregsbits;
extern int icvm_numfltregs;
extern int icvm_fltregsmask;


/* end of icvm-opcode.h */
