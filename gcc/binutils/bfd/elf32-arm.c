/* ARM support for 32-bit ELF
   Copyright 1993, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.

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
#include "bfdlink.h"
#include "libbfd.h"
#include "elf-bfd.h"
#include "elf/arm.h"

static reloc_howto_type *elf_arm_reloc_type_lookup
  PARAMS ((bfd *, bfd_reloc_code_real_type));
static void elf_arm_info_to_howto
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rela *));
static void elf_arm_info_to_howto_rel
  PARAMS ((bfd *, arelent *, Elf32_Internal_Rel *));
static boolean elf_arm_check_relocs
  PARAMS ((bfd *, struct bfd_link_info *, asection *,
	   const Elf_Internal_Rela *));
static boolean elf_arm_adjust_dynamic_symbol
  PARAMS ((struct bfd_link_info *, struct elf_link_hash_entry *));
static boolean elf_arm_size_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));
static boolean elf_arm_relocate_section
  PARAMS ((bfd *, struct bfd_link_info *, bfd *, asection *, bfd_byte *,
	   Elf_Internal_Rela *, Elf_Internal_Sym *, asection **));
static boolean elf_arm_finish_dynamic_symbol
  PARAMS ((bfd *, struct bfd_link_info *, struct elf_link_hash_entry *,
	   Elf_Internal_Sym *));
static boolean elf_arm_finish_dynamic_sections
  PARAMS ((bfd *, struct bfd_link_info *));

#define USE_REL	1		/* use REL relocations instead of RELA */

