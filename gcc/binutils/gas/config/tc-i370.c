/* tc-i370.c -- Assembler for the IBM 360/370/390 instruction set.
   Loosely based on the ppc files by Linas Vepstas <linas@linas.org> 1998, 1999
   Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
   Written by Ian Lance Taylor, Cygnus Support.

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
   02111-1307, USA. */

/* This assembler implements a very hacked version of an elf-like thing 
 * that gcc emits (when gcc is suitably hacked).  To make it behave more 
 * HLASM-like, try turning on the -M or --mri flag (as there are various
 * similarities between HLASM and the MRI assemblers, such as section
 * names, lack of leading . in pseudo-ops, DC and DS, etc ...
 */

#include <stdio.h>
#include <ctype.h>
#include "as.h"
#include "subsegs.h"

#include "opcode/i370.h"

#ifdef OBJ_ELF
#include "elf/i370.h"
#endif

/* This is the assembler for the System/390 Architecture  */

/* Tell the main code what the endianness is.  */
extern int target_big_endian;


/* Generic assembler global variables which must be defined by all
   targets.  */

#ifdef OBJ_ELF
/* This string holds the chars that always start a comment.  If the
   pre-processor is disabled, these aren't very useful.  The macro
   tc_comment_chars points to this.  We use this, rather than the
   usual comment_chars, so that we can switch for Solaris conventions.  */
static const char i370_eabi_comment_chars[] = "#";

const char *i370_comment_chars = i370_eabi_comment_chars;
#else
const char comment_chars[] = "#";
#endif

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = "#*";

/* Characters which may be used to separate multiple commands on a
   single line.  */
const char line_separator_chars[] = ";";

/* Characters which are used to indicate an exponent in a floating
   point number.  */
const char EXP_CHARS[] = "eE";

/* Characters which mean that a number is a floating point constant,
   as in 0d1.0.  */
const char FLT_CHARS[] = "dD";


void
md_show_usage (stream)
     FILE *stream;
{
  fprintf(stream, "\
S/370 options: (these have not yet been tested may not work) \n\
-u        		ignored\n\
-mregnames        	Allow symbolic names for registers\n\
-mno-regnames        	Do not allow symbolic names for registers\n");
#ifdef OBJ_ELF
  fprintf(stream, "\
-mrelocatable        	support for GCC's -mrelocatble option\n\
-mrelocatable-lib       support for GCC's -mrelocatble-lib option\n\
-V        		print assembler version number\n");
#endif
}


static void i370_byte PARAMS ((int));
static void i370_tc PARAMS ((int));
static void i370_ebcdic PARAMS ((int));

static void i370_dc PARAMS ((int));
static void i370_ds PARAMS ((int));
static void i370_rmode PARAMS ((int));
static void i370_csect PARAMS ((int));
static void i370_dsect PARAMS ((int));
static void i370_ltorg PARAMS ((int));
static void i370_using PARAMS ((int));
static void i370_drop PARAMS ((int));
static void i370_make_relative PARAMS ((expressionS *exp));

#ifdef OBJ_ELF
static bfd_reloc_code_real_type i370_elf_suffix PARAMS ((char **, expressionS *));
static void i370_elf_cons PARAMS ((int));
static void i370_elf_rdata PARAMS ((int));
static void i370_elf_lcomm PARAMS ((int));
static void i370_elf_validate_fix PARAMS ((fixS *, segT));
#endif



/* The target specific pseudo-ops which we support.  */

const pseudo_typeS md_pseudo_table[] =
{
  /* Pseudo-ops which must be overridden.  */
  { "byte",        i370_byte,	0 },

  { "dc",          i370_dc,	0 },
  { "ds",          i370_ds,	0 },
  { "rmode",       i370_rmode,	0 },
  { "csect",       i370_csect,	0 },
  { "dsect",       i370_dsect,	0 },

  /* enable ebcdic strings e.g. for 3270 support */
  { "ebcdic",     i370_ebcdic,	0 },

#ifdef OBJ_ELF
  { "long",        i370_elf_cons,	4 },
  { "word",        i370_elf_cons,	4 },
  { "short",       i370_elf_cons,	2 },
  { "rdata",       i370_elf_rdata,	0 },
  { "rodata",      i370_elf_rdata,	0 },
  { "lcomm",       i370_elf_lcomm,	0 },
#endif

  /* This pseudo-op is used even when not generating XCOFF output.  */
  { "tc",        i370_tc,	0 },

  /* dump the literal pool */
  { "ltorg",     i370_ltorg,	0 },

  /* support the hlasm-style USING directive */
  { "using",     i370_using,	0 },
  { "drop",      i370_drop,	0 },

  { NULL,        NULL,		0 }
};

/* ***************************************************************** */

/* Whether to use user friendly register names.  */
#define TARGET_REG_NAMES_P true

static boolean reg_names_p = TARGET_REG_NAMES_P;

static boolean register_name PARAMS ((expressionS *));
static void i370_set_cpu PARAMS ((void));
static i370_insn_t i370_insert_operand
  PARAMS ((i370_insn_t insn, const struct i370_operand *operand,
           offsetT val, char *file, unsigned int line));
static void i370_macro PARAMS ((char *str, const struct i370_macro *macro));

/* Predefined register names if -mregnames */
/* In general, there are lots of them, in an attempt to be compatible */
/* with a number of assemblers.                      */

/* Structure to hold information about predefined registers.  */
struct pd_reg
  {
    char *name;
    int value;
  };
 
/* List of registers that are pre-defined:

   Each general register has predefined names of the form:
   1. r<reg_num> which has the value <reg_num>.
   2. r.<reg_num> which has the value <reg_num>.


   Each floating point register has predefined names of the form:
   1. f<reg_num> which has the value <reg_num>.
   2. f.<reg_num> which has the value <reg_num>.

   There are only four floating point registers, and these are 
   commonly labelled 0,2,4 and 6.  Thus, there is no f1, f3, etc.


   There are individual registers as well:
   rbase or r.base has the value  3  (base register)
   rpgt or r.pgt   has the value  4  (page origin table pointer)
   rarg or r.arg   has the value 11  (argument pointer)
   rtca or r.tca   has the value 12  (table of contents pointer)
   rtoc or r.toc   has the value 12  (table of contents pointer)
   sp or r.sp      has the value 13  (stack pointer)
   dsa or r.dsa    has the value 13  (stack pointer)
   lr              has the value 14  (link reg)
 
   The table is sorted. Suitable for searching by a binary search. */

static const struct pd_reg pre_defined_registers[] =
{
  { "arg", 11 },  /* Argument Pointer */
  { "base", 3 },  /* Base Reg */

  { "f.0", 0 },     /* Floating point registers */
  { "f.2", 2 }, 
  { "f.4", 4 }, 
  { "f.6", 6 }, 

  { "f0", 0 }, 
  { "f2", 2 }, 
  { "f4", 4 }, 
  { "f6", 6 }, 


  { "dsa",13 },    /* stack pointer */
  { "lr", 14 },    /* Link Register */
  { "pgt", 4 },    /* Page Origin Table Pointer */

  { "r.0", 0 },    /* General Purpose Registers */
  { "r.1", 1 },
  { "r.10", 10 },
  { "r.11", 11 },
  { "r.12", 12 },
  { "r.13", 13 },
  { "r.14", 14 },
  { "r.15", 15 },
  { "r.2", 2 },
  { "r.3", 3 },
  { "r.4", 4 },
  { "r.5", 5 },
  { "r.6", 6 },
  { "r.7", 7 },
  { "r.8", 8 },
  { "r.9", 9 },

  { "r.arg", 11 },  /* Argument Pointer */
  { "r.base", 3 },  /* Base Reg */
  { "r.dsa", 13 },  /* Stack Pointer */
  { "r.pgt", 4 },   /* Page Origin Table Pointer */
  { "r.sp", 13 },   /* Stack Pointer */

  { "r.tca", 12 },  /* Pointer to the table of contents */
  { "r.toc", 12 },  /* Pointer to the table of contents */

  { "r0", 0 },     /* More general purpose registers */
  { "r1", 1 },
  { "r10", 10 },
  { "r11", 11 },
  { "r12", 12 },
  { "r13", 13 },
  { "r14", 14 },
  { "r15", 15 },
  { "r2", 2 },
  { "r3", 3 },
  { "r4", 4 },
  { "r5", 5 },
  { "r6", 6 },
  { "r7", 7 },
  { "r8", 8 },
  { "r9", 9 },

  { "rbase", 3 },  /* Base Reg */

  { "rtca", 12 },  /* Pointer to the table of contents */
  { "rtoc", 12 },  /* Pointer to the table of contents */

  { "sp", 13 },   /* Stack Pointer */
 
};
 
#define REG_NAME_CNT        (sizeof(pre_defined_registers) / sizeof(struct pd_reg))
 
/* Given NAME, find the register number associated with that name, return
   the integer value associated with the given name or -1 on failure.  */

static int reg_name_search
  PARAMS ((const struct pd_reg *, int, const char * name));

static int
reg_name_search (regs, regcount, name)
     const struct pd_reg *regs;
     int regcount;
     const char *name;
{
  int middle, low, high;
  int cmp;

  low = 0;
  high = regcount - 1;

  do
    {
      middle = (low + high) / 2;
      cmp = strcasecmp (name, regs[middle].name);
      if (cmp < 0)
        high = middle - 1;
      else if (cmp > 0)
        low = middle + 1;
      else
        return regs[middle].value;
    }
  while (low <= high);

  return -1;
}
 
/*
 * Summary of register_name().
 *
 * in:        Input_line_pointer points to 1st char of operand.
 *
 * out:        A expressionS.
 *      The operand may have been a register: in this case, X_op == O_register,
 *      X_add_number is set to the register number, and truth is returned.
 *        Input_line_pointer->(next non-blank) char after operand, or is in its
 *      original state.
 */

static boolean
register_name (expressionP)
     expressionS *expressionP;
{
  int reg_number;
  char *name;
  char *start;
  char c;

  /* Find the spelling of the operand */
  start = name = input_line_pointer;
  if (name[0] == '%' && isalpha (name[1]))
    name = ++input_line_pointer;

  else if (!reg_names_p)
    return false;

  while (' ' == *name)  name = ++input_line_pointer;

  /* if its a number, treat it as a number */
  /* if its alpha, look to see if it's in the register table */
  if (!isalpha (name[0])) {
    reg_number = get_single_number();
    c = get_symbol_end ();
  } else {
    c = get_symbol_end ();
    reg_number = reg_name_search (pre_defined_registers, REG_NAME_CNT, name);
  }

  /* if numeric, make sure its not out of bounds */
  if ((0 <= reg_number) && (16 >= reg_number)) 
    {
      expressionP->X_op = O_register;
      expressionP->X_add_number = reg_number;
      
      /* make the rest nice */
      expressionP->X_add_symbol = NULL;
      expressionP->X_op_symbol = NULL;
      *input_line_pointer = c;   /* put back the delimiting char */
      return true;
    }

    /* reset the line as if we had not done anything */
    *input_line_pointer = c;   /* put back the delimiting char */
    input_line_pointer = start; /* reset input_line pointer */
    return false;
}

