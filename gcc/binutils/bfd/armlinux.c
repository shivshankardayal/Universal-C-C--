/* BFD back-end for linux flavored ARM a.out binaries.
   Copyright (C) 1997 Free Software Foundation, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.  */

#define N_TXTADDR(x) \
        (N_IS_QMAGIC(x) ? EXEC_BYTES_SIZE + TARGET_PAGE_SIZE : \
                  N_MAGIC(x) == NMAGIC ? TARGET_PAGE_SIZE : \
                            N_MAGIC(x) != ZMAGIC ? 0 : \
                                      TEXT_START_ADDR)

#define TEXT_START_ADDR		0x8000
#define TARGET_PAGE_SIZE	0x8000
#define SEGMENT_SIZE		TARGET_PAGE_SIZE
#define ZMAGIC_DISK_BLOCK_SIZE	1024
#define N_SHARED_LIB(x)		0
#define BYTES_IN_WORD		4
#define N_HEADER_IN_TEXT(x)	0
#define ARCH_SIZE		32

/* Avoid multiple defininitions from aoutx if supporting standarad
   a.out as well as our own.  */
#define NAME(x,y) CAT3(armlinux,_32_,y)

#include "bfd.h"
#include "sysdep.h"
#include "assert.h"
/*#include "libbfd.h"*/
#include "aout/aout64.h"
#include "aout/stab_gnu.h"
#include "aout/ar.h"

#if 0
#define WRITE_HEADERS(abfd, execp)					   \
  {									   \
    bfd_size_type text_size; /* dummy vars */				   \
    file_ptr text_end;							   \
    if (adata(abfd).magic == undecided_magic)				   \
      NAME(aout,adjust_sizes_and_vmas) (abfd, &text_size, &text_end);	   \
									   \
    execp->a_syms = bfd_get_symcount (abfd) * EXTERNAL_NLIST_SIZE;	   \
    execp->a_entry = bfd_get_start_address (abfd);			   \
									   \
    execp->a_trsize = ((obj_textsec (abfd)->reloc_count) *		   \
		       obj_reloc_entry_size (abfd));			   \
    execp->a_drsize = ((obj_datasec (abfd)->reloc_count) *		   \
		       obj_reloc_entry_size (abfd));			   \
    NAME(aout,swap_exec_header_out) (abfd, execp, &exec_bytes); 	   \
									   \
    if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0) return false;	   \
    if (bfd_write ((PTR) &exec_bytes, 1, EXEC_BYTES_SIZE, abfd) 	   \
	!= EXEC_BYTES_SIZE)						   \
      return false;							   \
    /* Now write out reloc info, followed by syms and strings */	   \
									   \
    if (bfd_get_outsymbols (abfd) != (asymbol **) NULL			   \
	&& bfd_get_symcount (abfd) != 0)				   \
      { 								   \
	if (bfd_seek (abfd, (file_ptr)(N_SYMOFF(*execp)), SEEK_SET) != 0)  \
	  return false; 						   \
	if (! NAME(aout,write_syms)(abfd)) return false;		   \
									   \
	if (bfd_seek (abfd, (file_ptr)(N_TRELOFF(*execp)), SEEK_SET) != 0) \
	  return false; 						   \
	if (! MY(squirt_out_relocs) (abfd, obj_textsec (abfd))) 	   \
	  return false; 						   \
									   \
	if (bfd_seek (abfd, (file_ptr)(N_DRELOFF(*execp)), SEEK_SET) != 0) \
	  return false; 						   \
	if (!NAME(aout,squirt_out_relocs)(abfd, obj_datasec (abfd)))	   \
	  return false; 						   \
      } 								   \
  }
#endif

#include "libaout.h"		/* BFD a.out internal data structures */

#define DEFAULT_ARCH bfd_arch_arm
#define MY(OP) CAT(armlinux_,OP)
#define TARGETNAME "a.out-arm-linux"

static bfd_reloc_status_type
MY(fix_pcrel_26_done)	PARAMS ((bfd *, arelent *, asymbol *, PTR,
				 asection *, bfd *, char **));
static bfd_reloc_status_type
MY(fix_pcrel_26)	PARAMS ((bfd *, arelent *, asymbol *, PTR,
				 asection *, bfd *, char **));

static void MY(swap_std_reloc_in) PARAMS ((bfd *, struct reloc_std_external *,
					   arelent *, asymbol **,
					   bfd_size_type));

static void MY(swap_std_reloc_out) PARAMS ((bfd *, arelent *,
					    struct reloc_std_external *));

static boolean MY(bfd_final_link)PARAMS ((bfd *, struct bfd_link_info *));

static struct bfd_hash_entry * armlinux_link_hash_newfunc
		PARAMS ((struct bfd_hash_entry *,
			 struct bfd_hash_table *,
			 const char *));

static struct bfd_link_hash_table * armlinux_link_hash_table_create
		PARAMS ((bfd *));

static boolean MY(write_object_contents) PARAMS ((bfd *));

static boolean armlinux_link_create_dynamic_sections
		PARAMS ((bfd *, struct bfd_link_info *));

static boolean armlinux_add_one_symbol
		PARAMS ((struct bfd_link_info *, bfd *, const char *,
			 flagword, asection *, bfd_vma, const char *,
			 boolean, boolean,
			 struct bfd_link_hash_entry **));

static boolean armlinux_finish_dynamic_link
		PARAMS ((bfd *, struct bfd_link_info *));

extern reloc_howto_type * MY(reloc_howto)
		PARAMS ((bfd *, struct reloc_std_external *,
			 int *, int *, int *));