static reloc_howto_type elf_howto_table[]=
{
  HOWTO(R_ARM_NONE,	 0,0, 0,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_NONE"     ,true,0x00000000,0x00000000,false),
  HOWTO(R_ARM_PC24,	 2,2,26,true ,0,complain_overflow_signed  , bfd_elf_generic_reloc,"R_ARM_PC24"     ,true,0x00ffffff,0x00ffffff,true ),
  HOWTO(R_ARM_ABS32,	 0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_ABS32"    ,true,0xffffffff,0xffffffff,false),
  HOWTO(R_ARM_REL32,	 0,2,32,true ,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_REL32"     ,true,0xffffffff,0xffffffff,true ),
  { 4 },
  { 5 },
  { 6 }, 
  { 7 }, 
  { 8 }, 
  { 9 },
  { 10 },
  { 11 },
  { 12 },
  { 13 },
  { 14 },
  { 15 },
  { 16 },
  { 17 },
  { 18 },
  { 19 },
  HOWTO(R_ARM_COPY,      0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_COPY"     ,true,0xffffffff,0xffffffff,false),
  HOWTO(R_ARM_GLOB_DAT,  0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_GLOB_DAT", true,0xffffffff,0xffffffff,false),
  HOWTO(R_ARM_JUMP_SLOT, 0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_JUMP_SLOT",true,0xffffffff,0xffffffff,false),
  HOWTO(R_ARM_RELATIVE,  0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_RELATIVE", true,0xffffffff,0xffffffff,false),
  HOWTO(R_ARM_GOTOFF,    0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_GOTOFF",   true,0xffffffff,0xffffffff,false),
  HOWTO(R_ARM_GOTPC,     0,2,32,true ,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_GOTPC",    true,0xffffffff,0xffffffff,true ),
  HOWTO(R_ARM_GOT32,	 0,2,32,false,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_GOT32"     ,true,0xffffffff,0xffffffff,false),
  HOWTO(R_ARM_PLT32,	 2,2,26,true ,0,complain_overflow_bitfield, bfd_elf_generic_reloc,"R_ARM_PLT32"    ,true,0x00ffffff,0x00ffffff,true ),
};

//#define DEBUG_GEN_RELOC
#ifdef DEBUG_GEN_RELOC
#define TRACE(str) fprintf (stderr, "arm bfd reloc lookup %d (%s)\n", code, str)
#else
#define TRACE(str)
#endif

static reloc_howto_type *
elf_arm_reloc_type_lookup (abfd, code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
  switch (code)
    {
    case BFD_RELOC_NONE:
      TRACE ("BFD_RELOC_NONE");
      return &elf_howto_table[ (int)R_ARM_NONE ];

    case BFD_RELOC_32:
      TRACE ("BFD_RELOC_32");
      return &elf_howto_table[ (int)R_ARM_ABS32 ];

    case BFD_RELOC_ARM_PCREL_BRANCH:
      TRACE ("BFD_RELOC_ARM_PCREL_BRANCH");
      return &elf_howto_table[ (int)R_ARM_PC24 ];

    case BFD_RELOC_ARM_GOT32:
      TRACE ("BFD_RELOC_ARM_GOT32");
      return &elf_howto_table[ (int)R_ARM_GOT32 ];

    case BFD_RELOC_ARM_COPY:
      TRACE ("BFD_RELOC_ARM_COPY");
      return &elf_howto_table[ (int)R_ARM_COPY ];

    case BFD_RELOC_ARM_GLOB_DAT:
      TRACE ("BFD_RELOC_ARM_GLOB_DAT");
      return &elf_howto_table[ (int)R_ARM_GLOB_DAT ];

    case BFD_RELOC_ARM_JUMP_SLOT:
      TRACE ("BFD_RELOC_ARM_JUMP_SLOT");
      return &elf_howto_table[ (int)R_ARM_JUMP_SLOT ];

    case BFD_RELOC_ARM_RELATIVE:
      TRACE ("BFD_RELOC_ARM_RELATIVE");
      return &elf_howto_table[ (int)R_ARM_RELATIVE ];

    case BFD_RELOC_ARM_GOTPC:
      TRACE ("BFD_RELOC_ARM_GOTPC");
      return &elf_howto_table[ (int)R_ARM_GOTPC ];

    case BFD_RELOC_ARM_GOTOFF:
      TRACE ("BFD_RELOC_ARM_GOTOFF");
      return &elf_howto_table[ (int)R_ARM_GOTOFF ];
    case BFD_RELOC_32_PCREL:
      TRACE ("BFD_RELOC_PC32");
      return &elf_howto_table[ (int)R_ARM_REL32 ];

    /* ScottB: February 3, 1998 */
    /* Modified relocation howto lookup to handle PLT32 entries. */
    case BFD_RELOC_ARM_PLT32:
      TRACE ("BFD_RELOC_ARM_PLT32");
      return &elf_howto_table[ (int)R_ARM_PLT32 ];

    default:
      break;
    }

  TRACE ("Unknown");
  return 0;
}

static void
elf_arm_info_to_howto (abfd, cache_ptr, dst)
     bfd		*abfd;
     arelent		*cache_ptr;
     Elf32_Internal_Rela *dst;
{
  BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_ARM_NUM);

  cache_ptr->howto = &elf_howto_table[ELF32_R_TYPE(dst->r_info)];
}

static void
elf_arm_info_to_howto_rel (abfd, cache_ptr, dst)
     bfd		*abfd;
     arelent		*cache_ptr;
     Elf32_Internal_Rel *dst;
{
  BFD_ASSERT (ELF32_R_TYPE(dst->r_info) < (unsigned int) R_ARM_NUM);

  cache_ptr->howto = &elf_howto_table[ELF32_R_TYPE(dst->r_info)];
}

/* The name of the dynamic interpreter.  This is put in the .interp
   section.  */
#define ELF_DYNAMIC_INTERPRETER "/usr/lib/ld.so.1"

/* The size in bytes of an entry in the procedure linkage table.  */

#define PLT_ENTRY_SIZE 16

#if 0
/* The first entry in an absolute procedure linkage table looks like
   this.  It is set up so that any shared library function that is
   called before the relocation has been set up calles the dynamic
   linker first */

static const bfd_byte elf_arm_plt0_entry[PLT_ENTRY_SIZE] =
{
  0x04, 0xc0, 0x2d, 0xe5,	/* str   ip, [sp, #-4]!     */
  0x00, 0xc0, 0x9f, 0xe5,	/* ldr   ip, [pc, #0]       */
  0x04, 0xf0, 0x1c, 0xe5,	/* ldr   pc, [ip, #-4]      */
  0x00, 0x00, 0x00, 0x00	/* address of got + 12      */
};

/* Subsequent entries in an absolute procedure linkage table look like
   this.  */

static const bfd_byte elf_arm_plt_entry[PLT_ENTRY_SIZE] =
{
  0x00, 0xc0, 0x9f, 0xe5,	/* ldr   ip, [pc, #0]       */
  0x00, 0xf0, 0x9c, 0xe5,	/* ldr   pc, [ip]           */
  0x00, 0x00, 0x00, 0x00,	/* address of symbol in got */
  0x00, 0x00, 0x00, 0x00
};

/* The first entry in a PIC procedure linkage table look like this.  */

static const bfd_byte elf_arm_pic_plt0_entry[PLT_ENTRY_SIZE] =
{
  0x04, 0xc0, 0x2d, 0xe5,	/* str   ip, [sp, #-4]!     */
  0x0c, 0xc0, 0x8a, 0xe2,	/* add   ip, sl, #12        */
  0x04, 0xf0, 0x1c, 0xe5,	/* ldr   pc, [ip, #-4]      */
  0x00, 0x00, 0x00, 0x00	/* unused                   */
};

/* Subsequent entries in a PIC procedure linkage table look like this.  */

static const bfd_byte elf_arm_pic_plt_entry[PLT_ENTRY_SIZE] =
{
  0x00, 0xc0, 0x9f, 0xe5,	/* ldr   ip, [pc, #0]       */
  0x0a, 0xf0, 0xbc, 0xe7,	/* ldr   pc, [ip, sl]!      */
  0x00, 0x00, 0x00, 0x00,       /* offset of symbol in got  */
  0x00, 0x00, 0x00, 0x00
};
#endif
/* The first entry in a procedure linkage table looks like
   this.  It is set up so that any shared library function that is
   called before the relocation has been set up calles the dynamic
   linker first */

static const bfd_byte elf_arm_plt0_entry[PLT_ENTRY_SIZE] =
{
  0x04, 0xe0, 0x2d, 0xe5,	/* str   lr, [sp, #-4]!     */
  0x10, 0xe0, 0x9f, 0xe5,	/* ldr   lr, [pc, #16]      */
  0x0e, 0xe0, 0x8f, 0xe0,	/* adr   lr, pc, lr         */
  0x08, 0xf0, 0xbe, 0xe5	/* ldr   pc, [lr, #-4]      */
};

/* Subsequent entries in a procedure linkage table look like
   this.  */

static const bfd_byte elf_arm_plt_entry[PLT_ENTRY_SIZE] =
{
  0x04, 0xc0, 0x9f, 0xe5,	/* ldr   ip, [pc, #4]       */
  0x0c, 0xc0, 0x8f, 0xe0,	/* add   ip, pc, ip         */
  0x00, 0xf0, 0x9c, 0xe5,	/* ldr   pc, [ip]           */
  0x00, 0x00, 0x00, 0x00        /* offset to symbol in got  */
};


/* The ARM linker needs to keep track of the number of relocs that it
   decides to copy in check_relocs for each symbol.  This is so that
   it can discard PC relative relocs if it doesn't need them when
   linking with -Bsymbolic.  We store the information in a field
   extending the regular ELF linker hash table.  */

/* This structure keeps track of the number of PC relative relocs we
   have copied for a given symbol.  */

struct elf_arm_pcrel_relocs_copied
{
  /* Next section.  */
  struct elf_arm_pcrel_relocs_copied *next;
  /* A section in dynobj.  */
  asection *section;
  /* Number of relocs copied in this section.  */
  bfd_size_type count;
};

/* arm ELF linker hash entry.  */

struct elf_arm_link_hash_entry
{
  struct elf_link_hash_entry root;

  /* Number of PC relative relocs copied for this symbol.  */
  struct elf_arm_pcrel_relocs_copied *pcrel_relocs_copied;
};

/* arm ELF linker hash table.  */

struct elf_arm_link_hash_table
{
  struct elf_link_hash_table root;
};

/* Declare this now that the above structures are defined.  */

static boolean elf_arm_discard_copies
  PARAMS ((struct elf_arm_link_hash_entry *, PTR));

/* Traverse an arm ELF linker hash table.  */

#define elf_arm_link_hash_traverse(table, func, info)			\
  (elf_link_hash_traverse						\
   (&(table)->root,							\
    (boolean (*) PARAMS ((struct elf_link_hash_entry *, PTR))) (func),	\
    (info)))

/* Get the arm ELF linker hash table from a link_info structure.  */

#define elf_arm_hash_table(p) \
  ((struct elf_arm_link_hash_table *) ((p)->hash))

/* Create an entry in an arm ELF linker hash table.  */

static struct bfd_hash_entry *
elf_arm_link_hash_newfunc (entry, table, string)
     struct bfd_hash_entry *entry;
     struct bfd_hash_table *table;
     const char *string;
{
  struct elf_arm_link_hash_entry *ret =
    (struct elf_arm_link_hash_entry *) entry;

  /* Allocate the structure if it has not already been allocated by a
     subclass.  */
  if (ret == (struct elf_arm_link_hash_entry *) NULL)
    ret = ((struct elf_arm_link_hash_entry *)
	   bfd_hash_allocate (table,
			      sizeof (struct elf_arm_link_hash_entry)));
  if (ret == (struct elf_arm_link_hash_entry *) NULL)
    return (struct bfd_hash_entry *) ret;

  /* Call the allocation method of the superclass.  */
  ret = ((struct elf_arm_link_hash_entry *)
	 _bfd_elf_link_hash_newfunc ((struct bfd_hash_entry *) ret,
				     table, string));
  if (ret != (struct elf_arm_link_hash_entry *) NULL)
    {
      ret->pcrel_relocs_copied = NULL;
    }

  return (struct bfd_hash_entry *) ret;
}

/* Create an arm ELF linker hash table.  */

static struct bfd_link_hash_table *
elf_arm_link_hash_table_create (abfd)
     bfd *abfd;
{
  struct elf_arm_link_hash_table *ret;

  ret = ((struct elf_arm_link_hash_table *)
	 bfd_alloc (abfd, sizeof (struct elf_arm_link_hash_table)));
  if (ret == (struct elf_arm_link_hash_table *) NULL)
    return NULL;

  if (! _bfd_elf_link_hash_table_init (&ret->root, abfd,
				       elf_arm_link_hash_newfunc))
    {
      bfd_release (abfd, ret);
      return NULL;
    }

  return &ret->root.root;
}

/* Look through the relocs for a section during the first phase, and
   allocate space in the global offset table or procedure linkage
   table.  */

static boolean
elf_arm_check_relocs (abfd, info, sec, relocs)
     bfd *abfd;
     struct bfd_link_info *info;
     asection *sec;
     const Elf_Internal_Rela *relocs;
{
  bfd *dynobj;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  bfd_vma *local_got_offsets;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;
  asection *sgot;
  asection *srelgot;
  asection *sreloc;

  if (info->relocateable)
    return true;

  dynobj = elf_hash_table (info)->dynobj;
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  local_got_offsets = elf_local_got_offsets (abfd);

  sgot = NULL;
  srelgot = NULL;
  sreloc = NULL;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;

      r_symndx = ELF32_R_SYM (rel->r_info);

      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      /* Some relocs require a global offset table.  */
      if (dynobj == NULL)
	{
	  switch (ELF32_R_TYPE (rel->r_info))
	    {
	    case R_ARM_GOT32:
	    case R_ARM_GOTOFF:
	    case R_ARM_GOTPC:
	      elf_hash_table (info)->dynobj = dynobj = abfd;
	      if (! _bfd_elf_create_got_section (dynobj, info))
		return false;
	      break;

	    default:
	      break;
	    }
	}

      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_ARM_GOT32:
	  /* This symbol requires a global offset table entry.  */
     
	  if (sgot == NULL)
	    {
	      sgot = bfd_get_section_by_name (dynobj, ".got");
	      BFD_ASSERT (sgot != NULL);
	    }

	  if (srelgot == NULL
	      && (h != NULL || info->shared))
	    {
	      srelgot = bfd_get_section_by_name (dynobj, ".rel.got");
	      if (srelgot == NULL)
		{
		  srelgot = bfd_make_section (dynobj, ".rel.got");
		  if (srelgot == NULL
		      || ! bfd_set_section_flags (dynobj, srelgot,
						  (SEC_ALLOC
						   | SEC_LOAD
						   | SEC_HAS_CONTENTS
						   | SEC_IN_MEMORY
						   | SEC_LINKER_CREATED
						   | SEC_READONLY))
		      || ! bfd_set_section_alignment (dynobj, srelgot, 2))
		    return false;
		}
	    }

	  if (h != NULL)
	    {
	      if (h->got_offset != (bfd_vma) -1)
		{
		  /* We have already allocated space in the .got.  */
		  break;
		}
	      h->got_offset = sgot->_raw_size;

	      /* Make sure this symbol is output as a dynamic symbol.  */
	      if (h->dynindx == -1)
		{
		  if (! bfd_elf32_link_record_dynamic_symbol (info, h))
		    return false;
		}

	      srelgot->_raw_size += sizeof (Elf32_External_Rel);
	    }
	  else
	    {
     	      /* This is a global offset table entry for a local
                 symbol.  */
	      if (local_got_offsets == NULL)
		{
		  size_t size;
		  register unsigned int i;

		  size = symtab_hdr->sh_info * sizeof (bfd_vma);
		  local_got_offsets = (bfd_vma *) bfd_alloc (abfd, size);
		  if (local_got_offsets == NULL)
		    return false;
		  elf_local_got_offsets (abfd) = local_got_offsets;
		  for (i = 0; i < symtab_hdr->sh_info; i++)
		    local_got_offsets[i] = (bfd_vma) -1;
		}
	      if (local_got_offsets[r_symndx] != (bfd_vma) -1)
		{
		  /* We have already allocated space in the .got.  */
		  break;
		}
	      local_got_offsets[r_symndx] = sgot->_raw_size;

	      if (info->shared)
		{
		  /* If we are generating a shared object, we need to
                     output a R_ARM_RELATIVE reloc so that the dynamic
                     linker can adjust this GOT entry.  */
		  srelgot->_raw_size += sizeof (Elf32_External_Rel);
		}
	    }

	  sgot->_raw_size += 4;

	  break;

	case R_ARM_PLT32:
	  /* This symbol requires a procedure linkage table entry.  We
             actually build the entry in adjust_dynamic_symbol,
             because this might be a case of linking PIC code which is
             never referenced by a dynamic object, in which case we
             don't need to generate a procedure linkage table entry
             after all.  */

	  /* If this is a local symbol, we resolve it directly without
             creating a procedure linkage table entry.  */
	  if (h == NULL)
	    continue;

	  h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_PLT;

	  break;

	case R_ARM_ABS32:
	case R_ARM_REL32:
	case R_ARM_PC24:
	  /* If we are creating a shared library, and this is a reloc
             against a global symbol, or a non PC relative reloc
             against a local symbol, then we need to copy the reloc
             into the shared library.  However, if we are linking with
             -Bsymbolic, we do not need to copy a reloc against a
             global symbol which is defined in an object we are
             including in the link (i.e., DEF_REGULAR is set).  At
             this point we have not seen all the input files, so it is
             possible that DEF_REGULAR is not set now but will be set
             later (it is never cleared).  We account for that
             possibility below by storing information in the
             pcrel_relocs_copied field of the hash table entry.  */
	  if (info->shared
	      && (ELF32_R_TYPE (rel->r_info) != R_ARM_PC24
		  || (h != NULL
		      && (! info->symbolic
			  || (h->elf_link_hash_flags
			      & ELF_LINK_HASH_DEF_REGULAR) == 0))))
	    {
	      /* When creating a shared object, we must copy these
                 reloc types into the output file.  We create a reloc
                 section in dynobj and make room for this reloc.  */
	      if (sreloc == NULL)
		{
		  const char *name;

		  name = (bfd_elf_string_from_elf_section
			  (abfd,
			   elf_elfheader (abfd)->e_shstrndx,
			   elf_section_data (sec)->rel_hdr.sh_name));
		  if (name == NULL)
		    return false;

		  BFD_ASSERT (strncmp (name, ".rel", 4) == 0
			      && strcmp (bfd_get_section_name (abfd, sec),
					 name + 4) == 0);

		  sreloc = bfd_get_section_by_name (dynobj, name);
		  if (sreloc == NULL)
		    {
		      flagword flags;

		      sreloc = bfd_make_section (dynobj, name);
		      flags = (SEC_HAS_CONTENTS | SEC_READONLY
			       | SEC_IN_MEMORY | SEC_LINKER_CREATED);
		      if ((sec->flags & SEC_ALLOC) != 0)
			flags |= SEC_ALLOC | SEC_LOAD;
		      if (sreloc == NULL
			  || ! bfd_set_section_flags (dynobj, sreloc, flags)
			  || ! bfd_set_section_alignment (dynobj, sreloc, 2))
			return false;
		    }
		}

	      sreloc->_raw_size += sizeof (Elf32_External_Rel);
	      /* If we are linking with -Bsymbolic, and this is a
                 global symbol, we count the number of PC relative
                 relocations we have entered for this symbol, so that
                 we can discard them again if the symbol is later
                 defined by a regular object.  Note that this function
                 is only called if we are using an elf_i386 linker
                 hash table, which means that h is really a pointer to
                 an elf_i386_link_hash_entry.  */
	      if (h != NULL && info->symbolic
		  && ELF32_R_TYPE (rel->r_info) == R_ARM_PC24)
		{
		  struct elf_arm_link_hash_entry *eh;
		  struct elf_arm_pcrel_relocs_copied *p;

		  eh = (struct elf_arm_link_hash_entry *) h;

		  for (p = eh->pcrel_relocs_copied; p != NULL; p = p->next)
		    if (p->section == sreloc)
		      break;

		  if (p == NULL)
		    {
		      p = ((struct elf_arm_pcrel_relocs_copied *)
			   bfd_alloc (dynobj, sizeof *p));
		      if (p == NULL)
			return false;
		      p->next = eh->pcrel_relocs_copied;
		      eh->pcrel_relocs_copied = p;
		      p->section = sreloc;
		      p->count = 0;
		    }

		  ++p->count;
		}
	    }
	     
	  break;

	default:
	  break;
	}
    }

  return true;
}

/* Adjust a symbol defined by a dynamic object and referenced by a
   regular object.  The current definition is in some section of the
   dynamic object, but we're not including those sections.  We have to
   change the definition to something the rest of the link can
   understand.  */

static boolean
elf_arm_adjust_dynamic_symbol (info, h)
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
{
  bfd *dynobj;
  asection *s;
  unsigned int power_of_two;

  dynobj = elf_hash_table (info)->dynobj;

  /* Make sure we know what is going on here.  */
  BFD_ASSERT (dynobj != NULL
	      && ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT)
		  || h->weakdef != NULL
		  || ((h->elf_link_hash_flags
		       & ELF_LINK_HASH_DEF_DYNAMIC) != 0
		      && (h->elf_link_hash_flags
			  & ELF_LINK_HASH_REF_REGULAR) != 0
		      && (h->elf_link_hash_flags
			  & ELF_LINK_HASH_DEF_REGULAR) == 0)));

  /* If this is a function, put it in the procedure linkage table.  We
     will fill in the contents of the procedure linkage table later,
     when we know the address of the .got section.  */
  if (h->type == STT_FUNC
      || (h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) != 0)
    {
      if (! info->shared
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_DYNAMIC) == 0
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_REF_DYNAMIC) == 0)
	{
	  /* This case can occur if we saw a PLT32 reloc in an input
             file, but the symbol was never referred to by a dynamic
             object.  In such a case, we don't actually need to build
             a procedure linkage table, and we can just do a PC32
             reloc instead.  */
	  BFD_ASSERT ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_PLT) != 0);
	  return true;
	}

      /* Make sure this symbol is output as a dynamic symbol.  */
      if (h->dynindx == -1)
	{
	  if (! bfd_elf32_link_record_dynamic_symbol (info, h))
	    return false;
	}

      s = bfd_get_section_by_name (dynobj, ".plt");
      BFD_ASSERT (s != NULL);

      /* If this is the first .plt entry, make room for the special
	 first entry.  */
      if (s->_raw_size == 0)
	s->_raw_size += PLT_ENTRY_SIZE;

      /* If this symbol is not defined in a regular file, and we are
	 not generating a shared library, then set the symbol to this
	 location in the .plt.  This is required to make function
	 pointers compare as equal between the normal executable and
	 the shared library.  */
      if (! info->shared
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
	{
	  h->root.u.def.section = s;
	  h->root.u.def.value = s->_raw_size;
	}

      h->plt_offset = s->_raw_size;

      /* Make room for this entry.  */
      s->_raw_size += PLT_ENTRY_SIZE;

      /* We also need to make an entry in the .got.plt section, which
	 will be placed in the .got section by the linker script.  */

      s = bfd_get_section_by_name (dynobj, ".got.plt");
      BFD_ASSERT (s != NULL);
      s->_raw_size += 4;

      /* We also need to make an entry in the .rel.plt section.  */

      s = bfd_get_section_by_name (dynobj, ".rel.plt");
      BFD_ASSERT (s != NULL);
      s->_raw_size += sizeof (Elf32_External_Rel);

      return true;
    }

  /* If this is a weak symbol, and there is a real definition, the
     processor independent code will have arranged for us to see the
     real definition first, and we can just use the same value.  */
  if (h->weakdef != NULL)
    {
      BFD_ASSERT (h->weakdef->root.type == bfd_link_hash_defined
		  || h->weakdef->root.type == bfd_link_hash_defweak);
      h->root.u.def.section = h->weakdef->root.u.def.section;
      h->root.u.def.value = h->weakdef->root.u.def.value;
      return true;
    }

  /* This is a reference to a symbol defined by a dynamic object which
     is not a function.  */

  /* If we are creating a shared library, we must presume that the
     only references to the symbol are via the global offset table.
     For such cases we need not do anything here; the relocations will
     be handled correctly by relocate_section.  */
  if (info->shared)
    return true;

  /* We must allocate the symbol in our .dynbss section, which will
     become part of the .bss section of the executable.  There will be
     an entry for this symbol in the .dynsym section.  The dynamic
     object will contain position independent code, so all references
     from the dynamic object to this symbol will go through the global
     offset table.  The dynamic linker will use the .dynsym entry to
     determine the address it must put in the global offset table, so
     both the dynamic object and the regular object will refer to the
     same memory location for the variable.  */

  s = bfd_get_section_by_name (dynobj, ".dynbss");
  BFD_ASSERT (s != NULL);

  /* We must generate a R_ARM_COPY reloc to tell the dynamic linker to
     copy the initial value out of the dynamic object and into the
     runtime process image.  We need to remember the offset into the
     .rel.bss section we are going to use.  */
  if ((h->root.u.def.section->flags & SEC_ALLOC) != 0)
    {
      asection *srel;

      srel = bfd_get_section_by_name (dynobj, ".rel.bss");
      BFD_ASSERT (srel != NULL);
      srel->_raw_size += sizeof (Elf32_External_Rel);
      h->elf_link_hash_flags |= ELF_LINK_HASH_NEEDS_COPY;
    }

  /* We need to figure out the alignment required for this symbol.  I
     have no idea how ELF linkers handle this.  */
  power_of_two = bfd_log2 (h->size);
  if (power_of_two > 3)
    power_of_two = 3;

  /* Apply the required alignment.  */
  s->_raw_size = BFD_ALIGN (s->_raw_size,
			    (bfd_size_type) (1 << power_of_two));
  if (power_of_two > bfd_get_section_alignment (dynobj, s))
    {
      if (! bfd_set_section_alignment (dynobj, s, power_of_two))
	return false;
    }

  /* Define the symbol as being at this point in the section.  */
  h->root.u.def.section = s;
  h->root.u.def.value = s->_raw_size;

  /* Increment the section size to make room for the symbol.  */
  s->_raw_size += h->size;

  return true;
}

/* Set the sizes of the dynamic sections.  */

static boolean
elf_arm_size_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  bfd *dynobj;
  asection *s;
  boolean plt;
  boolean relocs;
  boolean reltext;

  dynobj = elf_hash_table (info)->dynobj;
  BFD_ASSERT (dynobj != NULL);

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      /* Set the contents of the .interp section to the interpreter.  */
      if (! info->shared)
	{
	  s = bfd_get_section_by_name (dynobj, ".interp");
	  BFD_ASSERT (s != NULL);
	  s->_raw_size = sizeof ELF_DYNAMIC_INTERPRETER;
	  s->contents = (unsigned char *) ELF_DYNAMIC_INTERPRETER;
	}
    }
  else
    {
      /* We may have created entries in the .rel.got section.
         However, if we are not creating the dynamic sections, we will
         not actually use these entries.  Reset the size of .rel.got,
         which will cause it to get stripped from the output file
         below.  */
      s = bfd_get_section_by_name (dynobj, ".rel.got");
      if (s != NULL)
	s->_raw_size = 0;
    }

  /* If this is a -Bsymbolic shared link, then we need to discard all
     PC relative relocs against symbols defined in a regular object.
     We allocated space for them in the check_relocs routine, but we
     will not fill them in in the relocate_section routine.  */
  if (info->shared && info->symbolic)
    elf_arm_link_hash_traverse (elf_arm_hash_table (info),
				 elf_arm_discard_copies,
				 (PTR) NULL);

  /* The check_relocs and adjust_dynamic_symbol entry points have
     determined the sizes of the various dynamic sections.  Allocate
     memory for them.  */
  plt = false;
  relocs = false;
  reltext = false;
  for (s = dynobj->sections; s != NULL; s = s->next)
    {
      const char *name;
      boolean strip;

      if ((s->flags & SEC_LINKER_CREATED) == 0)
	continue;

      /* It's OK to base decisions on the section name, because none
	 of the dynobj section names depend upon the input files.  */
      name = bfd_get_section_name (dynobj, s);

      strip = false;

      if (strcmp (name, ".plt") == 0)
	{
	  if (s->_raw_size == 0)
	    {
	      /* Strip this section if we don't need it; see the
                 comment below.  */
	      strip = true;
	    }
	  else
	    {
	      /* Remember whether there is a PLT.  */
	      plt = true;
	    }
	}
      else if (strncmp (name, ".rel", 4) == 0)
	{
	  if (s->_raw_size == 0)
	    {
	      /* If we don't need this section, strip it from the
		 output file.  This is mostly to handle .rel.bss and
		 .rel.plt.  We must create both sections in
		 create_dynamic_sections, because they must be created
		 before the linker maps input sections to output
		 sections.  The linker does that before
		 adjust_dynamic_symbol is called, and it is that
		 function which decides whether anything needs to go
		 into these sections.  */
	      strip = true;
	    }
	  else
	    {
	      asection *target;

	      /* Remember whether there are any reloc sections other
                 than .rel.plt.  */
	      if (strcmp (name, ".rel.plt") != 0)
		{
		  const char *outname;

		  relocs = true;

		  /* If this relocation section applies to a read only
		     section, then we probably need a DT_TEXTREL
		     entry.  The entries in the .rel.plt section
		     really apply to the .got section, which we
		     created ourselves and so know is not readonly.  */
		  outname = bfd_get_section_name (output_bfd,
						  s->output_section);
		  target = bfd_get_section_by_name (output_bfd, outname + 4);
		  if (target != NULL
		      && (target->flags & SEC_READONLY) != 0
		      && (target->flags & SEC_ALLOC) != 0)
		    reltext = true;
		}

	      /* We use the reloc_count field as a counter if we need
		 to copy relocs into the output file.  */
	      s->reloc_count = 0;
	    }
	}
      else if (strncmp (name, ".got", 4) != 0)
	{
	  /* It's not one of our sections, so don't allocate space.  */
	  continue;
	}

      if (strip)
	{
	  asection **spp;

	  for (spp = &s->output_section->owner->sections;
	       *spp != s->output_section;
	       spp = &(*spp)->next)
	    ;
	  *spp = s->output_section->next;
	  --s->output_section->owner->section_count;

	  continue;
	}

      /* Allocate memory for the section contents.  */
      s->contents = (bfd_byte *) bfd_zalloc (dynobj, s->_raw_size);
      if (s->contents == NULL && s->_raw_size != 0)
	return false;
    }

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      /* Add some entries to the .dynamic section.  We fill in the
	 values later, in elf_arm_finish_dynamic_sections, but we
	 must add the entries now so that we get the correct size for
	 the .dynamic section.  The DT_DEBUG entry is filled in by the
	 dynamic linker and used by the debugger.  */
      if (! info->shared)
	{
	  if (! bfd_elf32_add_dynamic_entry (info, DT_DEBUG, 0))
	    return false;
	}

      if (plt)
	{
	  if (! bfd_elf32_add_dynamic_entry (info, DT_PLTGOT, 0)
	      || ! bfd_elf32_add_dynamic_entry (info, DT_PLTRELSZ, 0)
	      || ! bfd_elf32_add_dynamic_entry (info, DT_PLTREL, DT_REL)
	      || ! bfd_elf32_add_dynamic_entry (info, DT_JMPREL, 0))
	    return false;
	}

      if (relocs)
	{
	  if (! bfd_elf32_add_dynamic_entry (info, DT_REL, 0)
	      || ! bfd_elf32_add_dynamic_entry (info, DT_RELSZ, 0)
	      || ! bfd_elf32_add_dynamic_entry (info, DT_RELENT,
						sizeof (Elf32_External_Rel)))
	    return false;
	}

      if (reltext)
	{
	  if (! bfd_elf32_add_dynamic_entry (info, DT_TEXTREL, 0))
	    return false;
	}
    }

  return true;
}