/* Local variables.  */

/* The type of processor we are assembling for.  This is one or more
   of the I370_OPCODE flags defined in opcode/i370.h.  */
static int i370_cpu = 0;

/* The size of the processor we are assembling for.  This is either
   I370_OPCODE_32 or I370_OPCODE_64.  */
static int i370_size = I370_OPCODE_32;

/* The base register to use for opcode with optional operands */ 
static int i370_using_regno = -1;

/* The base address for address literals */
static expressionS i370_using_baseaddr;

/* Opcode hash table.  */
static struct hash_control *i370_hash;

/* Macro hash table.  */
static struct hash_control *i370_macro_hash;

#ifdef OBJ_ELF
/* What type of shared library support to use */
static enum { SHLIB_NONE, SHLIB_PIC, SHILB_MRELOCATABLE } shlib = SHLIB_NONE;
#endif

/* Flags to set in the elf header */
static flagword i370_flags = 0;

#ifndef WORKING_DOT_WORD
const int md_short_jump_size = 4;
const int md_long_jump_size = 4;
#endif

#ifdef OBJ_ELF
CONST char *md_shortopts = "l:um:K:VQ:";
#else
CONST char *md_shortopts = "um:";
#endif
struct option md_longopts[] = {
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
    case 'u':
      /* -u means that any undefined symbols should be treated as
         external, which is the default for gas anyhow.  */
      break;

#ifdef OBJ_ELF
    case 'K':
      /* Recognize -K PIC */
      if (strcmp (arg, "PIC") == 0 || strcmp (arg, "pic") == 0)
        {
          shlib = SHLIB_PIC;
          i370_flags |= EF_I370_RELOCATABLE_LIB;
        }
      else
        return 0;

      break;
#endif

    case 'm':

      /* -m360 mean to assemble for the ancient 360 architecture */
     if (strcmp (arg, "360") == 0 || strcmp (arg, "i360") == 0)
        i370_cpu = I370_OPCODE_360;
     /* -mxa means to assemble for the IBM 370 XA  */
     else if (strcmp (arg, "xa") == 0)
        i370_cpu = I370_OPCODE_370_XA;
      /* -many means to assemble for any architecture (370/XA).  */
      else if (strcmp (arg, "any") == 0)
        i370_cpu = I370_OPCODE_370;

      else if (strcmp (arg, "regnames") == 0)
        reg_names_p = true;

      else if (strcmp (arg, "no-regnames") == 0)
        reg_names_p = false;

#ifdef OBJ_ELF
      /* -mrelocatable/-mrelocatable-lib -- warn about initializations that require relocation */
      else if (strcmp (arg, "relocatable") == 0)
        {
          shlib = SHILB_MRELOCATABLE;
          i370_flags |= EF_I370_RELOCATABLE;
        }

      else if (strcmp (arg, "relocatable-lib") == 0)
        {
          shlib = SHILB_MRELOCATABLE;
          i370_flags |= EF_I370_RELOCATABLE_LIB;
        }

#endif
      else
        {
          as_bad ("invalid switch -m%s", arg);
          return 0;
        }
      break;

#ifdef OBJ_ELF
      /* -V: SVR4 argument to print version ID.  */
    case 'V':
      print_version_id ();
      break;

      /* -Qy, -Qn: SVR4 arguments controlling whether a .comment section
         should be emitted or not.  FIXME: Not implemented.  */
    case 'Q':
      break;

#endif

    default:
      return 0;
    }

  return 1;
}
 

/* Set i370_cpu if it is not already set.  */

static void
i370_set_cpu ()
{
  const char *default_os  = TARGET_OS;
  const char *default_cpu = TARGET_CPU;

  if (i370_cpu == 0)
    {
      if (strcmp (default_cpu, "i360") == 0)
        i370_cpu = I370_OPCODE_360;
      else if (strcmp (default_cpu, "i370") == 0)
        i370_cpu = I370_OPCODE_370;
      else if (strcmp (default_cpu, "XA") == 0)
        i370_cpu = I370_OPCODE_370_XA;
      else
        as_fatal ("Unknown default cpu = %s, os = %s", default_cpu, default_os);
    }
}
 
/* Figure out the BFD architecture to use.  */
// hack alert -- specify the different 370 architectures

enum bfd_architecture
i370_arch ()
{
   return bfd_arch_i370;
//  const char *default_cpu = TARGET_CPU;
//   ppc_set_cpu ();
// 
//   if ((ppc_cpu & PPC_OPCODE_PPC) != 0)
//     return bfd_arch_powerpc;
//   else if ((ppc_cpu & PPC_OPCODE_POWER) != 0)
//     return bfd_arch_rs6000;
//   else if ((ppc_cpu & (PPC_OPCODE_COMMON | PPC_OPCODE_ANY)) != 0)
//     {
//       if (strcmp (default_cpu, "rs6000") == 0)
//         return bfd_arch_rs6000;
//       else if (strcmp (default_cpu, "powerpc") == 0
//                || strcmp (default_cpu, "powerpcle") == 0)
//         return bfd_arch_powerpc;
//     }
// 
//   as_fatal ("Neither Power nor PowerPC opcodes were selected.");
//   return bfd_arch_unknown;
}
// 
/* This function is called when the assembler starts up.  It is called
   after the options have been parsed and the output file has been
   opened.  */

void
md_begin ()
{
  register const struct i370_opcode *op;
  const struct i370_opcode *op_end;
  const struct i370_macro *macro;
  const struct i370_macro *macro_end;
  boolean dup_insn = false;

  i370_set_cpu ();

#ifdef OBJ_ELF
  /* Set the ELF flags if desired. */
  if (i370_flags)
    bfd_set_private_flags (stdoutput, i370_flags);
#endif

  /* Insert the opcodes into a hash table.  */
  i370_hash = hash_new ();

   op_end = i370_opcodes + i370_num_opcodes;
   for (op = i370_opcodes; op < op_end; op++)
     {
       know ((op->opcode & op->mask) == op->opcode);
 
       if ((op->flags & i370_cpu) != 0
          && ((op->flags & (I370_OPCODE_32 | I370_OPCODE_64)) == 0
              || (op->flags & (I370_OPCODE_32 | I370_OPCODE_64)) == i370_size))
         {
           const char *retval;
 
           retval = hash_insert (i370_hash, op->name, (PTR) op);
           if (retval != (const char *) NULL)
             {
 
               as_bad ("Internal assembler error for instruction %s", op->name);
               dup_insn = true;
             }
         }
     }
 
  /* Insert the macros into a hash table.  */
  i370_macro_hash = hash_new ();

  macro_end = i370_macros + i370_num_macros;
  for (macro = i370_macros; macro < macro_end; macro++)
    {
      if ((macro->flags & i370_cpu) != 0)
        {
          const char *retval;

          retval = hash_insert (i370_macro_hash, macro->name, (PTR) macro);
          if (retval != (const char *) NULL)
            {
              as_bad ("Internal assembler error for macro %s", macro->name);
              dup_insn = true;
            }
        }
    }

  if (dup_insn)
    abort ();
}

/* Insert an operand value into an instruction.  */

static i370_insn_t 
i370_insert_operand (insn, operand, val, file, line)
     i370_insn_t insn;
     const struct i370_operand *operand;
     offsetT val;
     char *file;
     unsigned int line;
{
//   if (operand->bits != 32)
//     {
//       long min, max;
//       offsetT test;
// 
//       if ((operand->flags & PPC_OPERAND_SIGNED) != 0)
//         {
//           if ((operand->flags & PPC_OPERAND_SIGNOPT) != 0
//               && ppc_size == PPC_OPCODE_32)
//             max = (1 << operand->bits) - 1;
//           else
//             max = (1 << (operand->bits - 1)) - 1;
//           min = - (1 << (operand->bits - 1));
// 
//           if (ppc_size == PPC_OPCODE_32)
//             {
//               /* Some people write 32 bit hex constants with the sign
//         	 extension done by hand.  This shouldn't really be
//         	 valid, but, to permit this code to assemble on a 64
//         	 bit host, we sign extend the 32 bit value.  */
//               if (val > 0
//         	  && (val & 0x80000000) != 0
//         	  && (val & 0xffffffff) == val)
//         	{
//         	  val -= 0x80000000;
//         	  val -= 0x80000000;
//         	}
//             }
//         }
//       else
//         {
//           max = (1 << operand->bits) - 1;
//           min = 0;
//         }
// 
//       if ((operand->flags & PPC_OPERAND_NEGATIVE) != 0)
//         test = - val;
//       else
//         test = val;
// 
//       if (test < (offsetT) min || test > (offsetT) max)
//         {
//           const char *err =
//             "operand out of range (%s not between %ld and %ld)";
//           char buf[100];
// 
//           sprint_value (buf, test);
//           if (file == (char *) NULL)
//             as_bad (err, buf, min, max);
//           else
//             as_bad_where (file, line, err, buf, min, max);
//         }
//     }

  if (operand->insert)
    {
      const char *errmsg;

      /* used for 48-bit insn's */
      errmsg = NULL;
      insn = (*operand->insert) (insn, (long) val, &errmsg);
      if (errmsg != (const char *) NULL)
        as_bad (errmsg);
    }
  else
    {
    /* this is used only for 16, 32 bit insn's */
    insn.i[0] |= (((long) val & ((1 << operand->bits) - 1))
             << operand->shift);
    }

  return insn;
}


#ifdef OBJ_ELF
/* Parse @got, etc. and return the desired relocation. 
 * Currently, i370 does not support (don't really need to support) any
 * of these fancier markups ... for example, no one is going to 
 * write 'L 6,=V(bogus)@got' it just doesn't make sense (at least to me).
 * So basically, we could get away with this routine returning
 * BFD_RELOC_UNUSED in all circumstances.  However, I'll leave
 * in for now in case someone ambitious finds a good use for this stuff ...
 * this routine was pretty much just copied from the powerpc code ...
 */