extern void MY(put_reloc) PARAMS ((bfd *, int, int, long,
				   reloc_howto_type *,
				   struct reloc_std_external *));

extern void MY(relocatable_reloc) PARAMS ((reloc_howto_type *, bfd *,
					   struct reloc_std_external *,
					   bfd_vma *, bfd_vma));

extern reloc_howto_type * MY(reloc_type_lookup)
		PARAMS ((bfd *, bfd_reloc_code_real_type));

reloc_howto_type MY(howto_table)[] = {
  /* type   rs size bsz pcrel bitpos ovrf                     sf		  name     part_inpl readmask setmask     pcdone */
  HOWTO( 0, 0, 0,   8,  false, 0, complain_overflow_bitfield, 0			   , "8",      true, 0x000000ff, 0x000000ff, false),
  HOWTO( 1, 0, 1,   16, false, 0, complain_overflow_bitfield, 0			   , "16",     true, 0x0000ffff, 0x0000ffff, false),
  HOWTO( 2, 0, 2,   32, false, 0, complain_overflow_bitfield, 0			   , "32",     true, 0xffffffff, 0xffffffff, false),
  HOWTO( 3, 2, 2,   26, true,  0, complain_overflow_signed,   MY(fix_pcrel_26)	   , "ARM26",  true, 0x00ffffff, 0x00ffffff, true),
  HOWTO( 4, 0, 0,   8,  true,  0, complain_overflow_signed,   0			   , "DISP8",  true, 0x000000ff, 0x000000ff, true),
  HOWTO( 5, 0, 1,   16, true,  0, complain_overflow_signed,   0			   , "DISP16", true, 0x0000ffff, 0x0000ffff, true),
  HOWTO( 6, 0, 2,   32, true,  0, complain_overflow_signed,   0			   , "DISP32", true, 0xffffffff, 0xffffffff, true),
  HOWTO( 7, 2, 2,   26, false, 0, complain_overflow_dont,     MY(fix_pcrel_26_done), "ARM26D", true, 0x00000000, 0x00000000, false),
  {-1},
  HOWTO( 9, 0, -1,  16, false, 0, complain_overflow_bitfield, 0			   , "NEG16",  true, 0x0000ffff, 0x0000ffff, false),
  HOWTO(10, 0, -2,  32, false, 0, complain_overflow_bitfield, 0			   , "NEG32",  true, 0xffffffff, 0xffffffff, false),
  {-1}
};

#define RELOC_ARM_BITS_NEG_BIG      ((unsigned int) 0x08)
#define RELOC_ARM_BITS_NEG_LITTLE   ((unsigned int) 0x10)

#define ARMAOUT_TABLE_SIZE \
  (sizeof (MY(howto_table)) / sizeof (reloc_howto_type))
const int MY(table_size) = ARMAOUT_TABLE_SIZE;

reloc_howto_type *
MY(reloc_howto)(abfd, rel, r_index, r_extern, r_pcrel)
     bfd *abfd;
     struct reloc_std_external *rel;
     int *r_index;
     int *r_extern;
     int *r_pcrel;
{
  unsigned int r_length;
  unsigned int r_pcrel_done;
  unsigned int r_neg;
  int index;

  *r_pcrel = 0;
  if (bfd_header_big_endian (abfd))
    {
      *r_index     =  ((rel->r_index[0] << 16)
		       | (rel->r_index[1] << 8)
		       | rel->r_index[2]);
      *r_extern    = (0 != (rel->r_type[0] & RELOC_STD_BITS_EXTERN_BIG));
      r_pcrel_done = (0 != (rel->r_type[0] & RELOC_STD_BITS_PCREL_BIG));
      r_neg        = (0 != (rel->r_type[0] & RELOC_ARM_BITS_NEG_BIG));
      r_length     = ((rel->r_type[0] & RELOC_STD_BITS_LENGTH_BIG)
		      >> RELOC_STD_BITS_LENGTH_SH_BIG);
    }
  else
    {
      *r_index     = ((rel->r_index[2] << 16)
		      | (rel->r_index[1] << 8)
		      | rel->r_index[0]);
      *r_extern    = (0 != (rel->r_type[0] & RELOC_STD_BITS_EXTERN_LITTLE));
      r_pcrel_done = (0 != (rel->r_type[0] & RELOC_STD_BITS_PCREL_LITTLE));
      r_neg        = (0 != (rel->r_type[0] & RELOC_ARM_BITS_NEG_LITTLE));
      r_length     = ((rel->r_type[0] & RELOC_STD_BITS_LENGTH_LITTLE)
		      >> RELOC_STD_BITS_LENGTH_SH_LITTLE);
    }
  index = r_length + 4 * r_pcrel_done + 8 * r_neg;
  if (index == 3)
    *r_pcrel = 1;

  return MY(howto_table) + index;
}

#define MY_reloc_howto(BFD, REL, IN, EX, PC) \
	MY(reloc_howto) (BFD, REL, &IN, &EX, &PC)

void
MY(put_reloc)(abfd, r_extern, r_index, value, howto, reloc)
     bfd *abfd;
     int r_extern;
     int r_index;
     long value;
     reloc_howto_type *howto;
     struct reloc_std_external *reloc;
{
  unsigned int r_length;
  int r_pcrel;
  int r_neg;

  PUT_WORD(abfd, value, reloc->r_address);
  r_length = howto->size;	/* size as a power of two */

  /* Special case for branch relocations. */
  if (howto->type == 3 || howto->type == 7)
    r_length = 3;

