
/* Definitions of target machine for GNU compiler.  Sun 68000/68020 version.
   Copyright (C) 1987, 88, 93-97, 1998 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */


#undef ICVMabort
#define ICVMabort() abort();

#include <stdio.h>

extern int lineno;

#undef ICVMnote
#define ICVMnote(s)  {fprintf(stderr,"NOTE; %s in (%s line %d) at %d\n",s,__FILE__,__LINE__,lineno);}
//#define ICVMnote(s)

#undef ICVMnotertl
/* #define ICVMnotertl(s,x)  {fprintf(stderr,"NOTE; %s in (%s line %d) at %d \n",s,__FILE__,__LINE__,lineno);print_rtl(stderr,x);fprintf(stderr,"\n");}*/
#define ICVMnotertl(s,x) 

#undef ICVMabortrtl
#define ICVMabortrtl(s,x)  {fprintf(stderr,"NOTE; %s in (%s line %d) at %d \n",s,__FILE__,__LINE__,lineno);print_rtl(stderr,x);fprintf(stderr,"\n");abort();}


#define IsTreeBitfield(tree) (\
		(TREE_CODE(tree)==COMPONENT_REF || TREE_CODE(tree)==BIT_FIELD_REF) &&\
		DECL_BIT_FIELD (TREE_OPERAND (tree, 1))\
)

#define GCC_ICVM		/* for ICVM specific changes */

#define ICVMLEGALREGISTER(x) \
	(GET_CODE(x)==REG || (GET_CODE(x)==MEM && IsIndirectFrameOffset(x)) )


/* return true if N is the number of an ICVM frame register*/
#define _ICVM_ISSTACKREG(n) ((n)<FIRST_PSEUDO_REGISTER && (n)>=32)

#define ICVM_ISSTACKREG(x) (GET_CODE(x)==REG && _ICVM_ISSTACKREG(REGNO(x)))


/* Note that some other tm.h files include this one and then override
   many of the definitions that relate to assembler syntax.  */


/* Names to predefine in the preprocessor for this target machine.  */

/* See sun3.h, sun2.h, isi.h for different CPP_PREDEFINES.  */

/* Print subsidiary information on the compiler version in use.  */
#define TARGET_VERSION fprintf (stderr, " (ICVM - Internet C/C++ Virtual Machine)");


/* Run-time compilation parameters selecting different hardware subsets.  */




/* kill call to _main */
#define HAS_INIT_SECTION 1


void rdata_section ();



extern int target_flags;

/* Macros used in the machine description to test the flags.  */

/* Compile for a 68020 (not a 68000 or 68010).  */
#define MASK_68020	1
#define TARGET_68020 (target_flags & MASK_68020)

/* Compile 68881 insns for floating point (not library calls).  */
#define MASK_68881	2
#define TARGET_68881 (target_flags & MASK_68881)

/* Compile using 68020 bitfield insns.  */
#define MASK_BITFIELD	4
#define TARGET_BITFIELD (target_flags & MASK_BITFIELD)

/* Compile using rtd insn calling sequence.
   This will not work unless you use prototypes at least
   for all functions that can take varying numbers of args.  */
#define MASK_RTD	8
#define TARGET_RTD (target_flags & MASK_RTD)

/* Compile passing first two args in regs 0 and 1.
   This exists only to test compiler features that will
   be needed for RISC chips.  It is not usable
   and is not intended to be usable on this cpu.  */
#define MASK_REGPARM	16
#define TARGET_REGPARM (target_flags & MASK_REGPARM)

/* Compile with 16-bit `int'.  */
#define MASK_SHORT	32
#define TARGET_SHORT (target_flags & MASK_SHORT)

/* Compile with special insns for Sun FPA.  */
#define MASK_FPA	64
#define TARGET_FPA (target_flags & MASK_FPA)

/* Compile (actually, link) for Sun SKY board.  */
#define MASK_SKY	128
#define TARGET_SKY (target_flags & MASK_SKY)

/* Optimize for 68040, but still allow execution on 68020
   (-m68020-40 or -m68040).
   The 68040 will execute all 68030 and 68881/2 instructions, but some
   of them must be emulated in software by the OS.  When TARGET_68040 is
   turned on, these instructions won't be used.  This code will still
   run on a 68030 and 68881/2. */
#define MASK_68040	256
#define TARGET_68040 (target_flags & MASK_68040)

/* Use the 68040-only fp instructions (-m68040 or -m68060).  */
#define MASK_68040_ONLY	512
#define TARGET_68040_ONLY (target_flags & MASK_68040_ONLY)

/* Optimize for 68060, but still allow execution on 68020
   (-m68020-60 or -m68060).
   The 68060 will execute all 68030 and 68881/2 instructions, but some
   of them must be emulated in software by the OS.  When TARGET_68060 is
   turned on, these instructions won't be used.  This code will still
   run on a 68030 and 68881/2. */
#define MASK_68060	1024
#define TARGET_68060 (target_flags & MASK_68060)

/* Compile for mcf5200 */
#define MASK_5200	2048
#define TARGET_5200 (target_flags & MASK_5200)

/* Align ints to a word boundary.  This breaks compatibility with the 
   published ABI's for structures containing ints, but produces faster
   code on cpus with 32 bit busses (020, 030, 040, 060, CPU32+, coldfire).
   It's required for coldfire cpus without a misalignment module.  */
#define MASK_ALIGN_INT	4096
#define TARGET_ALIGN_INT (target_flags & MASK_ALIGN_INT)

/* Compile for a CPU32 */
	/* A 68020 without bitfields is a good heuristic for a CPU32 */
#define TARGET_CPU32	(TARGET_68020 && !TARGET_BITFIELD)



/* output code for little endian machine */

#define MASK_BIGENDIAN    8192


/* NOTE -- ************** ****************************
   THIS AFFECTS several files in regular gcc
  do search on NOLOWLPARTCONVERSIONS to find the changes
  **************************************************** */



/* Macro to define tables used to set the flags.
   This is a list in braces of pairs in braces,
   each pair being { "NAME", VALUE }
   where VALUE is the bits to set or minus the bits to clear.
   An empty string NAME is used to identify the default VALUE.  */

#define TARGET_SWITCHES  \
  { \
    { "littleendian", -MASK_BIGENDIAN},					\
    { "bigendian", MASK_BIGENDIAN},					\
    { "regparm", -MASK_REGPARM},					\
    { "noregparm", MASK_REGPARM},					\
    { "bitfield", MASK_BITFIELD},					\
    { "nobitfield", - MASK_BITFIELD},					\
    { "rtd", MASK_RTD},							\
    { "nortd", - MASK_RTD},						\
    { "short", MASK_SHORT},						\
    { "noshort", - MASK_SHORT},						\
    { "fpa", -(MASK_SKY|MASK_68040_ONLY|MASK_68881)},			\
    { "fpa", MASK_FPA},							\
    { "nofpa", - MASK_FPA},						\
    { "sky", -(MASK_FPA|MASK_68040_ONLY|MASK_68881)},			\
    { "sky", MASK_SKY},							\
    { "nosky", - MASK_SKY},						\
    { "align-int", MASK_ALIGN_INT },					\
    { "no-align-int", -MASK_ALIGN_INT },				\
    SUBTARGET_SWITCHES							\
    { "", TARGET_DEFAULT}}
/* TARGET_DEFAULT is  the auto flags of above to pass as a default*/

#define TARGET_DEFAULT 0


/* This macro is similar to `TARGET_SWITCHES' but defines names of
   command options that have values.  Its definition is an
   initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   fixed part of the option name, and the address of a variable.  The
   variable, type `char *', is set to the variable part of the given
    option if the fixed part matches.  The actual option name is made
   by appending `-m' to the specified name.  */
#define TARGET_OPTIONS							\
{ { "align-loops=",	&icvm_align_loops_string },			\
  { "align-jumps=",	&icvm_align_jumps_string },			\
  { "align-functions=",	&icvm_align_funcs_string },			\
  SUBTARGET_OPTIONS							\
}

/* Sometimes certain combinations of command options do not make
   sense on a particular target machine.  You can define a macro
   `OVERRIDE_OPTIONS' to take account of this.  This macro, if
   defined, is executed once just after all the command options have
   been parsed.

   Don't use this macro to turn on various extra optimizations for
   `-O'.  That is what `OPTIMIZATION_OPTIONS' is for.  */

#define OVERRIDE_OPTIONS		\
{					\
  override_options();			\
  SUBTARGET_OVERRIDE_OPTIONS;		\
}

/* turn off some O2 options - bad with I64 stuff and does NOT optimize 
  very well for icvm - also some of it is very buggy and does not work.														
*/

#define OPTIMIZATION_OPTIONS(a,b) {\
      flag_strength_reduce = 0;\
      flag_force_mem = 0;\
}


#ifdef NOT_NOW
/* some of these may need to still be disabled */
							
      flag_gcse = 0;\
      flag_expensive_optimizations = 0;\
      flag_force_mem = 0;\
      flag_force_mem = 0;
      flag_strength_reduce = 0;
      flag_rerun_loop_opt = 0;
      flag_rerun_cse_after_loop = 0;
      flag_strict_aliasing = 0; 
      flag_regmove = 0; 
      flag_caller_saves = 0; 

#endif

/* redefine include files */

#undef INCLUDE_DEFAULTS
#define INCLUDE_DEFAULTS			\
  {						\
    { GPLUSPLUS_INCLUDE_DIR, "G++", 1, 1},\
    { GCC_INCLUDE_DIR, "GCC", 0, 0},\
    { CROSS_INCLUDE_DIR, "GCC", 0, 0},\
    { TOOL_INCLUDE_DIR, "BINUTILS", 0, 0},\
    { CROSS_INCLUDE_DIR, 0, 0, 0},\
    { CROSS_INCLUDE_DIR "/sys", 0, 0, 0},\
    { 0, 0, 0, 0}\
  }



/* These are meant to be redefined in the host dependent files */
#define SUBTARGET_SWITCHES
#define SUBTARGET_OPTIONS
#define SUBTARGET_OVERRIDE_OPTIONS

/* target machine storage layout */

/* Define for XFmode extended real floating point support.
   This will automatically cause REAL_ARITHMETIC to be defined.  */

/* Define if you don't want extended real, but do want to use the
   software floating point emulator for REAL_ARITHMETIC and
   decimal <-> binary conversion. */
/*#define LONG_DOUBLE_TYPE_SIZE 96 */
/* #define REAL_ARITHMETIC */



/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.
   This is true for 68020 insns such as bfins and bfexts. */


#define BITS_BIG_ENDIAN 0

/* float conversions are always signed */

#define FIXUNS_TRUNC_LIKE_FIX_TRUNC

/* Define this if most significant byte of a word is the lowest numbered.  */
#define BYTES_BIG_ENDIAN ((target_flags & MASK_BIGENDIAN))

/* Define this if most significant word of a multiword number is the lowest
   numbered.  */

#define WORDS_BIG_ENDIAN ((target_flags & MASK_BIGENDIAN))