static bfd_reloc_code_real_type
i370_elf_suffix (str_p, exp_p)
     char **str_p;
     expressionS *exp_p;
{
  struct map_bfd {
    char *string;
    int length;
    bfd_reloc_code_real_type reloc;
  };

  char ident[20];
  char *str = *str_p;
  char *str2;
  int ch;
  int len;
  struct map_bfd *ptr;

#define MAP(str,reloc) { str, sizeof(str)-1, reloc }

  static struct map_bfd mapping[] = {
//     MAP ("l",        	BFD_RELOC_LO16),
//     MAP ("h",        	BFD_RELOC_HI16),
//     MAP ("ha",        	BFD_RELOC_HI16_S),
//     MAP ("got",        	BFD_RELOC_16_GOTOFF),
//     MAP ("got@l",        BFD_RELOC_LO16_GOTOFF),
//     MAP ("got@h",        BFD_RELOC_HI16_GOTOFF),
//     MAP ("got@ha",        BFD_RELOC_HI16_S_GOTOFF),
    MAP ("fixup",        BFD_RELOC_CTOR),          /* warnings with -mrelocatable */
//     MAP ("plt",		BFD_RELOC_24_PLT_PCREL),
//     MAP ("pltrel24",		BFD_RELOC_24_PLT_PCREL),
//     MAP ("plt@l",		BFD_RELOC_LO16_PLTOFF),
//     MAP ("plt@h",		BFD_RELOC_HI16_PLTOFF),
//     MAP ("plt@ha",		BFD_RELOC_HI16_S_PLTOFF),
//     MAP ("copy",        BFD_RELOC_PPC_COPY),

//     MAP ("globdat",        BFD_RELOC_PPC_GLOB_DAT),
//     MAP ("pltrel",        BFD_RELOC_32_PLT_PCREL),
//     MAP ("sdarel",        BFD_RELOC_GPREL16),
//     MAP ("sectoff",        BFD_RELOC_32_BASEREL),
//     MAP ("sectoff@l",        BFD_RELOC_LO16_BASEREL),
//     MAP ("sectoff@h",        BFD_RELOC_HI16_BASEREL),
//     MAP ("sectoff@ha",        BFD_RELOC_HI16_S_BASEREL),
// 
    { (char *)0,        0,	BFD_RELOC_UNUSED }
   };

  if (*str++ != '@')
    return BFD_RELOC_UNUSED;

  for (ch = *str, str2 = ident;
       (str2 < ident + sizeof (ident) - 1
        && (isalnum (ch) || ch == '@'));
       ch = *++str)
    {
      *str2++ = (islower (ch)) ? ch : tolower (ch);
    }

  *str2 = '\0';
  len = str2 - ident;

  ch = ident[0];
  for (ptr = &mapping[0]; ptr->length > 0; ptr++)
    if (ch == ptr->string[0]
        && len == ptr->length
        && memcmp (ident, ptr->string, ptr->length) == 0)
      {
        if (exp_p->X_add_number != 0
            && (ptr->reloc == BFD_RELOC_16_GOTOFF
        	|| ptr->reloc == BFD_RELOC_LO16_GOTOFF
        	|| ptr->reloc == BFD_RELOC_HI16_GOTOFF
        	|| ptr->reloc == BFD_RELOC_HI16_S_GOTOFF))
          as_warn ("identifier+constant@got means identifier@got+constant");

        /* Now check for identifier@suffix+constant */
        if (*str == '-' || *str == '+')
          {
            char *orig_line = input_line_pointer;
            expressionS new_exp;

            input_line_pointer = str;
            expression (&new_exp);
            if (new_exp.X_op == O_constant)
              {
        	exp_p->X_add_number += new_exp.X_add_number;
        	str = input_line_pointer;
              }

            if (&input_line_pointer != str_p)
              input_line_pointer = orig_line;
          }

        *str_p = str;
        return ptr->reloc;
      }

  return BFD_RELOC_UNUSED;
}

/* Like normal .long/.short/.word, except support @got, etc. */
/* clobbers input_line_pointer, checks end-of-line. */
static void
i370_elf_cons (nbytes)
     register int nbytes;        /* 1=.byte, 2=.word, 4=.long */
{
  expressionS exp;
  bfd_reloc_code_real_type reloc;

  if (is_it_end_of_statement ())
    {
      demand_empty_rest_of_line ();
      return;
    }

  do
    {
      expression (&exp);
      if (exp.X_op == O_symbol
          && *input_line_pointer == '@'
          && (reloc = i370_elf_suffix (&input_line_pointer, &exp)) != BFD_RELOC_UNUSED)
        {
          reloc_howto_type *reloc_howto = bfd_reloc_type_lookup (stdoutput, reloc);
          int size = bfd_get_reloc_size (reloc_howto);

          if (size > nbytes)
            as_bad ("%s relocations do not fit in %d bytes\n", reloc_howto->name, nbytes);

          else
            {
              register char *p = frag_more ((int) nbytes);
              int offset = nbytes - size;

              fix_new_exp (frag_now, p - frag_now->fr_literal + offset, size, &exp, 0, reloc);
            }
        }
      else
        emit_expr (&exp, (unsigned int) nbytes);
    }
  while (*input_line_pointer++ == ',');

  input_line_pointer--;        	/* Put terminator back into stream. */
  demand_empty_rest_of_line ();
}
 

/* ASCII to EBCDIC conversion table.  */
static unsigned char ascebc[256] =
{
 /*00  NL    SH    SX    EX    ET    NQ    AK    BL */
     0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,
 /*08  BS    HT    LF    VT    FF    CR    SO    SI */
     0x16, 0x05, 0x15, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
 /*10  DL    D1    D2    D3    D4    NK    SN    EB */
     0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,
 /*18  CN    EM    SB    EC    FS    GS    RS    US */
     0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,
 /*20  SP     !     "     #     $     %     &     ' */
     0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
 /*28   (     )     *     +     ,     -    .      / */
     0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
 /*30   0     1     2     3     4     5     6     7 */
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
 /*38   8     9     :     ;     <     =     >     ? */
     0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
 /*40   @     A     B     C     D     E     F     G */
     0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
 /*48   H     I     J     K     L     M     N     O */
     0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
 /*50   P     Q     R     S     T     U     V     W */
     0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,
 /*58   X     Y     Z     [     \     ]     ^     _ */
     0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,
 /*60   `     a     b     c     d     e     f     g */
     0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
 /*68   h     i     j     k     l     m     n     o */
     0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
 /*70   p     q     r     s     t     u     v     w */
     0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 /*78   x     y     z     {     |     }     ~    DL */
     0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x07,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
     0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0xFF
};

/* EBCDIC to ASCII conversion table.  */
unsigned char ebcasc[256] =
{
 /*00  NU    SH    SX    EX    PF    HT    LC    DL */
     0x00, 0x01, 0x02, 0x03, 0x00, 0x09, 0x00, 0x7F,
 /*08              SM    VT    FF    CR    SO    SI */
     0x00, 0x00, 0x00, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
 /*10  DE    D1    D2    TM    RS    NL    BS    IL */
     0x10, 0x11, 0x12, 0x13, 0x14, 0x0A, 0x08, 0x00,
 /*18  CN    EM    CC    C1    FS    GS    RS    US */
     0x18, 0x19, 0x00, 0x00, 0x1C, 0x1D, 0x1E, 0x1F,
 /*20  DS    SS    FS          BP    LF    EB    EC */
     0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x17, 0x1B,
 /*28              SM    C2    EQ    AK    BL       */
     0x00, 0x00, 0x00, 0x00, 0x05, 0x06, 0x07, 0x00,
 /*30              SY          PN    RS    UC    ET */
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
 /*38                    C3    D4    NK          SU */
     0x00, 0x00, 0x00, 0x00, 0x14, 0x15, 0x00, 0x1A,
 /*40  SP                                           */
     0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*48                     .     <     (     +     | */
     0x00, 0x00, 0x00, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
 /*50   &                                           */
     0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*58               !     $     *     )     ;     ^ */
     0x00, 0x00, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
 /*60   -     /                                     */
     0x2D, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*68                     ,     %     _     >     ? */
     0x00, 0x00, 0x00, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
 /*70                                               */
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*78         `     :     #     @     '     =     " */
     0x00, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
 /*80         a     b     c     d     e     f     g */
     0x00, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
 /*88   h     i           {                         */
     0x68, 0x69, 0x00, 0x7B, 0x00, 0x00, 0x00, 0x00,
 /*90         j     k     l     m     n     o     p */
     0x00, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
 /*98   q     r           }                         */
     0x71, 0x72, 0x00, 0x7D, 0x00, 0x00, 0x00, 0x00,
 /*A0         ~     s     t     u     v     w     x */
     0x00, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
 /*A8   y     z                       [             */
     0x79, 0x7A, 0x00, 0x00, 0x00, 0x5B, 0x00, 0x00,
 /*B0                                               */
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*B8                                 ]             */
     0x00, 0x00, 0x00, 0x00, 0x00, 0x5D, 0x00, 0x00,
 /*C0   {     A     B     C     D     E     F     G */
     0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 /*C8   H     I                                     */
     0x48, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*D0   }     J     K     L     M     N     O     P */
     0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
 /*D8   Q     R                                     */
     0x51, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*E0   \           S     T     U     V     W     X */
     0x5C, 0x00, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
 /*E8   Y     Z                                     */
     0x59, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 /*F0   0     1     2     3     4     5     6     7 */
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
 /*F8   8     9                                     */
     0x38, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF
};

/* ebcdic translation tables needed for 3270 support */
static void
i370_ebcdic (xxx)
     int xxx; 
{
  char *p, *end;
  char delim = 0;
  size_t nbytes;

  nbytes = strlen (input_line_pointer);
  end = input_line_pointer + nbytes;
  while ('\r' == *end) end --;
  while ('\n' == *end) end --;

  delim = *input_line_pointer;
  if (('\'' == delim) || ('\"' == delim)) {
    input_line_pointer ++;
    end = rindex (input_line_pointer, delim);
  }

  if (end > input_line_pointer) {
    nbytes = end - input_line_pointer +1;
    p = frag_more (nbytes);
    while (end > input_line_pointer) {
       *p = ascebc [(unsigned char)(*input_line_pointer)];
       ++p; ++input_line_pointer;
    }
    *p = '\0';
  }
  if (delim == *input_line_pointer) ++input_line_pointer;
}


/* stub out a couple of routines */
static void
i370_rmode (xxx)
     int xxx; 
{
  as_tsktsk ("rmode ignored");
}

static void
i370_dsect (xxx)
     int xxx; 
{
  as_tsktsk ("dsect not supported");
}

static void
i370_csect (xxx)
     int xxx; 
{
  char *save_line = input_line_pointer;
  static char section[] = ".text\n";

  /* Just pretend this is .section .text */
  input_line_pointer = section;
  obj_elf_section (xxx);

  input_line_pointer = save_line;
}


/* DC Define Const  is only partially supported.
 * For samplecode on what to do, look at i370_elf_cons() above.
 * This code handles pseudoops of the style 
 * DC   D'3.141592653'   # in sysv4, .double 3.14159265
 * DC   F'1'             # in sysv4, .long   1
 */
static void
i370_dc(xxx)
     int xxx; 
{
  char * p, tmp[50];
  int nbytes=0;
  expressionS exp;
  char type=0;

  if (is_it_end_of_statement ())
    {
      demand_empty_rest_of_line ();
      return;
    }

  /* figure out the size */
  type = *input_line_pointer++;
  switch (type) {
     case 'H':  /* 16-bit */
        nbytes = 2;
        break;
     case 'E':  /* 32-bit */
     case 'F':  /* 32-bit */
        nbytes = 4;
        break;
     case 'D':  /* 64-bit */
        nbytes = 8;
        break;
     default:
        as_bad ("unsupported DC type");
       return;
  }

  /* get rid of pesky quotes */
  if ('\'' == *input_line_pointer) {
     char * close;
       ++input_line_pointer;
       close = strchr (input_line_pointer, '\'');
       if (close) *close= ' ';
       else as_bad ("missing end-quote");
    }
    if ('\"' == *input_line_pointer) {
       char * close;
       ++input_line_pointer;
       close = strchr (input_line_pointer, '\"');
       if (close) *close= ' ';
       else as_bad ("missing end-quote");
    }

  switch (type) {
     case 'H':  /* 16-bit */
     case 'F':  /* 32-bit */
        expression (&exp);
        emit_expr (&exp, nbytes);
        break;
     case 'E':  /* 32-bit */
     case 'D':  /* 64-bit */
        md_atof (type, tmp, &nbytes);
        p = frag_more (nbytes);
        memcpy (p, tmp, nbytes);
        break;
     default:
        as_bad ("unsupported DC type");
       return;
  }

  demand_empty_rest_of_line ();
}


/* provide minimal support for DS Define Storage */
static void
i370_ds (xxx)
     int xxx; 
{
  /* DS 0H or DS 0F or DS 0D */
  if ('0' == *input_line_pointer) 
    {
      int alignment = 0;  /* left shift 1<<align */
      input_line_pointer ++;
      switch (*input_line_pointer++) {
         case 'H':  /* 16-bit */
            alignment = 1;
            break;
         case 'F':  /* 32-bit */
            alignment = 2;
            break;
         case 'D':  /* 64-bit */
            alignment = 3;
            break;
         default:
            as_bad ("unsupported alignment");
	    return;
      }
      frag_align (alignment, 0, 0);
      record_alignment (now_seg, alignment);
    }
  else 
    {
      as_bad ("this DS form not yet supported");
    }
}

/* Solaris pseudo op to change to the .rodata section.  */
static void
i370_elf_rdata (xxx)
     int xxx;
{
  char *save_line = input_line_pointer;
  static char section[] = ".rodata\n";

  /* Just pretend this is .section .rodata */
  input_line_pointer = section;
  obj_elf_section (xxx);

  input_line_pointer = save_line;
}

/* Pseudo op to make file scope bss items */
static void
i370_elf_lcomm(xxx)
     int xxx;
{
  register char *name;
  register char c;
  register char *p;
  offsetT size;
  register symbolS *symbolP;
  offsetT align;
  segT old_sec;
  int old_subsec;
  char *pfrag;
  int align2;

  name = input_line_pointer;
  c = get_symbol_end ();

  /* just after name is now '\0' */
  p = input_line_pointer;
  *p = c;
  SKIP_WHITESPACE ();
  if (*input_line_pointer != ',')
    {
      as_bad ("Expected comma after symbol-name: rest of line ignored.");
      ignore_rest_of_line ();
      return;
    }

  input_line_pointer++;        	/* skip ',' */
  if ((size = get_absolute_expression ()) < 0)
    {
      as_warn (".COMMon length (%ld.) <0! Ignored.", (long) size);
      ignore_rest_of_line ();
      return;
    }

  /* The third argument to .lcomm is the alignment.  */
  if (*input_line_pointer != ',')
    align = 8;
  else
    {
      ++input_line_pointer;
      align = get_absolute_expression ();
      if (align <= 0)
        {
          as_warn ("ignoring bad alignment");
          align = 8;
        }
    }

  *p = 0;
  symbolP = symbol_find_or_make (name);
  *p = c;

  if (S_IS_DEFINED (symbolP) && ! S_IS_COMMON (symbolP))
    {
      as_bad ("Ignoring attempt to re-define symbol `%s'.",
              S_GET_NAME (symbolP));
      ignore_rest_of_line ();
      return;
    }

  if (S_GET_VALUE (symbolP) && S_GET_VALUE (symbolP) != (valueT) size)
    {
      as_bad ("Length of .lcomm \"%s\" is already %ld. Not changed to %ld.",
              S_GET_NAME (symbolP),
              (long) S_GET_VALUE (symbolP),
              (long) size);

      ignore_rest_of_line ();
      return;
    }

  /* allocate_bss: */
  old_sec = now_seg;
  old_subsec = now_subseg;
  if (align)
    {
      /* convert to a power of 2 alignment */
      for (align2 = 0; (align & 1) == 0; align >>= 1, ++align2);
      if (align != 1)
        {
          as_bad ("Common alignment not a power of 2");
          ignore_rest_of_line ();
          return;
        }
    }
  else
    align2 = 0;

  record_alignment (bss_section, align2);
  subseg_set (bss_section, 0);
  if (align2)
    frag_align (align2, 0, 0);
  if (S_GET_SEGMENT (symbolP) == bss_section)
    symbolP->sy_frag->fr_symbol = 0;
  symbolP->sy_frag = frag_now;
  pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP, size,
        	    (char *) 0);
  *pfrag = 0;
  S_SET_SIZE (symbolP, size);
  S_SET_SEGMENT (symbolP, bss_section);
  subseg_set (old_sec, old_subsec);
  demand_empty_rest_of_line ();
}