  r_pcrel  = howto->type & 4;   /* PC Relative done? */
  r_neg = howto->type & 8;      /* Negative relocation */
  if (bfd_header_big_endian (abfd))
    {
      reloc->r_index[0] = r_index >> 16;
      reloc->r_index[1] = r_index >> 8;
      reloc->r_index[2] = r_index;
      reloc->r_type[0] =
        ((r_extern ?     RELOC_STD_BITS_EXTERN_BIG : 0)
         | (r_pcrel ?    RELOC_STD_BITS_PCREL_BIG : 0)
         | (r_neg ?      RELOC_ARM_BITS_NEG_BIG : 0)
         | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG));
    }
  else
    {
      reloc->r_index[2] = r_index >> 16;
      reloc->r_index[1] = r_index >> 8;
      reloc->r_index[0] = r_index;
      reloc->r_type[0] =
        ((r_extern ?     RELOC_STD_BITS_EXTERN_LITTLE : 0)
         | (r_pcrel ?    RELOC_STD_BITS_PCREL_LITTLE : 0)
         | (r_neg ?      RELOC_ARM_BITS_NEG_LITTLE : 0)
         | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE));
    }
}

#define MY_put_reloc(BFD, EXT, IDX, VAL, HOWTO, RELOC) \
	MY(put_reloc)(BFD, EXT, IDX, VAL, HOWTO, RELOC)

void
MY(relocatable_reloc)(howto, abfd, reloc, amount, r_addr)
     reloc_howto_type *howto;
     bfd *abfd;
     struct reloc_std_external *reloc;
     bfd_vma *amount;
     bfd_vma r_addr;
{
  if (howto->type == 3)
    {
      if (reloc->r_type[0]
	  & (bfd_header_big_endian (abfd)
	     ? RELOC_STD_BITS_EXTERN_BIG : RELOC_STD_BITS_EXTERN_LITTLE))
	{
	  /* The reloc is still external, so don't modify anything. */
	  *amount = 0;
	}
      else
	{
	  *amount -= r_addr;
	  /* Change the r_pcrel value -- on the ARM, this bit is set once the
	     relocation is done.  */
	  if (bfd_header_big_endian (abfd))
	    reloc->r_type[0] |= RELOC_STD_BITS_PCREL_BIG;
	  else
	    reloc->r_type[0] |= RELOC_STD_BITS_PCREL_LITTLE;
	}
    }
  else if (howto->type == 7)
    *amount = 0;
}

#define MY_relocatable_reloc(HOW, BFD, REL, AMOUNT, ADDR) \
  MY(relocatable_reloc)(HOW, BFD, REL, &(AMOUNT), ADDR)

static bfd_reloc_status_type
MY(fix_pcrel_26_done) (abfd, reloc_entry, symbol, data, input_section,
		       output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  /* This is dead simple at present.  */
  return bfd_reloc_ok;
}

