/* i370-opc.c -- 370/390 architecture opcode list
   Copyright 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support
   I370 table loosely based on the PPC table, 
   Written by Linas Vepstas <linas@linas.org> in 1998, 1999

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
2, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#include <stdio.h>
#include "ansidecl.h"
#include "opcode/i370.h"

/* This file holds the i370 opcode table.  The opcode table
   includes almost all of the extended instruction mnemonics.  This
   permits the disassembler to use them, and simplifies the assembler
   logic, at the cost of increasing the table size.  The table is
   strictly constant data, so the compiler should be able to put it in
   the .text section.

   This file also holds the operand table.  All knowledge about
   inserting operands into instructions and vice-versa is kept in this
   file.  */

/* Local insertion and extraction functions.  */
static i370_insn_t insert_ss_b2  PARAMS (( i370_insn_t, long, const char **));
static i370_insn_t insert_ss_d2  PARAMS (( i370_insn_t, long, const char **));
static long extract_ss_b2 PARAMS (( i370_insn_t, int *));
static long extract_ss_d2 PARAMS (( i370_insn_t, int *));


/* The operands table.
   The fields are bits, shift, insert, extract, flags, name.  
   The types:
   I370_OPERAND_GPR register, must name a register, must be present
   I370_OPERAND_RELATIVE displacement or legnth field, must be present
   I370_OPERAND_BASE base register; if present, must name a register
                      if absent, should take value of zero
   I370_OPERAND_INDEX index register; if present, must name a register
                      if absent, should take value of zero
   I370_OPERAND_OPTIONAL other optional operand (usuall reg?)
*/