/* Validate any relocations emitted for -mrelocatable, possibly adding
   fixups for word relocations in writable segments, so we can adjust
   them at runtime.  */
static void
i370_elf_validate_fix (fixp, seg)
     fixS *fixp;
     segT seg;
{
  if (fixp->fx_done || fixp->fx_pcrel)
    return;

  switch (shlib)
    {
    case SHLIB_NONE:
    case SHLIB_PIC:
      return;

    case SHILB_MRELOCATABLE:
      if (fixp->fx_r_type <= BFD_RELOC_UNUSED
          && fixp->fx_r_type != BFD_RELOC_16_GOTOFF
          && fixp->fx_r_type != BFD_RELOC_HI16_GOTOFF
          && fixp->fx_r_type != BFD_RELOC_LO16_GOTOFF
          && fixp->fx_r_type != BFD_RELOC_HI16_S_GOTOFF
          && fixp->fx_r_type != BFD_RELOC_32_BASEREL
          && fixp->fx_r_type != BFD_RELOC_LO16_BASEREL
          && fixp->fx_r_type != BFD_RELOC_HI16_BASEREL
          && fixp->fx_r_type != BFD_RELOC_HI16_S_BASEREL
          && strcmp (segment_name (seg), ".got2") != 0
          && strcmp (segment_name (seg), ".dtors") != 0
          && strcmp (segment_name (seg), ".ctors") != 0
          && strcmp (segment_name (seg), ".fixup") != 0
          && strcmp (segment_name (seg), ".stab") != 0
          && strcmp (segment_name (seg), ".gcc_except_table") != 0
          && strcmp (segment_name (seg), ".ex_shared") != 0)
        {
          if ((seg->flags & (SEC_READONLY | SEC_CODE)) != 0
              || fixp->fx_r_type != BFD_RELOC_CTOR)
            {
              as_bad_where (fixp->fx_file, fixp->fx_line,
        		    "Relocation cannot be done when using -mrelocatable");
            }
        }
      return;
    }
}
#endif /* OBJ_ELF */



#define LITERAL_POOL_SUPPORT
#ifdef LITERAL_POOL_SUPPORT
/* Provide support for literal pools within the text section. */
/* Loosely based on similar code from tc-arm.c  */
/*
 * current_poolP points at the symbol that names our literal pool
 * lit_pool_num increments from zero to infinity and uniquely id's 
 *    -- its used to generate the current_poolP symbol name.
 */

#define MAX_LITERAL_POOL_SIZE 1024

typedef struct literalS
{
  struct expressionS  exp;
  char * sym_name;
  char size;  /* 1,2,4 or 8 */
} literalT;

literalT literals[MAX_LITERAL_POOL_SIZE];
int next_literal_pool_place = 0; /* Next free entry in the pool */

static symbolS *current_poolP = NULL;
static int lit_pool_num = 1;

/* create a new, empty symbol */
static symbolS *
symbol_make_empty (void) 
{
  symbolS *symbolP; 

  symbolP = (symbolS *) obstack_alloc (&notes, sizeof (symbolS));

  /* symbol must be born in some fixed state.  This seems as good as any. */
  memset (symbolP, 0, sizeof (symbolS));

  symbolP->bsym = bfd_make_empty_symbol (stdoutput);
  assert (symbolP->bsym != 0);
  symbolP->bsym->udata.p = (PTR) symbolP;

  return symbolP;
}

/* add an expression to the literal pool */
static  void
add_to_lit_pool (expressionS *exx, char *name, int sz)
{
  int lit_count = 0;
  int total_sz = 0;

  /* start a new pool, if necessary */
  if (NULL == current_poolP) current_poolP = symbol_make_empty();
 
  /* Check if this literal value is already in the pool: */
  /* hack alert -- we should probably be checking expressions
   * of type O_symbol as well ...  */
  /* hack alert XXX this is probably(certainly?) broken for O_big,
   * which includes 64-bit long-longs ... 
   */
  while (lit_count < next_literal_pool_place)
    {
      if (   
             ( (exx->X_op == O_constant) ) 
          && (literals[lit_count].exp.X_op == exx->X_op)
          && (literals[lit_count].exp.X_add_number == exx->X_add_number)
          && (literals[lit_count].exp.X_unsigned == exx->X_unsigned)
          && (literals[lit_count].size  == sz)
          )
        { break; }
      else
      if (    (NULL != literals[lit_count].sym_name)
          &&  (NULL != name)
          &&  (!strcmp (name, literals[lit_count].sym_name)) )
        { break; }
      total_sz += literals[lit_count].size;
      lit_count ++;
    }

  if (lit_count == next_literal_pool_place) /* new entry */
    {
      if (next_literal_pool_place > MAX_LITERAL_POOL_SIZE)
        {
          as_bad("Literal Pool Overflow");
        }

      literals[next_literal_pool_place].exp = *exx;
      literals[next_literal_pool_place].size = sz;
      if (name) {
         literals[next_literal_pool_place].sym_name = strdup (name);
      } else {
         literals[next_literal_pool_place].sym_name = NULL;
      }
      next_literal_pool_place++;  
    }

  /* current_poolP points to the begining of the literal pool.
   * X_add_number is the offset from the begining of the
   * literal pool to this expr minus the location of the most
   * recent .using directive.  Thus, the grand total value of the
   * expression is the distance from .using to the literal.
   */
  exx->X_add_symbol = current_poolP;
  exx->X_add_number = total_sz;
  exx->X_op_symbol = NULL;
  i370_make_relative (exx);
}
 
/* The symbol setup for the literal pool is done in two steps.  First,
 * a symbol that represents the start of the literal pool is created,
 * above, in the add_to_pool() routine. This sym current_poolP.
 * However, we don't know what fragment its in until a bit later.
 * So we defer the frag_now thing, and the symbol name, until .ltorg time
 */

/* Can't use symbol_new here, so have to create a symbol and then at
   a later date assign it a value. Thats what these functions do */
static void
symbol_locate (symbolP, name, segment, valu, frag)
     symbolS *symbolP; 
     CONST char *name;		/* It is copied, the caller can modify */
     segT segment;		/* Segment identifier (SEG_<something>) */
     valueT valu;		/* Symbol value */
     fragS *frag;		/* Associated fragment */
{
  size_t name_length;
  char *preserved_copy_of_name;

  name_length = strlen (name) + 1;      /* +1 for \0 */
  obstack_grow (&notes, name, name_length);
  preserved_copy_of_name = obstack_finish (&notes);

  S_SET_NAME (symbolP, preserved_copy_of_name);

  S_SET_SEGMENT (symbolP, segment);
  S_SET_VALUE (symbolP, valu);
  symbol_clear_list_pointers(symbolP);

  symbolP->sy_frag = frag;

  /*
   * Link to end of symbol chain.
   */
  {
    extern int symbol_table_frozen;
    if (symbol_table_frozen)
      abort ();
  }

  symbol_append (symbolP, symbol_lastP, &symbol_rootP, &symbol_lastP);

  obj_symbol_new_hook (symbolP);

#ifdef tc_symbol_new_hook
  tc_symbol_new_hook (symbolP);
#endif
 
#define DEBUG_SYMS
#ifdef DEBUG_SYMS
  verify_symbol_chain(symbol_rootP, symbol_lastP);
#endif /* DEBUG_SYMS */
}