static bfd_reloc_status_type
MY(fix_pcrel_26) (abfd, reloc_entry, symbol, data, input_section,
		  output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message;
{
  bfd_vma relocation;
  bfd_size_type addr = reloc_entry->address;
  long target = bfd_get_32 (abfd, (bfd_byte *) data + addr);
  bfd_reloc_status_type flag = bfd_reloc_ok;

  /* If this is an undefined symbol, return error */
  if (symbol->section == &bfd_und_section
      && (symbol->flags & BSF_WEAK) == 0)
    return output_bfd ? bfd_reloc_ok : bfd_reloc_undefined;

  /* If the sections are different, and we are doing a partial relocation,
     just ignore it for now.  */
  if (symbol->section->name != input_section->name
      && output_bfd != (bfd *)NULL)
    return bfd_reloc_ok;

  relocation = (target & 0x00ffffff) << 2;
  relocation = (relocation ^ 0x02000000) - 0x02000000; /* Sign extend */
  relocation += symbol->value;
  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc_entry->addend;
  relocation -= input_section->output_section->vma;
  relocation -= input_section->output_offset;
  relocation -= addr;
  if (relocation & 3)
    return bfd_reloc_overflow;

  /* Check for overflow */
  if (relocation & 0x02000000)
    {
      if ((relocation & ~0x03ffffff) != ~0x03ffffff)
        flag = bfd_reloc_overflow;
    }
  else if (relocation & ~0x03ffffff)
    flag = bfd_reloc_overflow;

  target &= ~0x00ffffff;
  target |= (relocation >> 2) & 0x00ffffff;
  bfd_put_32 (abfd, target, (bfd_byte *) data + addr);

  /* Now the ARM magic... Change the reloc type so that it is marked as done.
     Strictly this is only necessary if we are doing a partial relocation.  */
  reloc_entry->howto = &MY(howto_table)[7];

  return flag;
}

reloc_howto_type *
MY(reloc_type_lookup) (abfd, code)
      bfd *abfd;
      bfd_reloc_code_real_type code;
{
#define ASTD(i,j)	case i: return &MY(howto_table)[j]
  if (code == BFD_RELOC_CTOR)
    switch (bfd_get_arch_info (abfd)->bits_per_address)
      {
      	case 32:
      	  code = BFD_RELOC_32;
      	  break;
      	default: return (CONST struct reloc_howto_struct *) 0;
      }

  switch (code)
    {
      ASTD (BFD_RELOC_16, 1);
      ASTD (BFD_RELOC_32, 2);
      ASTD (BFD_RELOC_ARM_PCREL_BRANCH, 3);
      ASTD (BFD_RELOC_8_PCREL, 4);
      ASTD (BFD_RELOC_16_PCREL, 5);
      ASTD (BFD_RELOC_32_PCREL, 6);
    default: return (CONST struct reloc_howto_struct *) 0;
    }
}

#define MY_bfd_reloc_type_lookup armlinux_reloc_type_lookup
#define MY_swap_std_reloc_in MY(swap_std_reloc_in)
#define MY_swap_std_reloc_out MY(swap_std_reloc_out)
#define MY_get_section_contents _bfd_generic_get_section_contents
#define MY_bfd_final_link armlinux_bfd_final_link

#include "aoutx.h"

static void
MY(swap_std_reloc_in) (abfd, bytes, cache_ptr, symbols, symcount)
     bfd *abfd;
     struct reloc_std_external *bytes;
     arelent *cache_ptr;
     asymbol **symbols;
     bfd_size_type symcount;
{
  int r_index;
  int r_extern;
  unsigned int r_length;
  int r_pcrel;
  struct aoutdata *su = &(abfd->tdata.aout_data->a);

  cache_ptr->address = bfd_h_get_32 (abfd, bytes->r_address);

  cache_ptr->howto = MY_reloc_howto (abfd, bytes, r_index, r_extern, r_pcrel);

  MOVE_ADDRESS (0);
}

static void
MY(swap_std_reloc_out) (abfd, g, natptr)
     bfd *abfd;
     arelent *g;
     struct reloc_std_external *natptr;
{
  int r_index;
  asymbol *sym = *(g->sym_ptr_ptr);
  int r_extern;
  int r_length;
  int r_pcrel;
  int r_neg = 0;	/* Negative relocs use the BASEREL bit.  */
  asection *output_section = sym->section->output_section;

  PUT_WORD(abfd, g->address, natptr->r_address);

  r_length = g->howto->size;	/* Size as a power of two */
  if (r_length < 0)
    {
      r_length = -r_length;
      r_neg = 1;
    }

  r_pcrel = (int) g->howto->pc_relative; /* Relative to PC? */

  /* For Linux, in pc-relative relocs the r_pcrel bit means that the
     relocation has been done already (Only for the 26-bit one I think)???!!! */

  if (g->howto->type == 3)
    {
      r_length = 3;
      r_pcrel = 0;
    }
  else if (g->howto->type == 7)
    {
      r_length = 3;
      r_pcrel = 1;
    }

  /* name was clobbered by aout_write_syms to be symbol index */

  /* If this relocation is relative to a symbol then set the
     r_index to the symbols index, and the r_extern bit.

     Absolute symbols can come in in two ways, either as an offset
     from the abs section, or as a symbol which has an abs value.
     check for that here
     */

  if (bfd_is_com_section (output_section)
      || output_section == &bfd_abs_section
      || output_section == &bfd_und_section)
    {
      if (bfd_abs_section.symbol == sym)
        {
          /* Whoops, looked like an abs symbol, but is really an offset
             from the abs section */
          r_index = 0;
          r_extern = 0;
        }
      else
        {
          /* Fill in symbol */
          r_extern = 1;
          r_index =  (*(g->sym_ptr_ptr))->KEEPIT;
        }
    }
  else
    {
      /* Just an ordinary section */
      r_extern = 0;
      r_index  = output_section->target_index;
    }

  /* now the fun stuff */
  if (bfd_header_big_endian (abfd))
    {
      natptr->r_index[0] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[2] = r_index;
      natptr->r_type[0] =
        (  (r_extern ?   RELOC_STD_BITS_EXTERN_BIG: 0)
         | (r_pcrel  ?   RELOC_STD_BITS_PCREL_BIG: 0)
         | (r_neg    ?   RELOC_ARM_BITS_NEG_BIG: 0)
         | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG));
    }
  else
    {
      natptr->r_index[2] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[0] = r_index;
      natptr->r_type[0] =
        (  (r_extern ?   RELOC_STD_BITS_EXTERN_LITTLE: 0)
         | (r_pcrel  ?   RELOC_STD_BITS_PCREL_LITTLE: 0)
         | (r_neg    ?   RELOC_ARM_BITS_NEG_LITTLE: 0)
         | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE));
    }
}

/* Set the machine type correctly.  */
static boolean
MY(write_object_contents) (abfd)
     bfd *abfd;
{
  struct external_exec exec_bytes;
  struct internal_exec *execp = exec_hdr (abfd);

  N_SET_MACHTYPE (*execp, M_ARM);

  obj_reloc_entry_size (abfd) = RELOC_STD_SIZE;

  WRITE_HEADERS(abfd, execp);

  return true;
}

#define MY_write_object_contents armlinux_write_object_contents

/* We always generate QMAGIC files in preference to ZMAGIC files.  It
   would be possible to make this a linker option, if that ever
   becomes important.  */

static void MY_final_link_callback
  PARAMS ((bfd *, file_ptr *, file_ptr *, file_ptr *));

static boolean
MY(bfd_final_link) (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  obj_aout_subformat (abfd) = q_magic_format;
  return NAME(aout,final_link) (abfd, info, MY_final_link_callback);
}
          

/* Code to link against Linux a.out shared libraries.  */

/* See if a symbol name is a reference to the global offset table.  */
#ifndef GOT_REF_PREFIX
#define	GOT_REF_PREFIX	"__GOT_"
#endif

#define IS_GOT_SYM(name) \
  (strncmp (name, GOT_REF_PREFIX, sizeof GOT_REF_PREFIX - 1) == 0)

/* See if a symbol name is a reference to the procedure linkage table.  */

#ifndef PLT_REF_PREFIX
#define	PLT_REF_PREFIX	"__PLT_"
#endif

#define IS_PLT_SYM(name) \
  (strncmp (name, PLT_REF_PREFIX, sizeof PLT_REF_PREFIX - 1) == 0)

/* This string is used to generate specialized error messages.  */

