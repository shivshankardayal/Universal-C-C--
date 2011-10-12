/* icvm-parse.h -- header file for icvm assembler
   Copyright (C) 1987, 91, 92, 93, 94, 1995 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#ifndef ICVM_PARSE_H
#define ICVM_PARSE_H

/* This header file defines things which are shared between the
   operand parser in icvm.y and the icvm assembler proper in
   tc-icvm.c.  */

/* The various icvm registers.  */

/* DATA and ADDR have to be contiguous, so that reg-DATA gives
   0-7==data reg, 8-15==addr reg for operands that take both types.

   We don't use forms like "ADDR0 = ADDR" here because this file is
   likely to be used on an Apollo, and the broken Apollo compiler
   gives an `undefined variable' error if we do that, according to
   troy@cbme.unsw.edu.au.  */

#define DATA DATA0
#define QDATA QDATA0
#define XDATA XDATA0
#define VDATA VDATA0
#define MDATA MDATA0
#define ADDR ADDR0
#define BAD BAD0
#define BAC BAC0

enum icvm_register
{
  DATA0 = 1,			/*   1- 8 == data registers 0-7 */
  DATA1,
  DATA2,
  DATA3,
  DATA4,
  DATA5,
  DATA6,
  DATA7,

  ADDR0,
  ADDR1,
  ADDR2,
  ADDR3,
  ADDR4,
  ADDR5,
  ADDR6,
  ADDR7,

  FP0,				/* Eight FP registers */
  FP1,
  FP2,
  FP3,

  COP0,				/* Co-processor #0-#7 */
  COP1,
  COP2,
  COP3,
  COP4,
  COP5,
  COP6,
  COP7,

  PC,				/* Program counter */
  ZPC,				/* Hack for Program space, but 0 addressing */
  SR,				/* Status Reg */
  CCR,				/* Condition code Reg */

  /* These have to be grouped together for the movec instruction to work. */
  USP,				/*  User Stack Pointer */
  ISP,				/*  Interrupt stack pointer */
  SFC,
  DFC,
  CACR,
  VBR,
  CAAR,
  MSP,
  ITT0,
  ITT1,
  DTT0,
  DTT1,
  MMUSR,
  TC,
  SRP,
  URP,
  BUSCR,			/* 68060 added these */
  PCR,
  ROMBAR,			/* mcf5200 added these */
  RAMBAR0,
  RAMBAR1,
  MBAR,
#define last_movec_reg MBAR
  /* end of movec ordering constraints */

  FPI,
  FPS,
  FPC,

  DRP,				/* 68851 or 68030 MMU regs */
  CRP,
  CAL,
  VAL,
  SCC,
  AC,
  BAD0,
  BAD1,
  BAD2,
  BAD3,
  BAD4,
  BAD5,
  BAD6,
  BAD7,
  BAC0,
  BAC1,
  BAC2,
  BAC3,
  BAC4,
  BAC5,
  BAC6,
  BAC7,
  PSR,				/* aka MMUSR on 68030 (but not MMUSR on 68040)
				   and ACUSR on 68ec030 */
  PCSR,

  IC,				/* instruction cache token */
  DC,				/* data cache token */
  NC,				/* no cache token */
  BC,				/* both caches token */

  TT0,				/* 68030 access control unit regs */
  TT1,

  ZDATA0,			/* suppressed data registers.  */
  ZDATA1,
  ZDATA2,
  ZDATA3,
  ZDATA4,
  ZDATA5,
  ZDATA6,
  ZDATA7,