const struct i370_operand i370_operands[] =
{
  /* The zero index is used to indicate the end of the list of
     operands.  */
#define UNUSED 0
  { 0, 0, 0, 0, 0, "unused" },

  /* The R1 register field in an RR form instruction.  */
#define RR_R1 (UNUSED + 1)
#define RR_R1_MASK (0xf << 4)
  { 4, 4, 0, 0, I370_OPERAND_GPR, "RR R1" },

  /* The R2 register field in an RR form instruction.  */
#define RR_R2 (RR_R1 + 1)
#define RR_R2_MASK (0xf)
  { 4, 0, 0, 0, I370_OPERAND_GPR, "RR R2" },

  /* The I field in an RR form SVC-style instruction.  */
#define RR_I (RR_R2 + 1)
#define RR_I_MASK (0xff)
  { 8, 0, 0, 0, I370_OPERAND_RELATIVE, "RR I (svc)" },

  /* The R1 register field in an RX, RS, RSI, or RI form instruction.  */
#define RX_R1 (RR_I + 1)
#define RX_R1_MASK (0xf << 20)
  { 4, 20, 0, 0, I370_OPERAND_GPR, "RX R1" },

  /* The X2 index field in an RX form instruction.  */
#define RX_X2 (RX_R1 + 1)
#define RX_X2_MASK (0xf << 16)
  { 4, 16, 0, 0, I370_OPERAND_GPR | I370_OPERAND_INDEX, "RX X2"},

  /* The B2 base field in an RX or RS form instruction.  */
#define RX_B2 (RX_X2 + 1)
#define RX_B2_MASK (0xf << 12)
  { 4, 12, 0, 0, I370_OPERAND_GPR | I370_OPERAND_BASE, "RX B2"},

  /* The D2 displacement field in an RX or SS form instruction.  */
#define RX_D2 (RX_B2 + 1)
#define RX_D2_MASK (0xfff)
  { 12, 0, 0, 0, I370_OPERAND_RELATIVE, "RX D2"},

  /* The D2 displacement field in an RX, RS or SS form instruction.  */
#define RS_D2 (RX_D2 + 1)
#define RS_D2_MASK (0xfff)
  { 12, 0, 0, 0, I370_OPERAND_RELATIVE, "RS D2"},

  /* The R3 register field in an RS form instruction.  */
#define RS_R3 (RS_D2 + 1)
#define RS_R3_MASK (0xf << 16)
  { 4, 16, 0, 0, I370_OPERAND_GPR, "RS R3" },

  /* The optional B2 base field in an RS form instruction.  */
  /* Note that this field will almost always be absent */
#define RS_B2_OPT (RS_R3 + 1)
#define RS_B2_OPT_MASK (0xf << 12)
  { 4, 12, 0, 0, I370_OPERAND_GPR | I370_OPERAND_OPTIONAL, "RS B2 OPT"},

 /* The I2 index field in an SI form instruction.  */
#define SI_I2 (RS_B2_OPT + 1)
#define SI_I2_MASK (0xff << 16)
  { 8, 16, 0, 0, I370_OPERAND_RELATIVE, "SI I2"},

 /* The B1 base register field in an SI form instruction.  */
#define SI_B1 (SI_I2 + 1)
#define SI_B1_MASK (0xf << 12)
  { 4, 12, 0, 0, I370_OPERAND_GPR, "SI B1" },

  /* The D1 displacement field in an SI form instruction.  */
#define SI_D1 (SI_B1 + 1)
#define SI_D1_MASK (0xfff)
  { 12, 0, 0, 0, I370_OPERAND_RELATIVE, "SI D1" },

  /* The L length field in an SS form instruction. */
#define SS_L (SI_D1 + 1)
#define SS_L_MASK (0xffff<<16)
  { 8, 16, 0, 0, I370_OPERAND_RELATIVE, "SS L" },

 /* The B1 base register field in an SS form instruction.  */
#define SS_B1 (SS_L + 1)
#define SS_B1_MASK (0xf << 12)
  { 4, 12, 0, 0, I370_OPERAND_GPR, "SS B1" },

  /* The D1 displacement field in an SS form instruction.  */
#define SS_D1 (SS_B1 + 1)
#define SS_D1_MASK (0xfff)
  { 12, 0, 0, 0, I370_OPERAND_RELATIVE, "SS D1" },

 /* The B2 base register field in an SS form instruction.  */
#define SS_B2 (SS_D1 + 1)
#define SS_B2_MASK (0xf << 12)
  { 4, 12, insert_ss_b2, extract_ss_b2, I370_OPERAND_GPR | I370_OPERAND_BASE, "SS B2" },

  /* The D2 displacement field in an SS form instruction.  */
#define SS_D2 (SS_B2 + 1)
#define SS_D2_MASK (0xfff)
  { 12, 0, insert_ss_d2, extract_ss_d2, I370_OPERAND_RELATIVE, "SS D2" },

 /* The B2 base register field in an S form instruction.  */
#define S_B2 (SS_D2 + 1)
#define S_B2_MASK (0xf << 12)
  { 4, 12, 0, 0, I370_OPERAND_GPR | I370_OPERAND_BASE, "S B2" },

  /* The D2 displacement field in an S form instruction.  */
#define S_D2 (S_B2 + 1)
#define S_D2_MASK (0xfff)
  { 12, 0, 0, 0, I370_OPERAND_RELATIVE, "S D2" },

  /* The R1 register field in an RRE form instruction.  */
#define RRE_R1 (S_D2 + 1)
#define RRE_R1_MASK (0xf << 4)
  { 4, 4, 0, 0, I370_OPERAND_GPR, "RRE R1" },

  /* The R2 register field in an RRE form instruction.  */
#define RRE_R2 (RRE_R1 + 1)
#define RRE_R2_MASK (0xf)
  { 4, 0, 0, 0, I370_OPERAND_GPR, "RRE R2" },

};

/* The functions used to insert and extract complicated operands.  */

/*ARGSUSED*/
static i370_insn_t
insert_ss_b2 (insn, value, errmsg)
     i370_insn_t insn;
     long value;
     const char **errmsg;
{
  insn.i[1] |= (value & 0xf) << 28;
  return insn;
}