/* number of bits in an addressable storage unit */
#define BITS_PER_UNIT 8

/* Width in bits of a "word", which is the contents of a machine register.
   Note that this is not necessarily the width of data type `int';
   if using 16-bit ints on a 68000, this would still be 32.
   But on a machine with 16-bit registers, this would be 16.  */
#define BITS_PER_WORD 32

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD 4


/* Width in bits of a pointer.
   See also the macro `Pmode' defined below.  */
#define POINTER_SIZE 32

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY 64

/* Boundary (in *bits*) on which stack pointer should be aligned.  */
#define STACK_BOUNDARY 64

/* Allocation boundary (in *bits*) for the code of a function.  */
#define FUNCTION_BOUNDARY 16


/* Every structure's size must be a multiple of this.  */
#define STRUCTURE_SIZE_BOUNDARY 8

/* A C expression which can inhibit the returning of certain function
   values in registers, based on the type of value.  A nonzero value says
   to return the function value in memory, just as large structures are
   always returned.  Here TYPE will be a C expression of type
   `tree', representing the data type of the value.

   Note that values of mode `BLKmode' must be explicitly
   handled by this macro.  Also, the option `-fpcc-struct-return'
   takes effect regardless of this macro.  On most systems, it is
   possible to leave the macro undefined; this causes a default
   definition to be used, whose value is the constant 1 for BLKmode
   values, and 0 otherwise.

   GCC normally converts 1 byte structures into chars, 2 byte
   structs into shorts, and 4 byte structs into ints, and returns
   them this way.  Defining the following macro overrides this,
   and forces blocks to memory.  */

#define RETURN_IN_MEMORY(TYPE)	\
  (TYPE_MODE (TYPE) == BLKmode)

/* A bitfield declared as `int' forces `int' alignment for the struct.  */
#define PCC_BITFIELD_TYPE_MATTERS 1


/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 8

/* Maximum size (in bits) to use for the largest integral type that
   replaces a BLKmode type. */

#define MAX_FIXED_MODE_SIZE 8


/* No data type wants to be aligned rounder than this. */ 

#define BIGGEST_ALIGNMENT 64

/* The best alignment to use in cases where we have a choice.  */

#define FASTEST_ALIGNMENT 32


/* Make strings word-aligned so strcpy from constants will be faster.  */
#define CONSTANT_ALIGNMENT(EXP, ALIGN)  \
  ((TREE_CODE (EXP) == STRING_CST	\
    && (ALIGN) < FASTEST_ALIGNMENT)	\
   ? FASTEST_ALIGNMENT : (ALIGN))

/* Make arrays of chars word-aligned for the same reasons.  */
#define DATA_ALIGNMENT(TYPE, ALIGN)		\
  (TREE_CODE (TYPE) == ARRAY_TYPE		\
   && TYPE_MODE (TREE_TYPE (TYPE)) == QImode	\
   && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))



/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
/*#define STRICT_ALIGNMENT 0 */
#define STRICT_ALIGNMENT 1
#define SLOW_UNALIGNED_ACCESS 0 /* set if non-aligned access is slow */

/* Layout of source language data types.  */

#define SHORT_TYPE_SIZE		16
#define INT_TYPE_SIZE		32
#define LONG_TYPE_SIZE		32
#define LONG_LONG_TYPE_SIZE	64
#define FLOAT_TYPE_SIZE		32
#define DOUBLE_TYPE_SIZE	64
#define LONG_DOUBLE_TYPE_SIZE	64



/* Maximum power of 2 that code can be aligned to.  */
#define MAX_CODE_ALIGN	1			/* 2 byte alignment */

/* Align loop starts for optimal branching.  */
#define ASM_OUTPUT_LOOP_ALIGN(FILE) ASM_OUTPUT_ALIGN ((FILE), icvm_align_loops)

/* This is how to align an instruction for optimal branching. */
#define ASM_OUTPUT_ALIGN_CODE(FILE) ASM_OUTPUT_ALIGN ((FILE), icvm_align_jumps)

#define SELECT_RTX_SECTION(MODE, X)					\
{									\
  if (!flag_pic)							\
    readonly_data_section();						\
  else if (LEGITIMATE_PIC_OPERAND_P (X))				\
    readonly_data_section();						\
  else									\
    data_section();							\
}

/* Define number of bits in most basic integer type.
   (If undefined, default is BITS_PER_WORD).  */


/* Define these to avoid dependence on meaning of `int'.
   Note that WCHAR_TYPE_SIZE is used in cexp.y,
   where TARGET_SHORT is not available.  */

#define SIZE_TYPE "unsigned int"
#define PTRDIFF_TYPE "int"
#define WCHAR_TYPE "int"
#define WCHAR_TYPE_SIZE 32

#define ISQMODE(MODE) (\
	(MODE)==DImode ||\
	(MODE)==TImode ||\
	(MODE)==OImode ||\
	(MODE)==TIQImode ||\
	(MODE)==TIHImode ||\
	(MODE)==TISImode ||\
	(MODE)==TIDImode ||\
	(MODE)==TISFmode ||\
	(MODE)==TIDFmode ||\
	(MODE)==OIQImode ||\
	(MODE)==OIHImode ||\
	(MODE)==OISImode ||\
	(MODE)==OIDImode ||\
	(MODE)==OISFmode ||\
	(MODE)==OIDFmode \
)


/* Standard register usage.  */
#define SMALL_REGISTER_CLASSES 1


/* Number of actual hardware registers. 
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.
   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers. */

#define FIRST_PSEUDO_REGISTER 128

/* This defines the register which is used to hold the offset table for PIC. */
#define PIC_OFFSET_TABLE_REGNUM 128 /* fictional till I remove -- BOB */

/* Used to output a (use pic_offset_table_rtx) so that we 
   always save/restore a5 in functions that use PIC relocation
   at *any* time during the compilation process. */
#define FINALIZE_PIC finalize_pic()


/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator. */

#define FIXED_REGISTERS        \
 {/* Data registers.  */       \
  0, 0, 0, 0, 1, 1, 1, 1,      \
                               \
  /* Address and long registers.  */    \
  1, 1, 1, 1, 0, 0, 1, 1,      \
                               \
  /* Floating point registers */ \
  0, 0, 0, 1, 1, 1, 1, 1,\
                               \
  /* data registers 28-31        */  \
  1, 1, 1, 1, 1, 1, 1, 1,\
\
  /* local general mem variable registers 32-63       */  \
  0, 0, 0, 0, 0, 0, 0, 0,\
  0, 0, 0, 0, 0, 0, 0, 0,\
  0, 0, 0, 0, 0, 0, 0, 0,\
  0, 0, 0, 0, 0, 0, 0, 0,\
  /* local general mem variable registers 64-95       */  \
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,\
  0,0,0,0,0,0,0,0,\
  1,1,1,1,1,1,1,1,\
  /* local general mem variable registers 96-127       */  \
  1,1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1,1,\
  1,1,1,1,1,1,1,1,\
\
 }

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you like.  */

#define CALL_USED_REGISTERS \
 {\
  1, 0, 0, 0, 1, 1, 1, 1,   \
  1, 1, 1, 1, 1, 0, 1, 1,   \
  1, 0, 0, 1, 1, 1, 1, 1,  \
  1, 1, 1, 1, 1, 1, 1, 1,  \
\
  0, 0, 0, 0, 0, 0, 0, 0,  \
  0, 0, 0, 0, 0, 0, 0, 0,  \
  0, 0, 0, 0, 0, 0, 0, 0,  \
  0, 0, 0, 0, 0, 0, 0, 0,  \
\
  0, 0, 0, 0, 0, 0, 0, 0,  \
  0, 0, 0, 0, 0, 0, 0, 0,  \
  0, 0, 0, 0, 0, 0, 0, 0,  \
  1, 1, 1, 1, 1, 1, 1, 1,  \
\
  1, 1, 1, 1, 1, 1, 1, 1,   \
  1, 1, 1, 1, 1, 1, 1, 1,   \
  1, 1, 1, 1, 1, 1, 1, 1,  \
  1, 1, 1, 1, 1, 1, 1, 1,  \
\
}

/* If defined, an initializer for a vector of integers, containing the
   numbers of hard registers in the order in which GNU CC should
   prefer to use them (from most preferred to least).  */

#define REG_ALLOC_ORDER \
{ \
 0,1,2,3,\
 16,17,18,\
 12,13,\
 32,33,34,35,36,37,38,39,\
 40,41,42,43,44,45,46,47,\
 48,49,50,51,52,53,54,55,\
 56,57,58,59,60,61,62,63,\
 64,65,66,67,68,69,70,71,\
 72,73,74,75,76,77,78,79,\
 80,81,82,83,84,85,86,87,\
 }



#define HARD_REGNO_NREGS(REGNO, MODE) 1


/* Value is 1 if hard register REGNO can hold a value of machine-mode MODE. */

#define HI_HARD_REGNO_MODE_OK(REGNO, MODE) \
  ( (MODE)==HImode ? ISHIREG(REGNO) : 0)

#define QI_HARD_REGNO_MODE_OK(REGNO, MODE) \
  ( (MODE)==QImode ? ISQIREG(REGNO) : 0)

#define SI_HARD_REGNO_MODE_OK(REGNO, MODE) \
  ( (MODE)==SImode ? ISSIREG(REGNO) : 0)

/*
  #define FLOAT_HARD_REGNO_MODE_OK(REGNO, MODE) \
  (( (MODE)==SFmode || (MODE)==DFmode ) ?\
         ISFLOATREG(REGNO) : 0)
*/
  
#define FLOAT32_HARD_REGNO_MODE_OK(REGNO, MODE) \
  (( (MODE)==SFmode) ?\
         ISSFREG(REGNO) : 0)
  
#define FLOAT64_HARD_REGNO_MODE_OK(REGNO, MODE) \
  (( (MODE)==DFmode) ?\
         ISDFREG(REGNO) : 0)
  
  

#define INT64_HARD_REGNO_MODE_OK(REGNO, MODE) \
  ( ISQMODE(MODE) ?\
         ISLONGREG(REGNO) : 0)
  


#define HARD_REGNO_MODE_OK(REGNO, MODE) \
	(\
	SI_HARD_REGNO_MODE_OK(REGNO, MODE) || \
	QI_HARD_REGNO_MODE_OK(REGNO, MODE) || \
	HI_HARD_REGNO_MODE_OK(REGNO, MODE) || \
	FLOAT32_HARD_REGNO_MODE_OK(REGNO, MODE) || \
	FLOAT64_HARD_REGNO_MODE_OK(REGNO, MODE) || \
	INT64_HARD_REGNO_MODE_OK(REGNO, MODE))



/* A C expression that is nonzero if it is desirable to choose
   register allocation so as to avoid move instructions between a
   value of mode MODE1 and a value of mode MODE2.

   If `HARD_REGNO_MODE_OK (R, MODE1)' and `HARD_REGNO_MODE_OK (R,
   MODE2)' are ever different for any R, then `MODES_TIEABLE_P (MODE1,
   MODE2)' must be zero.  */