/* This function is called via elf_arm_link_hash_traverse if we are
   creating a shared object with -Bsymbolic.  It discards the space
   allocated to copy PC relative relocs against symbols which are
   defined in regular objects.  We allocated space for them in the
   check_relocs routine, but we won't fill them in in the
   relocate_section routine.  */

/*ARGSUSED*/
static boolean
elf_arm_discard_copies (h, ignore)
     struct elf_arm_link_hash_entry *h;
     PTR ignore;
{
  struct elf_arm_pcrel_relocs_copied *s;

  /* We only discard relocs for symbols defined in a regular object.  */
  if ((h->root.elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
    return true;

  for (s = h->pcrel_relocs_copied; s != NULL; s = s->next)
    s->section->_raw_size -= s->count * sizeof (Elf32_External_Rel);

  return true;
}
/* Relocate an arm ELF section.  */

static boolean
elf_arm_relocate_section (output_bfd, info, input_bfd, input_section,
			   contents, relocs, local_syms, local_sections)
     bfd *output_bfd;
     struct bfd_link_info *info;
     bfd *input_bfd;
     asection *input_section;
     bfd_byte *contents;
     Elf_Internal_Rela *relocs;
     Elf_Internal_Sym *local_syms;
     asection **local_sections;
{
  bfd *dynobj;
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  bfd_vma *local_got_offsets;
  asection *sgot;
  asection *splt;
  asection *sreloc;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  dynobj = elf_hash_table (info)->dynobj;
  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  local_got_offsets = elf_local_got_offsets (input_bfd);

  sgot = NULL;
  splt = NULL;
  sreloc = NULL;

  rel = relocs;
  relend = relocs + input_section->reloc_count;
  for (; rel < relend; rel++)
    {
      int r_type;
      reloc_howto_type *howto;
      unsigned long r_symndx;
      struct elf_link_hash_entry *h;
      Elf_Internal_Sym *sym;
      asection *sec;
      bfd_vma relocation;
      bfd_reloc_status_type r;

      r_type = ELF32_R_TYPE (rel->r_info);
      if (r_type < 0 || r_type >= (int) R_ARM_NUM)
	{
	  bfd_set_error (bfd_error_bad_value);
	  return false;
	}
      howto = elf_howto_table + r_type;

      r_symndx = ELF32_R_SYM (rel->r_info);

      if (info->relocateable)
	{
	  /* This is a relocateable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;
	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  bfd_vma val;

		  sec = local_sections[r_symndx];
		  val = bfd_get_32 (input_bfd, contents + rel->r_offset);
		  val += sec->output_offset + sym->st_value;
		  bfd_put_32 (input_bfd, val, contents + rel->r_offset);
		}
	    }

	  continue;
	}

      /* This is a final link.  */
      h = NULL;
      sym = NULL;
      sec = NULL;
//patb
//printf("working on a final reloc for type: %x section: %s\n",
//r_type,bfd_get_section_name(input_bfd, input_section));                
      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];
	  relocation = (sec->output_section->vma
			+ sec->output_offset
			+ sym->st_value);
//printf("hash not set: r_symndx: %x, limit: %x\n",r_symndx,symtab_hdr->sh_info);			
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];
//printf("hash set to %x\n",h);
	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;
//printf("into a final reloc for type: %x section: %s\n",
//r_type,bfd_get_section_name(input_bfd, input_section)),
//printf("hash type: %x, pltoff:%x, infoshared:%x hashflags: x\n",
//h->root.type,h->plt_offset,info->shared,h->elf_link_hash_flags);          
	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;
	      if (r_type == R_ARM_GOTPC
		  || (r_type == R_ARM_PLT32
		      && h->plt_offset != (bfd_vma) -1)
		  || (r_type == R_ARM_GOT32
		      && elf_hash_table (info)->dynamic_sections_created
		      && (! info->shared
			  || (! info->symbolic && h->dynindx != -1)
			  || (h->elf_link_hash_flags
			      & ELF_LINK_HASH_DEF_REGULAR) == 0))
		  || (info->shared
		      && ((! info->symbolic && h->dynindx != -1)
			  || (h->elf_link_hash_flags
			      & ELF_LINK_HASH_DEF_REGULAR) == 0)
		      && (r_type == R_ARM_ABS32
			  || r_type == R_ARM_PC24)
		      && (input_section->flags & SEC_ALLOC) != 0))
		{
		  /* In these cases, we don't need the relocation
                     value.  We check specially because in some
                     obscure cases sec->output_section will be NULL.  */
//patb
//printf("throw out a reloc name:%s type: %x section: %s\n",h->root.root.string,r_type,bfd_get_section_name(input_bfd, input_section));                
		  relocation = 0;
		}
	      else if (sec->output_section == NULL)
	        {
	          (*_bfd_error_handler)
	            ("%s: warning: unresolvable relocation against symbol `%s' from %s section",
	             bfd_get_filename (input_bfd), h->root.root.string,
	             bfd_get_section_name (input_bfd, input_section));
	          relocation = 0;
	        }
	      else
		relocation = (h->root.u.def.value
			      + sec->output_section->vma
			      + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    relocation = 0;
	  else if (info->shared && !info->symbolic)
	    relocation = 0;
          else
	    {
	      if (! ((*info->callbacks->undefined_symbol)
		     (info, h->root.root.string, input_bfd,
		      input_section, rel->r_offset)))
		return false;
	      relocation = 0;
	    }
	}

      switch (r_type)
	{
	case R_ARM_GOT32:
	  /* Relocation is to the entry for this symbol in the global
	     offset table.  */
	  if (sgot == NULL)
	    {
	      sgot = bfd_get_section_by_name (dynobj, ".got");
	      BFD_ASSERT (sgot != NULL);
	    }

	  if (h != NULL)
	    {
	      bfd_vma off;

	      off = h->got_offset;
	      BFD_ASSERT (off != (bfd_vma) -1);

	      if (! elf_hash_table (info)->dynamic_sections_created
		  || (info->shared
		      && (info->symbolic || h->dynindx == -1)
		      && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR)))
		{
		  /* This is actually a static link, or it is a
		     -Bsymbolic link and the symbol is defined
		     locally.  We must initialize this entry in the
		     global offset table.  Since the offset must
		     always be a multiple of 4, we use the least
		     significant bit to record whether we have
		     initialized it already.

		     When doing a dynamic link, we create a .rel.got
		     relocation entry to initialize the value.  This
		     is done in the finish_dynamic_symbol routine.  */
		  if ((off & 1) != 0)
		    off &= ~1;
		  else
		    {
		      bfd_put_32 (output_bfd, relocation,
				  sgot->contents + off);
		      h->got_offset |= 1;
		    }
		}

	      relocation = sgot->output_offset + off;
	    }
	  else
	    {
	      bfd_vma off;

	      BFD_ASSERT (local_got_offsets != NULL
			  && local_got_offsets[r_symndx] != (bfd_vma) -1);

	      off = local_got_offsets[r_symndx];

	      /* The offset must always be a multiple of 4.  We use
                 the least significant bit to record whether we have
                 already generated the necessary reloc.  */
	      if ((off & 1) != 0)
		off &= ~1;
	      else
		{
		  bfd_put_32 (output_bfd, relocation, sgot->contents + off);

		  if (info->shared)
		    {
		      asection *srelgot;
		      Elf_Internal_Rel outrel;

		      srelgot = bfd_get_section_by_name (dynobj, ".rel.got");
		      BFD_ASSERT (srelgot != NULL);

//printf("1shared: %x\nsgot->offset%x\toff:%x\n",
//info->shared,sgot->output_offset,off);

		      outrel.r_offset = (sgot->output_section->vma
					 + sgot->output_offset
					 + off);
		      outrel.r_info = ELF32_R_INFO (0, R_ARM_RELATIVE);
		      bfd_elf32_swap_reloc_out (output_bfd, &outrel,
						(((Elf32_External_Rel *)
						  srelgot->contents)
						 + srelgot->reloc_count));
		      ++srelgot->reloc_count;
		    }

		  local_got_offsets[r_symndx] |= 1;
		}

	      relocation = sgot->output_offset + off;
	    }

	  break;

	case R_ARM_GOTOFF:
	  /* Relocation is relative to the start of the global offset
	     table.  */

	  if (sgot == NULL)
	    {
	      sgot = bfd_get_section_by_name (dynobj, ".got");
	      BFD_ASSERT (sgot != NULL);
	    }

	  /* Note that sgot->output_offset is not involved in this
	     calculation.  We always want the start of .got.  If we
	     defined _GLOBAL_OFFSET_TABLE in a different way, as is
	     permitted by the ABI, we might have to change this
	     calculation.  */
	  relocation -= sgot->output_section->vma;

	  break;

	case R_ARM_GOTPC:
	  /* Use global offset table as symbol value.  */

	  if (sgot == NULL)
	    {
	      sgot = bfd_get_section_by_name (dynobj, ".got");
	      BFD_ASSERT (sgot != NULL);
	    }

	  relocation = sgot->output_section->vma;

	  break;

	case R_ARM_PLT32:
	  /* Relocation is to the entry for this symbol in the
	     procedure linkage table.  */

	  /* Resolve a PLT32 reloc again a local symbol directly,
             without using the procedure linkage table.  */
	  if (h == NULL)
	    break;

	  if (h->plt_offset == (bfd_vma) -1)
	    {
	      /* We didn't make a PLT entry for this symbol.  This
                 happens when statically linking PIC code, or when
                 using -Bsymbolic.  */
	      break;
	    }

	  if (splt == NULL)
	    {
	      splt = bfd_get_section_by_name (dynobj, ".plt");
	      BFD_ASSERT (splt != NULL);
	    }

	  relocation = (splt->output_section->vma
			+ splt->output_offset
			+ h->plt_offset);

	  break;

	case R_ARM_ABS32:
	case R_ARM_REL32:
	case R_ARM_PC24:
//printf("working on an ultimate reloc for name %s sreloc: %x\tr_type %x \n",
//h ? h->root.root.string : 0, sreloc, r_type);
	  if (info->shared
	      && (r_type != R_ARM_PC24
		  || (h != NULL
		      && h->dynindx != -1
		      && (! info->symbolic
			  || (h->elf_link_hash_flags
			      & ELF_LINK_HASH_DEF_REGULAR) == 0))))
	    {
	      Elf_Internal_Rel outrel;
	      boolean skip, relocate;

	      /* When generating a shared object, these relocations
		 are copied into the output file to be resolved at run
		 time.  */

	      if (sreloc == NULL)
		{
		  const char *name;

		  name = (bfd_elf_string_from_elf_section
			  (input_bfd,
			   elf_elfheader (input_bfd)->e_shstrndx,
			   elf_section_data (input_section)->rel_hdr.sh_name));
		  if (name == NULL)
		    return false;

		  BFD_ASSERT (strncmp (name, ".rel", 4) == 0
			      && strcmp (bfd_get_section_name (input_bfd,
							       input_section),
					 name + 4) == 0);

		  sreloc = bfd_get_section_by_name (dynobj, name);
		  BFD_ASSERT (sreloc != NULL);
		}

	      skip = false;

	      if (elf_section_data (input_section)->stab_info == NULL)
		outrel.r_offset = rel->r_offset;
	      else
		{
		  bfd_vma off;

		  off = (_bfd_stab_section_offset
			 (output_bfd, &elf_hash_table (info)->stab_info,
			  input_section,
			  &elf_section_data (input_section)->stab_info,
			  rel->r_offset));
		  if (off == (bfd_vma) -1)
		    skip = true;
		  outrel.r_offset = off;
		}

	      outrel.r_offset += (input_section->output_section->vma
				  + input_section->output_offset);

	      if (skip)
		{
		  memset (&outrel, 0, sizeof outrel);
		  relocate = false;
		}
              else if (r_type == R_ARM_PC24)
		{
		  BFD_ASSERT (h != NULL && h->dynindx != -1);
		  if ((input_section->flags & SEC_ALLOC) != 0)
		    relocate = false;
		  else
		    relocate = true;
		  outrel.r_info = ELF32_R_INFO (h->dynindx, R_ARM_PC24);
		}
	      else
		{
//printf("final of ultimate reloc for name ?? h: %x dynindx: %x\n",
//h, h ? h->dynindx : 0);
		  if (h == NULL
		      || ((info->symbolic || h->dynindx == -1)
			  && (h->elf_link_hash_flags
			      & ELF_LINK_HASH_DEF_REGULAR) != 0))
		    {
		      relocate = true;
		      outrel.r_info = ELF32_R_INFO (0, R_ARM_RELATIVE);
		    }
		  else
		    {
		      BFD_ASSERT (h->dynindx != -1);
		      if ((input_section->flags & SEC_ALLOC) != 0)
		        relocate = false;
		      else
		        relocate = true;
		      outrel.r_info = ELF32_R_INFO (h->dynindx, R_ARM_ABS32);
		    }
		}

	      bfd_elf32_swap_reloc_out (output_bfd, &outrel,
					(((Elf32_External_Rel *)
					  sreloc->contents)
					 + sreloc->reloc_count));
	      ++sreloc->reloc_count;

	      /* If this reloc is against an external symbol, we do
		 not want to fiddle with the addend.  Otherwise, we
		 need to include the symbol value so that it becomes
		 an addend for the dynamic reloc.  */
	      if (! relocate)
		continue;
	    }

	  break;

	default:
	  break;
	}

      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
				    contents, rel->r_offset,
				    relocation, (bfd_vma) 0);

      if (r != bfd_reloc_ok)
	{
	  switch (r)
	    {
	    default:
	    case bfd_reloc_outofrange:
	      abort ();
	    case bfd_reloc_overflow:
	      {
		const char *name;

		if (h != NULL)
		  name = h->root.root.string;
		else
		  {
		    name = bfd_elf_string_from_elf_section (input_bfd,
							    symtab_hdr->sh_link,
							    sym->st_name);
		    if (name == NULL)
		      return false;
		    if (*name == '\0')
		      name = bfd_section_name (input_bfd, sec);
		  }
		if (! ((*info->callbacks->reloc_overflow)
		       (info, name, howto->name, (bfd_vma) 0,
			input_bfd, input_section, rel->r_offset)))
		  return false;
	      }
	      break;
	    }
	}
    }

  return true;
}

