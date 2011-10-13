/* This file is tc-icvm.h
   Copyright (C) 1987, 89, 90, 91, 92, 93, 94, 95, 96, 1997
   Free Software Foundation, Inc.

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
#ifndef _tcicvm_h
#define _tcicvm_h
#include "struc-symbol.h"
#include "write.h"
#include "tc.h"
#define TC_ICVM 1

#define ICVM_ENABLE_DUAL_ENDIAN 1	/* to enable dual endian writes */


/* big/little endian support stuff */
#define LTEXT_SECTION_NAME	".ltext"
#define LDATA_SECTION_NAME	".ldata"
#define LRDATA_SECTION_NAME	".lrdata"
#define LBSS_SECTION_NAME	".lbss"
#define LCTORS_SECTION_NAME	".lctors"
#define LDTORS_SECTION_NAME	".ldtors"
#define BTEXT_SECTION_NAME	".btext"
#define BDATA_SECTION_NAME	".bdata"
#define BRDATA_SECTION_NAME	".brdata"
#define BBSS_SECTION_NAME	".bbss"
#define BCTORS_SECTION_NAME	".bctors"
#define BDTORS_SECTION_NAME	".bdtors"

extern segT Atext_section, Adata_section, Abss_section;
extern segT BEtext_section, BEdata_section, BErdata_section,BEbss_section,
	BEctors_section,BEdtors_section;
extern segT LEtext_section, LEdata_section, LErdata_section,LEbss_section,
	LEctors_section,LEdtors_section;
extern segT comm_section;

#define text_section (target_big_endian ? BEtext_section : LEtext_section) 
#define data_section (target_big_endian ? BEdata_section : LEdata_section) 
#define rdata_section (target_big_endian ? BErdata_section : LErdata_section) 
#define ctors_section (target_big_endian ? BEctors_section : LEctors_section) 
#define dtors_section (target_big_endian ? BEdtors_section : LEdtors_section) 
#define bss_section (target_big_endian ? BEbss_section : LEbss_section) 

#ifdef ANSI_PROTOTYPES
struct symbol;
struct fix;
#endif
																
#define TARGET_BYTES_BIG_ENDIAN 0


#ifdef OBJ_ELF
#define TARGET_FORMAT "elf32-icvm"
#endif

				
#define BFD_ARCH  bfd_arch_icvm  /* for non-BFD_ASSEMBLER */
#define TARGET_ARCH bfd_arch_icvm  /* BFD_ASSEMBLER */


#define tc_comment_chars icvm_comment_chars
extern const char *icvm_comment_chars;

#define tc_crawl_symbol_chain(a)	{;}	/* not used */
#define tc_headers_hook(a)		{;}	/* not used */
#define tc_aout_pre_write_hook(x)	{;}	/* not used */

#define LISTING_WORD_SIZE 2	/* A word is 2 bytes */
#define LISTING_LHS_WIDTH 2	/* One word on the first line */
#define LISTING_LHS_WIDTH_SECOND 2	/* One word on the second line */
#define LISTING_LHS_CONT_LINES 4/* And 4 lines max */
#define LISTING_HEADER "ICVM GAS "

#ifndef REGISTER_PREFIX
#define REGISTER_PREFIX '%'
#endif

#if !defined (REGISTER_PREFIX_OPTIONAL)
#if defined (ICVMCOFF) || defined (OBJ_ELF)

#ifndef BFD_ASSEMBLER
#define LOCAL_LABEL(name) (name[0] == '.' \
			   && (name[1] == 'L' || name[1] == '.'))
#endif /* ! BFD_ASSEMBLER */

#define REGISTER_PREFIX_OPTIONAL 0
#else /* ! (COFF || ELF) */
#define REGISTER_PREFIX_OPTIONAL 1
#endif /* ! (COFF || ELF) */
#endif /* not def REGISTER_PREFIX and not def OPTIONAL_REGISTER_PREFIX */


extern void icvm_mri_mode_change PARAMS ((int));
#define MRI_MODE_CHANGE(i) icvm_mri_mode_change (i)

extern int icvm_conditional_pseudoop PARAMS ((pseudo_typeS *));
#define tc_conditional_pseudoop(pop) icvm_conditional_pseudoop (pop)

extern void icvm_frob_label PARAMS ((struct symbol *));
#define tc_frob_label(sym) icvm_frob_label (sym)

extern void icvm_flush_pending_output PARAMS ((void));
#define md_flush_pending_output() icvm_flush_pending_output ()

extern void icvm_frob_symbol PARAMS ((struct symbol *));


#ifdef BFD_ASSEMBLER

#define tc_frob_symbol(sym,punt)				\
do								\
  {								\
    if (S_GET_SEGMENT (sym) == reg_section)			\
      punt = 1;							\
    icvm_frob_symbol (sym);					\
  }								\
while (0)

#define NO_RELOC BFD_RELOC_NONE

#ifdef OBJ_ELF

/* This expression evaluates to false if the relocation is for a local object
   for which we still want to do the relocation at runtime.  True if we
   are willing to perform this relocation while building the .o file.  */

#define TC_RELOC_RTSYM_LOC_FIXUP(FIX)			\
	((FIX)->fx_r_type != BFD_RELOC_8_PLT_PCREL	\
	 && (FIX)->fx_r_type != BFD_RELOC_16_PLT_PCREL	\
	 && (FIX)->fx_r_type != BFD_RELOC_32_PLT_PCREL	\
	 && (FIX)->fx_r_type != BFD_RELOC_8_GOT_PCREL	\
	 && (FIX)->fx_r_type != BFD_RELOC_16_GOT_PCREL	\
	 && (FIX)->fx_r_type != BFD_RELOC_32_GOT_PCREL)

#define tc_fix_adjustable(X) tc_icvm_fix_adjustable(X)
extern int tc_icvm_fix_adjustable PARAMS ((struct fix *));
#endif

#else /* ! BFD_ASSEMBLER */

#define tc_frob_coff_symbol(sym) icvm_frob_symbol (sym)

#define NO_RELOC 0

#endif /* ! BFD_ASSEMBLER */

#define DIFF_EXPR_OK

extern void icvm_init_after_args PARAMS ((void));
#define tc_init_after_args icvm_init_after_args

extern int icvm_parse_long_option PARAMS ((char *));
#define md_parse_long_option icvm_parse_long_option

#define md_operand(x)

#define TARGET_WORD_SIZE 32

extern struct relax_type md_relax_table[];
#define TC_GENERIC_RELAX_TABLE md_relax_table

/* Copied from write.c */
/* This was formerly called ICVM_AIM_KLUDGE.  */
#define md_prepare_relax_scan(fragP, address, aim, this_state, this_type) \
  if (aim==0 && this_state== 4) { /* hard encoded from tc-icvm.c */ \
    aim=this_type->rlx_forward+1; /* Force relaxation into word mode */ \
  }

/* end of tc-icvm.h */
#endif


