
/* Subroutines for insn-output.c for Motorola 68000 family.
   Copyright (C) 1987, 93, 94, 95, 96, 1997 Free Software Foundation, Inc.

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


#define REG_OK_STRICT


/* Some output-actions in icvm.md need these.  */
#include "config.h"
#include <stdio.h>
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "insn-flags.h"
#include "output.h"
#include "insn-attr.h"
#include "tree.h"

/* Needed for use_return_insn.  */
#include "flags.h"
						

rtx r_compare_op0, r_compare_op1;


/* This flag is used to communicate between movhi and ASM_OUTPUT_CASE_END,
   if SGS_SWITCH_TABLE.  */
int switch_table_difference_label_flag;

static rtx find_addr_reg ();
rtx legitimize_pic_address ();


/* Alignment to use for loops and jumps */
/* Specify power of two alignment used for loops. */
char *icvm_align_loops_string;
/* Specify power of two alignment used for non-loop jumps. */
char *icvm_align_jumps_string;
/* Specify power of two alignment used for functions. */
char *icvm_align_funcs_string;

/* Specify power of two alignment used for loops. */
int icvm_align_loops;
/* Specify power of two alignment used for non-loop jumps. */
int icvm_align_jumps;
/* Specify power of two alignment used for functions. */
int icvm_align_funcs;

/* Nonzero if the last compare/test insn had FP operands.  The
   sCC expanders peek at this to determine what to do for the
   68060, which has no fsCC instructions.  */
int icvm_last_compare_had_fp_operands;


/* our own convert that protects the que first. Fixes endian problems */

struct rtx_def *ConvertToMode(int mode,struct rtx_def *from,
			int unsignedp){
  	from=protect_from_queue (from, 0);
	if(IsICVMNopTrunc(from,mode))
		return gen_lowpart(mode,from);
	return convert_to_mode(mode,from, unsignedp);
}



/* Sometimes certain combinations of command options do not make
   sense on a particular target machine.  You can define a macro
   `OVERRIDE_OPTIONS' to take account of this.  This macro, if
   defined, is executed once just after all the command options have
   been parsed.

   Don't use this macro to turn on various extra optimizations for
   `-O'.  That is what `OPTIMIZATION_OPTIONS' is for.  */

void
override_options ()
{
  int def_align;

  def_align = 1;

  /* Validate -malign-loops= value, or provide default */
  if (icvm_align_loops_string)
    {
      icvm_align_loops = atoi (icvm_align_loops_string);
      if (icvm_align_loops < 1 || icvm_align_loops > MAX_CODE_ALIGN)
	fatal ("-malign-loops=%d is not between 1 and %d",
	       icvm_align_loops, MAX_CODE_ALIGN);
    }
  else
    icvm_align_loops = def_align;

  /* Validate -malign-jumps= value, or provide default */
  if (icvm_align_jumps_string)
    {
      icvm_align_jumps = atoi (icvm_align_jumps_string);
      if (icvm_align_jumps < 1 || icvm_align_jumps > MAX_CODE_ALIGN)
	fatal ("-malign-jumps=%d is not between 1 and %d",
	       icvm_align_jumps, MAX_CODE_ALIGN);
    }
  else
    icvm_align_jumps = def_align;

  /* Validate -malign-functions= value, or provide default */
  if (icvm_align_funcs_string)
    {
      icvm_align_funcs = atoi (icvm_align_funcs_string);
      if (icvm_align_funcs < 1 || icvm_align_funcs > MAX_CODE_ALIGN)
	fatal ("-malign-functions=%d is not between 1 and %d",
	       icvm_align_funcs, MAX_CODE_ALIGN);
    }
  else
    icvm_align_funcs = def_align;
}

/* Emit a (use pic_offset_table_rtx) if we used PIC relocation in the 
   function at any time during the compilation process.  In the future 
   we should try and eliminate the USE if we can easily determine that 
   all PIC references were deleted from the current function.  That would 
   save an address register */
   
void
finalize_pic ()
{
  if (flag_pic && current_function_uses_pic_offset_table)
    {
      rtx insn = gen_rtx (USE, VOIDmode, pic_offset_table_rtx);
      emit_insn_after (insn, get_insns ());
      emit_insn (insn);
    }
}


int ICVM_SavedRegsSize(void){
  int sz=0;
  int regno;	
  for (regno =0; regno < 4; regno++){	
    if (regs_ever_live[regno] && ! call_used_regs[regno])	
      	sz+=4;;
  }
  for (regno =12; regno < 14; regno++){	
    if (regs_ever_live[regno] && ! call_used_regs[regno])	
      	sz+=8;;
  }
  for (regno =16; regno < 20; regno++){	
    if (regs_ever_live[regno] && ! call_used_regs[regno])	
      	sz+=8;;
  }
  return sz;
}

#ifdef NOT_NOW
int ICVM_SavedRegsMask(void){
  int mask=0;
  int regno;	
  for (regno =0; regno < 4; regno++){	
    if (regs_ever_live[regno] && ! call_used_regs[regno])	
      	mask|=(1<<regno);;
  }
  for (regno =12; regno < 14; regno++){	
    if (regs_ever_live[regno] && ! call_used_regs[regno])	
      	mask|=(1<<regno);;
  }
  for (regno =16; regno < 20; regno++){	
    if (regs_ever_live[regno] && ! call_used_regs[regno])	
      	mask|=(1<<regno);;
  }
  return mask;
}

#endif

int ICVM_NumStackRegsUsed(void){
  int regno;	
  int numregs=0;
  for (regno = LIDX_LO; regno <= LIDX_HI; regno++){	
/*    if (regs_ever_live[regno] && ! call_used_regs[regno])	*/
    if (regs_ever_live[regno])	
      numregs=(regno-LIDX_LO)+1;
  }
  return numregs;
}



/* This function generates the assembly code for function entry.
   STREAM is a stdio stream to output the code to.
   SIZE is an int: how many units of temporary storage to allocate.
   Refer to the array `regs_ever_live' to determine which registers
   to save; `regs_ever_live[I]' is nonzero if register number I
   is ever used in the function.  This function is responsible for
   knowing which registers should not be saved even if used.  */


/* Note that the order of the bit mask for fmovem is the opposite
   of the order for movem!  */


void
output_function_prologue (stream, size)
     FILE *stream;
     int size;
{
  register int regno;
  register int mask = 0;
  int num_saved_regs = 0;
  extern char call_used_regs[];
/*  int fsize = (size + 3) & -4; */
    int fsize;
  int cfa_offset = INCOMING_FRAME_SP_OFFSET, cfa_store_offset = cfa_offset;
  INITIAL_FRAME_POINTER_OFFSET(fsize);
  if(fsize>32767 || fsize<-32767)	
	  asm_fprintf (stream, "\tframel %0I%d\n",-fsize);
  else
	  asm_fprintf (stream, "\tframew %0I%d\n",-fsize);
  if (dwarf2out_do_frame ())
	{
	  char *l = dwarf2out_cfi_label ();

	  cfa_store_offset += 4;
	  cfa_offset = cfa_store_offset;
	  dwarf2out_def_cfa (l, FRAME_POINTER_REGNUM, cfa_offset);
	  dwarf2out_reg_save (l, FRAME_POINTER_REGNUM, -cfa_store_offset);
	  cfa_store_offset += fsize;
	}
}

/* Return true if this function's epilogue can be output as RTL.  */

int
use_return_insn ()
{
  int regno;

  if (!reload_completed || frame_pointer_needed || get_frame_size () != 0)
    return 0;
  
  /* Copied from output_function_epilogue ().  We should probably create a
     separate layout routine to perform the common work.  */
  
  for (regno = 0 ; regno < FIRST_PSEUDO_REGISTER ; regno++)
    if (regs_ever_live[regno] && ! call_used_regs[regno])
      return 0;
  
  return 1;
}