#ifndef NEEDS_SHRLIB
#define NEEDS_SHRLIB "__NEEDS_SHRLIB_"
#endif

/* This special symbol is a set vector that contains a list of
   pointers to fixup tables.  It will be present in any dynamicly
   linked file.  The linker generated fixup table should also be added
   to the list, and it should always appear in the second slot (the
   first one is a dummy with a magic number that is defined in
   crt0.o).  */

#ifndef SHARABLE_CONFLICTS
#define SHARABLE_CONFLICTS "__SHARABLE_CONFLICTS__"
#endif

/* We keep a list of fixups.  The terminology is a bit strange, but
   each fixup contains two 32 bit numbers.  A regular fixup contains
   an address and a pointer, and at runtime we should store the
   address at the location pointed to by the pointer.  A builtin fixup
   contains two pointers, and we should read the address using one
   pointer and store it at the location pointed to by the other
   pointer.  Builtin fixups come into play when we have duplicate
   __GOT__ symbols for the same variable.  The builtin fixup will copy
   the GOT pointer from one over into the other.  */

struct fixup
{
  struct fixup *next;
  struct linux_link_hash_entry *h;
  bfd_vma value;

  /* Nonzero if this is a branch instruction that needs to be fixed,
     zero if this is just a pointer */
  char jump;

  char builtin;
};

/* We don't need a special hash table entry structure, but we do need
   to keep some information between linker passes, so we use a special
   hash table.  */

struct linux_link_hash_entry
{
  struct aout_link_hash_entry root;
};

struct linux_link_hash_table
{
  struct aout_link_hash_table root;

  /* First dynamic object found in link.  */
  bfd *dynobj;

  /* Number of fixups.  */
  size_t fixup_count;

  /* Number of builtin fixups.  */
  size_t local_builtins;

  /* List of fixups.  */
  struct fixup *fixup_list;
};

static struct fixup * new_fixup
		PARAMS ((struct bfd_link_info *,
			 struct linux_link_hash_entry *, bfd_vma value,
			 int));

static boolean armlinux_tally_symbols
		PARAMS ((struct linux_link_hash_entry *, PTR));

static struct bfd_hash_entry *
armlinux_link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct linux_link_hash_entry *ret = (struct linux_link_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct linux_link_hash_entry *) NULL)
    ret = ((struct linux_link_hash_entry *)
	   bfd_hash_allocate (table, sizeof (struct linux_link_hash_entry)));
  if (ret == NULL)
    return (struct bfd_hash_entry *) ret;

  /* Call the allocation method of the superclass.  */
  ret = ((struct linux_link_hash_entry *)
	 NAME(aout,link_hash_newfunc) ((struct bfd_hash_entry *) ret,
				       table, string));
  if (ret != NULL)
    {
      /* Set local fields; there aren't any.  */
    }

  return (struct bfd_hash_entry *) ret;
}

/* Create a Linux link hash table.  */

static struct bfd_link_hash_table *
armlinux_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct linux_link_hash_table *ret;

  ret = ((struct linux_link_hash_table *)
	 bfd_alloc (abfd, sizeof (struct linux_link_hash_table)));
  if (ret == (struct linux_link_hash_table *) NULL)
    return (struct bfd_link_hash_table *) NULL;
  if (! NAME(aout,link_hash_table_init) (&ret->root, abfd,
					 armlinux_link_hash_newfunc))
    {
      free (ret);
      return (struct bfd_link_hash_table *) NULL;
    }

  ret->dynobj = NULL;
  ret->fixup_count = 0;
  ret->local_builtins = 0;
  ret->fixup_list = NULL;

  return &ret->root.root;
}

/* Look up an entry in a Linux link hash table.  */

#define linux_link_hash_lookup(table, string, create, copy, follow)	\
  ((struct linux_link_hash_entry *)					\
   aout_link_hash_lookup (&(table)->root, (string), (create), (copy),	\
			  (follow)))

/* Traverse a Linux link hash table.  */

#define linux_link_hash_traverse(table, func, info)			\
  (aout_link_hash_traverse						\
   (&(table)->root,							\
    (boolean (*) PARAMS ((struct aout_link_hash_entry *, PTR))) (func),	\
    (info)))

/* Get the Linux link hash table from the info structure.  This is
   just a cast.  */

#define linux_hash_table(p) ((struct linux_link_hash_table *) ((p)->hash))

/* Store the information for a new fixup.  */

static struct fixup *
new_fixup (info, h, value, builtin)
     struct bfd_link_info *info;
     struct linux_link_hash_entry *h;
     bfd_vma value;
     int builtin;
{
  struct fixup *f;

  f = (struct fixup *) bfd_hash_allocate (&info->hash->table,
					  sizeof (struct fixup));
  if (f == NULL)
    return f;
  f->next = linux_hash_table (info)->fixup_list;
  linux_hash_table (info)->fixup_list = f;
  f->h = h;
  f->value = value;
  f->builtin = builtin;
  f->jump = 0;
  ++linux_hash_table (info)->fixup_count;
  return f;
}

/* We come here once we realize that we are going to link to a shared
   library.  We need to create a special section that contains the
   fixup table, and we ultimately need to add a pointer to this into
   the set vector for SHARABLE_CONFLICTS.  At this point we do not
   know the size of the section, but that's OK - we just need to
   create it for now.  */

static boolean
armlinux_link_create_dynamic_sections (abfd, info)
     bfd *abfd;
     struct bfd_link_info *info;
{
  flagword flags;
  register asection *s;

  /* Note that we set the SEC_IN_MEMORY flag.  */
  flags = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS | SEC_IN_MEMORY;