/* Tie QI/HI/SI modes together.  */
#define MODES_TIEABLE_P(MODE1, MODE2) (\
(GET_MODE_CLASS (MODE1) == MODE_INT		\
 && GET_MODE_CLASS (MODE2) == MODE_INT		\
 && GET_MODE_SIZE (MODE1) == GET_MODE_SIZE (MODE2)\
 && GET_MODE_SIZE(MODE1)<=4) ||\
 (ISQMODE(MODE1) && ISQMODE(MODE2))\
)


										

/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */

/* #define PC_REGNUM  */

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM 15

/* Base register for access to local variables of the function.  */
#define FRAME_POINTER_REGNUM 14

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED 1

/* Base register for access to arguments of the function.  */
#define ARG_POINTER_REGNUM FRAME_POINTER_REGNUM

/* Register in which static-chain is passed to a function.  */
#define STATIC_CHAIN_REGNUM 0

/* Register in which address to store a structure value
   is passed to a function.  */
#define STRUCT_VALUE_REGNUM 0

/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS

   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.  */

enum reg_class {
  NO_REGS, 
  HARDINT_REGS,
  INT32_REGS,
  INT16_REGS,
  INT8_REGS,
  ADTL_REGS,
  HARDFLOAT_REGS,
  FLOAT32_REGS,
  FLOAT64_REGS,
  INDEXABLE_REGS,
  HARDLONG_REGS,	
  LONG_REGS,	
  MEM_REGS,	
  SI_REGS,	
  FIXEDSIZE_REGS,	
  GENERAL_REGS,
  ALL_REGS,
  LIM_REG_CLASSES 
  };

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.   */

#define REG_CLASS_NAMES \
 { "NO_REGS",\
     "HARDINT_REGS",              \
     "INT32_REGS",              \
     "INT16_REGS",              \
     "INT8_REGS",              \
   "ADTL_REGS",\
    "HARDFLOAT_REGS",              \
    "FLOAT32_REGS",              \
    "FLOAT64_REGS",              \
     "INDEXABLE_REGS",\
   "HARDLONG_REGS",\
   "LONG_REGS",\
   "MEM_REGS",\
   "SI_REGS",\
   "FIXEDSIZE_REGS",\
   "GENERAL_REGS",   \
    "ALL_REGS" \
	}

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */

#define REG_CLASS_CONTENTS \
{					\
0x00000000, 0x00000000, 0x00000000, 0x00000000,  	/* NO_REGS */		\
0x0000000f, 0x00000000,	0x00000000, 0x00000000, /* HARDINT_REGS */		\
0x0000000f, 0x0000ffff,	0x00000000, 0x00000000, /* INT32_REGS */		\
0x0000000f, 0x00000000,	0x000000ff, 0x00000000, /* INT16_REGS */		\
0x0000000f, 0x00000000,	0x0000ff00, 0x00000000, /* INT8_REGS */		\
0x0000c00c, 0x0000ffff,	0x00000000, 0x00000000, /* ADTL_REGS */		\
0x00070000, 0x00000000,	0x00000000, 0x00000000, /* HARDFLOAT_REGS */		\
0x00070000, 0x00000000,	0x00ff0000, 0x00000000, /* FLOAT32_REGS */		\
0x00070000, 0x00ff0000,	0x00000000, 0x00000000, /* FLOAT64_REGS */		\
0x0000c00c, 0x0000ffff,	0x00000000, 0x00000000, /* INDEXABLE_REGS */	\
0x00003000, 0x00000000,	0x00000000, 0x00000000, /* HARDLONG_REGS */		\
0x00003000, 0xff000000,	0x00000000, 0x00000000, /* LONG_REGS */		\
0x00000000, 0xffffffff,	0x00ffffff, 0x00000000, /* MEM_REGS */		\
0x0000c00f, 0x0000ffff,	0x00000000, 0x00000000, /* SI_REGS */		\
0x00073000, 0xffffffff,	0xffffffff, 0x00000000, /* FIXEDSIZE_REGS */		\
0x0007d00f, 0xffffffff,	0x00ffffff, 0x00000000, /* GENERAL_REGS */	\
0x0007f00f, 0xffffffff,	0x00ffffff, 0x00000000, /* ALL_REGS */		\
}


/* definitions to aid us in sorting registers */
#define ISLONGREG(REGNO) \
	((REGNO)==12 || (REGNO)==13 || ((REGNO)>=56 && (REGNO)<64))
#define ISHARDFLOATREG(REGNO) ((REGNO)>=16 && (REGNO)<=18)

/*#define ISFLOATREG(REGNO)\
	 ( ISHARDFLOATREG(REGNO) || ((REGNO)>=48 && (REGNO)<56))
*/

#define ISINDEXREG(REGNO)  (((REGNO)>=32 && (REGNO)<48) || (REGNO)==2 || (REGNO)==3) 
#define ISFRAMEREG(REGNO) ((REGNO)==14)
#define ISSTACKREG(REGNO) ((REGNO)==15)
#define ISBASEREG(REGNO)\
	 (ISSTACKREG(REGNO) || ISFRAMEREG(REGNO) || ISINDEXREG(REGNO))

#define ISHARDINTREG(REGNO)  ((REGNO)>=0 && (REGNO)<4)

#define ISSIREG(REGNO)  ( ISHARDINTREG(REGNO) || ISBASEREG(REGNO))
#define ISHIREG(REGNO)  ( ISHARDINTREG(REGNO) || ((REGNO)>=64 && (REGNO)<72))
#define ISQIREG(REGNO)  ( ISHARDINTREG(REGNO) || ((REGNO)>=72 && (REGNO)<80))
#define ISSFREG(REGNO)  ( ISHARDFLOATREG(REGNO) || ((REGNO)>=80 && (REGNO)<88))
#define ISDFREG(REGNO)  ( ISHARDFLOATREG(REGNO) || ((REGNO)>=48 && (REGNO)<56))
		

/* our stack based memory registers */
#define ISMEMREG(REGNO) ((REGNO)>=32 && (REGNO)<128)
/* hardware registers */
#define ISHARDREG(REGNO) ((REGNO)>=0 && (REGNO)<32)
				
#define SIRETURNREG	0
#define FLOATRETURNREG	16
#define LONGRETURNREG	12

#define IsHardI64Reg(x) (GET_CODE(x)==REG && ISLONGREG(REGNO(x))) 
#define IsHardI32Reg(x) (GET_CODE(x)==REG && ISHARDINTREG(REGNO(x))) 
#define IsHardI32RegMode(x) (GET_MODE(x)==SImode || GET_MODE(x)==HImode || GET_MODE(x)==QImode)
#define IsICVMNopTrunc(x,tomode) (\
((GET_MODE(x)==SImode  && (tomode==HImode || tomode==QImode)\
	|| (GET_MODE(x)==HImode && tomode==QImode)) ? IsHardI32Reg(x) :\
	GET_MODE(x)==tomode) || (IsHardI64Reg(x) && ISQMODE(tomode))\
)
 


#define IsSoftSIReg(x) (GET_CODE(x)==REG && ISMEMREG(REGNO(x)) && ISSIREG(REGNO(x))) 
#define IsSoftHIReg(x) (GET_CODE(x)==REG && ISMEMREG(REGNO(x)) && ISHIREG(REGNO(x))) 
#define IsSoftQIReg(x) (GET_CODE(x)==REG && ISMEMREG(REGNO(x)) && ISQIREG(REGNO(x))) 
#define IsSoftDIReg(x) (GET_CODE(x)==REG && ISMEMREG(REGNO(x)) && ISLONGREG(REGNO(x))) 
#define IsSoftDFReg(x) (GET_CODE(x)==REG && ISMEMREG(REGNO(x)) && ISDFREG(REGNO(x))) 
#define IsSoftSFReg(x) (GET_CODE(x)==REG && ISMEMREG(REGNO(x)) && ISSFREG(REGNO(x))) 

/* this macro checks stack register usage - double check GCC for proper reg handling */
#define IsBadSoftRegUsage(x) (\
	(IsSoftHIReg(x) && GET_MODE(x)!=HImode) ||\
	(IsSoftQIReg(x) && GET_MODE(x)!=QImode) ||\
	(IsSoftSIReg(x) && GET_MODE(x)!=SImode) ||\
	(IsSoftDIReg(x) && !ISQMODE(GET_MODE(x))) ||\
	(IsSoftSFReg(x) && GET_MODE(x)!=SFmode) ||\
	(IsSoftDFReg(x) && GET_MODE(x)!=DFmode)\
)

#define CheckSoftRegUsage(x)\
	{if(IsBadSoftRegUsage(x)) ICVMabortrtl("Bad Stack Reg Usage",x);}

#define HasSameRegClass(x,y) (\
	GET_CODE(x)==REG &&\
	GET_CODE(y)==REG &&\
	REGNO_REG_CLASS(REGNO(x))==REGNO_REG_CLASS(REGNO(y))\
)



/* to prevent all sorts of subreg optimizations -- BAD NEWS in the case of ICVM!!! */
#define CLASS_CANNOT_CHANGE_SIZE FIXEDSIZE_REGS


/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define LIDX_LO 32  /* local indexable variables lowest number */
#define LIDX_HI 63  /* local indexable variables highest number */


#define REGNO_REG_CLASS(REGNO) (\
	ISHARDINTREG(REGNO) ? HARDINT_REGS :\
	ISHARDFLOATREG(REGNO) ? HARDFLOAT_REGS :\
	ISSIREG(REGNO) ? SI_REGS :\
	ISHIREG(REGNO) ? INT16_REGS :\
	ISQIREG(REGNO) ? INT8_REGS :\
	ISSFREG(REGNO) ? FLOAT32_REGS :\
	ISDFREG(REGNO) ? FLOAT64_REGS :\
	ISLONGREG(REGNO) ? LONG_REGS : GENERAL_REGS)



/* The class value for index registers, and the one for base regs.  */

#define INDEX_REG_CLASS INDEXABLE_REGS
#define BASE_REG_CLASS INDEXABLE_REGS

/* Get reg_class from a letter such as appears in the machine description.
   We do a trick here to modify the effective constraints on the
   machine description; we zorch the constraint letters that aren't
   appropriate for a specific target.  This allows us to guarantee
   that a specific kind of register will not be used for a given target
   without fiddling with the register classes above. */


#define REG_CLASS_FROM_LETTER(C) \
  ((C) == 'q' ? LONG_REGS :			\
   (C) == 'd' ? INT32_REGS :			\
   (C) == 'W' ? INT16_REGS :			\
   (C) == 'B' ? INT8_REGS :			\
   (C) == 'b' ? SI_REGS :			\
    (C) == 'Z' ? FLOAT32_REGS :	\
    (C) == 'f' ? FLOAT64_REGS :	\
     NO_REGS)