/* Finish up dynamic symbol handling.  We set the contents of various
   dynamic sections here.  */

static boolean
elf_arm_finish_dynamic_symbol (output_bfd, info, h, sym)
     bfd *output_bfd;
     struct bfd_link_info *info;
     struct elf_link_hash_entry *h;
     Elf_Internal_Sym *sym;
{
  bfd *dynobj;

  dynobj = elf_hash_table (info)->dynobj;

  if (h->plt_offset != (bfd_vma) -1)
    {
      asection *splt;
      asection *sgot;
      asection *srel;
      bfd_vma plt_index;
      bfd_vma got_offset;
      Elf_Internal_Rel rel;

      /* This symbol has an entry in the procedure linkage table.  Set
	 it up.  */

      BFD_ASSERT (h->dynindx != -1);

      splt = bfd_get_section_by_name (dynobj, ".plt");
      sgot = bfd_get_section_by_name (dynobj, ".got.plt");
      srel = bfd_get_section_by_name (dynobj, ".rel.plt");
      BFD_ASSERT (splt != NULL && sgot != NULL && srel != NULL);

      /* Get the index in the procedure linkage table which
	 corresponds to this symbol.  This is the index of this symbol
	 in all the symbols for which we are making plt entries.  The
	 first entry in the procedure linkage table is reserved.  */
      plt_index = h->plt_offset / PLT_ENTRY_SIZE - 1;

      /* Get the offset into the .got table of the entry that
	 corresponds to this function.  Each .got entry is 4 bytes.
	 The first three are reserved.  */
      got_offset = (plt_index + 3) * 4;

      /* Fill in the entry in the procedure linkage table.  */
#if 0
      if (! info->shared)
	{
	  memcpy (splt->contents + h->plt_offset, elf_arm_plt_entry,
		  PLT_ENTRY_SIZE);
	  bfd_put_32 (output_bfd,
		      (sgot->output_section->vma
		       + sgot->output_offset
		       + got_offset),
		      splt->contents + h->plt_offset + 8);
	}
      else
	{
	  memcpy (splt->contents + h->plt_offset, elf_arm_pic_plt_entry,
		  PLT_ENTRY_SIZE);
	  bfd_put_32 (output_bfd, got_offset,
		      splt->contents + h->plt_offset + 8);
	}
#endif
      memcpy (splt->contents + h->plt_offset, elf_arm_plt_entry,
		  PLT_ENTRY_SIZE);
      bfd_put_32 (output_bfd,
		      (sgot->output_section->vma
		       + sgot->output_offset
		       + got_offset 
		       - splt->output_section->vma
		       - splt->output_offset
		       - h->plt_offset - 12),
		      splt->contents + h->plt_offset + 12);

      /* Fill in the entry in the global offset table.  */
      bfd_put_32 (output_bfd,
		  (splt->output_section->vma
		   + splt->output_offset),
		  sgot->contents + got_offset);

      /* Fill in the entry in the .rel.plt section.  */
      rel.r_offset = (sgot->output_section->vma
		      + sgot->output_offset
		      + got_offset);
      rel.r_info = ELF32_R_INFO (h->dynindx, R_ARM_JUMP_SLOT);
      bfd_elf32_swap_reloc_out (output_bfd, &rel,
				((Elf32_External_Rel *) srel->contents
				 + plt_index));

      if ((h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR) == 0)
	{
	  /* Mark the symbol as undefined, rather than as defined in
	     the .plt section.  Leave the value alone.  */
	  sym->st_shndx = SHN_UNDEF;
	}
    }

  if (h->got_offset != (bfd_vma) -1)
    {
      asection *sgot;
      asection *srel;
      Elf_Internal_Rel rel;

      /* This symbol has an entry in the global offset table.  Set it
	 up.  */
      
      sgot = bfd_get_section_by_name (dynobj, ".got");
      srel = bfd_get_section_by_name (dynobj, ".rel.got");
      BFD_ASSERT (sgot != NULL && srel != NULL);

      rel.r_offset = (sgot->output_section->vma
		      + sgot->output_offset
		      + (h->got_offset &~ 1));

//printf("shared: %x\nh: %x\tdynindx: %x\n",info->shared,h,h->dynindx);
      /* If this is a -Bsymbolic link, and the symbol is defined
	 locally, we just want to emit a RELATIVE reloc.  The entry in
	 the global offset table will already have been initialized in
	 the relocate_section function.  */
      if (info->shared
	  && (info->symbolic || h->dynindx == -1)
	  && (h->elf_link_hash_flags & ELF_LINK_HASH_DEF_REGULAR))
	rel.r_info = ELF32_R_INFO (0, R_ARM_RELATIVE);
      else
	{
	  bfd_put_32 (output_bfd, (bfd_vma) 0, sgot->contents + h->got_offset);
	  rel.r_info = ELF32_R_INFO (h->dynindx, R_ARM_GLOB_DAT);
	}

      bfd_elf32_swap_reloc_out (output_bfd, &rel,
				((Elf32_External_Rel *) srel->contents
				 + srel->reloc_count));
      ++srel->reloc_count;
    }

  if ((h->elf_link_hash_flags & ELF_LINK_HASH_NEEDS_COPY) != 0)
    {
      asection *s;
      Elf_Internal_Rel rel;

      /* This symbol needs a copy reloc.  Set it up.  */

      BFD_ASSERT (h->dynindx != -1
		  && (h->root.type == bfd_link_hash_defined
		      || h->root.type == bfd_link_hash_defweak));

      s = bfd_get_section_by_name (h->root.u.def.section->owner,
				   ".rel.bss");
      BFD_ASSERT (s != NULL);

      rel.r_offset = (h->root.u.def.value
		      + h->root.u.def.section->output_section->vma
		      + h->root.u.def.section->output_offset);
      rel.r_info = ELF32_R_INFO (h->dynindx, R_ARM_COPY);
      bfd_elf32_swap_reloc_out (output_bfd, &rel,
				((Elf32_External_Rel *) s->contents
				 + s->reloc_count));
      ++s->reloc_count;
    }

  /* Mark _DYNAMIC and _GLOBAL_OFFSET_TABLE_ as absolute.  */
  if (strcmp (h->root.root.string, "_DYNAMIC") == 0
      || strcmp (h->root.root.string, "_GLOBAL_OFFSET_TABLE_") == 0)
    sym->st_shndx = SHN_ABS;

  return true;
}

