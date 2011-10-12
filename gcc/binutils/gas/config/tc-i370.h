/* tc-i370.h -- Header file for tc-i370.c.
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

#define TC_I370

#ifdef ANSI_PROTOTYPES
struct fix;
#endif

/* Set the endianness we are using.  Default to big endian.  */
#ifndef TARGET_BYTES_BIG_ENDIAN
#define TARGET_BYTES_BIG_ENDIAN 1
#endif

#ifndef BFD_ASSEMBLER
 #error I370 support requires BFD_ASSEMBLER
#endif

/* The target BFD architecture.  */
#define TARGET_ARCH (i370_arch ())
extern enum bfd_architecture i370_arch PARAMS ((void));

/* Whether or not the target is big endian */
extern int target_big_endian;

/* The target BFD format.  */
#ifdef OBJ_ELF
#define TARGET_FORMAT ("elf32-i370")
#endif

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

/* $ is used to refer to the current location.  */
#define DOLLAR_DOT

#ifdef OBJ_ELF
#define DIFF_EXPR_OK		/* foo-. gets turned into PC relative relocs */
#endif

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

/* We set the fx_done field appropriately in md_apply_fix.  */
#define TC_HANDLES_FX_DONE


#ifdef OBJ_ELF

/* Branch prediction relocations must force relocation.  */
#define TC_FORCE_RELOCATION_SECTION(FIXP,SEC) 1	

/* Support for SHF_EXCLUDE and SHT_ORDERED */
extern int i370_section_letter PARAMS ((int, char **));
extern int i370_section_type PARAMS ((char **));
extern int i370_section_word PARAMS ((char **));
extern int i370_section_flags PARAMS ((int, int, int));

#define md_elf_section_letter(LETTER, PTR_MSG)	i370_section_letter (LETTER, PTR_MSG)
#define md_elf_section_type(PTR_STR)		i370_section_type (PTR_STR)
#define md_elf_section_word(PTR_STR)		i370_section_word (PTR_STR)
#define md_elf_section_flags(FLAGS, ATTR, TYPE)	i370_section_flags (FLAGS, ATTR, TYPE)

/* Add extra PPC sections -- Note, for now, make .sbss2 and .PPC.EMB.sbss0 a
   normal section, and not a bss section so that the linker doesn't crater
   when trying to make more than 2 sections.  */
// #define ELF_TC_SPECIAL_SECTIONS \
//   { ".tags",		SHT_ORDERED,	SHF_ALLOC }, \
//   { ".sdata",		SHT_PROGBITS,	SHF_ALLOC + SHF_WRITE }, \
//   { ".sbss",		SHT_NOBITS,	SHF_ALLOC + SHF_WRITE }, 

#define tc_comment_chars i370_comment_chars
extern const char *i370_comment_chars;

/* Keep relocations relative to the GOT, or non-PC relative. */
// #define tc_fix_adjustable(FIX)                          		\
//   ((FIX)->fx_r_type != BFD_RELOC_16_GOTOFF              		\
//    && (FIX)->fx_r_type != BFD_RELOC_LO16_GOTOFF         		\
//    && (FIX)->fx_r_type != BFD_RELOC_HI16_GOTOFF         		\
//    && (FIX)->fx_r_type != BFD_RELOC_HI16_S_GOTOFF       		\
//    && (FIX)->fx_r_type != BFD_RELOC_GPREL16             		\
//    && ! S_IS_EXTERNAL ((FIX)->fx_addsy)					\
//    && ! S_IS_WEAK ((FIX)->fx_addsy)					\
//    && ((FIX)->fx_pcrel				        		\
//        || ((FIX)->fx_subsy != NULL					\
// 	   && (S_GET_SEGMENT ((FIX)->fx_subsy)				\
// 	       == S_GET_SEGMENT ((FIX)->fx_addsy)))			\
//        || strchr (S_GET_NAME ((FIX)->fx_addsy), '\001') != NULL		\
//        || strchr (S_GET_NAME ((FIX)->fx_addsy), '\002') != NULL))

/* We must never ever try to resolve references to externally visible
   symbols in the assembler, because the .o file might go into a shared
   library, and some other shared library might override that symbol.  */
#define TC_RELOC_RTSYM_LOC_FIXUP(FIX)  \
  ((FIX)->fx_addsy == NULL \
   || (! S_IS_EXTERNAL ((FIX)->fx_addsy) \
       && ! S_IS_WEAK ((FIX)->fx_addsy) \
       && S_IS_DEFINED ((FIX)->fx_addsy) \
       && ! S_IS_COMMON ((FIX)->fx_addsy)))

#endif /* OBJ_ELF */

/* call md_apply_fix3 with segment instead of md_apply_fix */
#define MD_APPLY_FIX3

/* call md_pcrel_from_section, not md_pcrel_from */
#define MD_PCREL_FROM_SECTION(FIXP, SEC) md_pcrel_from_section(FIXP, SEC)
extern long md_pcrel_from_section PARAMS ((struct fix *, segT));

// we don't do anyything special to recognize funky symbols of any sort ...
// #define md_parse_name(name, exp) i370_parse_name (name, exp)
extern int i370_parse_name PARAMS ((const char *, struct expressionS *));

#define md_operand(x)