/* The letters I, J, K, L and M in a register constraint string
   can be used to stand for particular ranges of immediate operands.
   This macro defines what the ranges are.
   C is the letter, and VALUE is a constant value.
   Return 1 if VALUE is in the range specified by C.

   For the 68000, `I' is used for the range 1 to 8
   allowed as immediate shift counts and in addq.
   `J' is used for the range of signed numbers that fit in 16 bits.
   `K' is for numbers that moveq can't handle.
   `L' is for range -8 to -1, range of values that can be added with subq.
   `M' is for numbers that moveq+notb can't handle.
   'N' is for range 24 to 31, rotatert:SI 8 to 1 expressed as rotate.
   'O' is for 16 (for rotate using swap).
   'P' is for range 8 to 15, rotatert:HI 8 to 1 expressed as rotate.  */

#define CONST_OK_FOR_LETTER_P(VALUE, C) \
  ((C) == 'I' ? (VALUE) > 0 && (VALUE) <= 8 : \
   (C) == 'J' ? (VALUE) >= -0x8000 && (VALUE) <= 0x7FFF : \
   (C) == 'K' ? (VALUE) < -0x80 || (VALUE) >= 0x80 : \
   (C) == 'L' ? (VALUE) < 0 && (VALUE) >= -8 : \
   (C) == 'M' ? (VALUE) < -0x100 && (VALUE) >= 0x100 : \
   (C) == 'N' ? (VALUE) >= 24 && (VALUE) <= 31 : \
   (C) == 'O' ? (VALUE) == 16 : \
   (C) == 'P' ? (VALUE) >= 8 && (VALUE) <= 15 : 0)

/*
 * A small bit of explanation:
 * "G" defines all of the floating constants that are *NOT* 68881
 * constants.  this is so 68881 constants get reloaded and the
 * fpmovecr is used.  "H" defines *only* the class of constants that
 * the fpa can use, because these can be gotten at in any fpa
 * instruction and there is no need to force reloads.
 */
#ifndef SUPPORT_SUN_FPA
#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)  \
  ((C) == 'G' ? ! (TARGET_68881 && standard_68881_constant_p (VALUE)) : 0 )
#else /* defined SUPPORT_SUN_FPA */
#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)  \
  ((C) == 'G' ? ! (TARGET_68881 && standard_68881_constant_p (VALUE)) : \
   (C) == 'H' ? (TARGET_FPA && standard_sun_fpa_constant_p (VALUE)) : 0)
#endif /* defined SUPPORT_SUN_FPA */

/* A C expression that defines the optional machine-dependent constraint
   letters that can be used to segregate specific types of operands,  
   usually memory references, for the target machine.  It should return 1 if
   VALUE corresponds to the operand type represented by the constraint letter
   C.  If C is not defined as an extra constraint, the value returned should 
   be 0 regardless of VALUE.  */



/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
*/

			


int LimitReloadClass(int mode,int class);
					
											
#define LIMIT_RELOAD_CLASS(MODE,CLASS) LimitReloadClass(MODE,CLASS)	

#ifdef NOT_NOW
#define LIMIT_RELOAD_CLASS(MODE,CLASS) ( \
	(CLASS==INDEXABLE_REGS) ? \
		INDEXABLE_REGS :\
	(CLASS==FLOAT64_REGS) ? \
		FLOAT64_REGS :\
	(CLASS==LONG_REGS) ? \
		LONG_REGS :\
	(CLASS==SI_REGS) ? \
		SI_REGS :\
	(MODE==SImode || MODE==QImode || MODE==HImode) ?\
		SI_REGS :\
	(MODE==SFmode || MODE==DFmode) ?\
		FLOAT64_REGS :\
	( ISQMODE(MODE) ) ?\
		LONG_REGS : CLASS)
#endif


															
											
#define PREFERRED_RELOAD_CLASS(X,CLASS) (LIMIT_RELOAD_CLASS(GET_MODE(X),CLASS))





/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */

#define CLASS_MAX_NREGS(CLASS, MODE)	\
 ((CLASS) == FLOAT64_REGS ? 1 \
  : ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD))



#define ADDRESS_COST(ADDR) 1
                                            
#define CONST_COSTS(X, CODE, OUTER_CODE) \
  case CONST_INT :						\
  case CONST :							\
  case LABEL_REF :						\
  case SYMBOL_REF :						\
  case CONST_DOUBLE :						\
    return 1;							

/* Compute extra cost of moving data between one register class
   and another.  */

#define REGISTER_MOVE_COST(CLASS1, CLASS2) 2 
#define MEMORY_MOVE_COST(MODE,CLASS,IN)  2

                                
/* The cost of a branch insn.  */
/* A value of 2 here causes GCC to avoid using branches in comparisons like
   while (a < N && a). */

#define BRANCH_COST 1


/* Define this macro if it is as good or better to call a constant
   function address than to call an address kept in a register.  */
#define NO_FUNCTION_CSE

/* Define this macro if it is as good or better for a function to call
   itself with an explicit address than to call an address kept in a
   register.  */
#define NO_RECURSIVE_FUNCTION_CSE



/* Stack layout; function entry, exit and calling.  */

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */
#define STACK_GROWS_DOWNWARD

/* Nonzero if we need to generate stack-probe insns.
   On most systems they are not needed.
   When they are needed, define this as the stack offset to probe at.  */
#define NEED_PROBE 0

/* Define this if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
#define FRAME_GROWS_DOWNWARD


/* If we generate an insn to push BYTES bytes,
   this says how many the stack pointer really advances by. */
/* push 64 bytes */

#define PUSH_ROUNDING(BYTES) \
    (((BYTES) + 7) & -8)


/* Value is the number of byte of arguments automatically
   popped when returning from a subroutine call.
   FUNDECL is the declaration node of the function (as a tree),
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name.
   SIZE is the number of bytes of arguments passed on the stack.
*/

#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE)  0


/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */


#define lcGETMODE(MODE) (MODE)
				
#define lcGETREG(MODE) ( \
	   (GET_MODE_CLASS (MODE) == MODE_INT				\
	     && GET_MODE_SIZE (MODE) <= 4)				\
	    ? SIRETURNREG :	\
	   (GET_MODE_CLASS (MODE) == MODE_INT				\
	     && GET_MODE_SIZE (MODE) > 4)				\
	    ? LONGRETURNREG :	\
	   (GET_MODE_CLASS (MODE) == MODE_FLOAT)				\
	? FLOATRETURNREG : SIRETURNREG)
 

 
#define LIBCALL_VALUE(MODE)  gen_rtx (REG, lcGETMODE(MODE), lcGETREG(MODE))


/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.  */

#define FUNCTION_VALUE(VALTYPE, FUNC) LIBCALL_VALUE (TYPE_MODE (VALTYPE))


/* 1 if N is a possible register number for a function value. */

#define FUNCTION_VALUE_REGNO_P(N)\
	 ((N) == SIRETURNREG || (N)==LONGRETURNREG || (N)==FLOATRETURNREG )


#define NEEDS_UNTYPED_CALL 0

/* Define this if PCC uses the nonreentrant convention for returning
   structure and union values.  */





								
#define FUNCTION_ARG_REGNO_P(N) 0
#define CUMULATIVE_ARGS int
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,INDIRECT)
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) 0





/* Generate the assembly code for function entry. */
#define FUNCTION_PROLOGUE(FILE, SIZE) output_function_prologue(FILE, SIZE)

/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */

#define FUNCTION_PROFILER(FILE, LABELNO)  \
  asm_fprintf (FILE, "\tlea %LLP%d,%Ra0\n\tjsr mcount\n", (LABELNO))

/* Output assembler code to FILE to initialize this source file's
   basic block profiling info, if that has not already been done.  */

#define FUNCTION_BLOCK_PROFILER(FILE, BLOCK_OR_LABEL)	\
do							\
  {							\
    switch (profile_block_flag)				\
      {							\
      case 2:						\
        asm_fprintf (FILE, "\tpea %d\n\tpea %LLPBX0\n\tjsr %U__bb_init_trace_func\n\taddql %I8,%Rsp\n", \
                           (BLOCK_OR_LABEL)); \
        break;						\
							\
      default:						\
        asm_fprintf (FILE, "\ttstl %LLPBX0\n\tbne %LLPI%d\n\tpea %LLPBX0\n\tjsr %U__bb_init_func\n\taddql %I4,%Rsp\n%LLPI%d:\n", \
                           (BLOCK_OR_LABEL), (BLOCK_OR_LABEL)); \
        break;						\
      }							\
  }							\
while(0)

/* Output assembler code to FILE to increment the counter for
   the BLOCKNO'th basic block in this source file.  */

#define BLOCK_PROFILER(FILE, BLOCKNO)	\
do							\
  {							\
    switch (profile_block_flag)				\
      {							\
      case 2:						\
        asm_fprintf (FILE, "\tmovel %Ra1,%Rsp@-\n\tlea ___bb,%Ra1\n\tmovel %I%d,%Ra1@(0)\n\tmovel %I%LLPBX0,%Ra1@(4)\n\tmovel %Rsp@+,%Ra1\n\tjsr %U__bb_trace_func\n", \
                           BLOCKNO);			\
        break;						\
							\
      default:						\
        asm_fprintf (FILE, "\taddql %I1,%LLPBX2+%d\n", 4 * BLOCKNO); \
        break;						\
      }							\
  }							\
while(0)

/* Output assembler code to FILE to indicate return from 
   a function during basic block profiling. */

#define FUNCTION_BLOCK_PROFILER_EXIT(FILE)		\
  asm_fprintf (FILE, "\tjsr %U__bb_trace_ret\n");

/* Save all registers which may be clobbered by a function call.
   MACHINE_STATE_SAVE and MACHINE_STATE_RESTORE are target-code macros,
   used in libgcc2.c.  They may not refer to TARGET_* macros !!! */
#if defined (__mc68010__) || defined(mc68010) \
	|| defined(__mc68020__) || defined(mc68020) \
	|| defined(__mc68030__) || defined(mc68030) \
	|| defined(__mc68040__) || defined(mc68040) \
	|| defined(__mc68332__) || defined(mc68332)
#define MACHINE_STATE_m68010_up
#endif

#if defined(__mcf5200__)
#define MACHINE_STATE_SAVE(id)		\
    {					\
      asm ("sub.l 20,%sp");		\
      asm ("movm.l &0x0303,4(%sp)");	\
      asm ("move.w %ccr,%d0");		\
      asm ("movm.l &0x0001,(%sp)");	\
    }
#else /* !__mcf5200__ */
#if defined(MACHINE_STATE_m68010_up)
#ifdef __HPUX_ASM__
/* HPUX assembler does not accept %ccr.  */
#define MACHINE_STATE_SAVE(id)		\
    {					\
      asm ("move.w %cc,-(%sp)");	\
      asm ("movm.l &0xc0c0,-(%sp)");	\
    }
#else /* ! __HPUX_ASM__ */
#define MACHINE_STATE_SAVE(id)		\
    {					\
      asm ("move.w %ccr,-(%sp)");	\
      asm ("movm.l &0xc0c0,-(%sp)");	\
    }