/* Finish up the dynamic sections.  */

static boolean
elf_arm_finish_dynamic_sections (output_bfd, info)
     bfd *output_bfd;
     struct bfd_link_info *info;
{
  bfd *dynobj;
  asection *sgot;
  asection *sdyn;

  dynobj = elf_hash_table (info)->dynobj;

  sgot = bfd_get_section_by_name (dynobj, ".got.plt");
  BFD_ASSERT (sgot != NULL);
  sdyn = bfd_get_section_by_name (dynobj, ".dynamic");

  if (elf_hash_table (info)->dynamic_sections_created)
    {
      asection *splt;
      Elf32_External_Dyn *dyncon, *dynconend;

      splt = bfd_get_section_by_name (dynobj, ".plt");
      BFD_ASSERT (splt != NULL && sdyn != NULL);

      dyncon = (Elf32_External_Dyn *) sdyn->contents;
      dynconend = (Elf32_External_Dyn *) (sdyn->contents + sdyn->_raw_size);
      for (; dyncon < dynconend; dyncon++)
	{
	  Elf_Internal_Dyn dyn;
	  const char *name;
	  asection *s;

	  bfd_elf32_swap_dyn_in (dynobj, dyncon, &dyn);

	  switch (dyn.d_tag)
	    {
	    default:
	      break;

	    case DT_PLTGOT:
	      name = ".got";
	      goto get_vma;
	    case DT_JMPREL:
	      name = ".rel.plt";
	    get_vma:
	      s = bfd_get_section_by_name (output_bfd, name);
	      BFD_ASSERT (s != NULL);
	      dyn.d_un.d_ptr = s->vma;
	      bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	      break;

	    case DT_PLTRELSZ:
	      s = bfd_get_section_by_name (output_bfd, ".rel.plt");
	      BFD_ASSERT (s != NULL);
	      if (s->_cooked_size != 0)
		dyn.d_un.d_val = s->_cooked_size;
	      else
		dyn.d_un.d_val = s->_raw_size;
	      bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	      break;

	    case DT_RELSZ:
	      /* My reading of the SVR4 ABI indicates that the
		 procedure linkage table relocs (DT_JMPREL) should be
		 included in the overall relocs (DT_REL).  This is
		 what Solaris does.  However, UnixWare can not handle
		 that case.  Therefore, we override the DT_RELSZ entry
		 here to make it not include the JMPREL relocs.  Since
		 the linker script arranges for .rel.plt to follow all
		 other relocation sections, we don't have to worry
		 about changing the DT_REL entry.  */
	      s = bfd_get_section_by_name (output_bfd, ".rel.plt");
	      if (s != NULL)
		{
		  if (s->_cooked_size != 0)
		    dyn.d_un.d_val -= s->_cooked_size;
		  else
		    dyn.d_un.d_val -= s->_raw_size;
		}
	      bfd_elf32_swap_dyn_out (output_bfd, &dyn, dyncon);
	      break;
	    }
	}

      /* Fill in the first entry in the procedure linkage table.  */
      if (splt->_raw_size > 0)
	{
#if 0	
	  if (info->shared)
	    memcpy (splt->contents, elf_arm_pic_plt0_entry, PLT_ENTRY_SIZE);
	  else
	    {
#endif	    
	      memcpy (splt->contents, elf_arm_plt0_entry, PLT_ENTRY_SIZE);
#if 0	      
	      bfd_put_32 (output_bfd,
			  sgot->output_section->vma + sgot->output_offset + 12,
			  splt->contents + 12);
	    }
#endif
	}

      /* UnixWare sets the entsize of .plt to 4, although that doesn't
	 really seem like the right value.  */
      elf_section_data (splt->output_section)->this_hdr.sh_entsize = 4;
    }