/* This function generates the assembly code for function exit,
   on machines that need it.  Args are same as for FUNCTION_PROLOGUE.

   The function epilogue should not depend on the current stack pointer!
   It should use the frame pointer only, if there is a frame pointer.
   This is mandatory because of alloca; we also take advantage of it to
   omit stack adjustments before returning.  */

void
output_function_epilogue (stream, size)
     FILE *stream;
     int size;
{
  register int regno;
  register int mask, fmask;
  register int nregs;
  int offset, foffset, fpoffset;
  extern char call_used_regs[];
  int fsize = (size + 3) & -4;
  int big = 0;
  rtx insn = get_last_insn ();
  int restore_from_sp = 0;
  
  /* If the last insn was a BARRIER, we don't have to write any code.  */
  if (GET_CODE (insn) == NOTE)
    insn = prev_nonnote_insn (insn);
  if (insn && GET_CODE (insn) == BARRIER)
    {
      /* Output just a no-op so that debuggers don't get confused
	 about which function the pc is in at this address.  */
      asm_fprintf (stream, "\tnop\n");
      return;
    }

#ifdef FUNCTION_BLOCK_PROFILER_EXIT
  if (profile_block_flag == 2)
    {
      FUNCTION_BLOCK_PROFILER_EXIT (stream);
    }
#endif

#ifdef FUNCTION_EXTRA_EPILOGUE
  FUNCTION_EXTRA_EPILOGUE (stream, size);
#endif
    fprintf (stream, "\trtsframe\n");
}

/* Similar to general_operand, but exclude stack_pointer_rtx.  */

int
not_sp_operand (op, mode)
     register rtx op;
     enum machine_mode mode;
{
  return op != stack_pointer_rtx && general_operand (op, mode);
}


/* Returns 1 if OP is either a symbol reference or a sum of a symbol
   reference and a constant.  */

int
symbolic_operand (op, mode)
     register rtx op;
     enum machine_mode mode;
{
  switch (GET_CODE (op))
    {
    case SYMBOL_REF:
    case LABEL_REF:
      return 1;

    case CONST:
      op = XEXP (op, 0);
      return ((GET_CODE (XEXP (op, 0)) == SYMBOL_REF
	       || GET_CODE (XEXP (op, 0)) == LABEL_REF)
	      && GET_CODE (XEXP (op, 1)) == CONST_INT);

#if 0 /* Deleted, with corresponding change in icvm.h,
	 so as to fit the specs.  No CONST_DOUBLE is ever symbolic.  */
    case CONST_DOUBLE:
      return GET_MODE (op) == mode;
#endif

    default:
      return 0;
    }
}


/* for compiler */
const_int_cost (constant)
     rtx constant;
{
    return 0;
}





/* This code has been fixed for cross-compilation. */
  
static int inited_68881_table = 0;

char *strings_68881[7] = {
  "0.0",
  "1.0",
  "10.0",
  "100.0",
  "10000.0",
  "1e8",
  "1e16"
  };

int codes_68881[7] = {
  0x0f,
  0x32,
  0x33,
  0x34,
  0x35,
  0x36,
  0x37
  };

REAL_VALUE_TYPE values_68881[7];

/* Set up values_68881 array by converting the decimal values
   strings_68881 to binary.   */

void
init_68881_table ()
{
  int i;
  REAL_VALUE_TYPE r;
  enum machine_mode mode;

  mode = SFmode;
  for (i = 0; i < 7; i++)
    {
      if (i == 6)
        mode = DFmode;
      r = REAL_VALUE_ATOF (strings_68881[i], mode);
      values_68881[i] = r;
    }
  inited_68881_table = 1;
}

int
standard_68881_constant_p (x)
     rtx x;
{
  REAL_VALUE_TYPE r;
  int i;
  enum machine_mode mode;

#ifdef NO_ASM_FMOVECR
  return 0;
#endif

  /* fmovecr must be emulated on the 68040, so it shouldn't be used at all. */
  if (TARGET_68040)
    return 0;

#ifndef REAL_ARITHMETIC
#if HOST_FLOAT_FORMAT != TARGET_FLOAT_FORMAT
  if (! flag_pretend_float)
    return 0;
#endif
#endif

  if (! inited_68881_table)
    init_68881_table ();

  REAL_VALUE_FROM_CONST_DOUBLE (r, x);

  for (i = 0; i < 6; i++)
    {
      if (REAL_VALUES_EQUAL (r, values_68881[i]))
        return (codes_68881[i]);
    }
  
  if (GET_MODE (x) == SFmode)
    return 0;

  if (REAL_VALUES_EQUAL (r, values_68881[6]))
    return (codes_68881[6]);

  /* larger powers of ten in the constants ram are not used
     because they are not equal to a `double' C constant.  */
  return 0;
}

/* If X is a floating-point constant, return the logarithm of X base 2,
   or 0 if X is not a power of 2.  */