#endif /* __HPUX_ASM__ */
#else /* !MACHINE_STATE_m68010_up */
#define MACHINE_STATE_SAVE(id)		\
    {					\
      asm ("move.w %sr,-(%sp)");	\
      asm ("movm.l &0xc0c0,-(%sp)");	\
    }
#endif /* MACHINE_STATE_m68010_up */
#endif /* __mcf5200__ */


/* Restore all registers saved by MACHINE_STATE_SAVE. */

#if defined(__mcf5200__)
#define MACHINE_STATE_RESTORE(id)	\
    {					\
      asm ("movm.l (%sp),&0x0001");	\
      asm ("move.w %d0,%ccr");		\
      asm ("movm.l 4(%sp),&0x0303");	\
      asm ("add.l 20,%sp");		\
    }
#else /* !__mcf5200__ */
#ifdef __HPUX_ASM__
/* HPUX assembler does not accept %ccr.  */
#define MACHINE_STATE_RESTORE(id)	\
    {					\
      asm ("movm.l (%sp)+,&0x0303");	\
      asm ("move.w (%sp)+,%cc");	\
    }
#else /* ! __HPUX_ASM__ */
#define MACHINE_STATE_RESTORE(id)	\
    {					\
      asm ("movm.l (%sp)+,&0x0303");	\
      asm ("move.w (%sp)+,%ccr");	\
    }
#endif /* __HPUX_ASM__ */
#endif /* __mcf5200__ */

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */

#define EXIT_IGNORE_STACK 1

/* Generate the assembly code for function exit. */
#define FUNCTION_EPILOGUE(FILE, SIZE) output_function_epilogue (FILE, SIZE)
  
/* This is a hook for other tm files to change.  */
/* #define FUNCTION_EXTRA_EPILOGUE(FILE, SIZE) */

/* Determine if the epilogue should be output as RTL.
   You should override this if you define FUNCTION_EXTRA_EPILOGUE.  */
#define USE_RETURN_INSN use_return_insn ()

/* Store in the variable DEPTH the initial difference between the
   frame pointer reg contents and the stack pointer reg contents,
   as of the start of the function body.  This depends on the layout
   of the fixed parts of the stack frame and on how registers are saved.

   On ICVM up to 64 pseudo registers (64 bit) are kept on stack, so we
   need to find the max end of those use and make sure it is saved. */


int ICVM_NumStackRegsUsed(void);
int ICVM_SavedRegsSize(void);


#ifdef NOT_NOW /* stuff below does not work correctly all the time */

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH)			\
{ \
  int numregs=0;\
  int offset = (get_frame_size() +7) & -8;						\
  numregs=ICVM_NumStackRegsUsed();\
  offset=offset+numregs*8;\
  offset+=ICVM_SavedRegsSize();\
  (DEPTH) = offset;\
}

/*#define STARTING_FRAME_OFFSET  ((ICVM_NumStackRegsUsed() * -8)-8)
*/


#define STARTING_FRAME_OFFSET  (ICVM_NumStackRegsUsed() * -8)

#endif

/* this stuff is used for now to get temp registers on stack
 stuff above should be used but does not work all the time.
*/

#define INITIAL_FRAME_POINTER_OFFSET(DEPTH)			\
{ \
  int offset = (get_frame_size() +7) & -8;						\
  offset-=STARTING_FRAME_OFFSET;\
  (DEPTH) = offset;\
}


int ICVM_RegSpacePushed(void);


#define FIRST_PARM_OFFSET(FNDECL) (16)

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */
																																										

#define TRAMPOLINE_SIZE 4 /* til we figure out what it is */
#define INITIALIZE_TRAMPOLINE /* again, till we figure out what it is */


#ifdef NOT_NOW


/* Output assembler code for a block containing the constant parts
   of a trampoline, leaving space for the variable parts.  */

/* On the 68k, the trampoline looks like this:
     movl #STATIC,a0
     jmp  FUNCTION

   WARNING: Targets that may run on 68040+ cpus must arrange for
   the instruction cache to be flushed.  Previous incarnations of
   the icvm trampoline code attempted to get around this by either
   using an out-of-line transfer function or pc-relative data, but
   the fact remains that the code to jump to the transfer function
   or the code to load the pc-relative data needs to be flushed
   just as much as the "variable" portion of the trampoline.  
   Recognizing that a cache flush is going to be required anyway,
   dispense with such notions and build a smaller trampoline.  */

/* Since more instructions are required to move a template into
   place than to create it on the spot, don't use a template.  */

/* Length in units of the trampoline for entering a nested function.  */

#define TRAMPOLINE_SIZE 12

/* Alignment required for a trampoline in bits.  */

#define TRAMPOLINE_ALIGNMENT 16

/* Targets redefine this to invoke code to either flush the cache,
   or enable stack execution (or both).  */

#ifndef FINALIZE_TRAMPOLINE
#define FINALIZE_TRAMPOLINE(TRAMP)
#endif

/* Emit RTL insns to initialize the variable parts of a trampoline.
   FNADDR is an RTX for the address of the function's pure code.
   CXT is an RTX for the static chain value for the function.

   We generate a two-instructions program at address TRAMP :
	movea.l &CXT,%a0
	jmp FNADDR					*/

#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT)			\
{									\
  emit_move_insn (gen_rtx (MEM, HImode, TRAMP), GEN_INT(0x207C));	\
  emit_move_insn (gen_rtx (MEM, SImode, plus_constant (TRAMP, 2)), CXT); \
  emit_move_insn (gen_rtx (MEM, HImode, plus_constant (TRAMP, 6)),	\
		  GEN_INT(0x4EF9));					\
  emit_move_insn (gen_rtx (MEM, SImode, plus_constant (TRAMP, 8)), FNADDR); \
  FINALIZE_TRAMPOLINE(TRAMP);						\
}

/* This is the library routine that is used
   to transfer control from the trampoline
   to the actual nested function.
   It is defined for backward compatibility,
   for linking with object code that used the old
   trampoline definition.  */

/* A colon is used with no explicit operands
   to cause the template string to be scanned for %-constructs.  */
/* The function name __transfer_from_trampoline is not actually used.
   The function definition just permits use of "asm with operands"
   (though the operand list is empty).  */
#define TRANSFER_FROM_TRAMPOLINE				\
void								\
__transfer_from_trampoline ()					\
{								\
  register char *a0 asm ("%a0");				\
  asm (GLOBAL_ASM_OP " ___trampoline");				\
  asm ("___trampoline:");					\
  asm volatile ("move%.l %0,%@" : : "m" (a0[22]));		\
  asm volatile ("move%.l %1,%0" : "=a" (a0) : "m" (a0[18]));	\
  asm ("rts":);							\
}


#endif




/* Addressingd modes, and classification of registers for them.  */

#define HAVE_POST_INCREMENT (1)

#define HAVE_PRE_DECREMENT (1)


/* Macros to check register numbers against specific register classes.  */


/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.

   Most source files want to accept pseudo regs in the hope that
   they will get allocated to the class that the insn wants them to be in.
   Source files for reload pass need to be strict.
   After reload, it makes no difference, since pseudo regs have
   been eliminated by then.  */




#define OK_INDEXRANGE(x) ISINDEXREG(x)
#define OK_BASERANGE(x) ISBASEREG(x)

#define OK_FRAMEPOINTER(x) \
    ((reg_renumber[x]==FRAME_POINTER_REGNUM || (x)==FRAME_POINTER_REGNUM) ||\
    (reg_renumber[x]==STACK_POINTER_REGNUM || (x)==STACK_POINTER_REGNUM))

#define REGNO_OK_FOR_INDEX_P(REGNO)\
     ((REGNO) < FIRST_PSEUDO_REGISTER ? OK_INDEXRANGE(REGNO) : OK_INDEXRANGE(reg_renumber[REGNO]))

#define REGNO_OK_FOR_BASE_P(REGNO) \
     ((REGNO) < FIRST_PSEUDO_REGISTER ? OK_BASERANGE(REGNO) : OK_BASERANGE(reg_renumber[REGNO]))


extern short *reg_renumber; /* not defined in several places */

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  */

                                        
#define NOT_STRICT_REG_OK_FOR_INDEX_P(X) \
	(REGNO(X)>=FIRST_PSEUDO_REGISTER || OK_INDEXRANGE(REGNO(X)))

#define STRICT_REG_OK_FOR_INDEX_P(X) REGNO_OK_FOR_INDEX_P(REGNO(X))
									

/* Nonzero if X is a hard reg that can be used as a base reg
   or if it is a pseudo reg.  */



#define NOT_STRICT_REG_OK_FOR_BASE_P(X)\
	(REGNO(X)>=FIRST_PSEUDO_REGISTER || OK_BASERANGE(REGNO(X)))

#define STRICT_REG_OK_FOR_BASE_P(X) REGNO_OK_FOR_BASE_P(REGNO(X))




#define REG_OK_FOR_BASE_P(X) \
    (REG_OK_IS_STRICT  ? STRICT_REG_OK_FOR_BASE_P(X) :\
         NOT_STRICT_REG_OK_FOR_BASE_P(X))

#define REG_OK_FOR_INDEX_P(X) \
    (REG_OK_IS_STRICT  ? STRICT_REG_OK_FOR_INDEX_P(X) :\
         NOT_STRICT_REG_OK_FOR_INDEX_P(X))


#ifndef REG_OK_STRICT

#define REG_OK_IS_STRICT 0


#else

#define REG_OK_IS_STRICT 1


#endif





#ifdef NOT_NOW

#define REGNO_OK_FOR_DATA_P(REGNO) \
((REGNO) < 4 || (unsigned) reg_renumber[REGNO] < 4)


#define REGNO_OK_FOR_FP_P(REGNO) \
(((REGNO) ^ 020) < 8 || (unsigned) (reg_renumber[REGNO] ^ 020) < 8)

#endif





/* Now macros that check whether X is a register and also,
   strictly, whether it is in a specified class.

   These macros are specific to the 68000, and may be used only
   in code for printing assembler insns and in conditions for
   define_optimization.  */

/* 1 if X is a data register.  */

#define DATA_REG_P(X) (REG_P (X) && REGNO_OK_FOR_DATA_P (REGNO (X)))

/* 1 if X is an fp register.  */

#define FP_REG_P(X) (REG_P (X) && REGNO_OK_FOR_FP_P (REGNO (X)))

/* 1 if X is an address register  */

#define ADDRESS_REG_P(X) (REG_P (X) && REGNO_OK_FOR_BASE_P (REGNO (X)))

#ifdef SUPPORT_SUN_FPA
/* 1 if X is a register in the Sun FPA.  */
#define FPA_REG_P(X) (REG_P (X) && REGNO_OK_FOR_FPA_P (REGNO (X)))
#else
/* Answer must be no if we don't have an FPA.  */
#define FPA_REG_P(X) 0
#endif

/* Maximum number of registers that can appear in a valid memory address.  */

#define MAX_REGS_PER_ADDRESS 2

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */

#define LEGITIMATE_CONSTANT_P(X) 1

/* Nonzero if the constant value X is a legitimate general operand
   when generating PIC code.  It is given that flag_pic is on and 
   that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */

#define LEGITIMATE_PIC_OPERAND_P(X)	\
  ((! symbolic_operand (X, VOIDmode)				\
    && ! (GET_CODE (X) == CONST_DOUBLE && CONST_DOUBLE_MEM (X)	\
	  && GET_CODE (CONST_DOUBLE_MEM (X)) == MEM		\
	  && symbolic_operand (XEXP (CONST_DOUBLE_MEM (X), 0),	\
			       VOIDmode)))			\
   || (GET_CODE (X) == SYMBOL_REF && SYMBOL_REF_FLAG (X)))


/* Allow SUBREG everywhere we allow REG.  This results in better code.  It
   also makes function inlining work when inline functions are called with
   arguments that are SUBREGs.  */

#define LEGITIMATE_BASE_REG_P(X)   \
  ((GET_CODE (X) == REG && REG_OK_FOR_BASE_P (X))	\
   || (GET_CODE (X) == SUBREG				\
       && GET_CODE (SUBREG_REG (X)) == REG		\
       && REG_OK_FOR_BASE_P (SUBREG_REG (X))))

#define LEGITIMATE_INDEX_REG_P(X) (     \
  ((GET_CODE (X) == REG && REG_OK_FOR_INDEX_P (X))	\
   || (GET_CODE (X) == SUBREG				\
       && GET_CODE (SUBREG_REG (X)) == REG		\
       && REG_OK_FOR_INDEX_P (SUBREG_REG (X)))) \
)

#ifdef NOT_NOW
													
#define LEGITIMATE_BASE_REG_P(X)   \
  (GET_CODE (X) == REG && REG_OK_FOR_BASE_P (X))

#define LEGITIMATE_INDEX_REG_P(X)      \
  (GET_CODE (X) == REG && REG_OK_FOR_INDEX_P (X))	

#endif

struct rtx_def;

/* see if this is index types  (r)  (n,r)  or (n,r1,r2) or (r1,r2) */
int IsRegisterIndex(struct rtx_def *op,int mode,int strict);
int IsLegitimateAddress(struct rtx_def *op,int mode,int strict);


/* Recognize any constant value that is a valid address.  */

#define CONSTANT_ADDRESS_P(X)  \
  (GET_CODE (X) == LABEL_REF || GET_CODE (X) == SYMBOL_REF		\
   || GET_CODE (X) == CONST_INT \
)

	
/* stack push and pop addressing -(sp) (sp)+ */
#define STACKPUSHPOP_P(X)  \
   ((GET_CODE (X) == PRE_DEC || GET_CODE (X) == POST_INC)		\
       && REGNO(XEXP (X, 0))==STACK_POINTER_REGNUM)


																

#define GO_IF_LEGITIMATE_ADDRESS(MODE,X, ADDR) {	\
    {if(CONSTANT_ADDRESS_P(X)) goto ADDR;}\
    {if(STACKPUSHPOP_P(X)) goto ADDR;}\
    {if(IsLegitimateAddress(X,MODE,REG_OK_IS_STRICT)) goto ADDR;}\
}


/* Don't call memory_address_noforce for the address to fetch
   the switch offset.  This address is ok as it stands (see above),
   but memory_address_noforce would alter it.  */
/*#define PIC_CASE_VECTOR_ADDRESS(index) index */

/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE and WIN are passed so that this macro can use
   GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this macro to do nothing.  It exists to recognize
   opportunities to optimize the output.

   For the 68000, we handle X+REG by loading X into a register R and
   using R+REG.  R will go in an address reg and indexing will be used.
   However, if REG is a broken-out memory address or multiplication,
   nothing needs to be done because REG can certainly go in an address reg.  */

#define COPY_ONCE(Y) if (!copied) { Y = copy_rtx (Y); copied = ch = 1; }


                               
                             
#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN){}

/* Go to LABEL if ADDR (a legitimate address expression)
   has an effect that depends on the machine mode it is used for. */


/* all addressing in ICVM is mode dependant */
#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL)	\
 goto LABEL



/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE SImode

/* Define this if the tablejump instruction expects the table
   to contain offsets from the address of the table.
   Do not define this if the table should contain absolute addresses.  */
#define CASE_VECTOR_PC_RELATIVE 1

/* Specify the tree operation to be used to convert reals to integers.  */
#define IMPLICIT_FIX_EXPR FIX_ROUND_EXPR

/* This is the kind of divide that is easiest to do in the general case.  */
#define EASY_DIV_EXPR TRUNC_DIV_EXPR

/* Define this as 1 if `char' should by default be signed; else as 0.  */
#define DEFAULT_SIGNED_CHAR 1

/* Don't cse the address of the function being compiled.  */
#define NO_RECURSIVE_FUNCTION_CSE

/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX 			8
#define MOVE_MAX_PIECES 	16
#define TARGET_MEM_FUNCTIONS


/* Define this if zero-extension is slow (more than one real instruction).  */
/* #define SLOW_ZERO_EXTEND */

/* Nonzero if access to memory by bytes is slow and undesirable.  */
#define SLOW_BYTE_ACCESS 0

/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */

#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 0



/* We assume that the store-condition-codes instructions store 0 for false
   and some other value for true.  This is the value stored for true.  */

#define STORE_FLAG_VALUE 1


/* When a prototype says `char' or `short', really pass an `int'.  */
#define PROMOTE_PROTOTYPES




/* Define this macro if the promotion described by `PROMOTE_MODE'
   should also be done for outgoing function arguments.  */
								
/*#define PROMOTE_FUNCTION_ARGS */

/* Likewise, if the function return value is promoted.
   If defined, FUNCTION_VALUE must perform the same promotions done by
   PROMOTE_MODE.  */
								
/* #define PROMOTE_FUNCTION_RETURN */

/* Define if loading in MODE, an integral mode narrower than BITS_PER_WORD
   will either zero-extend or sign-extend.  The value of this macro should
   be the code that says which one of the two operations is implicitly
   done, NIL if none.  */
/* #define LOAD_EXTEND_OP(MODE) SIGN_EXTEND */



/* Define if operations between registers always perform the operation
   on the full register even if a narrower mode is specified.  */

/* #define WORD_REGISTER_OPERATIONS */


/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode SImode

/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */

#define FUNCTION_MODE QImode


/* Tell final.c how to eliminate redundant test instructions.  */

/* Here we define machine-dependent flags and fields in cc_status
   (see `conditions.h').  */

/* Set if the cc value is actually in the 68881, so a floating point
   conditional branch must be output.  */
#define CC_IN_68881 04000

/* Store in cc_status the expressions that the condition codes will
   describe after execution of an instruction whose pattern is EXP.
   Do not alter them if the instruction would not alter the cc's.  */

/* On the 68000, all the insns to store in an address register fail to
   set the cc's.  However, in some cases these instructions can make it
   possibly invalid to use the saved cc's.  In those cases we clear out
   some or all of the saved cc's so they won't be used.  */

#define NOTICE_UPDATE_CC(EXP,INSN) 

#define OUTPUT_JUMP(NORMAL, FLOAT, NO_OV)  \
{ if (cc_prev_status.flags & CC_IN_68881)			\
    return FLOAT;						\
  if (cc_prev_status.flags & CC_NO_OVERFLOW)			\
    return NO_OV;						\
  return NORMAL; }

/* Control the assembler format that we output.  */

/* Output at beginning of assembler file.  */

#define ASM_FILE_START(FILE)	\
  fprintf (FILE, "#NO_APP\n");

/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */

#define ASM_APP_ON "#APP\n"

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */

#define ASM_APP_OFF "#NO_APP\n"

/* Output before read-only data.  */

#define TEXT_SECTION_ASM_OP ".text"

#undef READONLY_DATA_SECTION
#define READONLY_DATA_SECTION  rdata_section 


#define RDATA_SECTION_ASM_OP ".rdata"

/* Output before writable data.  */

#define DATA_SECTION_ASM_OP ".data"

#undef CONST_SECTION_ASM_OP
#define CONST_SECTION_ASM_OP	".data"


#define CTORS_SECTION_ASM_OP 		"\t.ctors"
#define DTORS_SECTION_ASM_OP 		"\t.dtors"
#define EXTRA_SECTIONS 			in_ctors, in_dtors, in_rdata
#define EXTRA_SECTION_FUNCTIONS					\
void								\
ctors_section()							\
{								\
  if (in_section != in_ctors)					\
    {								\
      fprintf (asm_out_file, "%s\n", CTORS_SECTION_ASM_OP);	\
      in_section = in_ctors;					\
    }								\
}								\
void								\
dtors_section()							\
{								\
  if (in_section != in_dtors)					\
    {								\
      fprintf (asm_out_file, "%s\n", DTORS_SECTION_ASM_OP);	\
      in_section = in_dtors;					\
    }								\
}\
void									\
rdata_section ()							\
{									\
  if (in_section != in_rdata)						\
    {									\
      fprintf (asm_out_file, "%s\n", RDATA_SECTION_ASM_OP);		\
      in_section = in_rdata;						\
    }									\
}




/* Here are four prefixes that are used by asm_fprintf to
   facilitate customization for alternate assembler syntaxes.
   Machines with no likelihood of an alternate syntax need not
   define these and need not use asm_fprintf.  */

/* The prefix for register names.  Note that REGISTER_NAMES
   is supposed to include this prefix.  */

#define REGISTER_PREFIX "%"

/* The prefix for local labels.  You should be able to define this as
   an empty string, or any arbitrary string (such as ".", ".L%", etc)
   without having to make any other changes to account for the specific
   definition.  Note it is a string literal, not interpreted by printf
   and friends. */

#define LOCAL_LABEL_PREFIX "."

/* The prefix to add to user-visible assembler symbols.  */

#define USER_LABEL_PREFIX "_"


/* The prefix to add to user-visible assembler symbols. */

#define ASM_COMMENT_START "|"

/* The prefix for immediate operands.  */

#define IMMEDIATE_PREFIX "#"

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */

#define REGISTER_NAMES  {\
	"%r0","%r1","%r2","%r3","%???","%???","%???","%???",\
	"%???","%???","%???","%???","%q0","%q1","%fp","%sp",\
	"%fp0","%fp1","%fp2","%???","%???","%???","%???","%???",\
	"%???","%???","%???","%???","%???","%???","%???","%???",\
	"(-20,%fp)","(-24,%fp)","(-44,%fp)","(-48,%fp)",\
	"(-124,%fp)","(-128,%fp)","(-132,%fp)","(-136,%fp)",\
	"(-244,%fp)","(-248,%fp)","(-252,%fp)","(-256,%fp)",\
	"(-260,%fp)","(-264,%fp)","(-268,%fp)","(-272,%fp)",\
	"(-152,%fp)","(-160,%fp)","(-168,%fp)","(-176,%fp)",\
	"(-280,%fp)","(-288,%fp)","(-296,%fp)","(-304,%fp)",\
	"(-208,%fp)","(-240,%fp)","(-352,%fp)","(-384,%fp)",\
	"(-416,%fp)","(-448,%fp)","(-480,%fp)","(-512,%fp)",\
	"(-66,%fp)","(-68,%fp)","(-70,%fp)","(-72,%fp)",\
	"(-514,%fp)","(-516,%fp)","(-518,%fp)","(-520,%fp)",\
	"(-137,%fp)","(-138,%fp)","(-139,%fp)","(-140,%fp)",\
	"(-521,%fp)","(-522,%fp)","(-523,%fp)","(-524,%fp)",\
	"(-52,%fp)","(-56,%fp)","(-60,%fp)","(-64,%fp)",\
	"(-308,%fp)","(-312,%fp)","(-316,%fp)","(-320,%fp)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	"(???)","(???)","(???)","(???)",\
	}