  /* Fill in the first three entries in the global offset table.  */
  if (sgot->_raw_size > 0)
    {
      if (sdyn == NULL)
	bfd_put_32 (output_bfd, (bfd_vma) 0, sgot->contents);
      else
	bfd_put_32 (output_bfd,
		    sdyn->output_section->vma + sdyn->output_offset,
		    sgot->contents);
      bfd_put_32 (output_bfd, (bfd_vma) 0, sgot->contents + 4);
      bfd_put_32 (output_bfd, (bfd_vma) 0, sgot->contents + 8);
    }

  elf_section_data (sgot->output_section)->this_hdr.sh_entsize = 4;

  return true;
}

static const char *rel_sections[] =
  {
    ".rel.text", ".rel.data", ".rel.rodata", ".rel.got",
    ".rel.ctors", ".rel.dtors", ".rel.init", ".rel.fini",
    ".rel.bss", ".rel.plt", NULL
  };

static long
elf_arm_get_dynamic_reloc_upper_bound (abfd)
     bfd *abfd;
{
  long totalsize = 0;
  asection *s;
  int sec;

  for (sec = 0; rel_sections[sec]; sec++)
    {
      s = bfd_get_section_by_name (abfd, rel_sections[sec]);
      if (s)
        totalsize += s->_raw_size * sizeof (arelent *) / sizeof (Elf32_External_Rel);
    }

  if (!totalsize)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return -1;
    }

  return totalsize;
}