  ZADDR0,			/* suppressed address registers.  */
  ZADDR1,
  ZADDR2,
  ZADDR3,
  ZADDR4,
  ZADDR5,
  ZADDR6,
  ZADDR7,
  STACKPOINTER,
  FRAMEPOINTER,
  QDATA0,
  QDATA1,	
  QDATA2,	
  QDATA3,	
  VDATA0,
  VDATA1,	
  VDATA2,	
  VDATA3,	
  VDATA4,	
  VDATA5,	
  VDATA6,	
  VDATA7,	
  VDATA8,	
  VDATA9,	
  VDATA10,	
  VDATA11,	
  VDATA12,	
  VDATA13,	
  VDATA14,	
  VDATA15,	
  XDATA0,
  XDATA1,	
  XDATA2,	
  XDATA3,	
  XDATA4,	
  XDATA5,	
  XDATA6,	
  XDATA7,	
  XDATA8,	
  XDATA9,	
  XDATA10,	
  XDATA11,	
  XDATA12,	
  XDATA13,	
  XDATA14,	
  XDATA15,	
  MDATA0,
  MDATA1,	
  MDATA2,	
  MDATA3,	
  MDATA4,	
  MDATA5,	
  MDATA6,	
  MDATA7,	
  MDATA8,	
  MDATA9,	
  MDATA10,	
  MDATA11,	
  MDATA12,	
  MDATA13,	
  MDATA14,	
  MDATA15	
};

/* Size information.  */

enum icvm_size
{
  /* Unspecified.  */
  SIZE_UNSPEC,

  /* Byte.  */
  SIZE_BYTE,

  /* Word (2 bytes).  */
  SIZE_WORD,

  /* Longword (4 bytes).  */
  SIZE_LONG
};

/* The structure used to hold information about an index register.  */

struct icvm_indexreg
{
  /* The index register itself.  */
  enum icvm_register reg;

  /* The size to use.  */
  enum icvm_size size;

  /* The value to scale by.  */
  int scale;
};

#ifdef OBJ_ELF
/* The type of a PIC expression.  */

enum pic_relocation
{
  pic_none,			/* not pic */
  pic_plt_pcrel,		/* @PLTPC */
  pic_got_pcrel,		/* @GOTPC */
  pic_plt_off,			/* @PLT */
  pic_got_off			/* @GOT */
};
#endif

/* The structure used to hold information about an expression.  */

struct icvm_exp
{
  /* The size to use.  */
  enum icvm_size size;

#ifdef OBJ_ELF
  /* The type of pic relocation if any.  */
  enum pic_relocation pic_reloc;
#endif

  /* The expression itself.  */
  expressionS exp;
};

/* The operand modes.  */
/* these are ordered in the way that a triple cumulative operation should 
   be optimized for opcode count reduction */

enum icvm_operand_type
{
  MODE_IMMED = 1,
  MODE_r,
  MODE_F,
  MODE_c,
  MODE_m,
  MODE_v,
  MODE_x,
  MODE_s,
  MODE_b,
  MODE_p,
  MODE_8,
  MODE_7,
  MODE_6,
  MODE_5,
  MODE_4,
  MODE_3,
  MODE_2,
  MODE_1,
  MODE_A,
  MODE_M,
  MODE_a,
  MODE_W,
  MODE_V,
  MODE_H,
  MODE_G,
  MODE_Y,
  MODE_K,
  MODE_d,
  MODE_U,
  MODE_EorB,
  MODE_f,
  MODE_ADEC,
  MODE_AINC,


  AREG,
  CONTROL,
  AINDR,
  BASE,
  POST,
  PRE,
  REGLST,
  SPDISP,
};

/* The structure used to hold a parsed operand.  */

struct icvm_op
{
  /* The type of operand.  */
  enum icvm_operand_type mode;

  /* The main register.  */
  enum icvm_register reg;

  /* The register mask for mode REGLST.  */
  unsigned long mask;

  /* An error message.  */
  const char *error;

  /* The index register.  */
  struct icvm_indexreg index;

  /* The displacement.  */
  struct icvm_exp disp;

  /* The outer displacement.  */
  struct icvm_exp odisp;

/* additional displacements */

  struct icvm_exp disp1;

  struct icvm_exp disp2;
};

extern int target_big_endian;


#endif /* ! defined (ICVM_PARSE_H) */

/* The parsing function.  */

extern int icvm_ip_op PARAMS ((char *, struct icvm_op *));

/* Whether register prefixes are optional.  */
extern int flag_reg_prefix_optional;