  /* We choose to use the name ".linux-dynamic" for the fixup table.
     Why not? */
  s = bfd_make_section (abfd, ".linux-dynamic");
  if (s == NULL
      || ! bfd_set_section_flags (abfd, s, flags)
      || ! bfd_set_section_alignment (abfd, s, 2))
    return false;
  s->_raw_size = 0;
  s->contents = 0;

  return true;
}

/* Function to add a single symbol to the linker hash table.  This is
   a wrapper around _bfd_generic_link_add_one_symbol which handles the
   tweaking needed for dynamic linking support.  */

static boolean
armlinux_add_one_symbol (info, abfd, name, flags, section, value, string,
		      copy, collect, hashp)
     struct bfd_link_info *info;
     bfd *abfd;
     const char *name;
     flagword flags;
     asection *section;
     bfd_vma value;
     const char *string;
     boolean copy;
     boolean collect;
     struct bfd_link_hash_entry **hashp;
{
  struct linux_link_hash_entry *h;
  boolean insert;

  /* Look up and see if we already have this symbol in the hash table.
     If we do, and the defining entry is from a shared library, we
     need to create the dynamic sections.

     FIXME: What if abfd->xvec != info->hash->creator?  We may want to
     be able to link Linux a.out and ELF objects together, but serious
     confusion is possible.  */

  insert = false;

  if (! info->relocateable
      && linux_hash_table (info)->dynobj == NULL
      && strcmp (name, SHARABLE_CONFLICTS) == 0
      && (flags & BSF_CONSTRUCTOR) != 0
      && abfd->xvec == info->hash->creator)
    {
      if (! armlinux_link_create_dynamic_sections (abfd, info))
        return false;
      linux_hash_table (info)->dynobj = abfd;
      insert = true;
    }

  if (bfd_is_abs_section (section)
      && abfd->xvec == info->hash->creator)
    {
      h = linux_link_hash_lookup (linux_hash_table (info), name, false,
				  false, false);
      if (h != NULL
	  && (h->root.root.type == bfd_link_hash_defined
	      || h->root.root.type == bfd_link_hash_defweak))
	{
	  struct fixup *f;

	  if (hashp != NULL)
	    *hashp = (struct bfd_link_hash_entry *) h;

	  f = new_fixup (info, h, value, ! IS_PLT_SYM (name));
	  if (f == NULL)
	    return false;
	  f->jump = IS_PLT_SYM (name);

	  return true;
	}
    }

  /* Do the usual procedure for adding a symbol.  */
  if (! _bfd_generic_link_add_one_symbol (info, abfd, name, flags, section,
					  value, string, copy, collect,
					  hashp))
    return false;

  /* Insert a pointer to our table in the set vector.  The dynamic
     linker requires this information */
  if (insert)
    {
      asection *s;

      /* Here we do our special thing to add the pointer to the
	 dynamic section in the SHARABLE_CONFLICTS set vector.  */
      s = bfd_get_section_by_name (linux_hash_table (info)->dynobj,
				   ".linux-dynamic");
      BFD_ASSERT (s != NULL);

      if (! (_bfd_generic_link_add_one_symbol
	     (info, linux_hash_table (info)->dynobj, SHARABLE_CONFLICTS,
	      BSF_GLOBAL | BSF_CONSTRUCTOR, s, 0, NULL, false, false, NULL)))
	return false;
    }

  return true;
}

/* We will crawl the hash table and come here for every global symbol.
   We will examine each entry and see if there are indications that we
   need to add a fixup.  There are two possible cases - one is where
   you have duplicate definitions of PLT or GOT symbols - these will
   have already been caught and added as "builtin" fixups.  If we find
   that the corresponding non PLT/GOT symbol is also present, we
   convert it to a regular fixup instead.

   This function is called via linux_link_hash_traverse.  */