#define STARTING_FRAME_OFFSET  (-528)

/* **** PRIVATE FRAME MAPPINGS **** */
#define rSH_globalptr -4
#define rSH_r1 -8
#define rSH_r2 -12
#define rSH_r3 -16
#define rSH_fp1 -32
#define rSH_fp2 -40
#define rSH_q1 -104
#define rSH_reserved1 -108
#define rSH_savepc -112
#define rSH_return2 -116
#define rSH_return3 -120

/* Before the prologue, RA is at 0(%sp).  */
#define INCOMING_RETURN_ADDR_RTX \
  gen_rtx (MEM, VOIDmode, gen_rtx (REG, VOIDmode, STACK_POINTER_REGNUM))

#define PREFERRED_DEBUGGING_TYPE DBX_DEBUG

/* We must not use the DBX register numbers for the DWARF 2 CFA column
   numbers because that maps to numbers beyond FIRST_PSEUDO_REGISTER.
   Instead use the identity mapping.  */
#define DWARF_FRAME_REGNUM(REG) REG

/* Before the prologue, the top of the frame is at 4(%sp).  */
#define INCOMING_FRAME_SP_OFFSET 4

/* This is how to output the definition of a user-level label named NAME,
   such as the label on a static function or variable NAME.  */

#define ASM_OUTPUT_LABEL(FILE,NAME)	\
  do { assemble_name (FILE, NAME); fputs (":\n", FILE); } while (0)

/* This is how to output a command to make the user-level label named NAME
   defined for reference from other files.  */

#define GLOBAL_ASM_OP ".globl"
#define ASM_GLOBALIZE_LABEL(FILE,NAME)	\
  do { fprintf (FILE, "%s ", GLOBAL_ASM_OP);		\
       assemble_name (FILE, NAME);			\
       fputs ("\n", FILE);} while (0)

/* This is how to output a reference to a user-level label named NAME.
   `assemble_name' uses this.  */

#define ASM_OUTPUT_LABELREF(FILE,NAME)	\
  asm_fprintf (FILE, "%0U%s", NAME)

/* This is how to output an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.  */

#define ASM_OUTPUT_INTERNAL_LABEL(FILE,PREFIX,NUM)	\
  asm_fprintf (FILE, "%0L%s%d:\n", PREFIX, NUM)

/* This is how to store into the string LABEL
   the symbol_ref name of an internal numbered label where
   PREFIX is the class of label and NUM is the number within the class.
   This is suitable for output with `assemble_name'.  */

#define ASM_GENERATE_INTERNAL_LABEL(LABEL,PREFIX,NUM)	\
  sprintf (LABEL, "*%s%s%d", LOCAL_LABEL_PREFIX, PREFIX, NUM)

/* This is how to output a `long double' extended real constant. */
  
#define ASM_OUTPUT_DOUBLE(FILE,VALUE)	\
do {\
 	unsigned long long n;\
	unsigned long l[2];	\
	char *s=(char *)l;\
	char c;\
     REAL_VALUE_TO_TARGET_DOUBLE (VALUE, l);\
	 n=0;\
	if(BYTES_BIG_ENDIAN){\
		 n=l[0];\
		 n<<=32;\
		 n|=l[1];\
 	}else{\
		 n=l[1];\
		 n<<=32;\
		 n|=l[0];\
	}\
   fprintf (FILE, "\t.quad %qd\n",n);		\
   } while (0)



#define ASM_OUTPUT_LONG_DOUBLE ASM_OUTPUT_DOUBLE
  

/* This is how to output an assembler line defining a `float' constant.  */

#define ASM_OUTPUT_FLOAT(FILE,VALUE)			\
do { long l;						\
     REAL_VALUE_TO_TARGET_SINGLE (VALUE, l);		\
     if (sizeof (int) == sizeof (long))			\
       fprintf (FILE, "\t.long 0x%x\n", l);		\
     else						\
       fprintf (FILE, "\t.long 0x%lx\n", l);		\
   } while (0)

/* This is how to output an assembler line defining an `int' constant.  */