static i370_insn_t
insert_ss_d2 (insn, value, errmsg)
     i370_insn_t insn;
     long value;
     const char **errmsg;
{
  insn.i[1] |= (value & 0xfff) << 16;
  return insn;
}

static long 
extract_ss_b2 (insn, invalid)
     i370_insn_t insn;
     int *invalid;
{
  return (insn.i[1] >>28) & 0xf;
}

static long 
extract_ss_d2 (insn, invalid)
     i370_insn_t insn;
     int *invalid;
{
  return (insn.i[1] >>16) & 0xfff;
}


/* Macros used to form opcodes.  */

/* The short-instruction opcode.  */
#define OPS(x) ((((unsigned short)(x)) & 0xff) << 8)
#define OPS_MASK OPS (0xff)

/* the extended instruction opcode */
#define XOPS(x) ((((unsigned short)(x)) & 0xff) << 24)
#define XOPS_MASK XOPS (0xff)

/* the S instruction opcode */
#define SOPS(x) ((((unsigned short)(x)) & 0xffff) << 16)
#define SOPS_MASK SOPS (0xffff)

/* the E instruction opcode */
#define EOPS(x) (((unsigned short)(x)) & 0xffff)
#define EOPS_MASK EOPS (0xffff)

/* An RR form instruction.  */
#define RR(op, r1, r2) \
  (OPS (op) | ((((unsigned short)(r1)) & 0xf) << 4) |   \
              ((((unsigned short)(r2)) & 0xf) )) 

#define RR_MASK RR (0xff, 0x0, 0x0)

/* An SVC-style instruction.  */
#define RI(op, i) \
  (OPS (op) | (((unsigned short)(i)) & 0xff))

#define RI_MASK RI (0xff, 0x0)

/* An RX form instruction. */
#define RX(op, r1, x2, b2, d2) \
  (XOPS(op) | ((((unsigned short)(r1)) & 0xf) << 20) |  \
              ((((unsigned short)(x2)) & 0xf) << 16) |  \
              ((((unsigned short)(b2)) & 0xf) << 12) |  \
              ((((unsigned short)(d2)) & 0xfff)))  

#define RX_MASK RX (0xff, 0x0, 0x0, 0x0, 0x0)

/* An RS form instruction. */
#define RS(op, r1, b3, b2, d2) \
  (XOPS(op) | ((((unsigned short)(r1)) & 0xf) << 20) |  \
              ((((unsigned short)(b3)) & 0xf) << 16) |  \
              ((((unsigned short)(b2)) & 0xf) << 12) |  \
              ((((unsigned short)(d2)) & 0xfff)))  

#define RS_MASK RS (0xff, 0x0, 0x0, 0x0, 0x0)

/* An SI form instruction. */
#define SI(op, i2, b1, d1) \
  (XOPS(op) | ((((unsigned short)(i2)) & 0xff) << 16) |  \
              ((((unsigned short)(b1)) & 0xf)  << 12) |  \
              ((((unsigned short)(d1)) & 0xfff)))  

#define SI_MASK SI (0xff, 0x0, 0x0, 0x0)

/* An SS form instruction high word. */
#define SSH(op, l, b1, d1) \
  (XOPS(op) | ((((unsigned short)(l)) & 0xff) << 16) |  \
              ((((unsigned short)(b1)) & 0xf)  << 12) |  \
              ((((unsigned short)(d1)) & 0xfff)))  

/* An SS form instruction low word. */
#define SSL(b2, d2) \
            ( ((((unsigned short)(b1)) & 0xf)   << 28) |  \
              ((((unsigned short)(d1)) & 0xfff) << 16 ))  

#define SS_MASK SSH (0xff, 0x0, 0x0, 0x0)

/* An S form instruction. */
#define S(op, b2, d2) \
  (SOPS(op) | ((((unsigned short)(b2)) & 0xf) << 12) |  \
              ((((unsigned short)(d2)) & 0xfff)))  