/* i370_addr_offset() will convert operand expressions 
 * that appear to be absolute into thier base-register
 * relative form.  These expressions come in two types:
 *  
 * (1) of the form "* + const" * where "*" means 
 * relative offset since the last using 
 * i.e. "*" means ".-using_baseaddr"
 *
 * (2) labels, which are never absolute, but are always 
 * relative to the last "using".  Anything with an alpha
 * character is considered to be a label (since symbols 
 * can never be operands), and since we've already handled 
 * register operands. For example, "BL .L33" branch low 
 * to .L33 RX form insn frequently terminates for-loops, 
 */
static boolean
i370_addr_offset (expressionS *exx)
{
  char *dot, *lab;
  int islabel = 0;

  /* search for a label; anything with an alpha will do */
  lab = input_line_pointer;
  while (*lab && (',' != *lab) && ('(' != *lab)) {
     if (isdigit(*lab)) break;
     if (isalpha(*lab)) {
        islabel = 1;
        break;
     }
     ++lab;
  }

  /* See if operand has a * in it */
  dot = strchr (input_line_pointer, '*');

  if (!dot && !islabel) return false;

  /* replace * with . and let expr munch on it.  */
  if (dot) *dot = '.';
  expression (exx);

  /* OK, now we have to subtract the "using" location  */
  i370_make_relative (exx);

  /* put the * back */
  if (dot) *dot = '*';

  return true;
}

/* handle address constants of various sorts */
/* The currently supported types are 
 *    =A(some_symb)
 *    =V(some_extern)
 *    =X'deadbeef'    hexadecimal
 *    =F'1234'        32-bit const int
 *    =H'1234'        16-bit const int
 */
static boolean
i370_addr_cons (expressionS *exp)
{
  char *name;
  int nbytes=4;
  char *sym_name, delim;
  int name_len;
  int cons_len=0;

  name = input_line_pointer;
  sym_name = input_line_pointer;
  /* Find the spelling of the operand */
  if (name[0] == '=' && isalpha (name[1])) {
    name = ++input_line_pointer;
  } else {
    return false;
  }
  switch (name[0]) {
     case 'A':
     case 'V':
       /* A == address-of */
       /* V == extern */
       ++input_line_pointer;
       expression (exp);

       /* we use a simple string name to collapse together 
        * multiple refrences to the same address literal
        */
       name_len = strcspn (sym_name, ", ");
       delim = *(sym_name + name_len);
       *(sym_name + name_len) = 0x0;
       add_to_lit_pool (exp, sym_name, 4);
       *(sym_name + name_len) = delim;

       break;
     case 'H':
     case 'F':
     case 'X':
     case 'E':  /* single-precision float point */
     case 'D':  /* double-precision float point */

       /* H == 16-bit fixed-point const; expression must be const */
       /* F == fixed-point const; expression must be const */
       /* X == fixed-point const; expression must be const */
       if ('H' == name[0]) cons_len = 2;
       else if ('F' == name[0]) cons_len = 4;
       else if ('X' == name[0]) cons_len = 4;
       else if ('E' == name[0]) cons_len = 4;
       else if ('D' == name[0]) cons_len = 8;

       /* extract length, if it is present; hack alert -- assume single-digit
        * length */
       if ('L' == name[1]) {
          cons_len = name[2] - '0';  /* should work for ascii and ebcdic */
          input_line_pointer += 2;
       }

       ++input_line_pointer;

       /* get rid of pesky quotes */
       if ('\'' == *input_line_pointer) {
          char * close;
          ++input_line_pointer;
          close = strchr (input_line_pointer, '\'');
          if (close) *close= ' ';
          else as_bad ("missing end-quote");
       }
       if ('\"' == *input_line_pointer) {
          char * close;
          ++input_line_pointer;
          close = strchr (input_line_pointer, '\"');
          if (close) *close= ' ';
          else as_bad ("missing end-quote");
       }
       if (('X' == name[0]) || ('E' == name[0]) || ('D' == name[0])) {
          char tmp[50];
          char *save;
          /* I beleive this works even for =XL8'dada0000beeebaaa'
           * which should parse out to X_op == O_big 
           * Note that floats and doubles get represented as 
           * 0d3.14159265358979  or 0f 2.7
           */
          tmp[0] = '0';
          tmp[1] = name[0];
          tmp[2] = 0;
          strcat (tmp, input_line_pointer);
          save = input_line_pointer;
          input_line_pointer = tmp;
          expression (exp);
          input_line_pointer = save + (input_line_pointer-tmp-2);

          /* fix up lengths for floats and doubles */
          if (O_big == exp->X_op) {
             exp->X_add_number = cons_len / CHARS_PER_LITTLENUM;
          }
       } else {
          expression (exp);
       }
       /* O_big occurs when more than 4 bytes worth gets parsed */
       if ((exp->X_op != O_constant) && (exp->X_op != O_big)) {
          as_bad ("expression not a constant");
          return false;
       }
       add_to_lit_pool (exp, 0x0, cons_len);
       break;

     default:
       as_bad ("Unknown/unsupported address literal type");
       return false;
  }

  return true;
}


/* Dump the contents of the literal pool that we've accumulated so far.
 * This aligns the pool to the size of the largest literal in the pool.
 */

static void
i370_ltorg (ignore)
     int ignore;
{
  int lit_count = 0;
  int biggest_literal_size = 0;
  int biggest_align = 0;
  char pool_name[20];

  if (NULL == current_poolP)
    {
      /* Nothing to do */
      /* as_tsktsk ("Nothing to put in the pool\n"); */
      return;
    }
 
  /* find largest literal .. 2 4 or 8 */
  lit_count = 0;
  while (lit_count < next_literal_pool_place) {
    if (biggest_literal_size < literals[lit_count].size) 
        biggest_literal_size = literals[lit_count].size;
    lit_count ++;
  }
  if (2 == biggest_literal_size) biggest_align = 1;
  else if (4 == biggest_literal_size) biggest_align = 2;
  else if (8 == biggest_literal_size) biggest_align = 3;
  else as_bad ("bad alignment of %d bytes", biggest_literal_size);
  

  /* Align pool for short, word, double word accesses */
  frag_align (biggest_align, 0, 0);
  record_alignment (now_seg, biggest_align);

  sprintf (pool_name, ".LITP%03d", lit_pool_num++);

  symbol_locate (current_poolP, pool_name, now_seg,
                 (valueT) frag_now_fix (), frag_now);
  symbol_table_insert (current_poolP);

  /* Note that the gas listing will print only the first five 
   * entries in the pool .... wonder how to make it print more ...
   */
  /* output largest literals first, then the smaller ones. */
  lit_count = 0;
  while (lit_count < next_literal_pool_place) {

#define EMIT_ADDR_CONS_SYMBOLS
#ifdef EMIT_ADDR_CONS_SYMBOLS
     /* create a bogus symbol, add it to the pool ... 
      * For the most part, I think this is a useless excercise,
      * except that having these symbol names in the objects
      * is vaguely useful for debugging ...
      */
    if (literals[lit_count].sym_name) {
       symbolS * symP = symbol_make_empty();
       symbol_locate (symP, literals[lit_count].sym_name, now_seg,
                     (valueT) frag_now_fix (), frag_now);
       symbol_table_insert (symP);
    }
#endif /* EMIT_ADDR_CONS_SYMBOLS */

    emit_expr (&(literals[lit_count].exp), literals[lit_count].size);  
    lit_count ++;
  }

  next_literal_pool_place = 0;
  current_poolP = NULL;
}

#endif /* LITERAL_POOL_SUPPORT */
 

/* add support for the HLASM-like USING directive to indicate 
 * the base register to use ...  we don't support the full
 * hlasm semantics for this ... we merely pluck a base address 
 * and a register number out.  We print a warning if using is
 * called multiple times.  I suppose we should check to see 
 * if the regno is valid ...
 */
static void
i370_using (ignore)
     int ignore;
{
  expressionS ex;
  int iregno;
  char *star;

  /* if "*" appears in a using, it means "."  */
  /* replace it with "." so that expr doesn't get confused. */
  star = strchr (input_line_pointer, '*');
  if (star) *star = '.';

  /* the first arg to using will usually be ".", but it can
   * be a more complex exprsssion too ... */
  expression (&i370_using_baseaddr);
  if (star) *star = '*';
  if ((O_constant != i370_using_baseaddr.X_op) &&
      (O_symbol   != i370_using_baseaddr.X_op) &&
      (O_uminus   != i370_using_baseaddr.X_op))
  {
    as_bad (".using: base address expression illegal or too complex");
  }

  if (*input_line_pointer != '\0') ++input_line_pointer;

  /* the second arg to using had better be a register */
  register_name (&ex);
  demand_empty_rest_of_line ();
  iregno = ex.X_add_number;

  /* if we were strict, we could check that the current 
   * using was -1 and if not, flag it as a warning ... */
  if (-1 != i370_using_regno) {
    // In fact, there can be multiple usings outstanding;
    // the most recent or the one with the least offset is
    // the one that gets used.  We don't support this ...
    // but don't print the error mesg,  the common
    // usage doesn't mess things up but does print this msg.
    //     as_tsktsk (".using %d without dropping %d first", 
    //       iregno, i370_using_regno);
  }

  i370_using_regno = iregno;
}

static void
i370_drop (ignore)
     int ignore;
{
  expressionS ex;
  int iregno;

  register_name (&ex);
  demand_empty_rest_of_line ();
  iregno = ex.X_add_number;

  if (iregno != i370_using_regno) {
    as_bad ("droping register %d does not match using register %d", 
      iregno, i370_using_regno);
  }
  i370_using_regno = -1;
  i370_using_baseaddr.X_op = O_absent;
}

static void
i370_make_relative (expressionS *exx)
{
  if (O_constant == i370_using_baseaddr.X_op) 
  {
     exx->X_op = O_symbol;
     exx->X_add_number -= i370_using_baseaddr.X_add_number;
  } else
  if (O_symbol == i370_using_baseaddr.X_op) 
  {
     exx->X_op = O_subtract;
     exx->X_op_symbol = i370_using_baseaddr.X_add_symbol;
     exx->X_add_number -= i370_using_baseaddr.X_add_number;
  } else
  if (O_uminus == i370_using_baseaddr.X_op) 
  {
     exx->X_op = O_add;
     exx->X_op_symbol = i370_using_baseaddr.X_add_symbol;
     exx->X_add_number += i370_using_baseaddr.X_add_number;
   } else 
   {
      as_bad ("Missing or bad .using directive");
   }
}