static long
elf_arm_canonicalize_dynamic_reloc (abfd, storage, syms)
     bfd *abfd;
     arelent **storage;
     asymbol **syms;
{
  long total_rel = 0;
  arelent *relbuf;
  int sec;


  for (sec = 0; rel_sections[sec]; sec++)
    {
      asection *s;

      s = bfd_get_section_by_name (abfd, rel_sections[sec]);

      if (s)
          total_rel += s->_raw_size / sizeof (Elf32_External_Rel);
    }

  if (total_rel == 0)
    return 0;

  relbuf = bfd_alloc (abfd, total_rel * sizeof (arelent));
  if (relbuf == NULL)
    return -1;

  for (sec = 0; rel_sections[sec]; sec++)
    {
      asection *s;

      s = bfd_get_section_by_name (abfd, rel_sections[sec]);

      if (s)
        {
          bfd_byte *contents;
          int r, nr_rel;

          contents = bfd_malloc (s->_raw_size);
          if (contents == NULL)
            return -1;

          if (!bfd_get_section_contents (abfd, s, (PTR) contents,
                                         (file_ptr) 0, s->_raw_size))
            return -1;

          nr_rel = s->_raw_size / sizeof (Elf32_External_Rel);

          for (r = 0; r < nr_rel; r++, storage++, relbuf++)
            {
              Elf_Internal_Rel rel;
              bfd_elf32_swap_reloc_in (abfd, ((Elf32_External_Rel *) contents + r), &rel);


//patb
//printf("swapping and clearing at %x in %s\n",rel.r_offset, rel_sections[sec]);

              relbuf->sym_ptr_ptr = syms + ELF32_R_SYM(rel.r_info) - 1;
              relbuf->howto = &elf_howto_table[ELF32_R_TYPE(rel.r_info)];
              relbuf->address = rel.r_offset;
              relbuf->addend = 0;
              *storage = relbuf;
            }

          free (contents);
	}
    }
  *storage = NULL;

  return total_rel;
}