int
floating_exact_log2 (x)
     rtx x;
{
  REAL_VALUE_TYPE r, r1;
  int i;

#ifndef REAL_ARITHMETIC
#if HOST_FLOAT_FORMAT != TARGET_FLOAT_FORMAT
  if (! flag_pretend_float)
    return 0;
#endif
#endif

  REAL_VALUE_FROM_CONST_DOUBLE (r, x);

  if (REAL_VALUES_LESS (r, dconst0))
    return 0;

  r1 = dconst1;
  i = 0;
  while (REAL_VALUES_LESS (r1, r))
    {
      r1 = REAL_VALUE_LDEXP (dconst1, i);
      if (REAL_VALUES_EQUAL (r1, r))
        return i;
      i = i + 1;
    }
  return 0;
}

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

   The icvm specific codes are:

   '.' for dot needed in Motorola-style opcode names.
   '-' for an operand pushing on the stack:
       sp@-, -(sp) or -(%sp) depending on the style of syntax.
   '+' for an operand pushing on the stack:
       sp@+, (sp)+ or (%sp)+ depending on the style of syntax.
   '@' for a reference to the top word on the stack:
       sp@, (sp) or (%sp) depending on the style of syntax.
   '#' for an immediate operand prefix (# in MIT and Motorola syntax
       but & in SGS syntax, $ in CRDS/UNOS syntax).
   '!' for the cc register (used in an `and to cc' insn).
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
       possible, so it should not be used except for the SunFPA.

   */





static int regstrict;



#undef REG_OK_IS_STRICT
#define REG_OK_IS_STRICT regstrict




int IsOffset(rtx X){
	return GET_CODE(X)==CONST_INT &&
		(INTVAL(X))>=-32768 && (INTVAL(X))<32768;
}

					
void PrintOffset(rtx X,FILE *f){
	fprintf(f,"%d",INTVAL(X));
}



				
int IsMemIndexReg(rtx X){
	return LEGITIMATE_INDEX_REG_P(X) && (REGNO(X)!=FRAME_POINTER_REGNUM);
}


void PrintMemIndexReg(rtx X,FILE *f,int indirect){
	char *s=reg_names[REGNO(X)];
	if(indirect && *s!='(') /* some of our registers are indirect memory aliases */
		fprintf(f,"(");
	fprintf(f,"%s",reg_names[REGNO(X)] );
	if(indirect && *s!='(') 
		fprintf(f,")");
}
												
int IsFrameReg(rtx X){										
  return GET_CODE (X) == REG && 
			REGNO(X)==FRAME_POINTER_REGNUM;
}
													

														

/* check for offset+framebase */

int IsFrameOffsetPartIndex(rtx X,int n1,int n2){
	if(GET_CODE(X)!=PLUS)
		return 0;
	if(IsFrameReg(XEXP (X, n1)) && 
			IsOffset(XEXP (X, n2)) )
		return 1;
	return 0;
}


					
/*  single frame,offset  n,fp indexing */
int IsFrameOffset(rtx X){
	return IsFrameOffsetPartIndex(X,0,1) ||
			IsFrameOffsetPartIndex(X,1,0);
}
							
/*  single frame,offset  n,fp indexing */
void PrintFrameOffset(rtx X,FILE *f){
	if(IsOffset(XEXP(X,0)))
		PrintOffset(XEXP(X,0),f);
	else
		PrintOffset(XEXP(X,1),f);
	fprintf(f,",%%fp");	
}
							

													

/*  indirect  single register,offset  @ n,fp  indexing */
int IsIndirectFrameOffset(rtx X){
	if(IsMemIndexReg(X)) /* cheater -- allows only our virtual stack vars */
		return 1;
	if(GET_CODE(X)!=MEM)
		return 0;
	return IsFrameOffset(XEXP(X,0));
}

void PrintIndirectFrameOffset(rtx X,FILE *f){
	if(IsMemIndexReg(X)) /* cheater -- allows only our virtual stack vars */
		PrintMemIndexReg(X,f,0);
	else{
		fprintf(f,"(");
		PrintFrameOffset(XEXP(X,0),f);/* skip past MEM */
		fprintf(f,")");
	}
}

																

/* check for (n,fp) * scale */
int IsScaledPartialIndex(rtx X,int n1,int n2,int mode){
    if(GET_CODE (X) != MULT)
		return 0; 
	if(!IsIndirectFrameOffset(XEXP (X, n1)))
		return 0; 		
	if(GET_CODE (XEXP (X, n2)) == CONST_INT		
			&& INTVAL (XEXP (X, n2)) == GET_MODE_SIZE(mode)) 
 		return 1;
	return 0;


};

/* check for (n,fp) * scale */
int IsScaledPartIndex(rtx X,int mode){
	return IsScaledPartialIndex(X,0,1,mode) ||
		IsScaledPartialIndex(X,1,0,mode);
};
	

void PrintScaledPartIndex(rtx X,int mode,FILE *f){
	if(IsScaledPartialIndex(X,0,1,mode))
		PrintIndirectFrameOffset(XEXP(X,0),f);
	else
		PrintIndirectFrameOffset(XEXP(X,1),f);
	fprintf(f,"@");
};
	

																									
/* looks for (n,fp)+(n,fp) */
int IsFrameOffsetPlusFrameOffset(rtx X){
	if(GET_CODE(X)!=PLUS)
		return 0;
	return IsIndirectFrameOffset(XEXP(X,0)) &&
		IsIndirectFrameOffset(XEXP(X,1));
}
							
/* looks for (n,fp)+(n,fp)@ */
int IsFrameOffsetPlusScaledFrameOffset(rtx X,int mode){
	if(GET_CODE(X)!=PLUS)
		return 0;
	if(IsIndirectFrameOffset(XEXP(X,0)) &&
			IsScaledPartIndex(XEXP(X,1),mode))
		return 1;
	if(IsIndirectFrameOffset(XEXP(X,1)) &&
			IsScaledPartIndex(XEXP(X,0),mode))
		return 1;
	return 0;
}

/* looks for n+(n,fp)@ */
int IsOffsetPlusScaledFrameOffset(rtx X,int mode){
	if(GET_CODE(X)!=PLUS)
		return 0;
	if(IsOffset(XEXP(X,0)) &&
			IsScaledPartIndex(XEXP(X,1),mode))
		return 1;
	if(IsOffset(XEXP(X,1)) &&
			IsScaledPartIndex(XEXP(X,0),mode))
		return 1;
	return 0;
}

/* looks for (n,fp)+n */
int IsFrameOffsetPlusOffset(rtx X){
	if(GET_CODE(X)!=PLUS)
		return 0;
	if(IsIndirectFrameOffset(XEXP(X,0)) &&
			IsOffset(XEXP(X,1)))
		return 1;
	if(IsIndirectFrameOffset(XEXP(X,1)) &&
			IsOffset(XEXP(X,0)))
		return 1;
	return 0;
}

/* prints (n,fp)+n */
void PrintFrameOffsetPlusOffset(rtx X,FILE *f){
	if(IsIndirectFrameOffset(XEXP(X,0)) &&
			IsOffset(XEXP(X,1))){
		PrintOffset(XEXP(X,1),f);
		fprintf(f,",");
		PrintIndirectFrameOffset(XEXP(X,0),f);
		return;
	}										
	if(IsIndirectFrameOffset(XEXP(X,1)) &&
			IsOffset(XEXP(X,0))){
		PrintOffset(XEXP(X,0),f);
		fprintf(f,",");
		PrintIndirectFrameOffset(XEXP(X,1),f);
		return;
	}
	fprintf(stderr,"Error in PrintFrameOffsetPlusOffset\n");;
}


/* look for n,(n,fp),(n,fp) or (n,fp),(n,fp) */

int IsFrameOffsetsOffset(rtx X,int mode){
	if(GET_CODE(X)!=PLUS)
		return 0;
		/* (n,fp),(n,fp) */
	if(IsFrameOffsetPlusFrameOffset(X))
		return 1;
		/* 0=n 1=(n,fp)(n,fp) */						
	if(IsOffset(XEXP(X,0)) && IsFrameOffsetPlusFrameOffset(XEXP(X,1)))
		return 1;
		/* 1=n 0=(n,fp)(n,fp) */						
	if(IsOffset(XEXP(X,1)) && IsFrameOffsetPlusFrameOffset(XEXP(X,0)))
		return 1;
		/* 0=(n,fp) 1=n,(n,fp) */													
	if(IsIndirectFrameOffset(XEXP(X,0)) && IsFrameOffsetPlusOffset(XEXP(X,1)))
		return 1;
		/* 1=(n,fp) 0=n,(n,fp) */													
	if(IsIndirectFrameOffset(XEXP(X,1)) && IsFrameOffsetPlusOffset(XEXP(X,0)))
		return 1;
	return 0;
}

void PrintFrameOffsetsOffset(rtx X,FILE *f){ /* (n,fp),(n,fp) */
	rtx X1;
	/* (n,fp),(n,fp) */
	if(IsFrameOffsetPlusFrameOffset(X)){ /* turn to 0,(n,fp),(n,fp) */
		fprintf(f,"@");
		PrintIndirectFrameOffset(XEXP(X,0),f);
		fprintf(f,",");									
		PrintIndirectFrameOffset(XEXP(X,1),f);
		return;
	}										
		/* 0=n 1=(n,fp)(n,fp) */						
	if(IsOffset(XEXP(X,0)) && IsFrameOffsetPlusFrameOffset(XEXP(X,1))){
		PrintOffset(XEXP(X,0),f);
		X=XEXP(X,1);
		fprintf(f,",");
		PrintIndirectFrameOffset(XEXP(X,0),f);
		fprintf(f,",");									
		PrintIndirectFrameOffset(XEXP(X,1),f);
		return;
	}
		/* 0=(n,fp)(n,fp) 1=n */					
	if(IsOffset(XEXP(X,1)) && IsFrameOffsetPlusFrameOffset(XEXP(X,0))){
		PrintOffset(XEXP(X,1),f);
		X=XEXP(X,0);
		fprintf(f,",");
		PrintIndirectFrameOffset(XEXP(X,0),f);
		fprintf(f,",");									
		PrintIndirectFrameOffset(XEXP(X,1),f);
		return;
	}
				
		/* 0=(n,fp) 1=n,(n,fp) */													
	if(IsIndirectFrameOffset(XEXP(X,0)) && IsFrameOffsetPlusOffset(XEXP(X,1))){
		X1=XEXP(X,1); /* offset is over here somewhere */
		if(IsOffset(XEXP(X1,0))){
			PrintOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,1),f);
		}else{
			PrintOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,0),f);

		}
		return;
	}
		/* 1=(n,fp) 0=n,(n,fp) */													
	if(IsIndirectFrameOffset(XEXP(X,1)) && IsFrameOffsetPlusOffset(XEXP(X,0))){
		X1=XEXP(X,0); /* offset is over here somewhere */
		if(IsOffset(XEXP(X1,0))){
			PrintOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,1),f);
		}else{
			PrintOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,0),f);

		}
		return ;


	}																			
	fprintf(f,"[Err in PrintFrameOffsetsOffset]");
}


/* look for n,(n,fp),(n,fp)@ or (n,fp),(n,fp)@	*/

int IsScaledFrameOffsetsOffset(rtx X,int mode){
	if(GET_CODE(X)!=PLUS)
		return 0;
	/* (n,fp),(n,fp)@ */
	if(IsFrameOffsetPlusScaledFrameOffset(X,mode))
		return 1;
	/* 0=n  1=(n,fp),(n,fp)@ */
	if(IsOffset(XEXP(X,0)) && IsFrameOffsetPlusScaledFrameOffset(XEXP(X,1),mode))
		return 1;
	/* 0=(n,fp),(n,fp)@ 1=n */
	if(IsOffset(XEXP(X,1)) && IsFrameOffsetPlusScaledFrameOffset(XEXP(X,0),mode))
		return 1;
	/* 0= (n,fp) 1=n,(n,fp)@ */
	if(IsIndirectFrameOffset(XEXP(X,0)) && IsOffsetPlusScaledFrameOffset(XEXP(X,1),mode))
		return 1;
	/* 1= (n,fp) 0=n,(n,fp)@ */
	if(IsIndirectFrameOffset(XEXP(X,1)) && IsOffsetPlusScaledFrameOffset(XEXP(X,0),mode))
		return 1;
	/* 0=(n,fp)@ 1=n,(n,fp) */
	if(IsScaledPartIndex(XEXP(X,0),mode) && IsFrameOffsetPlusOffset(XEXP(X,1)))
		return 1;
	/* 1=(n,fp)@ 0=n,(n,fp) */
	if(IsScaledPartIndex(XEXP(X,1),mode) && IsFrameOffsetPlusOffset(XEXP(X,0)))
		return 1;
	return 0;
}

void PrintScaledFrameOffsetsOffset(rtx X,int mode,FILE *f){
	rtx X1;
	/* (n,fp),(n,fp)@ */
	if(IsFrameOffsetPlusScaledFrameOffset(X,mode)){
		fprintf(f,"@");
		if(IsIndirectFrameOffset(XEXP(X,0))){
			PrintIndirectFrameOffset(XEXP(X,0),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,1),mode,f);
		}else{			
			PrintIndirectFrameOffset(XEXP(X,1),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,0),mode,f);
		}			
		return ;
	}

	/* 0=n  1=(n,fp),(n,fp)@ */
	if(IsOffset(XEXP(X,0)) && IsFrameOffsetPlusScaledFrameOffset(XEXP(X,1),mode)){
		PrintOffset(XEXP(X,0),f);
		fprintf(f,",");
		X=XEXP(X,1); /* rest of it is over here */
		if(IsIndirectFrameOffset(XEXP(X,0))){
			PrintIndirectFrameOffset(XEXP(X,0),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,1),mode,f);
		}else{			
			PrintIndirectFrameOffset(XEXP(X,1),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,0),mode,f);
		}			
		return ;

	}

	/* 0=(n,fp),(n,fp)@ 1=n */
	if(IsOffset(XEXP(X,1)) && IsFrameOffsetPlusScaledFrameOffset(XEXP(X,0),mode)){
		PrintOffset(XEXP(X,1),f);
		fprintf(f,",");
		X=XEXP(X,0); /* rest of it is over here */
		if(IsIndirectFrameOffset(XEXP(X,0))){
			PrintIndirectFrameOffset(XEXP(X,0),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,1),mode,f);
		}else{			
			PrintIndirectFrameOffset(XEXP(X,1),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,0),mode,f);
		}			
		return ;

	}

	/* 0= (n,fp) 1=n,(n,fp)@ */
	if(IsIndirectFrameOffset(XEXP(X,0)) && IsOffsetPlusScaledFrameOffset(XEXP(X,1),mode)){
		X1=XEXP(X,1); /* rest of stuff is over here */
		if(IsOffset(XEXP(X1,0))){
			PrintOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,0),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X1,1),mode,f);
		}else{
			PrintOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,0),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X1,0),mode,f);
		}
		return;
	}

	/* 1= (n,fp) 0=n,(n,fp)@ */
	if(IsIndirectFrameOffset(XEXP(X,1)) && IsOffsetPlusScaledFrameOffset(XEXP(X,0),mode)){
		X1=XEXP(X,0); /* rest of stuff is over here */
		if(IsOffset(XEXP(X1,0))){
			PrintOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,1),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X1,1),mode,f);
		}else{
			PrintOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X,1),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X1,0),mode,f);
		}
		return;
	}
																						
	/* 0=(n,fp)@ 1=n,(n,fp) */
	if(IsScaledPartIndex(XEXP(X,0),mode) && IsFrameOffsetPlusOffset(XEXP(X,1))){
		X1=XEXP(X,1); /* rest of stuff is over here */
		if(IsOffset(XEXP(X1,0))){
			PrintOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,0),mode,f);
		}else{
			PrintOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,0),mode,f);
		}
		return;
	}
	/* 1=(n,fp)@ 0=n,(n,fp) */
	if(IsScaledPartIndex(XEXP(X,1),mode) && IsFrameOffsetPlusOffset(XEXP(X,0))){
		X1=XEXP(X,0); /* rest of stuff is over here */
		if(IsOffset(XEXP(X1,0))){
			PrintOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,1),mode,f);
		}else{
			PrintOffset(XEXP(X1,1),f);
			fprintf(f,",");
			PrintIndirectFrameOffset(XEXP(X1,0),f);
			fprintf(f,",");
			PrintScaledPartIndex(XEXP(X,1),mode,f);
		}
		return;
	}

	fprintf(f,"[PrintScaledFrameOffsetsOffset Error]");

}




/* ***************************************
 still need n,fp,(n,fp) and n,fp,(n,fp)@
 or kill it in favor of optimized ((n,fp),(n,fp)) and ((n,fp),(f,fp)@)
 ***************************************
*/

 
/* see if this is index types  (r)  (n,r)  or (n,r1,r2) or (r1,r2) */

int IsRegisterIndex(rtx op,int mode,int strict){
    REG_OK_IS_STRICT=strict; /* set up strict variable */


	if(IsFrameOffset(op))
		return 1;

	if(IsIndirectFrameOffset(op))
		return 1;

	if(IsFrameOffsetsOffset(op,mode))
		return 1;

/* DEBUG - ************* BOB */

	if(IsFrameOffsetPlusOffset(op))
		return 1;

	if(IsScaledFrameOffsetsOffset(op,mode))
		return 1;
	return 0;

}
 
   
int IsLegitimateAddress(rtx op,int mode,int strict){
    REG_OK_IS_STRICT=strict; /* set up strict variable */
	if(CONSTANT_ADDRESS_P(op))
		return 1;
	if(IsRegisterIndex(op,mode,strict))
		return 1;
	return 0;

}

#undef REG_OK_IS_STRICT
#define REG_OK_IS_STRICT 1  /* redefine this back to not strict */

void print_operand (file, op, letter)
     FILE *file;		/* file to write to */
     rtx op;			/* operand to print */
     int letter;		/* %<letter> or 0 */
{
  int i;

  if (letter == '.')
    {
      asm_fprintf (file, ".");
    }
  else if (letter == '#')
    {
      asm_fprintf (file, "%0I");
    }
  else if (letter == '-')
    {
      asm_fprintf (file, "-(%Rsp)");
    }
  else if (letter == '+')
    {
      asm_fprintf (file, "(%Rsp)+");
    }
  else if (letter == '@')
    {
      asm_fprintf (file, "(%Rsp)");
    }
  else if (letter == '!')
    {
      asm_fprintf (file, "%Rfpcr");
    }
  else if (letter == '$')
    {
      if (TARGET_68040_ONLY)
	{
	  fprintf (file, "s");
	}
    }
  else if (letter == '/')
    {
      asm_fprintf (file, "%R");
    }
  else if (GET_CODE (op) == REG){
	   fputs (reg_names[REGNO (op)], file);
    }
  else if (GET_CODE (op) == MEM)
    {
		if(IsFrameOffset(XEXP(op,0))){
			fprintf(file,"(");
			PrintFrameOffset(XEXP(op,0),file);
			fprintf(file,")");
		}else if(IsIndirectFrameOffset(XEXP(op,0))){
 			fprintf(file,"@");
			PrintIndirectFrameOffset(XEXP(op,0),file);
		}else if(IsFrameOffsetsOffset(XEXP(op,0),GET_MODE(op))){
			fprintf(file,"(");
			PrintFrameOffsetsOffset(XEXP(op,0),file);
			fprintf(file,")");
		}else if(IsScaledFrameOffsetsOffset(XEXP(op,0),GET_MODE(op))){
			fprintf(file,"(");
			PrintScaledFrameOffsetsOffset(XEXP(op,0),GET_MODE(op),file);
			fprintf(file,")");
		}else if(IsFrameOffsetPlusOffset(XEXP(op,0))){
			fprintf(file,"(");
			PrintFrameOffsetPlusOffset(XEXP(op,0),file);
			fprintf(file,")");
    	}else {	
          		output_address (XEXP (op, 0));
		}
    }
  else if (GET_CODE (op) == CONST_DOUBLE && GET_MODE (op) == SFmode)
    {
      REAL_VALUE_TYPE r;
      REAL_VALUE_FROM_CONST_DOUBLE (r, op);
      ASM_OUTPUT_FLOAT_OPERAND (letter, file, r);
    }
  else if (GET_CODE (op) == CONST_DOUBLE && GET_MODE (op) == XFmode)
    {
      REAL_VALUE_TYPE r;
      REAL_VALUE_FROM_CONST_DOUBLE (r, op);
      ASM_OUTPUT_LONG_DOUBLE_OPERAND (file, r);
    }
  else if (GET_CODE (op) == CONST_DOUBLE && GET_MODE (op) == DFmode)
    {
      REAL_VALUE_TYPE r;
      REAL_VALUE_FROM_CONST_DOUBLE (r, op);
      ASM_OUTPUT_DOUBLE_OPERAND (file, r);
    }
  else
    {
      asm_fprintf (file, "%0I"); 
      output_addr_const (file, op);
    }

}


/* A C compound statement to output to stdio stream STREAM the
   assembler syntax for an instruction operand that is a memory
   reference whose address is ADDR.  ADDR is an RTL expression.

   Note that this contains a kludge that knows that the only reason
   we have an address (plus (label_ref...) (reg...)) when not generating
   PIC code is in the insn before a tablejump, and we know that icvm.md
   generates a label LInnn: on such an insn.

   It is possible for PIC to generate a (plus (label_ref...) (reg...))
   and we handle that just like we would a (plus (symbol_ref...) (reg...)).

   Some SGS assemblers have a bug such that "Lnnn-LInnn-2.b(pc,d0.l*2)"
   fails to assemble.  Luckily "Lnnn(pc,d0.l*2)" produces the results
   we want.  This difference can be accommodated by using an assembler
   define such "LDnnn" to be either "Lnnn-LInnn-2.b", "Lnnn", or any other
   string, as necessary.  This is accomplished via the ASM_OUTPUT_CASE_END
   macro.  See icvm/sgs.h for an example; for versions without the bug.
   Some assemblers refuse all the above solutions.  The workaround is to
   emit "K(pc,d0.l*2)" with K being a small constant known to give the
   right behaviour.

   They also do not like things like "pea 1.w", so we simple leave off
   the .w on small constants. 

   This routine is responsible for distinguishing between -fpic and -fPIC 
   style relocations in an address.  When generating -fpic code the
   offset is output in word mode (eg movel a5@(_foo:w), a0).  When generating
   -fPIC code the offset is output in long mode (eg movel a5@(_foo:l), a0) */

#ifndef ASM_OUTPUT_CASE_FETCH


#define ASM_OUTPUT_CASE_FETCH(file, labelno, regname)\
	asm_fprintf (file, "%LL%d-%LLI%d.b(%Rpc,%s.", labelno, labelno, regname)


#endif /* ASM_OUTPUT_CASE_FETCH */

void
print_operand_address (file, addr)
     FILE *file;
     rtx addr;
{
  register rtx reg1, reg2, breg, ireg;
  rtx offset;

  switch (GET_CODE (addr))
    {
      case REG:
	    fprintf (file, "!!!!(0,%s)", reg_names[REGNO (addr)]);
	    break;
      case SUBREG:
	    fprintf (file, "SUBREG!!!!(0,%s)", reg_names[REGNO (addr)]);
	    break;
      case PRE_DEC:
	    fprintf (file, "-(%s)", reg_names[REGNO (XEXP (addr, 0))]);
	    break;
      case POST_INC:
	    fprintf (file, "(%s)+", reg_names[REGNO (XEXP (addr, 0))]);
	    break;

      default:
        if (GET_CODE (addr) == CONST_INT)
	  {
	    fprintf (file, "%d", INTVAL (addr));
	  }
	else
	  {
	    output_addr_const (file, addr);
	  }
	break;
    }
}


/* Accept integer operands in the range 0..0xffffffff.  We have to check the
   range carefully since this predicate is used in DImode contexts.  Also, we
   need some extra crud to make it work when hosted on 64-bit machines.  */

int
const_uint32_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
#if HOST_BITS_PER_WIDE_INT > 32
  /* All allowed constants will fit a CONST_INT.  */
  return (GET_CODE (op) == CONST_INT
	  && (INTVAL (op) >= 0 && INTVAL (op) <= 0xffffffffL));
#else
  return ((GET_CODE (op) == CONST_INT && INTVAL (op) >= 0)
	  || (GET_CODE (op) == CONST_DOUBLE && CONST_DOUBLE_HIGH (op) == 0));
#endif
}

/* Accept integer operands in the range -0x80000000..0x7fffffff.  We have
   to check the range carefully since this predicate is used in DImode
   contexts.  */

int
const_sint32_operand (op, mode)
     rtx op;
     enum machine_mode mode;
{
  /* All allowed constants will fit a CONST_INT.  */
  return (GET_CODE (op) == CONST_INT
	  && (INTVAL (op) >= (-0x7fffffff - 1) && INTVAL (op) <= 0x7fffffff));
}





static int reverseorderbccop(int op){
    switch(op){
        case GT:
            op=LT;
            break;

        case GE:
            op=LE;
            break;

        case LT:
            op=GT;
            break;

        case LE:
            op=GE;
            break;

        case GTU:
            op=LTU;
            break;

        case GEU:
            op=LEU;
            break;

        case LTU:
            op=GTU;
            break;

        case LEU:
            op=GEU;
            break;


        case EQ:
            op=EQ;
            break;

        case NE:
            op=NE;
            break;


    }
    return op;
}

/* klugey way to find register collisions not allowed in ICVM
 this only finds cases that have been problems so far
*/

static int srcdestcollision(rtx src,rtx dst){
	rtx dreg=0;

	if(GET_CODE(src)==REG && (REGNO(src)==2 || REGNO(src)==3) ){
	
		if(GET_CODE(dst)==MEM){
			dst=XEXP(dst,0);

/* detect %rn,(offset,%rn) where rn are identical */
			if(GET_CODE(dst)==PLUS){
				if(IsOffset(XEXP(dst,1)) && GET_CODE(XEXP(dst,0))==REG)
					dreg=XEXP(dst,0);		
				else if(IsOffset(XEXP(dst,0)) && GET_CODE(XEXP(dst,1))==REG)
					dreg=XEXP(dst,1);
				if(dreg && REGNO(dreg)==REGNO(src))
					return 1;
			}
			if(GET_CODE(dst)==REG){
				if(REGNO(dst)==REGNO(src))
					return 1;
			}			

			return 0; 
		}
								
	}
	return 0;
}


// check for identical registers

#define CMP_SAMEREG(src,dest) (\
	GET_CODE(src)==REG && GET_CODE(dest)==REG && REGNO(src)==REGNO(dest)\
)


char *buildcbcc(struct rtx_def *operands[],int op){
  static char buf[256];
  char *inst="BADOP";
  int hi=32767;  
  int lo=-32768;
  int nozero=0;
  operands[1]=r_compare_op0; /* set up for compare and branch instruction */
  operands[2]=r_compare_op1;

	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
	CheckSoftRegUsage(operands[2]);

  switch(op){

    case GT:
        inst="cbgt";
        break;

    case GE:
        inst="cbge";
        break;

    case LT:
        inst="cblt";
        break;

    case LE:
        inst="cble";
        break;

    case GTU:
        inst="cbgtu";
		hi=65535;
		lo=0;
		nozero=1;
        break;

    case GEU:
        inst="cbgeu";
		hi=65535;
		lo=0;
		nozero=1;
        break;

    case LTU:
        inst="cbltu";
		hi=65535;
		lo=0;
		nozero=1;
        break;

    case LEU:
        inst="cbleu";
		hi=65535;
		lo=0;
		nozero=1;
        break;


    case EQ:
        inst="cbeq";
		hi=32767;
		lo=0;
        break;

    case NE:
        inst="cbne";
		hi=32767;
		lo=0;
        break;

  }  

  if (nozero==0 && CONSTANT_P(operands[2]) && 
			(GET_CODE(operands[2])) == CONST_INT 
			&& (INTVAL(operands[2]))==0){

	  switch (GET_MODE(operands[1])){

    	  case SImode:
        	  sprintf(buf,"%sl  %%1,%%l0",inst);
          	  break;
   
      	  case HImode:
              sprintf(buf,"%sw  %%1,%%l0",inst);
              break;
   
          case QImode:
              sprintf(buf,"%sb  %%1,%%l0",inst);
              break;

          case DImode:
              sprintf(buf,"%sq  %%1,%%l0",inst);
              break;

          case SFmode:
              sprintf(buf,"%sf  %%1,%%l0",inst);
              break;

          case DFmode:
              sprintf(buf,"%sd  %%1,%%l0",inst);
              break;

        }


  }else{
	int i;
	int isshort=0;

/* need to turn it around here */
	op=reverseorderbccop(op);
	switch(op){

      case GT:
        inst="cbgt";
        break;

      case GE:
        inst="cbge";
        break;

      case LT:
        inst="cblt";
        break;

      case LE:
        inst="cble";
        break;

      case GTU:
        inst="cbgtu";
        break;

      case GEU:
        inst="cbgeu";
        break;

      case LTU:
        inst="cbltu";
        break;

      case LEU:
        inst="cbleu";
        break;

      case EQ:
        inst="cbeq";
        break;

      case NE:
        inst="cbne";
        break;

    }  

  	if (GET_CODE (operands[2]) == CONST_INT){
    	i = INTVAL (operands[2]);
    	if(i>=lo && i<=hi)
			isshort=1;
 	}
/* optimize both sides for short constants. Assembler will fix order */

  	if (GET_CODE (operands[1]) == CONST_INT){
    	i = INTVAL (operands[1]);
    	if(i>=lo && i<=hi)
			isshort=1;
 	}

/* check for register collisions and act accordingly */

	if(srcdestcollision(operands[2],operands[1]) 
		|| CMP_SAMEREG(operands[2],operands[1]))
	  switch (GET_MODE(operands[1])){

    	  case SImode:
			  if(isshort)
        	  		sprintf(buf,"movel %%1,-(%%%%sp)\n\t%swl  %%2,(%%%%sp)+,%%l0",inst);
        	   else	
        	  		sprintf(buf,"movel %%1,-(%%%%sp)\n\t%sl  %%2,(%%%%sp)+,%%l0",inst);
          	  break;
   
      	  case HImode:
        	  sprintf(buf,"movew %%1,-(%%%%sp)\n\t%sw  %%2,(%%%%sp)+,%%l0",inst);
              break;
   
          case QImode:
        	  sprintf(buf,"moveb %%1,-(%%%%sp)\n\t%sb  %%2,(%%%%sp)+,%%l0",inst);
              break;

          case DImode:
        	  sprintf(buf,"moveq %%1,-(%%%%sp)\n\t%sq  %%2,(%%%%sp)+,%%l0",inst);
              break;

          case SFmode:
        	  sprintf(buf,"movef %%1,-(%%%%sp)\n\t%sf  %%2,(%%%%sp)+,%%l0",inst);
              break;

          case DFmode:
        	  sprintf(buf,"moved %%1,-(%%%%sp)\n\t%sd  %%2,(%%%%sp)+,%%l0",inst);
              break;

		  default:
			fprintf(stderr,"buldcbcc: Should Not Happen\n");
			exit(3);	
        }
	else if(srcdestcollision(operands[1],operands[2])
		|| CMP_SAMEREG(operands[2],operands[1]))

	  switch (GET_MODE(operands[1])){

    	  case SImode:
			  if(isshort)
        	  		sprintf(buf,"movel %%2,-(%%%%sp)\n\t%swl  (%%%%sp)+,%%1,%%l0",inst);
        	   else	
        	  		sprintf(buf,"movel %%2,-(%%%%sp)\n\t%sl  (%%%%sp)+,%%1,%%l0",inst);
          	  break;
   
      	  case HImode:
        	  sprintf(buf,"movew %%2,-(%%%%sp)\n\t%sw  (%%%%sp)+,%%1,%%l0",inst);
              break;
   
          case QImode:
        	  sprintf(buf,"moveb %%2,-(%%%%sp)\n\t%sb  (%%%%sp)+,%%1,%%l0",inst);
              break;

          case DImode:
        	  sprintf(buf,"moveq %%2,-(%%%%sp)\n\t%sq  (%%%%sp)+,%%1,%%l0",inst);
              break;

          case SFmode:
        	  sprintf(buf,"movef %%2,-(%%%%sp)\n\t%sf  (%%%%sp)+,%%1,%%l0",inst);
              break;

          case DFmode:
        	  sprintf(buf,"moved %%2,-(%%%%sp)\n\t%sd  (%%%%sp)+,%%1,%%l0",inst);
              break;

		  default:
			fprintf(stderr,"buldcbcc: Should Not Happen Again\n");
			exit(3);	
        }
	
	else

	  switch (GET_MODE(operands[1])){

    	  case SImode:
			  if(isshort)
        	  		sprintf(buf,"%swl  %%2,%%1,%%l0",inst);
        	   else	
					sprintf(buf,"%sl  %%2,%%1,%%l0",inst);
          	  break;
   
      	  case HImode:
              sprintf(buf,"%sw  %%2,%%1,%%l0",inst);
              break;
   
          case QImode:
              sprintf(buf,"%sb  %%2,%%1,%%l0",inst);
              break;

          case DImode:
			  if(isshort)
	              sprintf(buf,"%swq  %%2,%%1,%%l0",inst);
			  else
	              sprintf(buf,"%sq  %%2,%%1,%%l0",inst);
              break;

          case SFmode:
              sprintf(buf,"%sf  %%2,%%1,%%l0",inst);
              break;

          case DFmode:
              sprintf(buf,"%sd  %%2,%%1,%%l0",inst);
              break;

        }


	}
    return buf;
}


#define SWAPRTX(n1,n2) \
    {rtx tmprtx=n1;n1=n2;n2=tmprtx;}

		
			
																												
char *output_optimized(rtx *operands,char *move,char *adds,char *addqs, int flags,
	int n0,int n1,int n2,char *opn0)
{
  static char buf[256];
  char nbuf0[16],nbuf1[16],nbuf2[16];
  long l;
  int canswapreg=flags & IG_CANSWAP;
  int istriple=flags & IG_TRIPLE;
  int optadd=strcmp(adds,"addl")==0;
  char *N0=opn0;	 
  char *N1=0;	 
  char *N2=0;	 
  sprintf(nbuf0,"%d",n0);
  sprintf(nbuf1,"%d",n1);
  sprintf(nbuf2,"%d",n2);
  if(N0==0)
		N0=nbuf0;
  if(N1==0)
		N1=nbuf1;
  if(N2==0)
		N2=nbuf2;
  if(!istriple)
		canswapreg=0;

/* assembler handled this */
  if (canswapreg){
        if(CONSTANT_P(operands[n1])){
            SWAPRTX(operands[n1],operands[n2]);
        } 
  }  

	
/* see if we can shorten the constant add/subtract */

  if (GET_CODE (operands[n2]) == CONST_INT){
      if (addqs && INTVAL (operands[n2]) >= -32768
    	        && INTVAL (operands[n2]) < 32768){
			if(optadd && opn0==0 && operands_match_p(operands[n0],operands[n1])){
				switch(INTVAL(operands[n2])){
					case 1:
  						sprintf(buf,"inc1l %%%s",N0);
						return buf;

					case 2:
  						sprintf(buf,"inc2l %%%s",N0);
						return buf;

					case 4:
  						sprintf(buf,"inc4l %%%s",N0);
						return buf;

					case 8:
  						sprintf(buf,"inc8l %%%s",N0);
						return buf;

					case -1:
  						sprintf(buf,"dec1l %%%s",N0);
						return buf;

					case -2:
  						sprintf(buf,"dec2l %%%s",N0);
						return buf;

					case -4:
  						sprintf(buf,"dec4l %%%s",N0);
						return buf;

					case -8:
  						sprintf(buf,"dec8l %%%s",N0);
						return buf;

					default:
						adds=addqs;
				}

			}else
				adds=addqs;
  		}
	}

  if(opn0==0 && operands_match_p (operands[n0], operands[n1])){
  	sprintf(buf,"%s %%%s,%%%s",adds,N2,N0);
	return buf;
  }	

  if(canswapreg && opn0==0 && operands_match_p (operands[n0], operands[n2]) && 
		GET_CODE(operands[n0])==REG){
  	sprintf(buf,"%s %%%s,%%%s",adds,N1,N0);
	return buf;
  }	



/* detect register collisions that ICVM can't handle */
  if(srcdestcollision(operands[n1],operands[n0])){
	  if(opn0)
		  sprintf(buf,"%s %%%s,%%%s,-(%%%%sp)\n\t%s (%%%%sp)+,%s",
			adds,N2,N1,move,N0);
		else
		  sprintf(buf,"%s %%%s,%%%s,-(%%%%sp)\n\t%s (%%%%sp)+,%%%s",
			adds,N2,N1,move,N0);
	  return buf;
  }	
	
  if(srcdestcollision(operands[n1],operands[n2])){
	  if(opn0)
		  sprintf(buf,"%s %%%s,-(%%%%sp)\n\t%s (%%%%sp)+,%%%s,%s",
			move, N2, adds,N1,N0);
		else
		  sprintf(buf,"%s %%%s,-(%%%%sp)\n\t%s (%%%%sp)+,%%%s,%%%s",
			move, N2, adds,N1,N0);
	  return buf;
  }	
	
  if(opn0)
	  sprintf(buf,"%s %%%s,%%%s,%s",adds,N2,N1,N0);
	else
	  sprintf(buf,"%s %%%s,%%%s,%%%s",adds,N2,N1,N0);
  return buf;
}

												
char *output_optimizedmove(rtx *operands,char *clrs,char *movew,char *moves,
	int n0,int n1)
{
  int i;
  static char buf[256];
  if (GET_CODE (operands[n1]) == CONST_INT){
      i = INTVAL (operands[n1]);
      if (i==0 && clrs){
            sprintf(buf,"%s %%%d",clrs,n0);
            return buf;
      }

      if(i>=-32786 && i<=32767 && movew){
            sprintf(buf,"%s %%%d,%%%d",movew,n1,n0);
            return buf;
      }


  }
  sprintf(buf,"%s %%%d,%%%d",moves,n1,n0);
  return buf;
}



char *output_general(rtx *operands,char *moves,char *adds,char *addqs, int flags){
	static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
	CheckSoftRegUsage(operands[2]);

/* optimize immediate refs to addresses to relative ones */
	if(strcmp("addl",adds)==0 && symbolic_operand(operands[2],SImode)){
		operands[2]=gen_rtx_MEM(SImode,operands[2]);
		if(operands_match_p(operands[0],operands[1]))
			sprintf(buf,"addral %%2,%%0");
		else
			sprintf(buf,"addral %%2,%%1,%%0");
		return buf;
	}


	return output_optimized(operands,moves,adds,addqs,flags,0,1,2,0);
}

		
char *output_unary(rtx *operands,char *adds)
{
  static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
  if(operands_match_p (operands[0], operands[1]))
  		sprintf(buf,"%s %%0",adds);
  else						
  		sprintf(buf,"%s %%1,%%0",adds);
  return buf;										
}

char *output_N2doublestackop(rtx *operands,char *moves,char *adds)
{
  static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
  sprintf(buf,"%s %%2,-(%%%%sp)\n\t%s (%%%%sp)+,%%0",moves,adds);
  return buf;										
}

char *output_doublestackop(rtx *operands,char *moves,char *adds)
{
  static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
  sprintf(buf,"%s %%1,-(%%%%sp)\n\t%s (%%%%sp)+,-(%%%%sp)\n\t%s (%%%%sp)+,%%0",moves,adds,moves);
  return buf;										
}

char *output_N1N2triplestackop(rtx *operands,char *moves,char *adds,
	char *addqs, int flags)
{
  static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
	CheckSoftRegUsage(operands[2]);
  if(operands_match_p (operands[0], operands[1]))
		return output_N2doublestackop(operands,moves,adds);
  sprintf(buf,"%s %%1,-(%%%%sp)\n\t%s %%2,-(%%%%sp)\n\t%s (%%%%sp)+,(%%%%sp)+,%%0",
		moves,moves,adds);
  return buf;										
}

char *output_N2triplestackop(rtx *operands,char *moves,char *adds,
	char *addqs, int flags)
{
  static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
	CheckSoftRegUsage(operands[2]);
  if(operands_match_p (operands[0], operands[1]))
		return output_N2doublestackop(operands,moves,adds);
  if(GET_CODE (operands[0]) == REG 
  			&& GET_CODE (operands[1]) == REG
			&& ISHARDREG(REGNO(operands[0]))
			&& ISHARDREG(REGNO(operands[1]))
			&& REGNO(operands[0])==REGNO(operands[1]))
		return output_N1N2triplestackop(operands,moves,adds,addqs,flags);

  sprintf(buf,"%s %%2,-(%%%%sp)\n\t%s (%%%%sp)+,%%1,%%0",
		moves,adds);
  return buf;										
}

char *output_N1triplestackop(rtx *operands,char *moves,char *adds,
	char *addqs, int flags)
{
  static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
	CheckSoftRegUsage(operands[2]);
  if(operands_match_p (operands[0], operands[1]))
		return output_N2doublestackop(operands,moves,adds);
  if(GET_CODE (operands[0]) == REG 
  			&& GET_CODE (operands[2]) == REG 
			&& ISHARDREG(REGNO(operands[0]))
			&& ISHARDREG(REGNO(operands[2]))
			&& REGNO(operands[0])==REGNO(operands[2]))
		return output_N1N2triplestackop(operands,moves,adds,addqs,flags);

  sprintf(buf,"%s %%1,-(%%%%sp)\n\t%s %%2,(%%%%sp)+,%%0",
		moves,adds);
  return buf;										
}

char *output_generalorstack(struct rtx_def **operands,char *move,
	char *adds,char *addqs, int flags){
  static char buf[256];
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
	CheckSoftRegUsage(operands[2]);
  if(operands_match_p (operands[0], operands[1]))
		return output_general(operands,move,adds,addqs,flags);
  return output_N1N2triplestackop(operands,move,adds,addqs,flags);
  return buf;
}

																	
char *output_generaltriplestack(struct rtx_def **operands,char *move,
	char *adds,char *addqs, int flags){
  static char buf[256];
  char *s1;
	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);
	CheckSoftRegUsage(operands[2]);
  if(operands_match_p (operands[0], operands[1]))
		return output_general(operands,move,adds,addqs,flags);
   s1=output_optimized(operands,move,adds,addqs,flags,0,1,2,"-(%%sp)");
   sprintf(buf,"%s\n\t%s (%%%%sp)+,%%0",s1,move);
  return buf;
}
						


char *output_moveit(rtx *operands,char *clrs,char *movew,char *moves)
{
	static char buf[256];

	CheckSoftRegUsage(operands[0]);
	CheckSoftRegUsage(operands[1]);

/* optimize immediate refs to addresses to relative ones */
	if(strcmp("movel",moves)==0 && symbolic_operand(operands[1],SImode)){
		operands[1]=gen_rtx_MEM(SImode,operands[1]);
		sprintf(buf,"moveral %%1,%%0");
		return buf;
	}

/* this  move code is a hack to get 32 bit floats back and forth to 32 bit iregs */

	if(strcmp(moves,"movef")==0 || strcmp(moves,"movel")==0){
		if(GET_CODE(operands[1])==REG && ISSFREG(REGNO(operands[1]))){
			if(GET_CODE(operands[0])==REG && ISSIREG(REGNO(operands[0]))){
			   sprintf(buf,"movef %%1,-(%%%%sp)\n\tmovel (%%%%sp)+,%%0");
			   return buf;
			}
		}

		if(GET_CODE(operands[1])==REG && ISSIREG(REGNO(operands[1]))){
			if(GET_CODE(operands[0])==REG && ISSFREG(REGNO(operands[0]))){
		   		sprintf(buf,"movel %%1,-(%%%%sp)\n\tmovef (%%%%sp)+,%%0");
		   		return buf;
			}
			return output_optimizedmove(operands,clrs,movew,"movel",0,1);
		}

	}
/* zero trunc hack
 for zero extending a byte from a word register
*/
	if(strcmp(moves,"moveubl")==0){
		if(GET_CODE(operands[1])==REG && ISHIREG(REGNO(operands[1]))
				&& !ISHARDINTREG(REGNO(operands[1])) ){
			sprintf(buf,"movewb %%1,-(%%%%sp)\n\tmoveubl (%%%%sp)+,%%0");
			return buf;
		}
	}	
/* optimize this a bit */
	if(strcmp(moves,"movewb")==0 || strcmp(moves,"movelb")==0){
		if(IsHardI32Reg(operands[1]) && IsHardI32Reg(operands[0]) &&
				REGNO(operands[1])==REGNO(operands[0]))
			return "";
	}
	return output_optimizedmove(operands,clrs,movew,moves,0,1);
}



int LimitReloadClass(int mode,int class){
	if(class==INDEXABLE_REGS)
		return class;
	if(class==LONG_REGS)
		return class;																		
	if(mode==SFmode)
		return FLOAT32_REGS;
	if(mode==DFmode)
		return FLOAT64_REGS;
	if(ISQMODE(mode))
		return LONG_REGS;
	return class;
}
																							


struct rtx_def *force_simpleoperand(struct rtx_def *x)
{
  register struct rtx_def *temp;

  if(general_simpleoperand(x,GET_MODE(x))){
		return x;
	}

  if (GET_CODE (x) != MEM || GET_MODE (x) == BLKmode)
    return x;
  temp = gen_reg_rtx (GET_MODE (x));
  emit_move_insn (temp, x);
  return temp;
}



int general_simpleoperand(struct rtx_def *op, int mode){
	if(!general_operand(op,mode))
		return 0;

	if(GET_CODE(op)!=MEM)
		return 1;

	if(CONSTANT_ADDRESS_P(XEXP(op,0)))
		return 1;

	if(IsFrameOffsetsOffset(XEXP(op,0),GET_MODE(op)))
		return 0;

	if(IsScaledFrameOffsetsOffset(XEXP(op,0),GET_MODE(op)))
		return 0;
	return 1;	
}

int nonimmediate_simpleoperand(struct rtx_def *op, int mode){
	if(!nonimmediate_operand(op,mode))
		return 0;
	if(!general_simpleoperand(op,mode))
		return 0;
	return 1;
}

int general_noindexoperand(struct rtx_def *op, int mode){
	if(!general_simpleoperand(op,mode))
		return 0;
	if(GET_CODE(op)!=MEM)
		return 1;
	if(IsFrameOffset(XEXP(op,0)))
		return 1;
	if(GET_CODE(XEXP(op,0))==PLUS)
		return 0;
	if(IsIndirectFrameOffset(XEXP(op,0)))
		return 0;
	if(IsFrameOffsetPlusOffset(XEXP(op,0)))
		return 0;
	return 1;
}


int nonimmediate_noindexoperand(struct rtx_def *op, int mode){
	if(!nonimmediate_simpleoperand(op,mode))
		return 0;
	if(!general_noindexoperand(op,mode))
		return 0;
	return 1;
}



typedef struct _syscallinfo{
	struct _syscallinfo *next;
	char *funcname;
	int operand;
	char *preamble;
	char *postop;
} syscallinfo;

static syscallinfo *syscalls;

syscallinfo *findsyscall(char *fname){
	static char buf[1024];
	syscallinfo *f=syscalls;
	while(f){
		if(strcmp(fname,f->funcname)==0)
			return f;	
		f=f->next;
	}
	return 0;
}



/* find and build a system call, if available. else return 0 */
char *buildsyscall(struct rtx_def *operand){
	static char st[2048];
	char *funcname;
	char buf[128];
	syscallinfo *f;
	char *s;
  if(GET_CODE(operand) != MEM
      || GET_CODE(XEXP(operand, 0)) != SYMBOL_REF)
		return 0;
	operand=XEXP(operand,0);
	funcname=XSTR(operand,0);
	/* short-circuit builtin assembly equivs */
	if(strcmp("bcopy",funcname)==0
		|| strcmp("bzero",funcname)==0
		|| strcmp("bcmp",funcname)==0
		|| strcmp("memset",funcname)==0
		|| strcmp("memcpy",funcname)==0
		|| strcmp("memcmp",funcname)==0
		|| strcmp("memmove",funcname)==0
		|| strcmp("strlen",funcname)==0
		|| strcmp("strcpy",funcname)==0
		|| strcmp("strcat",funcname)==0
		|| strcmp("strcmp",funcname)==0
		|| strcmp("strncpy",funcname)==0
		|| strcmp("strncat",funcname)==0
		|| strcmp("strncmp",funcname)==0
	)
		return funcname;		
	f=findsyscall(funcname);
	if(f==0)
		return 0;
	st[0]=0;
	if(f->operand)
		sprintf(buf,"\tsyscalll #%d | %s",f->operand,f->funcname);
	else
		buf[0]=0;
	if(strlen(f->preamble)){
		strcat(st,"\t");
		strcat(st,f->preamble);
		strcat(st,"\n");
	}
	strcat(st,buf);
	if(strlen(f->postop)){
		strcat(st,"\n\t");
		strcat(st,f->postop);
	}
	s=st;
	if(*s=='\t')
		++s;
	return s;
	
};

static char *buildsstr(char *in){
	char buf[2048];
	char *d=buf;
	char *s=in;
	while(*s){
		if(*s=='%')
			*d++='%'; /*always need two here */
		*d++=*s++;
	}
	*d=0;
	s=xmalloc(strlen(buf)+1);
	strcpy(s,buf);
	return s;
}

void icvm_addsyscall(char *funcname,int operand,char *preamble,char *postop){
	syscallinfo *f;
	if(findsyscall(funcname))
		return; /* already here */
	f=(syscallinfo *)xmalloc(sizeof(syscallinfo));
	f->funcname=(char *)xmalloc(strlen(funcname)+1);
	strcpy(f->funcname,funcname);
	if(strlen(preamble))
		f->preamble=buildsstr(preamble);
	else
		f->preamble="";
	if(strlen(postop))
		f->postop=buildsstr(postop);
	else
		f->postop="";
	f->operand=operand;
	f->next=syscalls;
	syscalls=f;
}