static boolean
armlinux_tally_symbols (h, data)
     struct linux_link_hash_entry *h;
     PTR data;
{
  struct bfd_link_info *info = (struct bfd_link_info *) data;
  struct fixup *f, *f1;
  int is_plt;
  struct linux_link_hash_entry *h1, *h2;
  boolean exists;

  if (h->root.root.type == bfd_link_hash_undefined
      && strncmp (h->root.root.root.string, NEEDS_SHRLIB,
		  sizeof NEEDS_SHRLIB - 1) == 0)
    {
      const char *name;
      char *p;
      char *alloc = NULL;

      name = h->root.root.root.string + sizeof NEEDS_SHRLIB - 1;
      p = strrchr (name, '_');
      if (p != NULL)
	alloc = (char *) bfd_malloc (strlen (name) + 1);

      if (p == NULL || alloc == NULL)
	(*_bfd_error_handler) ("Output file requires shared library `%s'\n",
			       name);
      else
	{
	  strcpy (alloc, name);
	  p = strrchr (alloc, '_');
	  *p++ = '\0';
	  (*_bfd_error_handler)
	    ("Output file requires shared library `%s.so.%s'\n",
	     alloc, p);
	  free (alloc);
	}

      abort ();
    }

  /* If this symbol is not a PLT/GOT, we do not even need to look at it */
  is_plt = IS_PLT_SYM (h->root.root.root.string);

  if (is_plt || IS_GOT_SYM (h->root.root.root.string))
    {
      /* Look up this symbol twice.  Once just as a regular lookup,
	 and then again following all of the indirect links until we
	 reach a real symbol.  */
      h1 = linux_link_hash_lookup (linux_hash_table (info),
				   (h->root.root.root.string
				    + sizeof PLT_REF_PREFIX - 1),
				   false, false, true);
      /* h2 does not follow indirect symbols. */
      h2 = linux_link_hash_lookup (linux_hash_table (info), 
				   (h->root.root.root.string
				    + sizeof PLT_REF_PREFIX - 1),
				   false, false, false);

      /* The real symbol must exist but if it is also an ABS symbol,
	 there is no need to have a fixup.  This is because they both
	 came from the same library.  If on the other hand, we had to
	 use an indirect symbol to get to the real symbol, we add the
	 fixup anyway, since there are cases where these symbols come
	 from different shared libraries */
      if (h1 != NULL
	  && (((h1->root.root.type == bfd_link_hash_defined
		|| h1->root.root.type == bfd_link_hash_defweak)
	       && ! bfd_is_abs_section (h1->root.root.u.def.section))
	      || h2->root.root.type == bfd_link_hash_indirect))
	{
	  /* See if there is a "builtin" fixup already present
	     involving this symbol.  If so, convert it to a regular
	     fixup.  In the end, this relaxes some of the requirements
	     about the order of performing fixups.  */
	  exists = false;
	  for (f1 = linux_hash_table (info)->fixup_list;
	       f1 != NULL;
	       f1 = f1->next)
	    {
	      if ((f1->h != h && f1->h != h1)
		  || (! f1->builtin && ! f1->jump))
		continue;
	      if (f1->h == h1)
		exists = true;
	      if (! exists
		  && bfd_is_abs_section (h->root.root.u.def.section))
		{
		  f = new_fixup (info, h1, f1->h->root.root.u.def.value, 0);
		  f->jump = is_plt;
		}
	      f1->h = h1;
	      f1->jump = is_plt;
	      f1->builtin = 0;
	      exists = true;
	    }
	  if (! exists
	      && bfd_is_abs_section (h->root.root.u.def.section))
	    {
	      f = new_fixup (info, h1, h->root.root.u.def.value, 0);
	      if (f == NULL)
		{
		  /* FIXME: No way to return error.  */
		  abort ();
		}
	      f->jump = is_plt;
	    }
	}

      /* Quick and dirty way of stripping these symbols from the
	 symtab. */
      if (bfd_is_abs_section (h->root.root.u.def.section))
	h->root.written = true;
    }

  return true;
}

extern const bfd_target MY(vec);

/* This is called to set the size of the .linux-dynamic section is.
   It is called by the Linux linker emulation before_allocation
   routine.  We have finished reading all of the input files, and now
   we just scan the hash tables to find out how many additional fixups
   are required.  */

boolean
bfd_armlinux_size_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  struct fixup *f;
  asection *s;

  if (output_bfd->xvec != &MY(vec))
    return true;

  /* First find the fixups... */
  linux_link_hash_traverse (linux_hash_table (info),
			    armlinux_tally_symbols,
			    (PTR) info);

  /* If there are builtin fixups, leave room for a marker.  This is
     used by the dynamic linker so that it knows that all that follow
     are builtin fixups instead of regular fixups.  */
  for (f = linux_hash_table (info)->fixup_list; f != NULL; f = f->next)
    {
      if (f->builtin)
	{
	  ++linux_hash_table (info)->fixup_count;
	  ++linux_hash_table (info)->local_builtins;
	  break;
	}
    }

  if (linux_hash_table (info)->dynobj == NULL)
    {
      if (linux_hash_table (info)->fixup_count > 0)
	abort ();
      return true;
    }

  /* Allocate memory for our fixup table.  We will fill it in later.  */
  s = bfd_get_section_by_name (linux_hash_table (info)->dynobj,
			       ".linux-dynamic");
  if (s != NULL)
    {
      s->_raw_size = 8 + linux_hash_table (info)->fixup_count * 8;
      s->contents = (bfd_byte *) bfd_alloc (output_bfd, s->_raw_size);
      if (s->contents == NULL)
	return false;
      memset (s->contents, 0, (size_t) s->_raw_size);
    }

  return true;
}

/* We come here once we are ready to actually write the fixup table to
   the output file.  Scan the fixup tables and so forth and generate
   the stuff we need.  */