/* Function to keep ARM-specific data in object headers */

boolean
elf_arm_set_private_flags (abfd, flags)
     bfd *abfd;
     flagword flags;
{
  BFD_ASSERT (!elf_flags_init (abfd)
	      || elf_elfheader (abfd)->e_flags == flags);

  elf_elfheader (abfd)->e_flags = flags;
  elf_flags_init (abfd) = true;
  return true;
}

/* Merge backend specific data from an object file to the output
   object file when linking.  */

boolean
elf_arm_merge_private_bfd_data (ibfd, obfd)
     bfd *ibfd;
     bfd *obfd;
{
  flagword old_flags;
  flagword new_flags;
  boolean ok;

  /* Check if we have the same endianess */
  if (ibfd->xvec->byteorder != obfd->xvec->byteorder
      && obfd->xvec->byteorder != BFD_ENDIAN_UNKNOWN
      && ibfd->xvec->byteorder != BFD_ENDIAN_UNKNOWN)
    {
      (*_bfd_error_handler)
	("%s: compiled for a %s endian system and target is %s endian",
	 bfd_get_filename (ibfd),
	 bfd_big_endian (ibfd) ? "big" : "little",
	 bfd_big_endian (obfd) ? "big" : "little");

      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  if (bfd_get_flavour (ibfd) != bfd_target_elf_flavour
      || bfd_get_flavour (obfd) != bfd_target_elf_flavour)
    return true;

  new_flags = elf_elfheader (ibfd)->e_flags;
  old_flags = elf_elfheader (obfd)->e_flags;

  if (! elf_flags_init (obfd))
    {
      elf_flags_init (obfd) = true;
      elf_elfheader (obfd)->e_flags = new_flags;

      if (bfd_get_arch (obfd) == bfd_get_arch (ibfd)
	  && bfd_get_arch_info (obfd)->the_default)
	{
	  if (! bfd_set_arch_mach (obfd, bfd_get_arch (ibfd),
				   bfd_get_mach (ibfd)))
	    return false;
	}

      return true;
    }

  /* Check flag compatibility.  */

  if ((new_flags & EF_APCS_26) != (old_flags & EF_APCS_26))
    {
      /* This is usually the wrong thing to do, but there are a few cases
	 where it might be legitimate.  So we warn but allow the user to
	 carry on.  */
      (*_bfd_error_handler)
	("warning: interlinking APCS-26 and APCS-32 files");
    }

  return true;
}


#define TARGET_LITTLE_SYM		bfd_elf32_arm_vec
#define TARGET_LITTLE_NAME		"elf32-arm"
#define ELF_ARCH			bfd_arch_arm
#define ELF_MACHINE_CODE		EM_ARM
#define ELF_MAXPAGESIZE			0x8000
#define elf_info_to_howto		elf_arm_info_to_howto
#define elf_info_to_howto_rel		elf_arm_info_to_howto_rel
#define bfd_elf32_bfd_reloc_type_lookup	elf_arm_reloc_type_lookup
#define bfd_elf32_bfd_set_private_flags	elf_arm_set_private_flags
#define bfd_elf32_bfd_merge_private_bfd_data elf_arm_merge_private_bfd_data
//#define bfd_elf32_get_dynamic_reloc_upper_bound \
//					elf_arm_get_dynamic_reloc_upper_bound
#define elf_backend_create_dynamic_sections \
					_bfd_elf_create_dynamic_sections
//#define bfd_elf32_canonicalize_dynamic_reloc \
//					elf_arm_canonicalize_dynamic_reloc
#define bfd_elf32_bfd_link_hash_table_create \
					elf_arm_link_hash_table_create
#define elf_backend_check_relocs	elf_arm_check_relocs
#define elf_backend_adjust_dynamic_symbol \
					elf_arm_adjust_dynamic_symbol
#define elf_backend_size_dynamic_sections \
					elf_arm_size_dynamic_sections
#define elf_backend_relocate_section	elf_arm_relocate_section
#define elf_backend_finish_dynamic_symbol \
					elf_arm_finish_dynamic_symbol
#define elf_backend_finish_dynamic_sections \
					elf_arm_finish_dynamic_sections
#define elf_backend_want_got_plt 1
#define elf_backend_plt_readonly 1
#define elf_backend_want_plt_sym 0

#include "elf32-target.h"