#define ASM_OUTPUT_DOUBLE_INT(FILE,VALUE)  \
( fprintf (FILE, "\t.quad "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

#define ASM_OUTPUT_INT(FILE,VALUE)  \
( fprintf (FILE, "\t.long "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

/* Likewise for `char' and `short' constants.  */

#define ASM_OUTPUT_SHORT(FILE,VALUE)  \
( fprintf (FILE, "\t.word "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

#define ASM_OUTPUT_CHAR(FILE,VALUE)  \
( fprintf (FILE, "\t.byte "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

/* This is how to output an assembler line for a numeric constant byte.  */

#define ASM_OUTPUT_BYTE(FILE,VALUE)  \
  fprintf (FILE, "\t.byte 0x%x\n", (VALUE))

/* This is how to output an insn to push a register on the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_PUSH(FILE,REGNO)  \
  asm_fprintf (FILE, "\tmovel %s,%Rsp@-\n", reg_names[REGNO])

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_POP(FILE,REGNO)  \
  asm_fprintf (FILE, "\tmovel %Rsp@+,%s\n", reg_names[REGNO])

/* This is how to output an element of a case-vector that is absolute.
   (The 68000 does not use such vectors,
   but we must define this macro anyway.)  */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  \
  asm_fprintf (FILE, "\t.long %LL%d\n", VALUE)

/* This is how to output an element of a case-vector that is relative.  */

#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE,BODY,VALUE, REL)  \
  asm_fprintf (FILE, "\t.long %LL%d-%LL%d\n", VALUE, REL)

#define ASM_OUTPUT_CONSTRUCTOR(FILE,NAME) \
   do { ctors_section();  asm_fprintf((FILE),"\t.long\t%U%s\n", (NAME)); } while (0)

#define ASM_OUTPUT_DESTRUCTOR(FILE,NAME) \
   do {  dtors_section();  asm_fprintf((FILE),"\t.long\t%U%s\n", (NAME)); } while (0)



/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**LOG bytes.  */


/* We don't have a way to align to more than a two-byte boundary, so do the
   best we can and don't complain.  */
#define ASM_OUTPUT_ALIGN(FILE,LOG){	\
  if ((LOG) == 1)			\
    fprintf (FILE, "\t.even\n");\
  else if ((LOG) > 1)			\
    fprintf (FILE, "\t.align %d\n",1<<(LOG));\
}

#define ASM_OUTPUT_SKIP(FILE,SIZE)  \
  fprintf (FILE, "\t.skip %u\n", (SIZE))

/* This says how to output an assembler line
   to define a global common symbol.  */


#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)  \
( fputs (".comm ", (FILE)),			\
  assemble_name ((FILE), (NAME)),		\
  fprintf ((FILE), ",%u\n", (ROUNDED)))



#ifdef NOT_NOW

#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)  \
( ASM_OUTPUT_LOCAL(FILE,NAME,SIZE,ROUNDED),\
  fputs (".globl ", (FILE)),			\
  assemble_name ((FILE), (NAME)),fprintf((FILE),"\n"))

#endif



/* This says how to output an assembler line
   to define a local common symbol.  */

#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)  \
( fputs (".lcomm ", (FILE)),			\
  assemble_name ((FILE), (NAME)),		\
  fprintf ((FILE), ",%u\n", (ROUNDED)))

/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */

#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)	\
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10),	\
  sprintf ((OUTPUT), "%s.%d", (NAME), (LABELNO)))

/* Define the parentheses used to group arithmetic operations
   in assembler code.  */

#define ASM_OPEN_PAREN "("
#define ASM_CLOSE_PAREN ")"

/* Define results of standard character escape sequences.  */
#define TARGET_BELL 007
#define TARGET_BS 010
#define TARGET_TAB 011
#define TARGET_NEWLINE 012
#define TARGET_VT 013
#define TARGET_FF 014
#define TARGET_CR 015

/* Output a float value (represented as a C double) as an immediate operand.
   This macro is a 68k-specific macro.  */

#define ASM_OUTPUT_FLOAT_OPERAND(CODE,FILE,VALUE)		\
 do {								\
          char dstr[30];					\
          REAL_VALUE_TO_DECIMAL (VALUE, "%.9g", dstr);		\
          asm_fprintf ((FILE), "%I0r%s", dstr);			\
     } while (0)

/* Output a double value (represented as a C double) as an immediate operand.
   This macro is a 68k-specific macro.  */
#define ASM_OUTPUT_DOUBLE_OPERAND(FILE,VALUE)				\
 do { char dstr[30];							\
      REAL_VALUE_TO_DECIMAL (VALUE, "%.20g", dstr);			\
      asm_fprintf (FILE, "%I0r%s", dstr);				\
    } while (0)

/* Note, long double immediate operands are not actually
   generated by icvm.md.  */
#define ASM_OUTPUT_LONG_DOUBLE_OPERAND(FILE,VALUE)			\
 do { char dstr[30];							\
      REAL_VALUE_TO_DECIMAL (VALUE, "%.20g", dstr);			\
      asm_fprintf (FILE, "%I0r%s", dstr);				\
    } while (0)

/* Print operand X (an rtx) in assembler syntax to file FILE.
   CODE is a letter or dot (`z' in `%z0') or 0 if no letter was specified.
   For `%' followed by punctuation, CODE is the punctuation and X is null.

   On the 68000, we use several CODE characters:
   '.' for dot needed in Motorola-style opcode names.
   '-' for an operand pushing on the stack:
       sp@-, -(sp) or -(%sp) depending on the style of syntax.
   '+' for an operand pushing on the stack:
       sp@+, (sp)+ or (%sp)+ depending on the style of syntax.
   '@' for a reference to the top word on the stack:
       sp@, (sp) or (%sp) depending on the style of syntax.
   '#' for an immediate operand prefix (# in MIT and Motorola syntax
       but & in SGS syntax).
   '!' for the fpcr register (used in some float-to-fixed conversions).
   '$' for the letter `s' in an op code, but only on the 68040.
   '&' for the letter `d' in an op code, but only on the 68040.
   '/' for register prefix needed by longlong.h.

   'b' for byte insn (no effect, on the Sun; this is for the ISI).
   'd' to force memory addressing to be absolute, not relative.
   'f' for float insn (print a CONST_DOUBLE as a float rather than in hex)
   'w' for FPA insn (print a CONST_DOUBLE as a SunFPA constant rather
       than directly).  Second part of 'y' below.
   'x' for float insn (print a CONST_DOUBLE as a float rather than in hex),
       or print pair of registers as rx:ry.
   'y' for a FPA insn (print pair of registers as rx:ry).  This also outputs
       CONST_DOUBLE's as SunFPA constant RAM registers if
       possible, so it should not be used except for the SunFPA. */

#define PRINT_OPERAND_PUNCT_VALID_P(CODE)				\
  ((CODE) == '.' || (CODE) == '#' || (CODE) == '-'			\
   || (CODE) == '+' || (CODE) == '@' || (CODE) == '!'			\
   || (CODE) == '$' || (CODE) == '&' || (CODE) == '/')

/* A C compound statement to output to stdio stream STREAM the
   assembler syntax for an instruction operand X.  X is an RTL
   expression.

   CODE is a value that can be used to specify one of several ways
   of printing the operand.  It is used when identical operands
   must be printed differently depending on the context.  CODE
   comes from the `%' specification that was used to request
   printing of the operand.  If the specification was just `%DIGIT'
   then CODE is 0; if the specification was `%LTR DIGIT' then CODE
   is the ASCII code for LTR.

   If X is a register, this macro should print the register's name.
   The names can be found in an array `reg_names' whose type is
   `char *[]'.  `reg_names' is initialized from `REGISTER_NAMES'.

   When the machine description has a specification `%PUNCT' (a `%'
   followed by a punctuation character), this macro is called with
   a null pointer for X and the punctuation character for CODE.

   See icvm.c for the icvm specific codes.  */

#define PRINT_OPERAND(FILE, X, CODE) print_operand (FILE, X, CODE)

/* A C compound statement to output to stdio stream STREAM the
   assembler syntax for an instruction operand that is a memory
   reference whose address is ADDR.  ADDR is an RTL expression.

   On some machines, the syntax for a symbolic address depends on
   the section that the address refers to.  On these machines,
   define the macro `ENCODE_SECTION_INFO' to store the information
   into the `symbol_ref', and then check for it here.  */

#define PRINT_OPERAND_ADDRESS(FILE, ADDR) print_operand_address (FILE, ADDR)


/* Definitions for generating bytecode */

/* Just so it's known this target is supported by the bytecode generator.
   If this define isn't found anywhere in the target config files, then
   dummy stubs are supplied by bytecode.h, and any attempt to use
   -fbytecode will result in an error message. */

#define TARGET_SUPPORTS_BYTECODE


/* don't think the alignments below are used anymore */

/* Minimal segment alignment within sections is 4 units. */
#define MACHINE_SEG_ALIGN 4

/* Integer alignment is 4 units. */
#define INT_ALIGN 4

/* Pointer alignment is 4 units. */
#define PTR_ALIGN 4

/* Global symbols begin with `_' */
#define NAMES_HAVE_UNDERSCORES

/* BC_xxx below are similar to their ASM_xxx counterparts above. */
#define BC_GLOBALIZE_LABEL(FP, NAME) bc_globalize_label(NAME)

#define BC_OUTPUT_COMMON(FP, NAME, SIZE, ROUNDED) \
  do { bc_emit_common(NAME, ROUNDED); bc_globalize_label(NAME); } while (0)

#define BC_OUTPUT_BSS(FP, NAME, SIZE, ROUNDED) \
  do { bc_data (); bc_emit_labeldef(NAME); bc_emit_skip (SIZE); } while (0)

#define BC_OUTPUT_LOCAL(FP, NAME, SIZE, ROUNDED) \
  bc_emit_common(NAME, ROUNDED)

#define BC_OUTPUT_ALIGN(FP, ALIGN) bc_align(ALIGN)

#define BC_OUTPUT_LABEL(FP, NAME) bc_emit_labeldef(NAME)

#define BC_OUTPUT_SKIP(FP, SIZE) bc_emit_skip(SIZE)

#define BC_OUTPUT_LABELREF(FP, NAME)					      \
  do {									      \
    char *foo = (char *) xmalloc(strlen(NAME) + 2);			      \
    strcpy(foo, "_");							      \
    strcat(foo, NAME);							      \
    bc_emit_labelref (foo);						      \
    free (foo);								      \
  } while (0)

#define BC_OUTPUT_FLOAT(FP, VAL)					      \
  do {									      \
    float F = VAL;							      \
    bc_emit ((char *) &F, sizeof F);					      \
  } while (0)

#define BC_OUTPUT_DOUBLE(FP, VAL)					      \
  do {									      \
    double D = VAL;							      \
    bc_emit ((char *) &D, sizeof D);					      \
  } while (0)

#define BC_OUTPUT_BYTE(FP, VAL)					      \
  do {									      \
    char C = VAL;							      \
    bc_emit (&C, 1);							      \
  } while (0)


#define BC_OUTPUT_FILE ASM_OUTPUT_FILE
#define BC_OUTPUT_ASCII ASM_OUTPUT_ASCII
#define BC_OUTPUT_IDENT ASM_OUTPUT_IDENT

/* Same as XSTR, but for bytecode */
#define BCXSTR(RTX)  ((RTX)->bc_label)


/* Flush bytecode buffer onto file */
#define BC_WRITE_FILE(FP) \
{ \
  fprintf (FP, ".text\n"); \
  bc_seg_write (bc_text_seg, FP); \
  fprintf(FP, "\n.data\n"); \
  bc_seg_write (bc_data_seg, FP); \
  bc_sym_write (FP);  /* do .globl, .bss, etc. */ \
}

/* Write one symbol */
#define BC_WRITE_SEGSYM(SEGSYM, FP) \
{ \
  prsym (FP, (SEGSYM)->sym->name); \
  fprintf (FP, ":\n"); \
}


/* Write one reloc entry */
#define BC_WRITE_RELOC_ENTRY(SEGRELOC, FP, OFFSET) \
{ \
  fprintf (FP, "\t.long "); \
  prsym (FP, (SEGRELOC)->sym->name); \
  fprintf (FP, " + %d\n", OFFSET); \
}

/* Start new line of bytecodes */
#define BC_START_BYTECODE_LINE(FP) \
{ \
  fprintf (FP, "\t.byte"); \
}

/* Write one bytecode */
#define BC_WRITE_BYTECODE(SEP, VAL, FP) \
{ \
  fprintf (FP, "%c0x%02X", (SEP), (VAL) & 0xff); \
}

/* Write one bytecode RTL entry */
#define BC_WRITE_RTL(R, FP) \
{ \
  fprintf (FP, "%s+%d/0x%08X\n", (R)->label, (R)->offset, (R)->bc_label); \
}


/* Emit function entry trampoline */
#define BC_EMIT_TRAMPOLINE(TRAMPSEG, CALLINFO) \
{ \
  short insn; \
 \
  /* Push a reference to the callinfo structure.  */ \
  insn = 0x4879;		/* pea xxx.L */ \
  seg_data (TRAMPSEG, (char *) &insn, sizeof insn); \
  seg_refsym (TRAMPSEG, CALLINFO, 0); \
 \
  /* Call __interp, pop arguments, and return.  */ \
  insn = 0x4eb9;		/* jsr xxx.L  */ \
  seg_data (TRAMPSEG, (char *) &insn, sizeof insn); \
  seg_refsym (TRAMPSEG, "__callint", 0); \
  insn = 0x588f;		/* addql #4, sp */ \
  seg_data (TRAMPSEG, (char *) &insn, sizeof insn); \
  insn = 0x4e75;		/* rts */ \
  seg_data (TRAMPSEG, (char *) &insn, sizeof insn); \
}



#if 0
#define VALIDATE_STACK()  if (stack_depth < 0) abort ();
#else
#if 0
#define VALIDATE_STACK() \
  fprintf (stderr, " %%%d%%", stack_depth);
#endif
#endif

/* Define functions defined in aux-output.c and used in templates.  */

extern struct rtx_def  *r_compare_op0, *r_compare_op1;



/* output_general flags */

#define IG_CANSWAP	(1<<0) /* ops 1 & 2 can be swapped if needed */
#define IG_TRIPLE   (1<<1) /* this is a triple op */
#define IG_OPTADD	(1<<2) /* optimize adds to incs and decs if possible */
							


extern char *output_moveit();
char *output_N2doublestackop(struct rtx_def **operands,char *move,char *add);
char *output_general(struct rtx_def **operands,char *move,char *adds,char *addqs, int flags);
char *output_generaltriplestack(struct rtx_def **operands,char *move,char *adds,char *addqs, int flags);
char *output_N2triplestackop(struct rtx_def **operands,char *move,char *adds,char *addqs, int flags);
char *output_N1triplestackop(struct rtx_def **operands,char *move,char *adds,char *addqs, int flags);
char *output_generalorstack(struct rtx_def **operands,char *move,char *adds,char *addqs, int flags);
char *output_N1N2triplestackop(struct rtx_def **operands,char *move,char *adds,char *addqs, int flags);
char *output_unary(struct rtx_def **operands,char *adds);
char *output_doublestackop(struct rtx_def **operands,char *moves,char *adds);
					

int general_simpleoperand(struct rtx_def *op, int mode);
int nonimmediate_simpleoperand(struct rtx_def *op, int mode);
int general_noindexoperand(struct rtx_def *op, int mode);
int nonimmediate_noindexoperand(struct rtx_def *op, int mode);
																																						
struct rtx_def *force_simpleoperand(struct rtx_def *x);


/* Variables in icvm.c */
extern char *icvm_align_loops_string;
extern char *icvm_align_jumps_string;
extern char *icvm_align_funcs_string;
extern int icvm_align_loops;
extern int icvm_align_jumps;
extern int icvm_align_funcs;
extern int icvm_last_compare_had_fp_operands;
extern void icvm_addsyscall(char *funcname,int operand,char *preamble,char *postop);
extern char *buildsyscall(struct rtx_def *operand);

char *buildcbcc(struct rtx_def *operands[],int op);

struct rtx_def *ConvertToMode(int mode,struct rtx_def *from,int unsignedp);





/* Currently, JUMP_TABLES_IN_TEXT_SECTION must be defined in order to
   keep switch tables in the text section.  */
  
/* keep jump tables in text section in ICVM   */
#define JUMP_TABLES_IN_TEXT_SECTION 1

#define ASM_RETURN_CASE_JUMP			\
  do {						\
    if (TARGET_5200)				\
      return "ext%.l %0\n\tjmp %%pc@(2,%0:l)";	\
    else					\
      return "jmp %%pc@(2,%0:w)";		\
  } while (0)


/* setjmp stuff */

#ifndef  DONT_USE_BUILTIN_SETJMP
#define DONT_USE_BUILTIN_SETJMP
#endif


/* overkill on setjmp buffer size */
#define JMP_BUF_SIZE 256	
									


/*
Local variables:
version-control: t
End:
*/