#define S_MASK S (0xffff, 0x0, 0x0)

/* An RRE form instruction.  */
#define RRE(op, r1, r2) \
  (SOPS (op) | ((((unsigned short)(r1)) & 0xf) << 4) |   \
               ((((unsigned short)(r2)) & 0xf) )) 

#define RRE_MASK RRE (0xffff, 0x0, 0x0)

/* An E form instruction.  */
#define E(op) EOPS (op)
#define E_MASK E (0xffff)


/* Smaller names for the flags so each entry in the opcodes table will
   fit on a single line.  */
#define ANY     I370_OPCODE_370


/* The opcode table.

   The format of the opcode table is:

   NAME	     OPCODE	MASK		FLAGS		{ OPERANDS }

   NAME is the name of the instruction.
   OPCODE is the instruction opcode.
   MASK is the opcode mask; this is used to tell the disassembler
     which bits in the actual opcode must match OPCODE.
   FLAGS are flags indicated what processors support the instruction.
   OPERANDS is the list of operands.

   The disassembler reads the table in order and prints the first
   instruction which matches, so this table is sorted to put more
   specific instructions before more general instructions.  It is also
   sorted by major opcode.  */

const struct i370_opcode i370_opcodes[] = {

/* E form instructions */
{ "pr",    2, E(0x0101),   0, E_MASK, 0, ANY, 0},
{ "upt",   2, E(0x0102),   0, E_MASK, 0, ANY, 0},

/* RR form instructions */
{ "ar",     2, RR(0x1a,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "adr",    2, RR(0x2a,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "aer",    2, RR(0x3a,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "alr",    2, RR(0x1e,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "aur",    2, RR(0x2e,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "awr",    2, RR(0x3e,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "axr",    2, RR(0x36,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "balr",   2, RR(0x05,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "basr",   2, RR(0x0d,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "bassm",  2, RR(0x0c,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "bsm",    2, RR(0x0b,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "bcr",    2, RR(0x07,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "bctr",   2, RR(0x06,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "cdr",    2, RR(0x29,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "cer",    2, RR(0x39,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "clr",    2, RR(0x15,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "clcl",   2, RR(0x0f,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "cr",     2, RR(0x19,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "ddr",    2, RR(0x2d,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "der",    2, RR(0x3d,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "dr",     2, RR(0x1d,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "hdr",    2, RR(0x24,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "her",    2, RR(0x34,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lcdr",   2, RR(0x23,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lcer",   2, RR(0x33,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lcr",    2, RR(0x13,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "ldr",    2, RR(0x28,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "ler",    2, RR(0x38,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lndr",   2, RR(0x21,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lner",   2, RR(0x31,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lnr",    2, RR(0x11,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lpdr",   2, RR(0x20,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lper",   2, RR(0x30,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lpr",    2, RR(0x10,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lr",     2, RR(0x18,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lrdr",   2, RR(0x25,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lrer",   2, RR(0x35,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "ltdr",   2, RR(0x22,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "lter",   2, RR(0x32,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "ltr",    2, RR(0x12,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "mdr",    2, RR(0x2c,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "mer",    2, RR(0x3c,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "mr",     2, RR(0x1c,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "mvcl",   2, RR(0x0e,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "mxdr",   2, RR(0x27,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "mxr",    2, RR(0x26,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "nr",     2, RR(0x14,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "or",     2, RR(0x16,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "sdr",    2, RR(0x2b,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "ser",    2, RR(0x3b,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "slr",    2, RR(0x1f,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "spm",    2, RR(0x04,0,0), 0, RR_MASK, 0, ANY,  { RR_R1 } },
{ "sr",     2, RR(0x1b,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "sur",    2, RR(0x3f,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "swr",    2, RR(0x2f,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "sxr",    2, RR(0x37,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },
{ "xr",     2, RR(0x17,0,0), 0, RR_MASK, 0, ANY,  { RR_R1, RR_R2 } },

/* unusual RR formats */
{ "svc",    2, RI(0x0a,0), 0, RI_MASK, 0, ANY,  { RR_I } },

/* RX form instructions */
{ "a",      4, RX(0x5a,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ad",     4, RX(0x6a,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ae",     4, RX(0x7a,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ah",     4, RX(0x4a,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "al",     4, RX(0x5e,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "au",     4, RX(0x7e,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "aw",     4, RX(0x6e,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "bal",    4, RX(0x45,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "bas",    4, RX(0x4d,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "bc",     4, RX(0x47,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "bct",    4, RX(0x46,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "c",      4, RX(0x59,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "cd",     4, RX(0x69,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ce",     4, RX(0x79,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ch",     4, RX(0x49,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "cl",     4, RX(0x55,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "cvb",    4, RX(0x4f,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "cvd",    4, RX(0x4e,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "d",      4, RX(0x5d,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "dd",     4, RX(0x6d,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "de",     4, RX(0x7d,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ex",     4, RX(0x44,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ic",     4, RX(0x43,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "l",      4, RX(0x58,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "la",     4, RX(0x41,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "lae",    4, RX(0x51,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ld",     4, RX(0x68,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "le",     4, RX(0x78,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "lh",     4, RX(0x48,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "lra",    4, RX(0xb1,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "m",      4, RX(0x5c,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "md",     4, RX(0x6c,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "me",     4, RX(0x7c,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "mh",     4, RX(0x4c,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ms",     4, RX(0x71,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "mxd",    4, RX(0x67,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "n",      4, RX(0x54,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "o",      4, RX(0x56,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "s",      4, RX(0x5b,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "sd",     4, RX(0x6b,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "se",     4, RX(0x7b,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "sh",     4, RX(0x4b,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "sl",     4, RX(0x5f,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "st",     4, RX(0x50,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "stc",    4, RX(0x42,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "std",    4, RX(0x60,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "ste",    4, RX(0x70,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "sth",    4, RX(0x40,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "su",     4, RX(0x7f,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "sw",     4, RX(0x6f,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},
{ "x",      4, RX(0x57,0,0,0,0), 0, RX_MASK, 0, ANY, {RX_R1, RX_D2, RX_X2, RX_B2}},

/* RS form instructions */
{ "bxh",    4, RS(0x86,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "bxle",   4, RS(0x87,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "cds",    4, RS(0xbb,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "clcle",  4, RS(0xa9,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "clm",    4, RS(0xbd,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "cs",     4, RS(0xba,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "icm",    4, RS(0xbf,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "lam",    4, RS(0x9a,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "lctl",   4, RS(0xb7,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "lm",     4, RS(0x98,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "mvcle",  4, RS(0xa8,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "sigp",   4, RS(0xae,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "stam",   4, RS(0x9b,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "stcm",   4, RS(0xbe,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "stctl",  4, RS(0xb6,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "stm",    4, RS(0x90,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},
{ "trace",  4, RS(0x99,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_R3, RS_D2, RX_B2}},

/* RS form instructions with blank R3 and optional B2 (shift left/right) */
{ "sla",    4, RS(0x8b,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},
{ "slda",   4, RS(0x8f,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},
{ "sldl",   4, RS(0x8d,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},
{ "sll",    4, RS(0x89,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},
{ "sra",    4, RS(0x8a,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},
{ "srda",   4, RS(0x8e,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},
{ "srdl",   4, RS(0x8c,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},
{ "srl",    4, RS(0x88,0,0,0,0), 0, RS_MASK, 0, ANY, {RX_R1, RS_D2, RS_B2_OPT}},

/* SI form instructions */
{ "cli",    4, SI(0x95,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "mc",     4, SI(0xaf,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "mvi",    4, SI(0x92,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "ni",     4, SI(0x94,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "oi",     4, SI(0x96,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "stnsm",  4, SI(0xac,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "stosm",  4, SI(0xad,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "tm",     4, SI(0x91,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},
{ "xi",     4, SI(0x97,0,0,0),   0, SI_MASK, 0, ANY,	{ SI_D1, SI_B1, SI_I2}},

/* SS form instructions */
{ "ap",     6, SSH(0xfa,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "clc",    6, SSH(0xd5,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "cp",     6, SSH(0xf9,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "dp",     6, SSH(0xfd,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "ed",     6, SSH(0xde,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "edmk",   6, SSH(0xdf,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvc",    6, SSH(0xd2,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvcin",  6, SSH(0xe8,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvck",   6, SSH(0xd9,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvcp",   6, SSH(0xda,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvcs",   6, SSH(0xdb,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvn",    6, SSH(0xd1,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvo",    6, SSH(0xf1,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "mvz",    6, SSH(0xd3,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "nc",     6, SSH(0xd4,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "oc",     6, SSH(0xd6,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "pack",   6, SSH(0xf2,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "plo",    6, SSH(0xee,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "tr",     6, SSH(0xdc,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "trt",    6, SSH(0xdd,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "unpk",   6, SSH(0xf3,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "xc",     6, SSH(0xd7,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},
{ "zap",    6, SSH(0xf8,0,0,0), 0, SS_MASK, 0, ANY, {SS_D1,SS_L,SS_B1,SS_D2,SS_B2}},

/* S form instructions */
{ "cfc",    4, S(0xb21a,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "csch",   4, S(0xb230,0,0),   0, S_MASK, 0, ANY, 0},
{ "hsch",   4, S(0xb231,0,0),   0, S_MASK, 0, ANY, 0},
{ "ipk",    4, S(0xb20b,0,0),   0, S_MASK, 0, ANY, 0},
{ "lpsw",   4, S(0x8200,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "msch",   4, S(0xb232,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "pc",     4, S(0xb218,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "ptlb",   4, S(0xb20d,0,0),   0, S_MASK, 0, ANY, 0},
{ "rchp",   4, S(0xb23b,0,0),   0, S_MASK, 0, ANY, 0},
{ "rsch",   4, S(0xb238,0,0),   0, S_MASK, 0, ANY, 0},
{ "sac",    4, S(0xb219,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "sacf",   4, S(0xb279,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "sal",    4, S(0xb237,0,0),   0, S_MASK, 0, ANY, 0},
{ "schm",   4, S(0xb23c,0,0),   0, S_MASK, 0, ANY, 0},
{ "sck",    4, S(0xb204,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "sckc",   4, S(0xb206,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "spka",   4, S(0xb20a,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "spt",    4, S(0xb208,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "spx",    4, S(0xb210,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "ssch",   4, S(0xb233,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "ssm",    4, S(0x8000,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stap",   4, S(0xb212,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stck",   4, S(0xb205,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stckc",  4, S(0xb207,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stcps",  4, S(0xb23a,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stcrw",  4, S(0xb239,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stidp",  4, S(0xb202,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stpt",   4, S(0xb209,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stpx",   4, S(0xb211,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "stsch",  4, S(0xb234,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "tpi",    4, S(0xb236,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "ts",     4, S(0x9300,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},
{ "tsch",   4, S(0xb235,0,0),   0, S_MASK, 0, ANY, { S_D2, S_B2}},

/* RRE form instructions */
{ "bakr",  4, RRE(0xb240,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "bsa",   4, RRE(0xb25a,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "bsg",   4, RRE(0xb258,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "cksm",  4, RRE(0xb241,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "clst",  4, RRE(0xb25d,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "cpya",  4, RRE(0xb24d,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "cuse",  4, RRE(0xb257,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "dxr",   4, RRE(0xb22d,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "ear",   4, RRE(0xb24f,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "epar",  4, RRE(0xb226,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1}},
{ "ereg",  4, RRE(0xb249,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "esar",  4, RRE(0xb227,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1}},
{ "esta",  4, RRE(0xb24a,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "iac",   4, RRE(0xb224,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1}},
{ "ipm",   4, RRE(0xb222,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1}},
{ "ipte",  4, RRE(0xb221,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "iske",  4, RRE(0xb229,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "ivsk",  4, RRE(0xb223,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "lura",  4, RRE(0xb24b,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "msr",   4, RRE(0xb252,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "msta",  4, RRE(0xb247,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1}},
{ "mvpg",  4, RRE(0xb254,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "mvst",  4, RRE(0xb255,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "palb",  4, RRE(0xb248,0,0),   0, RRE_MASK, 0, ANY, 0},
{ "prbe",  4, RRE(0xb22a,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "pt",    4, RRE(0xb228,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "rrbe",  4, RRE(0xb22a,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "sar",   4, RRE(0xb24e,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "sqdr",  4, RRE(0xb244,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "sqer",  4, RRE(0xb245,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "srst",  4, RRE(0xb25e,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "ssar",  4, RRE(0xb225,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1}},
{ "sske",  4, RRE(0xb22b,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "stura", 4, RRE(0xb246,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "tar",   4, RRE(0xb24c,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
{ "tb",    4, RRE(0xb22c,0,0),   0, RRE_MASK, 0, ANY, { RRE_R1, RRE_R2}},
/* */
};
 
const int i370_num_opcodes =
  sizeof (i370_opcodes) / sizeof (i370_opcodes[0]);

/* The macro table.  This is only used by the assembler.  */

const struct i370_macro i370_macros[] = {
{ "b",     1,   ANY,	"bc  15,%0" },
{ "br",    1,   ANY,	"bcr 15,%0" },

{ "nop",   1,   ANY,	"bc  0,%0" },
{ "nopr",  1,   ANY,	"bcr 0,%0" },

{ "bh",    1,   ANY,	"bc  2,%0" },
{ "bhr",   1,   ANY,	"bcr 2,%0" },
{ "bl",    1,   ANY,	"bc  4,%0" },
{ "blr",   1,   ANY,	"bcr 4,%0" },
{ "be",    1,   ANY,	"bc  8,%0" },
{ "ber",   1,   ANY,	"bcr 8,%0" },

{ "bnh",    1,   ANY,	"bc  13,%0" },
{ "bnhr",   1,   ANY,	"bcr 13,%0" },
{ "bnl",    1,   ANY,	"bc  11,%0" },
{ "bnlr",   1,   ANY,	"bcr 11,%0" },
{ "bne",    1,   ANY,	"bc  7,%0" },
{ "bner",   1,   ANY,	"bcr 7,%0" },

{ "bp",    1,   ANY,	"bc  2,%0" },
{ "bpr",   1,   ANY,	"bcr 2,%0" },
{ "bm",    1,   ANY,	"bc  4,%0" },
{ "bmr",   1,   ANY,	"bcr 4,%0" },
{ "bz",    1,   ANY,	"bc  8,%0" },
{ "bzr",   1,   ANY,	"bcr 8,%0" },
{ "bo",    1,   ANY,	"bc  1,%0" },
{ "bor",   1,   ANY,	"bcr 1,%0" },

{ "bnp",    1,   ANY,	"bc  13,%0" },
{ "bnpr",   1,   ANY,	"bcr 13,%0" },
{ "bnm",    1,   ANY,	"bc  11,%0" },
{ "bnmr",   1,   ANY,	"bcr 11,%0" },
{ "bnz",    1,   ANY,	"bc  7,%0" },
{ "bnzr",   1,   ANY,	"bcr 7,%0" },
{ "bno",    1,   ANY,	"bc  14,%0" },
{ "bnor",   1,   ANY,	"bcr 14,%0" },

{ "sync",   0,   ANY,	"bcr 15,0" },

};

const int i370_num_macros =
  sizeof (i370_macros) / sizeof (i370_macros[0]);