/* We need to keep a list of fixups.  We can't simply generate them as
   we go, because that would require us to first create the frag, and
   that would screw up references to ``.''.  */

struct i370_fixup
{
  expressionS exp;
  int opindex;
  bfd_reloc_code_real_type reloc;
};

#define MAX_INSN_FIXUPS (5)

/* This routine is called for each instruction to be assembled.  */

void
md_assemble (str)
     char *str;
{
  char *s, *opcode_str;
  const struct i370_opcode *opcode;
  i370_insn_t insn;
  const unsigned char *opindex_ptr;
  int have_optional_index, have_optional_basereg, have_optional_reg;
  int skip_optional_index, skip_optional_basereg, skip_optional_reg;
  int off_by_one;
  int need_paren = 0;
  struct i370_fixup fixups[MAX_INSN_FIXUPS];
  int fc;
  char *f;
  int i;
#ifdef OBJ_ELF
  bfd_reloc_code_real_type reloc;
#endif

  /* Get the opcode.  */
  for (s = str; *s != '\0' && ! isspace (*s); s++)
    ;
  if (*s != '\0')
    *s++ = '\0';
  opcode_str = str;

  /* Look up the opcode in the hash table.  */
  opcode = (const struct i370_opcode *) hash_find (i370_hash, str);
  if (opcode == (const struct i370_opcode *) NULL)
    {
      const struct i370_macro *macro;

      assert (i370_macro_hash);
      macro = (const struct i370_macro *) hash_find (i370_macro_hash, str);
      if (macro == (const struct i370_macro *) NULL)
        as_bad ("Unrecognized opcode: `%s'", str);
      else
         i370_macro (s, macro);

      return;
    }

   insn = opcode->opcode;
 
   str = s;
   while (isspace (*str))
     ++str;

   /* I370 operands are either expressions or address constants.  
      Many operand types are optional.  The optional operands
      are always surrounded by parens, and are used to denote the base
      register ... e.g. "A R1, D2" or "A R1, D2(,B2) as opposed to 
      the fully-formed "A R1, D2(X2,B2)".  Note also the = sign,
      such as A R1,=A(i) where the address-of operator =A implies
      use of both a base register, and a missing index register.

      So, before we start seriously parsing the operands, we check 
      to see if we have an optional operand, and, if we do, we count 
      the number of commas to see which operand should be omitted.  */

   have_optional_index = have_optional_basereg = have_optional_reg = 0;
   for (opindex_ptr = opcode->operands; *opindex_ptr != 0; opindex_ptr++)
     {
       const struct i370_operand *operand;
       operand = &i370_operands[*opindex_ptr];
       if ((operand->flags & I370_OPERAND_INDEX) != 0) 
           have_optional_index = 1;
       if ((operand->flags & I370_OPERAND_BASE) != 0) 
           have_optional_basereg = 1;
       if ((operand->flags & I370_OPERAND_OPTIONAL) != 0) 
           have_optional_reg = 1;
     }
   
   skip_optional_index = skip_optional_basereg = skip_optional_reg = 0;
   if (have_optional_index || have_optional_basereg) 
     {
        unsigned int opcount, nwanted;
 
        /* There is an optional operand.  Count the number of
           commas and open-parens in the input line.  */
        if (*str == '\0')
          opcount = 0;
        else
          {
            opcount = 1;
            s = str;
            while ((s = strpbrk (s, ",(=")) != (char *) NULL)
      	      {
                 ++opcount;
                 ++s;
                 if (',' == *s) ++s;  /* avoid counting things like (, */
                 if ('=' == *s) { ++s; --opcount; }
              }
          }
 
        /* If there are fewer operands in the line then are called
           for by the instruction, we want to skip the optional
           operand.  */
        nwanted = strlen (opcode->operands);
        if (have_optional_index) {
           if (opcount < nwanted) skip_optional_index = 1;
           if (have_optional_basereg && ((opcount+1) < nwanted)) skip_optional_basereg = 1;
           if (have_optional_reg && ((opcount+1) < nwanted)) skip_optional_reg = 1;
        } else {
           if (have_optional_basereg && (opcount < nwanted)) skip_optional_basereg = 1;
           if (have_optional_reg && (opcount < nwanted)) skip_optional_reg = 1;
        }
     }

   /* Perform some off-by-one hacks on the length field of certain instructions.
    * Its such a shame to have to do this, but the problem is that HLASM got 
    * defined so that the lengths differ by one from the actual machine instructions.
    * Sigh. Affected instructions are Compare Logical, Move and Exclusive OR
    * hack alert -- aren't *all* SS instructions affected ??
    */
   off_by_one = 0;
   if (0 == strcasecmp ("CLC", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("ED", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("EDMK", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("MVC", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("MVCIN", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("MVN", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("MVZ", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("NC", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("OC", opcode->name)) off_by_one = 1;
   else if (0 == strcasecmp ("XC", opcode->name)) off_by_one = 1;

   /* Gather the operands.  */
   fc = 0;
   for (opindex_ptr = opcode->operands; *opindex_ptr != 0; opindex_ptr++)
     {
       const struct i370_operand *operand;
       const char *errmsg;
       char *hold;
       expressionS ex;
       char endc;
 
       operand = &i370_operands[*opindex_ptr];
       errmsg = NULL;
 
       /* If this is an index operand, and we are skipping it, 
          just insert a zero.  */
      if (skip_optional_index &&
         ((operand->flags & I370_OPERAND_INDEX) != 0))
        {
          insn = i370_insert_operand (insn, operand, 0,
        			     (char *) NULL, 0);
          continue;
        }
 
       /* If this is the base operand, and we are skipping it, 
          just insert the current using basreg.  */
      if (skip_optional_basereg && 
          ((operand->flags & I370_OPERAND_BASE) != 0))
        {
          if (0 > i370_using_regno) {
            as_bad ("not using any base register");
          }
          insn = i370_insert_operand (insn, operand, i370_using_regno,
        			     (char *) NULL, 0);
          continue;
        }
 
       /* If this is an optional operand, and we are skipping it, 
          Use zero (since a non-zero value would denote a register)  */
      if (skip_optional_reg &&
         ((operand->flags & I370_OPERAND_OPTIONAL) != 0))
        {
          insn = i370_insert_operand (insn, operand, 0,
        			     (char *) NULL, 0);
          continue;
        }
 
      /* Gather the operand.  */
      hold = input_line_pointer;
      input_line_pointer = str;

      /* register names are only allowed where there are registers ... */
      if ((operand->flags & I370_OPERAND_GPR) != 0)
        {
          /* quickie hack to get past things like (,r13) */
          if (skip_optional_index && (',' == *input_line_pointer)) 
            {
              *input_line_pointer = ' ';
              input_line_pointer ++;
            }
          if (! register_name (&ex))
            {
              as_bad ("expecting a register for operand %d",
                     opindex_ptr - opcode->operands + 1); 
            }
        }

      /* check for a address constant expression */
      else if (i370_addr_cons (&ex)) {}
      else if (i370_addr_offset (&ex)) {}
      else expression (&ex);

      str = input_line_pointer;
      input_line_pointer = hold;

      /* perform some off-by-one hacks on the length field of certain instructions.
       * Its such a shame to have to do this, but the problem is that HLASM got 
       * defined so that the programmer specifies a length that is one greater
       * than what the machine instruction wants.
       * Sigh.
       */
      if (off_by_one && (0 == strcasecmp ("SS L", operand->name))) 
      {
        ex.X_add_number --;
      }

      if (ex.X_op == O_illegal)
        as_bad ("illegal operand");
      else if (ex.X_op == O_absent)
        as_bad ("missing operand");
      else if (ex.X_op == O_register)
        {
          insn = i370_insert_operand (insn, operand, ex.X_add_number,
        			     (char *) NULL, 0);
        }
      else if (ex.X_op == O_constant)
        {
#ifdef OBJ_ELF
          /* Allow @HA, @L, @H on constants. 
           * Well actually, no we don't; there really don't make sense
           * (at least not to me) for the i370.  However, this code is
           * left here for any dubious future expansion reasons ... */
          char *orig_str = str;

          if ((reloc = i370_elf_suffix (&str, &ex)) != BFD_RELOC_UNUSED)
            switch (reloc)
              {
              default:
        	str = orig_str;
        	break;

              case BFD_RELOC_LO16:
        	/* X_unsigned is the default, so if the user has done
                   something which cleared it, we always produce a
                   signed value.  */
//        	if (ex.X_unsigned
//         	    && (operand->flags & PPC_OPERAND_SIGNED) == 0)
//         	  ex.X_add_number &= 0xffff;
//         	else
         	  ex.X_add_number = (((ex.X_add_number & 0xffff)
        			      ^ 0x8000)
        			     - 0x8000);
        	break;

              case BFD_RELOC_HI16:
        	ex.X_add_number = (ex.X_add_number >> 16) & 0xffff;
        	break;

              case BFD_RELOC_HI16_S:
        	ex.X_add_number = (((ex.X_add_number >> 16) & 0xffff)
        			   + ((ex.X_add_number >> 15) & 1));
        	break;
              }
#endif
          insn = i370_insert_operand (insn, operand, ex.X_add_number,
        			     (char *) NULL, 0);
        }
#ifdef OBJ_ELF
      else if ((reloc = i370_elf_suffix (&str, &ex)) != BFD_RELOC_UNUSED)
        {
          as_tsktsk ("md_assemble(): suffixed relocations not supported\n");

          /* We need to generate a fixup for this expression.  */
          if (fc >= MAX_INSN_FIXUPS)
            as_fatal ("too many fixups");
          fixups[fc].exp = ex;
          fixups[fc].opindex = 0;
          fixups[fc].reloc = reloc;
          ++fc;
        }
#endif /* OBJ_ELF */

      else
        {
          /* We need to generate a fixup for this expression.  */
          /* Typically, the expression will just be a symbol ... 
           * printf ("insn %s needs fixup for %s \n", 
           *        opcode->name, ex.X_add_symbol->bsym->name);
           */

          if (fc >= MAX_INSN_FIXUPS)
            as_fatal ("too many fixups");
          fixups[fc].exp = ex;
          fixups[fc].opindex = *opindex_ptr;
          fixups[fc].reloc = BFD_RELOC_UNUSED;
          ++fc;
        }

// it might be nice to dust off this code to get nitpicky about parenthesis,
// but laziness sets in right about now ...
//       if (need_paren)
//         {
//           endc = ')';
//           need_paren = 0;
//         }
//       else if ((operand->flags & I370_OPERAND_PARENS) != 0)
//         {
//           endc = '(';
//           need_paren = 1;
//         }
//       else
//         endc = ',';
// 
//       /* The call to expression should have advanced str past any
//          whitespace.  */
//       if (*str != endc
//           && (endc != ',' || *str != '\0'))
//         {
//           as_bad ("syntax error; found `%c' but expected `%c'", *str, endc);
//           break;
//         }
 
       /* skip over delimiter (close paren, or comma) */
       if ((')' == *str) && (',' == *(str+1))) ++str;
       if (*str != '\0') ++str;
     }
 
   while (isspace (*str))
     ++str;
 
  if (*str != '\0')
    as_bad ("junk at end of line: `%s'", str);
 
   /* Write out the instruction.  */
   f = frag_more (opcode->len);
   if (4 >= opcode->len) {
      md_number_to_chars (f, insn.i[0], opcode->len);
   } else {
      md_number_to_chars (f, insn.i[0], 4);
      if (6 == opcode->len) {
         md_number_to_chars ((f+4), ((insn.i[1])>>16), 2);
      } else {
         /* not used --- don't have any 8 byte instructions */
         as_bad ("Internal Error: bad instruction length");
         md_number_to_chars ((f+4), insn.i[1], opcode->len -4);
      }
   }
 
   /* Create any fixups.  At this point we do not use a
      bfd_reloc_code_real_type, but instead just use the
      BFD_RELOC_UNUSED plus the operand index.  This lets us easily
      handle fixups for any operand type, although that is admittedly
      not a very exciting feature.  We pick a BFD reloc type in
      md_apply_fix.  */
   for (i = 0; i < fc; i++)
     {
       const struct i370_operand *operand;
 
       operand = &i370_operands[fixups[i].opindex];
       if (fixups[i].reloc != BFD_RELOC_UNUSED)
         {
           reloc_howto_type *reloc_howto = bfd_reloc_type_lookup (stdoutput, fixups[i].reloc);
           int size;
           fixS *fixP;
 
           if (!reloc_howto)
             abort ();
 
           size = bfd_get_reloc_size (reloc_howto);
 
           if (size < 1 || size > 4)
             abort();
 
printf (" gwana doo fixup %d \n", i);
           fixP = fix_new_exp (frag_now, f - frag_now->fr_literal, size,
         		      &fixups[i].exp, reloc_howto->pc_relative,
         		      fixups[i].reloc);
 
           /* Turn off complaints that the addend is too large for things like
              foo+100000@ha.  */
           switch (fixups[i].reloc)
             {
             case BFD_RELOC_16_GOTOFF:
             case BFD_RELOC_LO16:
             case BFD_RELOC_HI16:
             case BFD_RELOC_HI16_S:
               fixP->fx_no_overflow = 1;
               break;
             default:
               break;
             }
         }
       else
         fix_new_exp (frag_now, f - frag_now->fr_literal, opcode->len,
         	     &fixups[i].exp,
         	     (operand->flags & I370_OPERAND_RELATIVE) != 0,
         	     ((bfd_reloc_code_real_type)
         	       (fixups[i].opindex + (int) BFD_RELOC_UNUSED)));
     }
}

/* Handle a macro.  Gather all the operands, transform them as
   described by the macro, and call md_assemble recursively.  All the
   operands are separated by commas; we don't accept parentheses
   around operands here.  */

static void
i370_macro (str, macro)
     char *str;
     const struct i370_macro *macro;
{
  char *operands[10];
  unsigned int count;
  char *s;
  unsigned int len;
  const char *format;
  int arg;
  char *send;
  char *complete;

  /* Gather the users operands into the operands array.  */
  count = 0;
  s = str;
  while (1)
    {
      if (count >= sizeof operands / sizeof operands[0])
        break;
      operands[count++] = s;
      s = strchr (s, ',');
      if (s == (char *) NULL)
        break;
      *s++ = '\0';
    }  

  if (count != macro->operands)
    {
      as_bad ("wrong number of operands");
      return;
    }

  /* Work out how large the string must be (the size is unbounded
     because it includes user input).  */
  len = 0;
  format = macro->format;
  while (*format != '\0')
    {
      if (*format != '%')
        {
          ++len;
          ++format;
        }
      else
        {
          arg = strtol (format + 1, &send, 10);
          know (send != format && arg >= 0 && arg < count);
          len += strlen (operands[arg]);
          format = send;
        }
    }

  /* Put the string together.  */
  complete = s = (char *) alloca (len + 1);
  format = macro->format;
  while (*format != '\0')
    {
      if (*format != '%')
        *s++ = *format++;
      else
        {
          arg = strtol (format + 1, &send, 10);
          strcpy (s, operands[arg]);
          s += strlen (s);
          format = send;
        }
    }
  *s = '\0';

  /* Assemble the constructed instruction.  */
  md_assemble (complete);
}  

#ifdef OBJ_ELF
/* For ELF, add support for SHF_EXCLUDE and SHT_ORDERED */

int
i370_section_letter (letter, ptr_msg)
     int letter;
     char **ptr_msg;
{
  if (letter == 'e')
    return SHF_EXCLUDE;

  *ptr_msg = "Bad .section directive: want a,w,x,e in string";
  return 0;
}

int
i370_section_word (ptr_str)
     char **ptr_str;
{
  if (strncmp (*ptr_str, "exclude", sizeof ("exclude")-1) == 0)
    {
      *ptr_str += sizeof ("exclude")-1;
      return SHF_EXCLUDE;
    }

  return 0;
}

int
i370_section_type (ptr_str)
     char **ptr_str;
{
  if (strncmp (*ptr_str, "ordered", sizeof ("ordered")-1) == 0)
    {
      *ptr_str += sizeof ("ordered")-1;
      return SHT_ORDERED;
    }

  return 0;
}

int
i370_section_flags (flags, attr, type)
     int flags;
     int attr;
     int type;
{
  if (type == SHT_ORDERED)
    flags |= SEC_ALLOC | SEC_LOAD | SEC_SORT_ENTRIES;

  if (attr & SHF_EXCLUDE)
    flags |= SEC_EXCLUDE;

  return flags;
}
#endif /* OBJ_ELF */


/* Pseudo-op handling.  */

/* The .byte pseudo-op.  This is similar to the normal .byte
   pseudo-op, but it can also take a single ASCII string.  */

static void
i370_byte (ignore)
     int ignore;
{
  if (*input_line_pointer != '\"')
    {
      cons (1);
      return;
    }

  /* Gather characters.  A real double quote is doubled.  Unusual
     characters are not permitted.  */
  ++input_line_pointer;
  while (1)
    {
      char c;

      c = *input_line_pointer++;

      if (c == '\"')
        {
        if (*input_line_pointer != '\"')
            break;
          ++input_line_pointer;
        }

      FRAG_APPEND_1_CHAR (c);
    }

  demand_empty_rest_of_line ();
}

/* The .tc pseudo-op.  This is used when generating XCOFF and ELF.
   This takes two or more arguments.

   When generating XCOFF output, the first argument is the name to
   give to this location in the toc; this will be a symbol with class
   TC.  The rest of the arguments are 4 byte values to actually put at
   this location in the TOC; often there is just one more argument, a
   relocateable symbol reference.

   When not generating XCOFF output, the arguments are the same, but
   the first argument is simply ignored.  */

static void
i370_tc (ignore)
     int ignore;
{

  /* Skip the TOC symbol name.  */
  while (is_part_of_name (*input_line_pointer)
         || *input_line_pointer == '['
         || *input_line_pointer == ']'
         || *input_line_pointer == '{'
         || *input_line_pointer == '}')
    ++input_line_pointer;

  /* Align to a four byte boundary.  */
  frag_align (2, 0, 0);
  record_alignment (now_seg, 2);

  if (*input_line_pointer != ',')
    demand_empty_rest_of_line ();
  else
    {
      ++input_line_pointer;
      cons (4);
    }
}

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *litp.  The number
   of LITTLENUMS emitted is stored in *sizep .  An error message is
   returned, or NULL on OK.  */

char *
md_atof (type, litp, sizep)
     int type;
     char *litp;
     int *sizep;
{
  int prec;
  LITTLENUM_TYPE words[4];
  char *t;
  int i;

  switch (type)
    {
    case 'f':
    case 'E':
      type = 'f';
      prec = 2;
      break;

    case 'd':
    case 'D':
      type = 'd';
      prec = 4;
      break;

    default:
      *sizep = 0;
      return "bad call to md_atof";
    }

  /* 360/370/390 have two float formats: an old, funky 360 single-precision
   * format, and the ieee format.  Support only the ieee format. */
  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizep = prec * 2;

  for (i = 0; i < prec; i++)
    {
      md_number_to_chars (litp, (valueT) words[i], 2);
      litp += 2;
    }
     
  return NULL;
}

/* Write a value out to the object file, using the appropriate
   endianness.  */

void
md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
    number_to_chars_bigendian (buf, val, n);
}

/* Align a section (I don't know why this is machine dependent).  */

valueT
md_section_align (seg, addr)
     asection *seg;
     valueT addr;
{
  int align = bfd_get_section_alignment (stdoutput, seg);

  return ((addr + (1 << align) - 1) & (-1 << align));
}

/* We don't have any form of relaxing.  */

int
md_estimate_size_before_relax (fragp, seg)
     fragS *fragp;
     asection *seg;
{
  abort ();
  return 0;
}

/* Convert a machine dependent frag.  We never generate these.  */

void
md_convert_frag (abfd, sec, fragp)
     bfd *abfd;
     asection *sec;
     fragS *fragp;
{
  abort ();
}

/* We have no need to default values of symbols.  */

/*ARGSUSED*/
symbolS *
md_undefined_symbol (name)
     char *name;
{
  return 0;
}

/* Functions concerning relocs.  */

/* The location from which a PC relative jump should be calculated,
   given a PC relative reloc.  */

long
md_pcrel_from_section (fixp, sec)
     fixS *fixp;
     segT sec;
{
  return fixp->fx_frag->fr_address + fixp->fx_where;
}


// /* See whether a symbol is in the TOC section.  */
// 
// static int
// ppc_is_toc_sym (sym)
//      symbolS *sym;
// {
//   return strcmp (segment_name (S_GET_SEGMENT (sym)), ".got") == 0;
// }

/* Apply a fixup to the object code.  This is called for all the
   fixups we generated by the call to fix_new_exp, above.  In the call
   above we used a reloc code which was the largest legal reloc code
   plus the operand index.  Here we undo that to recover the operand
   index.  At this point all symbol values should be fully resolved,
   and we attempt to completely resolve the reloc.  If we can not do
   that, we determine the correct reloc code and put it back in the
   fixup.  

   See gas/cgen.c for more sample code and explanations of what's 
   going on here ...
*/

int
md_apply_fix3 (fixp, valuep, seg)
     fixS *fixp;
     valueT *valuep;
     segT seg;
{
  valueT value;

#ifdef OBJ_ELF
  value = *valuep;
  if (fixp->fx_addsy != NULL)
    {
#ifdef DEBUG 
      printf ("\nmd_apply_fix3: symbol %s at %s:%d addend=0x%x\n", 
            bfd_asymbol_name (fixp->fx_addsy->bsym),
            fixp->fx_file, fixp->fx_line,
            value);
#endif 

      /* if its a difference between two symbols in the same section,
       * there is nothing to be done, no relocation to be performed.
       */
      if (fixp->fx_subsy != NULL)
        {
          if ((S_GET_SEGMENT(fixp->fx_addsy) ==  S_GET_SEGMENT(fixp->fx_subsy))
              && (undefined_section != S_GET_SEGMENT(fixp->fx_addsy)))
            {
// printf ("duuuuuuuuuuuuuuuuuuuuuuuuude %x %x  %d %d %s\n",
// S_GET_VALUE (fixp->fx_addsy),
// S_GET_VALUE (fixp->fx_subsy),
// fixp->fx_addsy->sy_used_in_reloc,
// fixp->fx_subsy->sy_used_in_reloc,
// bfd_asymbol_value(fixp->fx_addsy->bsym),
// bfd_asymbol_value(fixp->fx_subsy->bsym),
// S_IS_DEFINED (fixp->fx_addsy),
// S_IS_DEFINED (fixp->fx_subsy),
// fixp->fx_addsy->sy_resolved,
// fixp->fx_subsy->sy_resolved,
// segment_name( S_GET_SEGMENT (fixp->fx_addsy)) ,
// segment_name( S_GET_SEGMENT (fixp->fx_subsy)),
// fixp->fx_subsy->bsym->name);

// stupid blasted code is telling me that things in the literal pool 
// are differences of a pair of symbols, and clearly that's not what I wanted
// ... what is the gig .... ??
// BTW, nmote that S_GET_VALUE (fixp->fx_subsy) is the address of the 
// .using used to construct the adress literal.
//
// well, here's my very cheesy hack: all I care about are expressions
// of the form .LFDSL01-.LFDSE01 so if the first two chars are .L we assume
// its just a plain old difference.
// unfortunately, this screws up other relocations ... help !!
#ifdef JUNK
              if (('.' == fixp->fx_subsy->bsym->name[0]) &&
                  ('L' == fixp->fx_subsy->bsym->name[1]))
              {
                 fixp->fx_done = 1;
                 return 1;
              }
#endif  /* JUNK */
            }
        }

#ifdef JUNK 
      /* `*valuep' may contain the value of the symbol on which the reloc
         will be based; we have to remove it.  */
      if (fixp->fx_addsy->sy_used_in_reloc
          && S_GET_SEGMENT (fixp->fx_addsy) != absolute_section
          && S_GET_SEGMENT (fixp->fx_addsy) != undefined_section
          && ! bfd_is_com_section (S_GET_SEGMENT (fixp->fx_addsy)))
        value -= S_GET_VALUE (fixp->fx_addsy);

printf ("addsy value is %x \n", S_GET_VALUE (fixp->fx_addsy));
printf ("value now %x \n", value);
      /* FIXME: Why '+'?  Better yet, what exactly is '*valuep'
         supposed to be?  I think this is related to various similar
         FIXMEs in tc-i386.c and tc-sparc.c and tc-ppc.c.  */
      if (fixp->fx_pcrel)
        value += fixp->fx_frag->fr_address + fixp->fx_where;
printf ("pcrel value is %x \n", fixp->fx_frag->fr_address + fixp->fx_where);
printf ("value now %x \n", value);
#endif /* JUNK */

        /* OK, heres the deal:
         * S_GET_VALUE (fixp->fx_addsy) is the offset of the symbol 
         *    relative to whatever section that symbol is in.
         * fixp->fx_frag->fr_address + fixp->fx_where is the offset
         *    of the instruction to whatever section it is in.
         *
         * Until we figure out what our ABI will be, we'll hard-code 
         * the symbol address and punt.
         */
        // value = S_GET_VALUE (fixp->fx_addsy);
    }
  else
    {
      fixp->fx_done = 1;
      return 1;
    }
#endif

  /* apply fixups to operands.  Note that there should be no relocations
   * for any operands, since no instruction ever takes an operand
   * that requires reloc.
   */
  if ((int) fixp->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      int opindex;
      const struct i370_operand *operand;
      char *where;
      i370_insn_t insn;

      opindex = (int) fixp->fx_r_type - (int) BFD_RELOC_UNUSED;

      operand = &i370_operands[opindex];

#ifdef DEBUG 
      printf ("\nmd_apply_fix3: fixup operand %s at 0x%x in %s:%d addend=0x%x\n", 
            operand->name,
            fixp->fx_frag->fr_address + fixp->fx_where,
            fixp->fx_file, fixp->fx_line,
            value);
#endif 
      /* Fetch the instruction, insert the fully resolved operand
         value, and stuff the instruction back again.  
         fisxp->fx_size is the length of the instruction. */
      where = fixp->fx_frag->fr_literal + fixp->fx_where;
      insn.i[0] = bfd_getb32 ((unsigned char *) where);
      if (6 <= fixp->fx_size) {    /* deal with 48-bit insn's */
         insn.i[1] = bfd_getb32 (((unsigned char *) where)+4);
      }
      insn = i370_insert_operand (insn, operand, (offsetT) value,
        			 fixp->fx_file, fixp->fx_line);
      bfd_putb32 ((bfd_vma) insn.i[0], (unsigned char *) where);
      if (6 <= fixp->fx_size) {   /* deal with 48-bit insn's */
          bfd_putb32 ((bfd_vma) insn.i[1], (((unsigned char *) where)+4));
      }

      /* we are done, right? right !! */
      fixp->fx_done = 1;
      if (fixp->fx_done)
        {
          /* Nothing else to do here.  */
          return 1;
        }

       /* Determine a BFD reloc value based on the operand information.
          We are only prepared to turn a few of the operands into
          relocs.  In fact, we support *zero* operand relocations ...
          Why?  because we are not expecting the compiler to generate
          any operands that need relocation.
        */

//      if ((operand->flags & I370_OPERAND_RELATIVE) != 0
//          && operand->bits == 12
//          && operand->shift == 0)
//        fixp->fx_r_type = BFD_RELOC_I370_D12;
//      else
        {
          char *sfile;
          unsigned int sline;

          /* Use expr_symbol_where to see if this is an expression
             symbol.  */
          if (expr_symbol_where (fixp->fx_addsy, &sfile, &sline))
            as_bad_where (fixp->fx_file, fixp->fx_line,
        		  "unresolved expression that must be resolved");
          else
            as_bad_where (fixp->fx_file, fixp->fx_line,
        		  "unsupported relocation type");
          fixp->fx_done = 1;
          return 1;
        }
    }
   else
     {
     /* We branch to here if the fixup is not to a symbol that 
      * appears in an instruction operand, but is rather some 
      * declared storage. 
      */
 #ifdef OBJ_ELF
      i370_elf_validate_fix (fixp, seg);
 #endif
 #ifdef DEBUG
      printf ("md_apply_fix3: reloc case %d in segment  %s %s:%d\n",
         fixp->fx_r_type, segment_name (seg), fixp->fx_file, fixp->fx_line);
      printf ("\tcurrent fixup value is 0x%x \n", value);
 #endif 
      switch (fixp->fx_r_type)
        {
        case BFD_RELOC_32:
        case BFD_RELOC_CTOR:
          if (fixp->fx_pcrel)
            fixp->fx_r_type = BFD_RELOC_32_PCREL;
        				/* fall through */

        case BFD_RELOC_RVA:
//         case BFD_RELOC_32_PCREL:
//         case BFD_RELOC_32_BASEREL:
#ifdef DEBUG
          printf ("\t32 bit relocation at 0x%x\n",
                fixp->fx_frag->fr_address + fixp->fx_where);
 #endif 
          md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
        		      value, 4);
          break;

        case BFD_RELOC_LO16:
        case BFD_RELOC_16:
//         case BFD_RELOC_GPREL16:
//         case BFD_RELOC_16_GOT_PCREL:
//         case BFD_RELOC_16_GOTOFF:
//         case BFD_RELOC_LO16_GOTOFF:
//         case BFD_RELOC_HI16_GOTOFF:
//         case BFD_RELOC_HI16_S_GOTOFF:
//         case BFD_RELOC_LO16_BASEREL:
//         case BFD_RELOC_HI16_BASEREL:
//         case BFD_RELOC_HI16_S_BASEREL:
//         case BFD_RELOC_PPC_TOC16:
          if (fixp->fx_pcrel)
            as_bad_where (fixp->fx_file, fixp->fx_line,
        		  "cannot emit PC relative %s relocation%s%s",
        		  bfd_get_reloc_code_name (fixp->fx_r_type),
        		  fixp->fx_addsy != NULL ? " against " : "",
        		  (fixp->fx_addsy != NULL
        		   ? S_GET_NAME (fixp->fx_addsy)
        		   : ""));

          md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
        		      value, 2);
          break;

          /* This case happens when you write, for example,
             lis %r3,(L1-L2)@ha
             where L1 and L2 are defined later.  */
        case BFD_RELOC_HI16:
          if (fixp->fx_pcrel)
            abort ();
          md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
        		      value >> 16, 2);
          break;
        case BFD_RELOC_HI16_S:
          if (fixp->fx_pcrel)
            abort ();
          md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
        		      value + 0x8000 >> 16, 2);
          break;

        case BFD_RELOC_8:
          if (fixp->fx_pcrel)
            abort ();

          md_number_to_chars (fixp->fx_frag->fr_literal + fixp->fx_where,
        		      value, 1);
          break;

//         case BFD_RELOC_24_PLT_PCREL:
//         case BFD_RELOC_PPC_LOCAL24PC:
//           if (!fixp->fx_pcrel && !fixp->fx_done)
//             abort ();
// 
//           if (fixp->fx_done)
//           {
//             char *where;
//             unsigned long insn;
//             
//             /* Fetch the instruction, insert the fully resolved operand
//                value, and stuff the instruction back again.  */
//             where = fixp->fx_frag->fr_literal + fixp->fx_where;
//             if (target_big_endian)
//               insn = bfd_getb32 ((unsigned char *) where);
//             else
//               insn = bfd_getl32 ((unsigned char *) where);
//             if ((value & 3) != 0)
//               as_bad_where (fixp->fx_file, fixp->fx_line,
//         		    "must branch to an address a multiple of 4");
//             if ((offsetT) value < -0x40000000
//         	|| (offsetT) value >= 0x40000000)
//               as_bad_where (fixp->fx_file, fixp->fx_line,
//         		    "@local or @plt branch destination is too far "
//         		    "away, %ld bytes",
//         		    value);
//             insn = insn | (value & 0x03fffffc);
//             bfd_putb32 ((bfd_vma) insn, (unsigned char *) where);
//           }
//           break;

        default:
          fprintf(stderr,
        	  "Gas failure, reloc value %d\n", fixp->fx_r_type);
          fflush(stderr);
          abort ();
        }
    }

  fixp->fx_addnumber = value;

  return 1;
}

/* Generate a reloc for a fixup.  */

arelent *
tc_gen_reloc (seg, fixp)
     asection *seg;
     fixS *fixp;
{
  arelent *reloc;

  reloc = (arelent *) xmalloc (sizeof (arelent));

  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);
  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
        	    "reloc %d not supported by object file format", (int)fixp->fx_r_type);
      return NULL;
    }
  reloc->addend = fixp->fx_addnumber;

#ifdef DEBUG
  printf ("\ngen_reloc(): sym %s (%s:%d) at addr 0x%x addend=0x%x\n", 
     fixp->fx_addsy->bsym->name,
     fixp->fx_file, fixp->fx_line,
     reloc->address, reloc->addend); 
#endif

  return reloc;
}