static boolean
armlinux_finish_dynamic_link (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  asection *s, *os, *is;
  bfd_byte *fixup_table;
  struct linux_link_hash_entry *h;
  struct fixup *f;
  unsigned int new_addr;
  int section_offset;
  unsigned int fixups_written;

  if (linux_hash_table (info)->dynobj == NULL)
    return true;

  s = bfd_get_section_by_name (linux_hash_table (info)->dynobj,
			       ".linux-dynamic");
  BFD_ASSERT (s != NULL);
  os = s->output_section;
  fixups_written = 0;

#ifdef LINUX_LINK_DEBUG
  printf ("Fixup table file offset: %x  VMA: %x\n", 
	  os->filepos + s->output_offset,
	  os->vma + s->output_offset);
#endif

  fixup_table = s->contents;
  bfd_put_32 (output_bfd, linux_hash_table (info)->fixup_count, fixup_table);
  fixup_table += 4;

  /* Fill in fixup table.  */
  for (f = linux_hash_table (info)->fixup_list; f != NULL; f = f->next)
    {
      if (f->builtin)
	continue;

      if (f->h->root.root.type != bfd_link_hash_defined
	  && f->h->root.root.type != bfd_link_hash_defweak)
	{
	  (*_bfd_error_handler)
	    ("Symbol %s not defined for fixups\n",
	     f->h->root.root.root.string);
	  continue;
	}

      is = f->h->root.root.u.def.section;
      section_offset = is->output_section->vma + is->output_offset;
      new_addr = f->h->root.root.u.def.value + section_offset;

#ifdef LINUX_LINK_DEBUG
      printf ("Fixup(%d) %s: %x %x\n",f->jump, f->h->root.root.string,
	      new_addr, f->value);
#endif

      if (f->jump)
	{
	  /* Relative address */
#if 0
	  new_addr = new_addr - (f->value + 5); 
	  bfd_put_32 (output_bfd, new_addr, fixup_table);
	  fixup_table += 4;
	  bfd_put_32 (output_bfd, f->value + 1, fixup_table);
	  fixup_table += 4;
#else
	  /* someday this will become an architecture configutable section of code */
	  /* generate an ARM branch instruction to insert into the jump table */
	  new_addr = (((new_addr - f->value) >> 2) - 2) & 0x00ffffff;
	  bfd_put_32 (output_bfd, new_addr | 0xea000000, fixup_table);
	  fixup_table += 4;
	  bfd_put_32 (output_bfd, f->value, fixup_table);
	  fixup_table += 4;
#endif
	}
      else
	{
	  bfd_put_32 (output_bfd, new_addr, fixup_table);
	  fixup_table += 4;
	  bfd_put_32 (output_bfd, f->value, fixup_table);
	  fixup_table += 4;
	}
      ++fixups_written;
    }

  if (linux_hash_table (info)->local_builtins != 0)
    {
      /* Special marker so we know to switch to the other type of fixup */
      bfd_put_32 (output_bfd, 0, fixup_table);
      fixup_table += 4;
      bfd_put_32 (output_bfd, 0, fixup_table);
      fixup_table += 4;
      ++fixups_written;
      for (f = linux_hash_table (info)->fixup_list; f != NULL; f = f->next)
	{
	  if (! f->builtin)
	    continue;

	  if (f->h->root.root.type != bfd_link_hash_defined
	      && f->h->root.root.type != bfd_link_hash_defweak)
	    {
	      (*_bfd_error_handler)
		("Symbol %s not defined for fixups\n",
		 f->h->root.root.root.string);
	      continue;
	    }

	  is = f->h->root.root.u.def.section;
	  section_offset = is->output_section->vma + is->output_offset;
	  new_addr = f->h->root.root.u.def.value + section_offset;

#ifdef LINUX_LINK_DEBUG
	  printf ("Fixup(B) %s: %x %x\n", f->h->root.root.string,
		  new_addr, f->value);
#endif

	  bfd_put_32 (output_bfd, new_addr, fixup_table);
	  fixup_table += 4;
	  bfd_put_32 (output_bfd, f->value, fixup_table);
	  fixup_table += 4;
	  ++fixups_written;
	}
  }

  if (linux_hash_table (info)->fixup_count != fixups_written)
    {
      (*_bfd_error_handler) ("Warning: fixup count mismatch\n");
      while (linux_hash_table (info)->fixup_count > fixups_written)
	{
	  bfd_put_32 (output_bfd, 0, fixup_table);
	  fixup_table += 4;
	  bfd_put_32 (output_bfd, 0, fixup_table);
	  fixup_table += 4;
	  ++fixups_written;
	}
    }

  h = linux_link_hash_lookup (linux_hash_table (info), 
			      "__BUILTIN_FIXUPS__",
			      false, false, false);

  if (h != NULL
      && (h->root.root.type == bfd_link_hash_defined
	  || h->root.root.type == bfd_link_hash_defweak))
    {
      is = h->root.root.u.def.section;
      section_offset = is->output_section->vma + is->output_offset;
      new_addr = h->root.root.u.def.value + section_offset;

#ifdef LINUX_LINK_DEBUG
      printf ("Builtin fixup table at %x\n", new_addr);
#endif

      bfd_put_32 (output_bfd, new_addr, fixup_table);
    }
  else
    bfd_put_32 (output_bfd, 0, fixup_table);

  if (bfd_seek (output_bfd, os->filepos + s->output_offset, SEEK_SET) != 0)
    return false;

  if (bfd_write ((PTR) s->contents, 1, s->_raw_size, output_bfd)
      != s->_raw_size)
    return false;

  return true;
}

#define MY_bfd_link_hash_table_create armlinux_link_hash_table_create
#define MY_add_one_symbol armlinux_add_one_symbol
#define MY_finish_dynamic_link armlinux_finish_dynamic_link

#define MY_zmagic_contiguous 1
                     
#define MY_BFD_TARGET

#include "aout-target.h"

const bfd_target armlinux_vec =
  {
  TARGETNAME,			/* name */
  bfd_target_aout_flavour,
  BFD_ENDIAN_LITTLE,		/* target byte order (little) */
  BFD_ENDIAN_LITTLE,		/* target headers byte order (little) */
  (HAS_RELOC | EXEC_P | 	/* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC), /* section flags */
  MY_symbol_leading_char,
  AR_PAD_CHAR,			/* ar_pad_char */
  15,				/* ar_max_namelen */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
     bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* data */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
     bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* hdrs */
    {_bfd_dummy_target, MY_object_p, /* bfd_check_format */
       bfd_generic_archive_p, MY_core_file_p},
    {bfd_false, MY_mkobject,	/* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, MY_write_object_contents, /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

     BFD_JUMP_TABLE_GENERIC (MY),
     BFD_JUMP_TABLE_COPY (MY),
     BFD_JUMP_TABLE_CORE (MY),
     BFD_JUMP_TABLE_ARCHIVE (MY),
     BFD_JUMP_TABLE_SYMBOLS (MY),
     BFD_JUMP_TABLE_RELOCS (MY),
     BFD_JUMP_TABLE_WRITE (MY),
     BFD_JUMP_TABLE_LINK (MY),
     BFD_JUMP_TABLE_DYNAMIC (MY),

  (PTR) MY_backend_data,
};

