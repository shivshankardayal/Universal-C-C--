


/* tc-icvm.c -- Assemble for the icvm family
   Copyright (C) 1987, 91, 92, 93, 94, 95, 96, 97, 1998
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


#include <ctype.h>
#include "as.h"
#include "obstack.h"
#include "subsegs.h"

#include "opcode/icvm.h"
#include "icvm-parse.h"

/* This string holds the chars that always start a comment.  If the
   pre-processor is disabled, these aren't very useful.  The macro
   tc_comment_chars points to this.  We use this, rather than the
   usual comment_chars, so that the --bitwise-or option will work.  */

void BE_md_number_to_chars ();

#if defined (TE_SVR4) || defined (TE_DELTA)
const char *icvm_comment_chars = "|#";
#else
const char *icvm_comment_chars = "|;";
#endif

/* This array holds the chars that only start a comment at the beginning of
   a line.  If the line seems to have the form '# 123 filename'
   .line and .file directives will appear in the pre-processed output */
/* Note that input_file.c hand checks for '#' at the beginning of the
   first line of the input file.  This is because the compiler outputs
   #NO_APP at the beginning of its output. */
/* Also note that comments like this one will always work. */
const char line_comment_chars[] = "#*";

const char line_separator_chars[] = "";

/* Chars that can be used to separate mant from exp in floating point nums */
CONST char EXP_CHARS[] = "eE";

/* Chars that mean this number is a floating point constant, as
   in "0f12.456" or "0d1.2345e12".  */

CONST char FLT_CHARS[] = "rRsSfFdDxXeEpP";

/* Also be aware that MAXIMUM_NUMBER_OF_CHARS_FOR_FLOAT may have to be
   changed in read.c .  Ideally it shouldn't have to know about it at all,
   but nothing is ideal around here.  */

const int md_reloc_size = 8;	/* Size of relocation record */

/* Are we trying to generate PIC code?  If so, absolute references
   ought to be made into linkage table references or pc-relative
   references.  Not implemented.  For ELF there are other means 
   to denote pic relocations.  */
int flag_want_pic;

static int flag_short_refs;	/* -l option */
static int flag_long_jumps;	/* -S option */

extern int target_big_endian;

segT Atext_section, Adata_section, Abss_section;
segT BEtext_section, BEdata_section, BErdata_section, BEbss_section,
	BEctors_section,BEdtors_section;
segT LEtext_section, LEdata_section, LErdata_section, LEbss_section,
	LEctors_section,LEdtors_section;

#ifdef REGISTER_PREFIX_OPTIONAL
int flag_reg_prefix_optional = REGISTER_PREFIX_OPTIONAL;
#else
int flag_reg_prefix_optional;
#endif

/* Whether --register-prefix-optional was used on the command line.  */
static int reg_prefix_optional_seen;

/* The floating point coprocessor to use by default.  */
static enum icvm_register icvm_float_copnum = COP1;

/* If this is non-zero, then references to number(%pc) will be taken
   to refer to number, rather than to %pc + number.  */
static int icvm_abspcadd;

/* If this is non-zero, then the quick forms of the move, add, and sub
   instructions are used when possible.  */
static int icvm_quick = 1;

/* If this is non-zero, then if the size is not specified for a base
   or outer displacement, the assembler assumes that the size should
   be 32 bits.  */
static int icvm_rel32 = 1;

/* This is non-zero if icvm_rel32 was set from the command line.  */
static int icvm_rel32_from_cmdline;

/* The default width to use for an index register when using a base
   displacement.  */
static enum icvm_size icvm_index_width_default = SIZE_LONG;

/* We want to warn if any text labels are misaligned.  In order to get
   the right line number, we need to record the line number for each
   label.  */

struct label_line
{
  struct label_line *next;
  symbolS *label;
  char *file;
  unsigned int line;
  int text;
};

/* The list of labels.  */

static struct label_line *labels;

/* The current label.  */

static struct label_line *current_label;

/* Its an arbitrary name:  This means I don't approve of it */
/* See flames below */
static struct obstack robyn;

#define TAB(x,y)	(((x)<<2)+(y))
#define TABTYPE(xy)     ((xy) >> 2)
#define BYTE		0
#define SHORT		1
#define LONG		2
#define SZ_UNDEF	3
#undef BRANCH
/* Case `g' except when BCC68000 is applicable.  */
#define ABRANCH		1
/* Coprocessor branches.  */
#define FBRANCH		2
/* Mode 7.2 -- program counter indirect with (16-bit) displacement,
   supported on all cpus.  Widens to 32-bit absolute.  */
#define PCREL		3
/* For inserting an extra jmp instruction with long offset on 68000,
   for expanding conditional branches.  (Not bsr or bra.)  Since the
   68000 doesn't support 32-bit displacements for conditional
   branches, we fake it by reversing the condition and branching
   around a jmp with an absolute long operand.  */
#define BCC68000        4
/* For the DBcc "instructions".  If the displacement requires 32 bits,
   the branch-around-a-jump game is played here too.  */
#define DBCC            5
/* Not currently used?  */
#define PCLEA		6
/* Mode AINDX (apc-relative) using PC, with variable target, might fit
   in 16 or 8 bits.  */
#define PCINDEX		7

struct icvm_incant
  {
    const char *m_operands;
    unsigned long m_opcode;
    unsigned long m_match;
    short m_opnum;
    short m_codenum;
    struct icvm_incant *m_next;
  };

#define getone(x)	((((x)->m_opcode)>>16)&0xffff)
#define gettwo(x)	(((x)->m_opcode)&0xffff)

#define checktwoop(x)	(twoop=((((x)->m_match)&0xffff)!=0))

int twoop;

static const enum icvm_register *control_regs;

/* internal form of an instruction */
struct icvm_it
{
  const char *error;
  const char *args;		/* list of opcode info */
  int numargs;

  int numo;			/* Number of shorts in opcode */
  short opcode[11];

  struct icvm_op operands[6];

  int nexp;			/* number of exprs in use */
  struct icvm_exp exprs[4];

  int nfrag;			/* Number of frags we have to produce */
  struct
    {
      int fragoff;		/* Where in the current opcode the frag ends */
      symbolS *fadd;
      offsetT foff;
      int fragty;
    }
  fragb[4];

  int nrel;			/* Num of reloc strucs in use */
  struct
    {
      int n;
      expressionS exp;
      char wid;
      char pcrel;
      /* In a pc relative address the difference between the address
	 of the offset and the address that the offset is relative
	 to.  This depends on the addressing mode.  Basically this
	 is the value to put in the offset field to address the
	 first byte of the offset, without regarding the special
	 significance of some values (in the branch instruction, for
	 example).  */
      int pcrel_fix;
#ifdef OBJ_ELF
      /* Whether this expression needs special pic relocation, and if
	 so, which.  */
      enum pic_relocation pic_reloc;
#endif
    }
  reloc[5];			/* Five is enough??? */
};

#define cpu_of_arch(x)		(1)

/* Macros for determining if cpu supports a specific addressing mode */
#define HAVE_LONG_BRANCH(x)	(1)

static struct icvm_it the_ins;	/* the instruction being assembled */

#define op(ex)		((ex)->exp.X_op)
#define adds(ex)	((ex)->exp.X_add_symbol)
#define subs(ex)	((ex)->exp.X_op_symbol)
#define offs(ex)	((ex)->exp.X_add_number)
#define xunsigned(ex)	((ex)->exp.X_unsigned)



/* endian independant readers and writers */

				
inline void putword(void *where,int what){
	char *d=where;
	if(target_big_endian){
		d[0]=what>>8;
		d[1]=what;
	}else{
		d[0]=what;
		d[1]=what>>8;
	}
}

inline int  getword(void *where){
	unsigned char *s=where;
	long l=0;
	if(target_big_endian){
		l=s[0];
		l<<=8;
		l|=s[1];
	}else{
		l=s[1];
		l<<=8;
		l|=s[0];
	}
	return l;
}
				
inline void orword(void *where,int what){
	putword(where,getword(where)|what);
}

inline void putsomething(void *where,void *what,int sz){
	unsigned short *s=(unsigned short *) what;
	unsigned short *d=(unsigned short *)where;
	int cnt=0;
	if(target_big_endian){
		while(cnt<sz){
			d[cnt]=(s[cnt]<<8) | ((s[cnt]>>8) & 0xff);
			++cnt;
		}
	}else{
		int n=sz-1;
		while(cnt<sz){
			d[cnt]=s[n];
			++cnt;
			--n;
		}
	}

}




#define addsomething(w,sz) {\
	putsomething(&(the_ins.opcode[the_ins.numo]),w,sz);\
	the_ins.numo+=sz;\
}


inline void putlong(void *where,long what){
	char *d=where;
	if(target_big_endian){
		d[0]=what>>24;
		d[1]=what>>16;
		d[2]=what>>8;
		d[3]=what;
	}else{
		d[0]=what;
		d[1]=what>>8;
		d[2]=what>>16;
		d[3]=what>>24;
	}
}
											
inline void putlonglong(void *where,long long what){
	char *d=where;
	if(target_big_endian){
		d[0]=what>>56;
		d[1]=what>>48;
		d[2]=what>>40;
		d[3]=what>>32;
		d[4]=what>>24;
		d[5]=what>>16;
		d[6]=what>>8;
		d[7]=what;
	}else{
		d[0]=what;
		d[1]=what>>8;
		d[2]=what>>16;
		d[3]=what>>24;
		d[4]=what>>32;
		d[5]=what>>40;
		d[6]=what>>48;
		d[7]=what>>56;
	}
}



inline long  getlong(void *where){
	unsigned char *s=where;
	long l=0;
	if(target_big_endian){
		l=s[0];
		l<<=8;
		l|=s[1];
		l<<=8;
		l|=s[2];
		l<<=8;
		l|=s[3];
	}else{
		l=s[3];
		l<<=8;
		l|=s[2];
		l<<=8;
		l|=s[1];
		l<<=8;
		l|=s[0];
	}
	return l;
}

inline void orlong(void *where,long what){
	putlong(where,getlong(where)|what);
}

/* Macros for adding things to the icvm_it struct */

#define addword(w) {\
	putword(&(the_ins.opcode[the_ins.numo]),w);\
	the_ins.numo++;\
}

#define addlong(w) {\
	putlong(&(the_ins.opcode[the_ins.numo]),w);\
	the_ins.numo+=2;\
}

#define addlonglong(w) {\
	putlonglong(&(the_ins.opcode[the_ins.numo]),w);\
	the_ins.numo+=4;\
}


#ifdef BOB_ALIGN32
#define aligninstlong(w) {if(the_ins.numo & 1) addword(w);}
#else
#define aligninstlong(w)
#endif
						
/* Static functions.  */

static void add_fix PARAMS ((int, struct icvm_exp *, int, int));
static void add_frag PARAMS ((symbolS *, offsetT, int));


/* The numo+1 kludge is so we can hit the low order byte of the prev word.
   Blecch.  */
static void
add_fix (width, exp, pc_rel, pc_fix)
     int width;
     struct icvm_exp *exp;
     int pc_rel;
     int pc_fix;
{
  the_ins.reloc[the_ins.nrel].n = ((width == 'B' || width == '3')
				   ? (the_ins.numo*2-1)
				   : (((width)=='b')
				      ? (the_ins.numo*2+1)
				      : (the_ins.numo*2)));
  the_ins.reloc[the_ins.nrel].exp = exp->exp;
  the_ins.reloc[the_ins.nrel].wid = width;
  the_ins.reloc[the_ins.nrel].pcrel_fix = pc_fix;
#ifdef OBJ_ELF
  the_ins.reloc[the_ins.nrel].pic_reloc = exp->pic_reloc;
#endif
  the_ins.reloc[the_ins.nrel++].pcrel = pc_rel;
}

/* Cause an extra frag to be generated here, inserting up to 10 bytes
   (that value is chosen in the frag_var call in md_assemble).  TYPE
   is the subtype of the frag to be generated; its primary type is
   rs_machine_dependent.

   The TYPE parameter is also used by md_convert_frag_1 and
   md_estimate_size_before_relax.  The appropriate type of fixup will
   be emitted by md_convert_frag_1.

   ADD becomes the FR_SYMBOL field of the frag, and OFF the FR_OFFSET.  */
static void
add_frag (add, off, type)
     symbolS *add;
     offsetT off;
     int type;
{
  the_ins.fragb[the_ins.nfrag].fragoff=the_ins.numo;
  the_ins.fragb[the_ins.nfrag].fadd=add;
  the_ins.fragb[the_ins.nfrag].foff=off;
  the_ins.fragb[the_ins.nfrag++].fragty=type;
}

#define isvar(ex) \
  (op (ex) != O_constant && op (ex) != O_big)

static char *crack_operand PARAMS ((char *str, struct icvm_op *opP));
static long long get_num PARAMS ((struct icvm_exp *exp, int ok));
static void icvm_ip PARAMS ((char *));
static void insert_reg PARAMS ((const char *, int));
static void init_regtable PARAMS ((void));
static int reverse_16_bits PARAMS ((int in));
static int reverse_8_bits PARAMS ((int in));
static void install_gen_operand PARAMS ((int mode, int val));
static void install_operand PARAMS ((int mode, int val));
static void s_bss PARAMS ((int));
static void s_relocation PARAMS ((int));
static void s_readonly PARAMS ((int));
static void s_ctors PARAMS ((int));
static void s_dtors PARAMS ((int));
static void s_data1 PARAMS ((int));
static void s_data2 PARAMS ((int));
static void s_even PARAMS ((int));
static void s_proc PARAMS ((int));
static void mri_chip PARAMS ((void));
static void s_chip PARAMS ((int));
static void s_fopt PARAMS ((int));
static void s_opt PARAMS ((int));
static void s_reg PARAMS ((int));
static void s_restore PARAMS ((int));
static void s_save PARAMS ((int));
static void s_mri_if PARAMS ((int));
static void s_mri_else PARAMS ((int));
static void s_mri_endi PARAMS ((int));
static void s_mri_break PARAMS ((int));
static void s_mri_next PARAMS ((int));
static void s_mri_for PARAMS ((int));
static void s_mri_endf PARAMS ((int));
static void s_mri_repeat PARAMS ((int));
static void s_mri_until PARAMS ((int));
static void s_mri_while PARAMS ((int));
static void s_mri_endw PARAMS ((int));
static void md_apply_fix_2 PARAMS ((fixS *, offsetT));
static void md_convert_frag_1 PARAMS ((fragS *));


/* there is alot of old 68000 junk still laying around in here.
 I will leave some of it, so I do not break anything.
*/

static int current_architecture=1;

struct icvm_cpu {
  unsigned long arch;
  const char *name;
  int alias;
};

/* really need to clean this up!!!! */


static const struct icvm_cpu archs[] = {
  { 0, "icvm", 0 },
};

static const int n_archs = sizeof (archs) / sizeof (archs[0]);


/* This table desribes how you change sizes for the various types of variable
   size expressions.  This version only supports two kinds. */

/* Note that calls to frag_var need to specify the maximum expansion
   needed; this is currently 10 bytes for DBCC.  */

/* The fields are:
   How far Forward this mode will reach:
   How far Backward this mode will reach:
   How many bytes this mode will add to the size of the frag
   Which mode to go to if the offset won't fit in this one
   */
relax_typeS md_relax_table[] =
{
  {1, 1, 0, 0},			/* First entries aren't used */
  {1, 1, 0, 0},			/* For no good reason except */
  {1, 1, 0, 0},			/* that the VAX doesn't either */
  {1, 1, 0, 0},

  {(127), (-128), 0, TAB (ABRANCH, SHORT)},
  {(32767), (-32768), 2, TAB (ABRANCH, LONG)},
  {0, 0, 4, 0},
  {1, 1, 0, 0},

  {1, 1, 0, 0},			/* FBRANCH doesn't come BYTE */
  {(32767), (-32768), 2, TAB (FBRANCH, LONG)},
  {0, 0, 4, 0},
  {1, 1, 0, 0},

  {1, 1, 0, 0},			/* PCREL doesn't come BYTE */
  {(32767), (-32768), 2, TAB (PCREL, LONG)},
  {0, 0, 4, 0},
  {1, 1, 0, 0},

  {(127), (-128), 0, TAB (BCC68000, SHORT)},
  {(32767), (-32768), 2, TAB (BCC68000, LONG)},
  {0, 0, 6, 0},			/* jmp long space */
  {1, 1, 0, 0},

  {1, 1, 0, 0},			/* DBCC doesn't come BYTE */
  {(32767), (-32768), 2, TAB (DBCC, LONG)},
  {0, 0, 10, 0},		/* bra/jmp long space */
  {1, 1, 0, 0},

  {1, 1, 0, 0},			/* PCLEA doesn't come BYTE */
  {32767, -32768, 2, TAB (PCLEA, LONG)},
  {0, 0, 6, 0},
  {1, 1, 0, 0},

  /* For, e.g., jmp pcrel indexed.  */
  {125, -130, 0, TAB (PCINDEX, SHORT)},
  {32765, -32770, 2, TAB (PCINDEX, LONG)},
  {0, 0, 4, 0},
  {1, 1, 0, 0},
};

/* These are the machine dependent pseudo-ops.  These are included so
   the assembler can work on the output from the SUN C compiler, which
   generates these.
   */

/* This table describes all the machine specific pseudo-ops the assembler
   has to support.  The fields are:
   pseudo-op name without dot
   function to call to execute this pseudo-op
   Integer arg to pass to the function
   */
const pseudo_typeS md_pseudo_table[] =
{
  {"rlp", s_relocation, 0},
  {"rdata", s_readonly, 0},
  {"ctors", s_ctors, 0},
  {"dtors", s_dtors, 0},
  {"data1", s_data1, 0},
  {"data2", s_data2, 0},
  {"bss", s_bss, 0},
  {"even", s_even, 0},
  {"skip", s_space, 0},
  {"proc", s_proc, 0},
  {"align", s_align_bytes, 0},
#ifdef OBJ_ELF
  {"swbeg", s_ignore, 0},
#endif
  {"extend", float_cons, 'x'},
  {"ldouble", float_cons, 'x'},

  /* The following pseudo-ops are supported for MRI compatibility.  */
  {"chip", s_chip, 0},
  {"comline", s_space, 1},
  {"fopt", s_fopt, 0},
  {"mask2", s_ignore, 0},
  {"opt", s_opt, 0},
  {"reg", s_reg, 0},
  {"restore", s_restore, 0},
  {"save", s_save, 0},

  {"if", s_mri_if, 0},
  {"if.b", s_mri_if, 'b'},
  {"if.w", s_mri_if, 'w'},
  {"if.l", s_mri_if, 'l'},
  {"else", s_mri_else, 0},
  {"else.s", s_mri_else, 's'},
  {"else.l", s_mri_else, 'l'},
  {"endi", s_mri_endi, 0},
  {"break", s_mri_break, 0},
  {"break.s", s_mri_break, 's'},
  {"break.l", s_mri_break, 'l'},
  {"next", s_mri_next, 0},
  {"next.s", s_mri_next, 's'},
  {"next.l", s_mri_next, 'l'},
  {"for", s_mri_for, 0},
  {"for.b", s_mri_for, 'b'},
  {"for.w", s_mri_for, 'w'},
  {"for.l", s_mri_for, 'l'},
  {"endf", s_mri_endf, 0},
  {"repeat", s_mri_repeat, 0},
  {"until", s_mri_until, 0},
  {"until.b", s_mri_until, 'b'},
  {"until.w", s_mri_until, 'w'},
  {"until.l", s_mri_until, 'l'},
  {"while", s_mri_while, 0},
  {"while.b", s_mri_while, 'b'},
  {"while.w", s_mri_while, 'w'},
  {"while.l", s_mri_while, 'l'},
  {"endw", s_mri_endw, 0},

  {0, 0, 0}
};


/* The mote pseudo ops are put into the opcode table, since they
   don't start with a . they look like opcodes to gas.
   */

#ifdef M68KCOFF
extern void obj_coff_section PARAMS ((int));
#endif

CONST pseudo_typeS mote_pseudo_table[] =
{

  {"dcl", cons, 4},
  {"dc", cons, 2},
  {"dcw", cons, 2},
  {"dcb", cons, 1},

  {"dsl", s_space, 4},
  {"ds", s_space, 2},
  {"dsw", s_space, 2},
  {"dsb", s_space, 1},

  {"xdef", s_globl, 0},
#ifdef OBJ_ELF
  {"align", s_align_bytes, 0},
#else
  {"align", s_align_ptwo, 0},
#endif
#ifdef M68KCOFF
  {"sect", obj_coff_section, 0},
  {"section", obj_coff_section, 0},
#endif
  {0, 0, 0}
};

#define issbyte(x)	((x)>=-128 && (x)<=127)
#define isubyte(x)	((x)>=0 && (x)<=255)
#define issword(x)	((x)>=-32768 && (x)<=32767)
#define isuword(x)	((x)>=0 && (x)<=65535)

#define isbyte(x)	((x)>= -255 && (x)<=255)
#define isword(x)	((x)>=-65536 && (x)<=65535)
#define islong(x)	((((long long) x ) & 0xffffffff00000000) ==0)

extern char *input_line_pointer;

static char mklower_table[256];
#define mklower(c) (mklower_table[(unsigned char)(c)])
static char notend_table[256];
static char alt_notend_table[256];
#define notend(s)						\
  (! (notend_table[(unsigned char) *s]				\
      || (*s == ':'						\
	  && alt_notend_table[(unsigned char) s[1]])))

#if defined (M68KCOFF) && !defined (BFD_ASSEMBLER)

#ifdef NO_PCREL_RELOCS

int
make_pcrel_absolute(fixP, add_number)
    fixS *fixP;
    long *add_number;
{
  register unsigned char *opcode = fixP->fx_frag->fr_opcode;

  /* rewrite the PC relative instructions to absolute address ones.
   * these are rumoured to be faster, and the apollo linker refuses
   * to deal with the PC relative relocations.
   */
  if (opcode[0] == 0x60 && opcode[1] == 0xff) /* BRA -> JMP */
    {
      opcode[0] = 0x4e;
      opcode[1] = 0xf9;
    }
  else if (opcode[0] == 0x61 && opcode[1] == 0xff) /* BSR -> JSR */
    {
      opcode[0] = 0x4e;
      opcode[1] = 0xb9;
    }
  else
    as_fatal ("Unknown PC relative instruction");
  *add_number -= 4;
  return 0;
}

#endif /* NO_PCREL_RELOCS */

short
tc_coff_fix2rtype (fixP)
     fixS *fixP;
{
  if (fixP->fx_tcbit && fixP->fx_size == 4)
    return R_RELLONG_NEG;
#ifdef NO_PCREL_RELOCS
  know (fixP->fx_pcrel == 0);
  return (fixP->fx_size == 1 ? R_RELBYTE
	  : fixP->fx_size == 2 ? R_DIR16
	  : R_DIR32);
#else
  return (fixP->fx_pcrel ?
	  (fixP->fx_size == 1 ? R_PCRBYTE :
	   fixP->fx_size == 2 ? R_PCRWORD :
	   R_PCRLONG) :
	  (fixP->fx_size == 1 ? R_RELBYTE :
	   fixP->fx_size == 2 ? R_RELWORD :
	   R_RELLONG));
#endif
}

#endif

#ifdef OBJ_ELF

/* Compute the relocation code for a fixup of SIZE bytes, using pc
   relative relocation if PCREL is non-zero.  PIC says whether a special
   pic relocation was requested.  */

static bfd_reloc_code_real_type get_reloc_code
  PARAMS ((int, int, enum pic_relocation));

static bfd_reloc_code_real_type
get_reloc_code (size, pcrel, pic)
     int size;
     int pcrel;
     enum pic_relocation pic;
{
  switch (pic)
    {
    case pic_got_pcrel:
      switch (size)
	{
	case 1:
	  return BFD_RELOC_8_GOT_PCREL;
	case 2:
	  return BFD_RELOC_16_GOT_PCREL;
	case 4:
	  return BFD_RELOC_32_GOT_PCREL;
	}
      break;

    case pic_got_off:
      switch (size)
	{
	case 1:
	  return BFD_RELOC_8_GOTOFF;
	case 2:
	  return BFD_RELOC_16_GOTOFF;
	case 4:
	  return BFD_RELOC_32_GOTOFF;
	}
      break;

    case pic_plt_pcrel:
      switch (size)
	{
	case 1:
	  return BFD_RELOC_8_PLT_PCREL;
	case 2:
	  return BFD_RELOC_16_PLT_PCREL;
	case 4:
	  return BFD_RELOC_32_PLT_PCREL;
	}
      break;

    case pic_plt_off:
      switch (size)
	{
	case 1:
	  return BFD_RELOC_8_PLTOFF;
	case 2:
	  return BFD_RELOC_16_PLTOFF;
	case 4:
	  return BFD_RELOC_32_PLTOFF;
	}
      break;

    case pic_none:
      if (pcrel)
	{
	  switch (size)
	    {
	    case 1:
	      return BFD_RELOC_8_PCREL;
	    case 2:
	      return BFD_RELOC_16_PCREL;
	    case 4:
	      return BFD_RELOC_32_PCREL;
	    }
	}
      else
	{
	  switch (size)
	    {
	    case 1:
	      return BFD_RELOC_8;
	    case 2:
	      return BFD_RELOC_16;
	    case 4:
	      return BFD_RELOC_32;
	    }
	}

    }

  as_bad ("Can not do %d byte %s%srelocation", size,
	  pcrel ? "pc-relative " : "",
	  pic == pic_none ? "" : "pic ");
  return BFD_RELOC_NONE;
}

/* Here we decide which fixups can be adjusted to make them relative
   to the beginning of the section instead of the symbol.  Basically
   we need to make sure that the dynamic relocations are done
   correctly, so in some cases we force the original symbol to be
   used.  */
int
tc_icvm_fix_adjustable (fixP)
     fixS *fixP;
{

#ifdef NOT_NOW
/* ************ THIS CAUSED BIG PROBLEMS */
  /* Prevent all adjustments to global symbols. */
  if (S_IS_EXTERNAL (fixP->fx_addsy))
    return 0;
#endif

  /* adjust_reloc_syms doesn't know about the GOT */
  switch (fixP->fx_r_type)
    {
    case BFD_RELOC_8_GOT_PCREL:
    case BFD_RELOC_16_GOT_PCREL:
    case BFD_RELOC_32_GOT_PCREL:
    case BFD_RELOC_8_GOTOFF:
    case BFD_RELOC_16_GOTOFF:
    case BFD_RELOC_32_GOTOFF:
    case BFD_RELOC_8_PLT_PCREL:
    case BFD_RELOC_16_PLT_PCREL:
    case BFD_RELOC_32_PLT_PCREL:
    case BFD_RELOC_8_PLTOFF:
    case BFD_RELOC_16_PLTOFF:
    case BFD_RELOC_32_PLTOFF:
      return 0;

    default:
      return 1;
    }
}

#else /* !OBJ_ELF */

#define get_reloc_code(SIZE,PCREL,OTHER) NO_RELOC

#endif /* OBJ_ELF */

#ifdef BFD_ASSEMBLER

arelent *
tc_gen_reloc (section, fixp)
     segT section;
     fixS *fixp;
{
  arelent *reloc;
  bfd_reloc_code_real_type code;

  if (fixp->fx_tcbit)
    abort ();

  if (fixp->fx_r_type != BFD_RELOC_NONE)
    {
      code = fixp->fx_r_type;

      /* Since DIFF_EXPR_OK is defined in tc-icvm.h, it is possible
         that fixup_segment converted a non-PC relative reloc into a
         PC relative reloc.  In such a case, we need to convert the
         reloc code.  */
      if (fixp->fx_pcrel)
	{
	  switch (code)
	    {
	    case BFD_RELOC_8:
	      code = BFD_RELOC_8_PCREL;
	      break;
	    case BFD_RELOC_16:
	      code = BFD_RELOC_16_PCREL;
	      break;
	    case BFD_RELOC_32:
	      code = BFD_RELOC_32_PCREL;
	      break;
	    case BFD_RELOC_8_PCREL:
	    case BFD_RELOC_16_PCREL:
	    case BFD_RELOC_32_PCREL:
	    case BFD_RELOC_8_GOT_PCREL:
	    case BFD_RELOC_16_GOT_PCREL:
	    case BFD_RELOC_32_GOT_PCREL:
	    case BFD_RELOC_8_GOTOFF:
	    case BFD_RELOC_16_GOTOFF:
	    case BFD_RELOC_32_GOTOFF:
	    case BFD_RELOC_8_PLT_PCREL:
	    case BFD_RELOC_16_PLT_PCREL:
	    case BFD_RELOC_32_PLT_PCREL:
	    case BFD_RELOC_8_PLTOFF:
	    case BFD_RELOC_16_PLTOFF:
	    case BFD_RELOC_32_PLTOFF:
	      break;
	    default:
	      as_bad_where (fixp->fx_file, fixp->fx_line,
			    "Cannot make %s relocation PC relative",
			    bfd_get_reloc_code_name (code));
	    }
	}
    }
  else
    {
#define F(SZ,PCREL)		(((SZ) << 1) + (PCREL))
      switch (F (fixp->fx_size, fixp->fx_pcrel))
	{
#define MAP(SZ,PCREL,TYPE)	case F(SZ,PCREL): code = (TYPE); break
	  MAP (1, 0, BFD_RELOC_8);
	  MAP (2, 0, BFD_RELOC_16);
	  MAP (4, 0, BFD_RELOC_32);
	  MAP (1, 1, BFD_RELOC_8_PCREL);
	  MAP (2, 1, BFD_RELOC_16_PCREL);
	  MAP (4, 1, BFD_RELOC_32_PCREL);
	default:
	  abort ();
	}
    }
#undef F
#undef MAP

  reloc = (arelent *) xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr = &fixp->fx_addsy->bsym;
  reloc->address = fixp->fx_frag->fr_address + fixp->fx_where;
#ifndef OBJ_ELF
  if (fixp->fx_pcrel)
    reloc->addend = fixp->fx_addnumber;
  else
    reloc->addend = 0;
#else
  if (!fixp->fx_pcrel)
    reloc->addend = fixp->fx_addnumber;
  else
    reloc->addend = (section->vma
		     + (fixp->fx_pcrel_adjust == 64
			? -1 : fixp->fx_pcrel_adjust)
		     + fixp->fx_addnumber
		     + md_pcrel_from (fixp));
#endif

  reloc->howto = bfd_reloc_type_lookup (stdoutput, code);
  assert (reloc->howto != 0);

  return reloc;
}

#endif /* BFD_ASSEMBLER */

/* return a reversal for a branch instruction, if input is a branch,
  else return 0  - does NOT revers beq or bne, but does return string,
  indicating it is reversable.
*/

char *GetBranchReversal(char *s){
	if(strcmp("cblt",s)==0)
		return "cbgt";
	if(strcmp("cble",s)==0)
		return "cbge";
	if(strcmp("cbgt",s)==0)
		return "cblt";
	if(strcmp("cbge",s)==0)
		return "cble";
	if(strcmp("cbeq",s)==0)
		return "cbeq";
	if(strcmp("cbne",s)==0)
		return "cbne";
	return 0;
}

/* takes a string, truncate it locally, check it for branch reversal,
   and if it is reversable, reverse it and return 1. Else, return 0 */
int DoBranchReversal(char *s){
	char *rv;
	char buf[8];
	buf[0]=s[0];
	buf[1]=s[1];
	buf[2]=s[2];
	buf[3]=s[3];
	buf[4]=0;
	rv=GetBranchReversal(buf);
	if(rv){ /* was reversed, place in reversal and return */
		s[0]=rv[0];
		s[1]=rv[1];
		s[2]=rv[2];
		s[3]=rv[3];
		return 1;
	}
	return 0;
}

/* see if this is a cummulative operation optimizable for triple ordering */

int IsTripleCumm(char *s){
	if(strcmp("addd",s)==0)
		return 1;
	if(strcmp("addf",s)==0)
		return 1;
	if(strcmp("addq",s)==0)
		return 1;
	if(strcmp("addl",s)==0)
		return 1;
	if(strcmp("addw",s)==0)
		return 1;
	if(strcmp("addb",s)==0)
		return 1;
	if(strcmp("muld",s)==0)
		return 1;
	if(strcmp("mulf",s)==0)
		return 1;
	if(strcmp("mulq",s)==0)
		return 1;
	if(strcmp("mull",s)==0)
		return 1;
	if(strcmp("mulw",s)==0)
		return 1;
	if(strcmp("mulb",s)==0)
		return 1;
	if(strcmp("andq",s)==0)
		return 1;
	if(strcmp("andl",s)==0)
		return 1;
	if(strcmp("andw",s)==0)
		return 1;
	if(strcmp("andb",s)==0)
		return 1;
	if(strcmp("orq",s)==0)
		return 1;
	if(strcmp("orl",s)==0)
		return 1;
	if(strcmp("orw",s)==0)
		return 1;
	if(strcmp("orb",s)==0)
		return 1;
	if(strcmp("eorq",s)==0)
		return 1;
	if(strcmp("eorl",s)==0)
		return 1;
	if(strcmp("eorw",s)==0)
		return 1;
	if(strcmp("eorb",s)==0)
		return 1;

	return 0;
}

/* Handle of the OPCODE hash table.  NULL means any use before
   icvm_ip_begin() will crash.  */

static struct hash_control *op_hash;



/* Assemble an icvm instruction.  */

static void
icvm_ip (instring)
     char *instring;
{
  register char *p;
  register struct icvm_op *opP;
  register const struct icvm_incant *opcode;
  register const char *s;
  register int tmpreg = 0, baseo = 0, outro = 0;
  long long  nextword,nextword1;
  char *pdot, *pdotmove;
  enum icvm_size siz1, siz2;
  char c;
  char  opcodestring[512];
  int losing;
  int opsfound;
  LITTLENUM_TYPE words[6];
  LITTLENUM_TYPE *wordp;
  unsigned long ok_arch = 0;
  struct icvm_op swapop;
  if (*instring == ' ')
    instring++;			/* skip leading whitespace */

  /* Scan up to end of operation-code, which MUST end in end-of-string
     or exactly 1 space. */
  pdot = 0;
  for (p = instring; *p != '\0'; p++)
	if(*p=='|')
		*p=0; /* force comment line back in */
  
  for (p = instring; *p != '\0'; p++)
    {
      if (*p == ' ')
	break;
      if (*p == '.')
	pdot = p;
    }

reparse:
  if (p == instring)
    {
      the_ins.error = "No operator";
      return;
    }

  /* p now points to the end of the opcode name, probably whitespace.
     Make sure the name is null terminated by clobbering the
     whitespace, look it up in the hash table, then fix it back.
     Remove a dot, first, since the opcode tables have none.  */
  if (pdot != NULL)
    {
      for (pdotmove = pdot; pdotmove < p; pdotmove++)
	*pdotmove = pdotmove[1];
      p--;
    }

  c = *p;
  *p = '\0';
  strcpy(opcodestring,instring); /* make local copy of opcode only */
  opcode = (const struct icvm_incant *) hash_find (op_hash, instring);
  *p = c;

  if (pdot != NULL)
    {
      for (pdotmove = p; pdotmove > pdot; pdotmove--)
	*pdotmove = pdotmove[-1];
      *pdot = '.';
      ++p;
    }

  if (opcode == NULL)
    {
      the_ins.error = "Unknown operator";
      return;
    }

  /* found a legitimate opcode, start matching operands */
  while (*p == ' ')
    ++p;

  if (opcode->m_operands == 0)
    {
      char *old = input_line_pointer;
      *old = '\n';
      input_line_pointer = p;
      /* Ahh - it's a motorola style psuedo op */
      mote_pseudo_table[opcode->m_opnum].poc_handler
	(mote_pseudo_table[opcode->m_opnum].poc_val);
      input_line_pointer = old;
      *old = 0;

      return;
    }

  if (flag_mri && opcode->m_opnum == 0)
    {
      /* In MRI mode, random garbage is allowed after an instruction
         which accepts no operands.  */
      the_ins.args = opcode->m_operands;
      the_ins.numargs = opcode->m_opnum;
      the_ins.numo = opcode->m_codenum;
      putword(&(the_ins.opcode[0]),getone (opcode));
      putword(&(the_ins.opcode[1]),gettwo (opcode));
	  checktwoop(opcode);
      return;
    }



  for (opP = &the_ins.operands[0]; *p; opP++)
    {
      p = crack_operand (p, opP);

      if (opP->error)
	{
	  the_ins.error = opP->error;
	  return;
	}
    }


  opsfound = opP - &the_ins.operands[0];


/* at this point opcodestring contains the null terminated opcode.
   we need to examine it and the operands and see if we need to switch orders
   on anything - BOB */

/* fixup any branch reversals needed */
	if(opsfound==3){
		if(the_ins.operands[0].mode > the_ins.operands[1].mode){ 
mightswap1:
			if(DoBranchReversal(instring)){
				/* update our opcode string as well */
			/* needs to be swapped */
				DoBranchReversal(opcodestring);
				swapop=the_ins.operands[0];
				the_ins.operands[0]=the_ins.operands[1];
				the_ins.operands[1]=swapop;

				/* look up new opcode */			
	  			opcode = (const struct icvm_incant *) 
					hash_find (op_hash, opcodestring);

			}										
		}else if(the_ins.operands[0].mode == the_ins.operands[1].mode){ 
			if( (the_ins.operands[0].mode==MODE_r ||
					the_ins.operands[0].mode==MODE_F ||
					the_ins.operands[0].mode==MODE_A ||
					the_ins.operands[0].mode==MODE_a ||
					the_ins.operands[0].mode==MODE_c)
					&& the_ins.operands[0].reg > the_ins.operands[1].reg)
				goto mightswap1;			

		}
	}


	/* see if reordering might be needed */
	if(opsfound==3 && IsTripleCumm(opcodestring)){
		if(the_ins.operands[0].mode > the_ins.operands[1].mode){ 
			/* needs to be swapped */
doswap:
			swapop=the_ins.operands[0];
			the_ins.operands[0]=the_ins.operands[1];
			the_ins.operands[1]=swapop;
			if(the_ins.operands[1].mode==MODE_a && /* cluge */
				the_ins.operands[2].mode==MODE_a &&
				the_ins.operands[1].reg == the_ins.operands[2].reg

			) /* cluge */
				opsfound=2;
		}										
		else if(the_ins.operands[0].mode == the_ins.operands[1].mode){ 
/* register checks */
			if( (the_ins.operands[0].mode==MODE_r ||
					the_ins.operands[0].mode==MODE_F ||
					the_ins.operands[0].mode==MODE_A ||
					the_ins.operands[0].mode==MODE_a ||
					the_ins.operands[0].mode==MODE_c)
				&& the_ins.operands[0].reg > the_ins.operands[1].reg)
				goto doswap;			


		}										
		
	}										

  /* We've got the operands.  Find an opcode that'll accept them */
  for (losing = 0;;)
    {
      /* If we didn't get the right number of ops, or we have no
	 common model with this pattern then reject this pattern. */

      ok_arch =-1;
      if (opsfound != opcode->m_opnum)
	++losing;
      else
	{
	  for (s = opcode->m_operands, opP = &the_ins.operands[0];
	       *s && !losing;
	       s += 2, opP++)
	    {
	      /* Warning: this switch is huge! */
	      /* I've tried to organize the cases into this order:
		 non-alpha first, then alpha by letter.  Lower-case
		 goes directly before uppercase counterpart.  */
	      /* Code with multiple case ...: gets sorted by the lowest
		 case ... it belongs to.  I hope this makes sense.  */
	      switch (*s)
		{


		case 'n':
		  switch (opP->mode)
		    {
		    case MODE_d:
		      break;
		    default:
		      losing++;
		    }
                  break;

		case 'o':
		  switch (opP->mode)
		    {
		    case BASE:
		    case MODE_EorB:
		    case MODE_IMMED:
		      break;
		    default:
		      losing++;
		    }
                  break;

		case '#':
		  if (opP->mode != MODE_IMMED)
		    losing++;
		  else if (s[1] == 'b'
			   && ! isvar (&opP->disp)
			   && (opP->disp.exp.X_op != O_constant
			       || ! isbyte (opP->disp.exp.X_add_number)))
		    losing++;
		  else if (s[1] == 'w'
			   && ! isvar (&opP->disp)
			   && (opP->disp.exp.X_op != O_constant
			       || ! isword (opP->disp.exp.X_add_number)))
		    losing++;
		  break;



		case '+':
		  if (opP->mode != MODE_AINC)
		    losing++;
		  break;

		case '-':
		  if (opP->mode != MODE_ADEC)
		    losing++;
		  break;

        
		case 'E':
		  if (opP->mode != MODE_EorB){
		    losing++;
           }
		  break;

		case 'a':
		  if (opP->mode != MODE_a ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 
		  )
		    losing++;
		  break;

		case 'G':
		  if (opP->mode != MODE_G ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 
		  )
		    losing++;
		  break;

		case 'H':
		  if (opP->mode != MODE_H ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 
		  )
		    losing++;
		  break;

		case 'K':
		  if (opP->mode != MODE_K ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 
		  )
		    losing++;
		  break;

		case 'V':
		  if (opP->mode != MODE_V ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 
		  )
		    losing++;
		  break;

		case 'Y':
		  if (opP->mode != MODE_Y ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 
		  )
		    losing++;
		  break;

		case 'W':
		  if (opP->mode != MODE_W ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 
		  )
		    losing++;
		  break;

		case 'A':
		  if (opP->mode != MODE_A
		      || ((opP->reg - DATA0) !=(s[1]-'0')))
		    losing++;
		  break;


		case 'M':
		  if (opP->mode != MODE_M)
		    losing++;
		  break;




/* 1 needs to handle both (n,r3,r2) and (n,r2,r3) */
		case '1':
		  if (opP->mode != MODE_1 ||
				opP->reg==opP->index.reg ||
				( (opP->reg -DATA0) != (s[1]-'0') && 
					(opP->index.reg -DATA0) != (s[1]-'0') 
				) 
			)
		    losing++;
		  break;

/* 2 needs to handle both (r3,r2) and (r2,r3) */
		case '2':
		  if (opP->mode != MODE_2 ||
				opP->reg==opP->index.reg ||
				( (opP->reg -DATA0) != (s[1]-'0') && 
					(opP->index.reg -DATA0) != (s[1]-'0') 
				) 

			)
		    losing++;
		  break;

		case '3':
		  if (opP->mode != MODE_3 ||
				opP->reg==opP->index.reg ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 

			)
		    losing++;
		  break;

		case '4':
		  if (opP->mode != MODE_4 ||
				opP->reg==opP->index.reg ||
				( (opP->reg -DATA0) != (s[1]-'0') ) 

			)
		    losing++;
		  break;



		case '5':
		  if (opP->mode != MODE_5)
		    losing++;
		  break;

		case '6':
		  if (opP->mode != MODE_6)
		    losing++;
		  break;

		case '7':
		  if (opP->mode != MODE_7)
		    losing++;
		  break;

		case '8':
		  if (opP->mode != MODE_8)
		    losing++;
		  break;



		case 'B':	/* FOO */

		  if (opP->mode != MODE_EorB)
			    losing++;
		  break;



		case 'd':
		  if (opP->mode != MODE_d)
		    losing++;
		  break;

		case 'S':
		  if (opP->mode != SPDISP)
		    losing++;
		  break;

		case 'U':
		  if (opP->mode != MODE_U)
		    losing++;
		  break;

		case 'f':
		  if (opP->mode != MODE_f)
		    losing++;
		  break;


		case 'F':
		  if (opP->mode != MODE_F
		      || ((opP->reg - FP0) !=(s[1]-'0')))
		    losing++;
		  break;

		case 'p':
		  if (opP->mode != MODE_p
		      || opP->reg != PC)
		    losing++;
		  break;



		case 'l':
		case 'L':
		  if (opP->mode == MODE_r
		      || opP->mode == AREG
		      || opP->mode == MODE_s
		      || opP->mode == MODE_b
		      || opP->mode == MODE_p
		      || opP->mode == MODE_F)
		    {
		      if (s[1] == '8')
			losing++;
		      else
			{
			  switch (opP->mode)
			    {
			    case MODE_r:
			      opP->mask = 1 << (opP->reg - DATA0);
			      break;
			    case AREG:
			    case MODE_s:
			    case MODE_b:
			    case MODE_p:
			      opP->mask = 1 << (opP->reg - ADDR0 + 8);
			      break;
			    case MODE_F:
			      opP->mask = 1 << (opP->reg - FP0 + 16);
			      break;
			    default:
			      abort ();
			    }
			  opP->mode = REGLST;
			}
		    }
		  else if (opP->mode == CONTROL)
		    {
		      if (s[1] != '8')
			losing++;
		      else
			{
			  switch (opP->reg)
			    {
			    case FPI:
			      opP->mask = 1 << 24;
			      break;
			    case FPS:
			      opP->mask = 1 << 25;
			      break;
			    case FPC:
			      opP->mask = 1 << 26;
			      break;
			    default:
			      losing++;
			      break;
			    }
			  opP->mode = REGLST;
			}
		    }
		  else if (opP->mode != REGLST)
		    losing++;
		  else if (s[1] == '8' && (opP->mask & 0x0ffffff) != 0)
		    losing++;
		  else if (s[1] == '3' && (opP->mask & 0x7000000) != 0)
		    losing++;
		  break;


		case 'r':
		  if (opP->mode != MODE_r
		      || ((opP->reg - DATA0) !=(s[1]-'0')))
		    losing++;
		  break;

		case 'c':
		  if (opP->mode != MODE_c
		      || ((opP->reg - QDATA0) !=(s[1]-'0')))
		    losing++;
		  break;

		case 'v':
		  if (opP->mode != MODE_v)
		    losing++;
		  break;

		case 'm':
		  if (opP->mode != MODE_m)
		    losing++;
		  break;

		case 'x':
		  if (opP->mode != MODE_x)
		    losing++;
		  break;

		case 's':
		  if (opP->mode != MODE_s
		      || opP->reg != STACKPOINTER)
		    losing++;
		  break;

		case 'b':
		  if (opP->mode != MODE_b
		      || opP->reg != FRAMEPOINTER)
		    losing++;
		  break;



		default:
        fprintf(stderr,"%c is missing - %s\n",*s,s);
		  abort ();
		}		/* switch on type of operand */

	      if (losing)
		break;
	    }			/* for each operand */
	}			/* if immediately wrong */

      if (!losing)
	{
	  break;
	}			/* got it. */

      opcode = opcode->m_next;

      if (!opcode)
	{
	  if (ok_arch && 0) /* ***** obsolete here */
	    {
	      char buf[200], *cp;

	      strcpy (buf,
		      "invalid instruction for this architecture; needs ");
	      cp = buf + strlen (buf);
	      switch (ok_arch)
		{
		default:
		  {
		    int got_one = 0, idx;
		    for (idx = 0; idx < sizeof (archs) / sizeof (archs[0]);
			 idx++)
		      {
			if ((archs[idx].arch & ok_arch)
			    && ! archs[idx].alias)
			  {
			    if (got_one)
			      {
				strcpy (cp, " or ");
				cp += strlen (cp);
			      }
			    got_one = 1;
			    strcpy (cp, archs[idx].name);
			    cp += strlen (cp);
			  }
		      }
		  }
		}
	      cp = xmalloc (strlen (buf) + 1);
	      strcpy (cp, buf);
	      the_ins.error = cp;
	    }
	  else
	    the_ins.error = "operands mismatch";
	  return;
	}			/* Fell off the end */


      losing = 0;
    }



  /* now assemble it */

  the_ins.args = opcode->m_operands;
  the_ins.numargs = opcode->m_opnum;
  the_ins.numo = opcode->m_codenum;
  putword(&(the_ins.opcode[0]),getone (opcode));
  putword(&(the_ins.opcode[1]),gettwo (opcode));
  checktwoop(opcode);
  for (s = the_ins.args, opP = &the_ins.operands[0]; *s; s += 2, opP++)
    {

      /* This switch is a doozy.
       Watch the first step; its a big one! */
      switch (s[0])
	{


	case '#':
/* this is where special precions are requested from get_num */
	  switch (s[1])
	    {			/* JF: I hate floating point! */
	    case 'j':
	      tmpreg = 70;
	      break;

	    case '8':
	      tmpreg = 20;
	      break;
	    case 'C':
	      tmpreg = 50;
	      break;
        case 'F':
        case 'f':
            tmpreg=90; /* request DOUBLE 4,11 */
            break;
        case 'x':
        case 'd':
            tmpreg=95; /* request XDOUBLE 4,11 */
            break;


	    case '3':
	    default:
	      tmpreg = 80; /* request FLOAT = 2,8 */
	      break;
	    }
	  nextword1 = get_num (&opP->disp, tmpreg);
	  tmpreg=nextword1;
	  if (isvar (&opP->disp))
	    add_fix (s[1], &opP->disp, 0, 0);
	  switch (s[1])
	    {
	    case 'b':		/* Danger:  These do no check for
				   certain types of overflow.
				   user beware! */
	      if (!isbyte (tmpreg))
		opP->error = "out of range";
	      addword(tmpreg);
	      if (isvar (&opP->disp))
		the_ins.reloc[the_ins.nrel - 1].n =
		  (opcode->m_codenum) * 2 + 1;
	      break;
	    case 'w':
	      if (!isword (tmpreg))
		opP->error = "out of range";
	      addword(tmpreg);
	      if (isvar (&opP->disp))
		the_ins.reloc[the_ins.nrel - 1].n = (opcode->m_codenum) * 2;
	      break;
	    case 'l':
         addlong(tmpreg);
	      if (isvar (&opP->disp))
		the_ins.reloc[the_ins.nrel - 1].n = (opcode->m_codenum) * 2;
	      break;
	    case 'q':
         addlonglong(nextword1);
		 break;
	    case '3':
	      tmpreg &= 0xFF;
	    case '8':
	    case 'C':
	    case 'j':
	      install_operand (s[1], tmpreg);
	      break;
	    default:
	      tmpreg = 0x3c;	/* 7.4 */
		nextword = get_num (&opP->disp, 0);
	      if (isvar (&opP->disp))
		add_fix (s[1], &opP->disp, 0, 0);
	      switch (s[1])
		{
		case 'b':
		  if (!isbyte (nextword))
		    opP->error = "operand out of range";
		  addword (nextword);
		  baseo = 0;
		  break;
		case 'w':
		  if (!isword (nextword))
		    opP->error = "operand out of range";
		  addword (nextword);
		  baseo = 0;
		  break;
		case 'l':
          addlong(nextword);
		  baseo = 0;
		  break;

		case 'q': /* NEEDS TO BE FIXED FOR LONG LONGS */
		{
														
fprintf(stderr,"NEED FIX QLOADS - LLll\n");
/*		  nextword = get_num (&opP->disp, 80); */
fprintf(stderr,"NW=$%qx tmpreg=$%x\n",nextword,tmpreg);
          addlonglong(nextword);
		  baseo = 0;
		 }
		  break;

		case 'f':
		  baseo = 2;
		  outro = 8;
		  break;
		case 'd':
		  baseo = 4;
		  outro = 11;
		  break;
		case 'x':
		  baseo = 6;
		  outro = 15;
		  break;
		case 'p':
		  baseo = 6;
		  outro = -1;
		  break;
		default:
fprintf(stderr,"Funny number =%c (%s)\n",s[1],s);
		  abort ();
		}

	      if (!baseo)
		break;

/* this is where the number is brought in with precision, #words etc */
	      gen_to_words (words, baseo, (long) outro);

          if(baseo>=2) /* long align */
                aligninstlong(0xeeff);      
    	  addsomething(words,baseo);
	      break;
	    }
	  break;


	case 'B':
	 

	  switch (s[1])
	    {
	    case 'L':
  	      aligninstlong(0xeebb);      
	    	  add_fix ('l', &opP->disp, 1,(the_ins.numo)<<1);
	      	addlong(0);
	      	break;

	    case 'W':
  	      aligninstlong(0xeebb);      
	    	  add_fix ('w', &opP->disp, 1,(the_ins.numo)<<1);
	      	addword(0);
	      	break;

	    default:
	      abort ();
	    }

	  break;


	case 'd':		/* JF this is a kludge */
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

    case 'U':
	case 'S':		/* JF this is a kludge */
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;


	case 'E':
        aligninstlong(0xee11);      
		add_fix ('l', &opP->disp, 0, 0);
        addlong(0);
		 break;

	case 'b':
	case 'p':
	case 's':
		break;


	case 'r':
	  break;

	case 'a':
	  break;

	case 'A':
	case 'G':
	case 'K':
	case 'H':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

	case '1':
	case '4':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

	case 'V':
	case 'Y':
	case 'W':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  tmpreg = get_num (&opP->disp1, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

	case '2':
	case '3':
	  break;

	case 'c':
	  break;

	case 'm':
	  install_operand (s[1], opP->reg - MDATA);
	  break;

	case 'M':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  install_operand (s[1], opP->reg - MDATA);
	  break;

	case 'v':
	  install_operand (s[1], opP->reg - VDATA);
	  break;

	case 'x':
	  install_operand (s[1], opP->reg - XDATA);
	  break;

	case '+':
	case '-':
        break;


	case '0':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

	case '5':
	case '8':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  tmpreg = get_num (&opP->disp1, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  tmpreg = get_num (&opP->disp2, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

	case '6':
	case '7':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  tmpreg = get_num (&opP->disp1, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

	case 'f':
	  tmpreg = get_num (&opP->disp, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  tmpreg = get_num (&opP->disp1, 80);
	  if (!issword (tmpreg))
	    {
	      as_warn ("Expression out of range, using 0");
	      tmpreg = 0;
	    }
	  addword (tmpreg);
	  break;

	case 'F':
	  break;




	case 'l':
	  tmpreg = opP->mask;
	  if (s[1] == 'w')
	    {
	      if (tmpreg & 0x7FF0000)
		as_bad ("Floating point register in register list");
	      addword(reverse_16_bits (tmpreg));
	    }
	  else
	    {
	      if (tmpreg & 0x700FFFF)
		as_bad ("Wrong register in floating-point reglist");
	      install_operand (s[1], reverse_8_bits (tmpreg >> 16));
	    }
	  break;

	case 'L':
	  tmpreg = opP->mask;
	  if (s[1] == 'w')
	    {
	      if (tmpreg & 0x7FF0000)
		as_bad ("Floating point register in register list");
	      addword(tmpreg);
	    }
	  else if (s[1] == '8')
	    {
	      if (tmpreg & 0x0FFFFFF)
		as_bad ("incorrect register in reglist");
	      install_operand (s[1], tmpreg >> 24);
	    }
	  else
	    {
	      if (tmpreg & 0x700FFFF)
		as_bad ("wrong register in floating-point reglist");
	      else
		install_operand (s[1], tmpreg >> 16);
	    }
	  break;


	default:
	  abort ();
	}
    }

  /* By the time whe get here (FINALLY) the_ins contains the complete
     instruction, ready to be emitted. . . */

    aligninstlong(0xffee); /* align to even word boundary */
}




static int
reverse_16_bits (in)
     int in;
{
  int out = 0;
  int n;

  static int mask[16] =
  {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
    0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000
  };
  for (n = 0; n < 16; n++)
    {
      if (in & mask[n])
	out |= mask[15 - n];
    }
  return out;
}				/* reverse_16_bits() */

static int
reverse_8_bits (in)
     int in;
{
  int out = 0;
  int n;

  static int mask[8] =
  {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
  };

  for (n = 0; n < 8; n++)
    {
      if (in & mask[n])
	out |= mask[7 - n];
    }
  return out;
}				/* reverse_8_bits() */

/* Cause an extra frag to be generated here, inserting up to 10 bytes
   (that value is chosen in the frag_var call in md_assemble).  TYPE
   is the subtype of the frag to be generated; its primary type is
   rs_machine_dependent.

   The TYPE parameter is also used by md_convert_frag_1 and
   md_estimate_size_before_relax.  The appropriate type of fixup will
   be emitted by md_convert_frag_1.

   ADD becomes the FR_SYMBOL field of the frag, and OFF the FR_OFFSET.  */
static void
install_operand (mode, val)
     int mode;
     int val;
{
 char *s=(char *)the_ins.opcode;
 if(twoop)
	s+=2;
  switch (mode)
    {

    case 's':
      orword(s, val & 0xFF);	/* JF FF is for M kludge */
      break;

    case 'd':
      orword(s,val << 9);
      break;

    case 'h':
      orword(s,val << 1);
      break;



    case 'x':
      orword(s,val << 6);
      break;


    case 'y':
      orword(s,val << 3);
      break;

    case 'u':
      orword(s,val << 2);
      break;

    case 'v':
      orword(s,val << 4);
      break;

    case 'e':
      orword(s,val << 5);
      break;

    case 'z':
      orword(s,val << 7);
      break;

    case 'p':
      orword(s,val << 8);
      break;


    case 'q':
      orword(s,val << 10);
      break;

    case 'g':
      orword(s,val = 0xff);
      break;

    case 'i':
      orword(s,val << 9);
      break;

    case 'b':
    case 'w':
    case 'l':
      break;

    case 'L':
	  putlong(s+2,val);	  
      break;

    case 'W':
	  putword(s+2,val);	  
      break;



    default:
failit:
      as_fatal ("failed sanity check 2.");
    }
}				/* install_operand() */

static void
install_gen_operand (mode, val)
     int mode;
     int val;
{
  switch (mode)
    {
    case 's':
      orword(the_ins.opcode,val);
      break;

    case 'd':
      orword(the_ins.opcode,(val & 0x07) << 9 | (val & 0x38) << 3);
      break;


    case 'b':
    case 'w':
    case 'l':
    case 'f':
    case 'F':
    case 'x':
    case 'p':
      orword(the_ins.opcode,val);
      break;
      /* more stuff goes here */
    default:
failit:
      as_fatal ("failed sanity check 3.");
    }
}				/* install_gen_operand() */

/*
 * verify that we have some number of paren pairs, do icvm_ip_op(), and
 * then deal with the bitfield hack.
 */

static char *
crack_operand (str, opP)
     register char *str;
     register struct icvm_op *opP;
{
  register int parens;
  register int c;
  register char *beg_str;
  int inquote = 0;

  if (!str)
    {
      return str;
    }
  beg_str = str;
  for (parens = 0; *str && (parens > 0 || inquote || notend (str)); str++)
    {
      if (! inquote)
	{
	  if (*str == '(')
	    parens++;
	  else if (*str == ')')
	    {
	      if (!parens)
		{			/* ERROR */
		  opP->error = "Extra )";
		  return str;
		}
	      --parens;
	    }
	}
      if (flag_mri && *str == '\'')
	inquote = ! inquote;
    }
  if (!*str && parens)
    {				/* ERROR */
      opP->error = "Missing )";
      return str;
    }
  c = *str;
  *str = '\0';
  if (icvm_ip_op (beg_str, opP) != 0)
    {
      *str = c;
      return str;
    }
  *str = c;
  if (c == '}')
    c = *++str;			/* JF bitfield hack */
  if (c)
    {
      c = *++str;
      if (!c)
	as_bad ("Missing operand");
    }

  /* Detect MRI REG symbols and convert them to REGLSTs.  */
  if (opP->mode == CONTROL && (int)opP->reg < 0)
    {
      opP->mode = REGLST;
      opP->mask = ~(int)opP->reg;
      opP->reg = 0;
    }

  return str;
}

/* This is the guts of the machine-dependent assembler.  STR points to a
   machine dependent instruction.  This function is supposed to emit
   the frags/bytes it assembles to.
   */

static void
insert_reg (regname, regnum)
     const char *regname;
     int regnum;
{
  char buf[100];
  int i;

#ifdef REGISTER_PREFIX
  if (!flag_reg_prefix_optional)
    {
      buf[0] = REGISTER_PREFIX;
      strcpy (buf + 1, regname);
      regname = buf;
    }
#endif

  symbol_table_insert (symbol_new (regname, reg_section, regnum,
				   &zero_address_frag));

  for (i = 0; regname[i]; i++)
    buf[i] = islower (regname[i]) ? toupper (regname[i]) : regname[i];
  buf[i] = '\0';

  symbol_table_insert (symbol_new (buf, reg_section, regnum,
				   &zero_address_frag));
}

struct init_entry
  {
    const char *name;
    int number;
  };

static const struct init_entry init_table[] =
{
  { "r0", DATA0 },
  { "r1", DATA1 },
  { "r2", DATA2 },
  { "r3", DATA3 },
  { "r4", DATA4 },
  { "r5", DATA5 },
  { "r6", DATA6 },
  { "r7", DATA7 },
  { "a0", ADDR0 },
  { "a1", ADDR1 },
  { "a2", ADDR2 },
  { "a3", ADDR3 },
  { "a4", ADDR4 },
  { "a5", ADDR5 },
  { "a6", ADDR6 },
  { "a7", ADDR7 },
  { "sp", STACKPOINTER },
  { "ssp", STACKPOINTER },
  { "fp", FRAMEPOINTER},
  { "fp0", FP0 },
  { "fp1", FP1 },
  { "fp2", FP2 },
  { "fp3", FP3 },
  { "fpi", FPI },
  { "fpiar", FPI },
  { "fpc", FPI },
  { "fps", FPS },
  { "fpsr", FPS },
  { "fpc", FPC },
  { "fpcr", FPC },
  { "control", FPC },
  { "status", FPS },
  { "iaddr", FPI },

  { "cop0", COP0 },
  { "cop1", COP1 },
  { "cop2", COP2 },
  { "cop3", COP3 },
  { "cop4", COP4 },
  { "cop5", COP5 },
  { "cop6", COP6 },
  { "cop7", COP7 },
  { "pc", PC },
  { "zpc", ZPC },
  { "sr", SR },

  { "ccr", CCR },
  { "cc", CCR },

  /* control registers */
  { "sfc", SFC },		/* Source Function Code */
  { "sfcr", SFC },
  { "dfc", DFC },		/* Destination Function Code */
  { "dfcr", DFC },
  { "cacr", CACR },		/* Cache Control Register */
  { "caar", CAAR },		/* Cache Address Register */

  { "usp", USP },		/* User Stack Pointer */
  { "vbr", VBR },		/* Vector Base Register */
  { "msp", MSP },		/* Master Stack Pointer */
  { "isp", ISP },		/* Interrupt Stack Pointer */

  { "itt0", ITT0 },		/* Instruction Transparent Translation Reg 0 */
  { "itt1", ITT1 },		/* Instruction Transparent Translation Reg 1 */
  { "dtt0", DTT0 },		/* Data Transparent Translation Register 0 */
  { "dtt1", DTT1 },		/* Data Transparent Translation Register 1 */

  /* 68ec040 versions of same */
  { "iacr0", ITT0 },		/* Instruction Access Control Register 0 */
  { "iacr1", ITT1 },		/* Instruction Access Control Register 0 */
  { "dacr0", DTT0 },		/* Data Access Control Register 0 */
  { "dacr1", DTT1 },		/* Data Access Control Register 0 */

  { "acr0", ITT0 },		/* Access Control Unit 0 */
  { "acr1", ITT1 },		/* Access Control Unit 1 */
  { "acr2", DTT0 },		/* Access Control Unit 2 */
  { "acr3", DTT1 },		/* Access Control Unit 3 */

  { "tc", TC },			/* MMU Translation Control Register */
  { "tcr", TC },

  { "srp", SRP },		/* User Root Pointer */
  { "urp", URP },		/* Supervisor Root Pointer */

  { "buscr", BUSCR },
  { "pcr", PCR },

  { "rombar", ROMBAR },		/* ROM Base Address Register */
  { "rambar0", RAMBAR0 },	/* ROM Base Address Register */
  { "rambar1", RAMBAR1 },	/* ROM Base Address Register */
  { "mbar", MBAR },		/* Module Base Address Register */
  /* end of control registers */

  { "ac", AC },
  { "bc", BC },
  { "cal", CAL },
  { "crp", CRP },
  { "drp", DRP },
  { "pcsr", PCSR },
  { "psr", PSR },
  { "scc", SCC },
  { "val", VAL },
  { "bad0", BAD0 },
  { "bad1", BAD1 },
  { "bad2", BAD2 },
  { "bad3", BAD3 },
  { "bad4", BAD4 },
  { "bad5", BAD5 },
  { "bad6", BAD6 },
  { "bad7", BAD7 },
  { "bac0", BAC0 },
  { "bac1", BAC1 },
  { "bac2", BAC2 },
  { "bac3", BAC3 },
  { "bac4", BAC4 },
  { "bac5", BAC5 },
  { "bac6", BAC6 },
  { "bac7", BAC7 },

  { "ic", IC },
  { "dc", DC },
  { "nc", NC },

  { "q0", QDATA0 },
  { "q1", QDATA1 },
  { "q2", QDATA2 },
  { "q3", QDATA3 },

  { "m0", MDATA0 },
  { "m1", MDATA1 },
  { "m2", MDATA2 },
  { "m3", MDATA3 },
  { "m4", MDATA4 },
  { "m5", MDATA5 },
  { "m6", MDATA6 },
  { "m7", MDATA7 },
  { "m8", MDATA8 },
  { "m9", MDATA9 },
  { "m10", MDATA10 },
  { "m11", MDATA11 },
  { "m12", MDATA12 },
  { "m13", MDATA13 },
  { "m14", MDATA14 },
  { "m15", MDATA15 },

  { "v0", VDATA0 },
  { "v1", VDATA1 },
  { "v2", VDATA2 },
  { "v3", VDATA3 },
  { "v4", VDATA4 },
  { "v5", VDATA5 },
  { "v6", VDATA6 },
  { "v7", VDATA7 },
  { "v8", VDATA8 },
  { "v9", VDATA9 },
  { "v10", VDATA10 },
  { "v11", VDATA11 },
  { "v12", VDATA12 },
  { "v13", VDATA13 },
  { "v14", VDATA14 },
  { "v15", VDATA15 },


  { "x0", XDATA0 },
  { "x1", XDATA1 },
  { "x2", XDATA2 },
  { "x3", XDATA3 },
  { "x4", XDATA4 },
  { "x5", XDATA5 },
  { "x6", XDATA6 },
  { "x7", XDATA7 },
  { "x8", XDATA8 },
  { "x9", XDATA9 },
  { "x10", XDATA10 },
  { "x11", XDATA11 },
  { "x12", XDATA12 },
  { "x13", XDATA13 },
  { "x14", XDATA14 },
  { "x15", XDATA15 },
  { "ac0", TT0 },
  { "ac1", TT1 },
  /* 68ec030 access control unit, identical to 030 MMU status reg */
  { "acusr", PSR },

  /* Suppressed data and address registers.  */
  { "zd0", ZDATA0 },
  { "zd1", ZDATA1 },
  { "zd2", ZDATA2 },
  { "zd3", ZDATA3 },
  { "zd4", ZDATA4 },
  { "zd5", ZDATA5 },
  { "zd6", ZDATA6 },
  { "zd7", ZDATA7 },
  { "za0", ZADDR0 },
  { "za1", ZADDR1 },
  { "za2", ZADDR2 },
  { "za3", ZADDR3 },
  { "za4", ZADDR4 },
  { "za5", ZADDR5 },
  { "za6", ZADDR6 },
  { "za7", ZADDR7 },

  { 0, 0 }
};

static void
init_regtable ()
{
  int i;
  for (i = 0; init_table[i].name; i++)
    insert_reg (init_table[i].name, init_table[i].number);
}


#ifdef OBJ_AOUT
/* a.out machine type.  Default to 68020.  */
int icvm_aout_machtype = 2;
#endif

void
md_assemble (str)
     char *str;
{
  const char *er;
  short *fromP;
  char *cfromP;
  char *toP = NULL;
  int m, n = 0;
  char *to_beg_P;
  int shorts_this_frag;
  fixS *fixP;
  /* In MRI mode, the instruction and operands are separated by a
     space.  Anything following the operands is a comment.  The label
     has already been removed.  */


  if (flag_mri)
    {
      char *s;
      int fields = 0;
      int infield = 0;
      int inquote = 0;

      for (s = str; *s != '\0'; s++)
	{
	  if ((*s == ' ' || *s == '\t') && ! inquote)
	    {
	      if (infield)
		{
		  ++fields;
		  if (fields >= 2)
		    {
		      *s = '\0';
		      break;
		    }
		  infield = 0;
		}
	    }
	  else
	    {
	      if (! infield)
		infield = 1;
	      if (*s == '\'')
		inquote = ! inquote;
	    }
	}
    }

  memset ((char *) (&the_ins), '\0', sizeof (the_ins));
  icvm_ip (str);
  er = the_ins.error;
  if (!er)
    {
      for (n = 0; n < the_ins.numargs; n++)
	if (the_ins.operands[n].error)
	  {
	    er = the_ins.operands[n].error;
	    break;
	  }
    }
  if (er)
    {
      as_bad ("%s -- statement `%s' ignored", er, str);
      return;
    }

  /* If there is a current label, record that it marks an instruction.  */
  if (current_label != NULL)
    {
      current_label->text = 1;
      current_label = NULL;
    }

  if (the_ins.nfrag == 0)
    {
      /* No frag hacking involved; just put it out */
      toP = frag_more (2 * the_ins.numo);
      cfromP = (char *)&the_ins.opcode[0];
      for (m = the_ins.numo; m; --m)
	{
	  BE_md_number_to_chars (toP, (long) cfromP[0], 1);
	  BE_md_number_to_chars (toP+1, (long) cfromP[1], 1);
	  toP += 2;
	  cfromP+=2;
	}
      /* put out symbol-dependent info */
      for (m = 0; m < the_ins.nrel; m++)
	{
	  switch (the_ins.reloc[m].wid)
	    {
	    case 'B':
	      n = 1;
	      break;
	    case 'b':
	      n = 1;
	      break;
	    case '3':
	      n = 1;
	      break;
	    case 'w':
	    case 'W':
	      n = 2;
	      break;
	    case 'l':
	      n = 4;
	      break;
	    case 'q':
	      n = 8;
	      break;
	    default:
	      as_fatal ("Don't know how to figure width of %c in md_assemble()",
			the_ins.reloc[m].wid);
	    }

	  fixP = fix_new_exp (frag_now,
			      ((toP - frag_now->fr_literal)
			       - the_ins.numo * 2 + the_ins.reloc[m].n),
			      n,
			      &the_ins.reloc[m].exp,
			      the_ins.reloc[m].pcrel,
			      get_reloc_code (n, the_ins.reloc[m].pcrel,
					      the_ins.reloc[m].pic_reloc));
	  fixP->fx_pcrel_adjust = the_ins.reloc[m].pcrel_fix;
	  if (the_ins.reloc[m].wid == 'B')
	    fixP->fx_signed = 1;
	}
      return;
    }

  /* There's some frag hacking */
  for (n = 0, fromP = &the_ins.opcode[0]; n < the_ins.nfrag; n++)
    {
      int wid;

      if (n == 0)
	wid = 2 * the_ins.fragb[n].fragoff;
      else
	wid = 2 * (the_ins.numo - the_ins.fragb[n - 1].fragoff);
      toP = frag_more (wid);
      to_beg_P = toP;
      shorts_this_frag = 0;
      for (m = wid / 2; m; --m)
	{
fprintf(stderr, "GAS needs FIX1\n");exit(3);
	  BE_md_number_to_chars (toP, (long) (*fromP), 2);
	  toP += 2;
	  fromP++;
	  shorts_this_frag++;
	}


      for (m = 0; m < the_ins.nrel; m++)
	{
	  if ((the_ins.reloc[m].n) >= 2 * shorts_this_frag)
	    {
	      the_ins.reloc[m].n -= 2 * shorts_this_frag;
	      break;
	    }
	  wid = the_ins.reloc[m].wid;
	  if (wid == 0)
	    continue;
	  the_ins.reloc[m].wid = 0;
	  wid = (wid == 'b') ? 1 : (wid == 'w') ? 2 : (wid == 'l') ? 4 : 4000;

	  fixP = fix_new_exp (frag_now,
			      ((toP - frag_now->fr_literal)
			       - the_ins.numo * 2 + the_ins.reloc[m].n),
			      wid,
			      &the_ins.reloc[m].exp,
			      the_ins.reloc[m].pcrel,
			      get_reloc_code (wid, the_ins.reloc[m].pcrel,
					      the_ins.reloc[m].pic_reloc));
	  fixP->fx_pcrel_adjust = the_ins.reloc[m].pcrel_fix;
	}
      (void) frag_var (rs_machine_dependent, 10, 0,
		       (relax_substateT) (the_ins.fragb[n].fragty),
		       the_ins.fragb[n].fadd, the_ins.fragb[n].foff, to_beg_P);
    }
  n = (the_ins.numo - the_ins.fragb[n - 1].fragoff);
  shorts_this_frag = 0;
  if (n)
    {
      toP = frag_more (n * sizeof (short));
      while (n--)
	{
fprintf(stderr, "GAS needs FIX2\n");exit(3);
	  BE_md_number_to_chars (toP, (long) (*fromP), 2);
	  toP += 2;
	  fromP++;
	  shorts_this_frag++;
	}
    }
  for (m = 0; m < the_ins.nrel; m++)
    {
      int wid;
      wid = the_ins.reloc[m].wid;
      if (wid == 0)
	continue;
      the_ins.reloc[m].wid = 0;
      wid = (wid == 'b') ? 1 : (wid == 'w') ? 2 : (wid == 'l') ? 4 : 4000;

      fixP = fix_new_exp (frag_now,
			  ((the_ins.reloc[m].n + toP - frag_now->fr_literal)
			   - shorts_this_frag * 2),
			  wid,
			  &the_ins.reloc[m].exp,
			  the_ins.reloc[m].pcrel,
			  get_reloc_code (wid, the_ins.reloc[m].pcrel,
					  the_ins.reloc[m].pic_reloc));
      fixP->fx_pcrel_adjust = the_ins.reloc[m].pcrel_fix;
    }
}


#define to(op,idx) (op[idx].opc)
#define te(op,idx) (op[idx].msk)
#define en(op,idx) (icvmstrpool[op[idx].operand])
#define mp(op,idx) (icvmstrpool[op[idx].opcode])

void
md_begin ()
{
  /*
   * md_begin -- set up hash tables with icvm instructions.
   * similar to what the vax assembler does.  ---phr
   */
  /* RMS claims the thing to do is take the icvm-opcode.h table, and make
     a copy of it at runtime, adding in the information we want but isn't
     there.  I think it'd be better to have an awk script hack the table
     at compile time.  Or even just xstr the table and use it as-is.  But
     my lord ghod hath spoken, so we do it this way.  Excuse the ugly var
     names.  */

  register const struct icvm_opcode *ins;
  register struct icvm_incant *hack, *slak;
  register const char *retval = 0;	/* empty string, or error msg text */
  register unsigned int i;
  register char c;

  if (flag_mri)
    {
      flag_reg_prefix_optional = 1;
      icvm_abspcadd = 1;
      if (! icvm_rel32_from_cmdline)
	icvm_rel32 = 0;
    }

  op_hash = hash_new ();

  obstack_begin (&robyn, 4000);
  for (i = 0; i < icvm_numopcodes; i++)
    {
      hack = slak = (struct icvm_incant *) obstack_alloc (&robyn, sizeof (struct icvm_incant));
      do
	{
	  ins = &icvm_opcodes[i];
	  /* We *could* ignore insns that don't match our arch here
	     but just leaving them out of the hash. */
	  slak->m_operands = en(ins,0);
	  slak->m_opnum = strlen (slak->m_operands) / 2;
	  slak->m_match = te(ins,0);
	  slak->m_opcode = to(ins,0);
	  /* This is kludgey */
	  slak->m_codenum = ((te(ins,0)) & 0xffffL) ? 2 : 1;
	  if (i + 1 != icvm_numopcodes
	      && !strcmp (mp(ins,0), 
			mp(icvm_opcodes,i+1)))
	    {
	      slak->m_next = (struct icvm_incant *) obstack_alloc (&robyn, sizeof (struct icvm_incant));
	      i++;
	    }
	  else
	    slak->m_next = 0;
	  slak = slak->m_next;
	}
      while (slak);
      retval = hash_insert (op_hash, mp(ins,0), (char *) hack);
      if (retval)
	as_fatal ("Internal Error:  Can't hash %s: %s", mp(ins,0), retval);
    }

  for (i = 0; i < icvm_numaliases; i++)
    {
      const char *name = icvm_opcode_aliases[i].primary;
      const char *alias = icvm_opcode_aliases[i].alias;
      PTR val = hash_find (op_hash, name);
      if (!val)
	as_fatal ("Internal Error: Can't find %s in hash table", name);
      retval = hash_insert (op_hash, alias, val);
      if (retval)
	as_fatal ("Internal Error: Can't hash %s: %s", alias, retval);
    }

  /* In MRI mode, all unsized branches are variable sized.  Normally,
     they are word sized.  */
  if (flag_mri)
    {
      static struct icvm_opcode_alias mri_aliases[] =
	{
	  { "bhi",	"jhi", },
	  { "bls",	"jls", },
	  { "bcc",	"jcc", },
	  { "bcs",	"jcs", },
	  { "bne",	"jne", },
	  { "beq",	"jeq", },
	  { "bvc",	"jvc", },
	  { "bvs",	"jvs", },
	  { "bpl",	"jpl", },
	  { "bmi",	"jmi", },
	  { "bge",	"jge", },
	  { "blt",	"jlt", },
	  { "bgt",	"jgt", },
	  { "ble",	"jle", },
	  { "bra",	"jra", },
	  { "bsr",	"jbsr", },
	};

      for (i = 0; i < sizeof mri_aliases / sizeof mri_aliases[0]; i++)
	{
	  const char *name = mri_aliases[i].primary;
	  const char *alias = mri_aliases[i].alias;
	  PTR val = hash_find (op_hash, name);
	  if (!val)
	    as_fatal ("Internal Error: Can't find %s in hash table", name);
	  retval = hash_jam (op_hash, alias, val);
	  if (retval)
	    as_fatal ("Internal Error: Can't hash %s: %s", alias, retval);
	}
    }

  for (i = 0; i < sizeof (mklower_table); i++)
    mklower_table[i] = (isupper (c = (char) i)) ? tolower (c) : c;

  for (i = 0; i < sizeof (notend_table); i++)
    {
      notend_table[i] = 0;
      alt_notend_table[i] = 0;
    }
  notend_table[','] = 1;
  notend_table['{'] = 1;
  notend_table['}'] = 1;
  alt_notend_table['a'] = 1;
  alt_notend_table['A'] = 1;
  alt_notend_table['d'] = 1;
  alt_notend_table['D'] = 1;
  alt_notend_table['#'] = 1;
  alt_notend_table['&'] = 1;
  alt_notend_table['f'] = 1;
  alt_notend_table['F'] = 1;
#ifdef REGISTER_PREFIX
  alt_notend_table[REGISTER_PREFIX] = 1;
#endif

  /* We need to put '(' in alt_notend_table to handle
       cas2 %d0:%d2,%d3:%d4,(%a0):(%a1)
     */
  alt_notend_table['('] = 1;

  /* We need to put '@' in alt_notend_table to handle
       cas2 %d0:%d2,%d3:%d4,@(%d0):@(%d1)
     */
  alt_notend_table['@'] = 1;

  /* We need to put digits in alt_notend_table to handle
       bfextu %d0{24:1},%d0
     */
  alt_notend_table['0'] = 1;
  alt_notend_table['1'] = 1;
  alt_notend_table['2'] = 1;
  alt_notend_table['3'] = 1;
  alt_notend_table['4'] = 1;
  alt_notend_table['5'] = 1;
  alt_notend_table['6'] = 1;
  alt_notend_table['7'] = 1;
  alt_notend_table['8'] = 1;
  alt_notend_table['9'] = 1;

#ifndef MIT_SYNTAX_ONLY
  /* Insert pseudo ops, these have to go into the opcode table since
     gas expects pseudo ops to start with a dot */
  {
    int n = 0;
    while (mote_pseudo_table[n].poc_name)
      {
	hack = (struct icvm_incant *)
	  obstack_alloc (&robyn, sizeof (struct icvm_incant));
	hash_insert (op_hash,
		     mote_pseudo_table[n].poc_name, (char *) hack);
	hack->m_operands = 0;
	hack->m_opnum = n;
	n++;
      }
  }
#endif

  init_regtable ();

#ifdef OBJ_ELF
  record_alignment (text_section, 2);
  record_alignment (data_section, 2);
  record_alignment (bss_section, 2);
#endif
}


void
icvm_init_after_args ()
{
  if (cpu_of_arch (current_architecture) == 0)
    {
      int i;
      const char *default_cpu = TARGET_CPU;

      if (*default_cpu == 'm')
	default_cpu++;
      for (i = 0; i < n_archs; i++){
		if (strcasecmp (default_cpu, archs[i].name) == 0)
	  		break;
	  }		
    }
  /* What other incompatibilities could we check for?  */




}

/* This is called when a label is defined.  */

void
icvm_frob_label (sym)
     symbolS *sym;
{
  struct label_line *n;

  n = (struct label_line *) xmalloc (sizeof *n);
  n->next = labels;
  n->label = sym;
  as_where (&n->file, &n->line);
  n->text = 0;
  labels = n;
  current_label = n;
}

/* This is called when a value that is not an instruction is emitted.  */

void
icvm_flush_pending_output ()
{
  current_label = NULL;
}

/* This is called at the end of the assembly, when the final value of
   the label is known.  We warn if this is a text symbol aligned at an
   odd location.  */

void
icvm_frob_symbol (sym)
     symbolS *sym;
{
  if (S_GET_SEGMENT (sym) == reg_section
      && (int) S_GET_VALUE (sym) < 0)
    {
      S_SET_SEGMENT (sym, absolute_section);
      S_SET_VALUE (sym, ~(int)S_GET_VALUE (sym));
    }
  else if ((S_GET_VALUE (sym) & 1) != 0)
    {
      struct label_line *l;

      for (l = labels; l != NULL; l = l->next)
	{
	  if (l->label == sym)
	    {
	      if (l->text)
		as_warn_where (l->file, l->line,
			       "text label `%s' aligned to odd boundary",
			       S_GET_NAME (sym));
	      break;
	    }
	}
    }
}

/* This is called if we go in or out of MRI mode because of the .mri
   pseudo-op.  */

void
icvm_mri_mode_change (on)
     int on;
{
  if (on)
    {
      if (! flag_reg_prefix_optional)
	{
	  flag_reg_prefix_optional = 1;
#ifdef REGISTER_PREFIX
	  init_regtable ();
#endif
	}
      icvm_abspcadd = 1;
      if (! icvm_rel32_from_cmdline)
	icvm_rel32 = 0;
    }
  else
    {
      if (! reg_prefix_optional_seen)
	{
#ifdef REGISTER_PREFIX_OPTIONAL
	  flag_reg_prefix_optional = REGISTER_PREFIX_OPTIONAL;
#else
	  flag_reg_prefix_optional = 0;
#endif
#ifdef REGISTER_PREFIX
	  init_regtable ();
#endif
	}
      icvm_abspcadd = 0;
      if (! icvm_rel32_from_cmdline)
	icvm_rel32 = 1;
    }
}

/* Equal to MAX_PRECISION in atof-ieee.c */
#define MAX_LITTLENUMS 6

/* Turn a string in input_line_pointer into a floating point constant
   of type type, and store the appropriate bytes in *litP.  The number
   of LITTLENUMS emitted is stored in *sizeP .  An error message is
   returned, or NULL on OK.  */

char *
md_atof (type, litP, sizeP)
     char type;
     char *litP;
     int *sizeP;
{
  int prec;
  LITTLENUM_TYPE words[MAX_LITTLENUMS];
  LITTLENUM_TYPE *wordP;
  char *t;
  switch (type)
    {
    case 'f':
    case 'd':
    case 's':
    case 'S':
      prec = 2;
      break;

    case 'F':
    case 'D':
    case 'r':
    case 'R':
      prec = 4;
      break;

    case 'x':
    case 'X':
      prec = 6;
      break;

    case 'p':
    case 'P':
      prec = 6;
      break;

    default:
      *sizeP = 0;
      return "Bad call to MD_ATOF()";
    }
  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;


  *sizeP = prec * sizeof (LITTLENUM_TYPE);
  putsomething(litP,words,prec);							

	
#ifdef NOT_NOW
  for (wordP = words; prec--;)
    {
fprintf(stderr, "GAS needs FIX3\n");exit(3);
      BE_md_number_to_chars (litP, (long) (*wordP++), sizeof (LITTLENUM_TYPE));
      litP += sizeof (LITTLENUM_TYPE);
    }
#endif

  return 0;
}

void BE_md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
  	number_to_chars_bigendian (buf, val, n);
}


void  md_number_to_chars (buf, val, n)
     char *buf;
     valueT val;
     int n;
{
	if(target_big_endian)
  		number_to_chars_bigendian (buf, val, n);
	else
  		number_to_chars_littleendian (buf, val, n);
}



static void
md_apply_fix_2 (fixP, val)
     fixS *fixP;
     offsetT val;
{
  addressT upper_limit;
  offsetT lower_limit;


  char *buf = fixP->fx_frag->fr_literal;

  buf += fixP->fx_where;

  if (val & 0x80000000)
    val |= ~(addressT)0x7fffffff;
  else
    val &= 0x7fffffff;

#ifdef OBJ_ELF
  if (fixP->fx_addsy)
    {
      memset (buf, 0, fixP->fx_size);
      fixP->fx_addnumber = val;	/* Remember value for emit_reloc */
      return;
    }
#endif

  switch (fixP->fx_size)
    {
      /* The cast to offsetT below are necessary to make code correct for
	 machines where ints are smaller than offsetT */
    case 1:
      *buf++ = val;
      upper_limit = 0x7f;
      lower_limit = - (offsetT) 0x80;
      break;
    case 2:
	  putword(buf,val);
		
      upper_limit = 0x7fff;
      lower_limit = - (offsetT) 0x8000;
      break;
    case 4:
	  putlong(buf,val);
	  buf+=4;

      upper_limit = 0x7fffffff;
      lower_limit = - (offsetT) 0x7fffffff - 1;	/* avoid constant overflow */
      break;
    default:
      BAD_CASE (fixP->fx_size);
    }

  /* Fix up a negative reloc.  */
  if (fixP->fx_addsy == NULL && fixP->fx_subsy != NULL)
    {
      fixP->fx_addsy = fixP->fx_subsy;
      fixP->fx_subsy = NULL;
      fixP->fx_tcbit = 1;
    }

  /* For non-pc-relative values, it's conceivable we might get something
     like "0xff" for a byte field.  So extend the upper part of the range
     to accept such numbers.  We arbitrarily disallow "-0xff" or "0xff+0xff",
     so that we can do any range checking at all.  */
  if (! fixP->fx_pcrel && ! fixP->fx_signed)
    upper_limit = upper_limit * 2 + 1;

  if ((addressT) val > upper_limit
      && (val > 0 || val < lower_limit))
    as_bad_where (fixP->fx_file, fixP->fx_line, "value out of range");

  /* A one byte PC-relative reloc means a short branch.  We can't use
     a short branch with a value of 0 or -1, because those indicate
     different opcodes (branches with longer offsets).  fixup_segment
     in write.c may have clobbered fx_pcrel, so we need to examine the
     reloc type.  */
  if ((fixP->fx_pcrel
#ifdef BFD_ASSEMBLER
       || fixP->fx_r_type == BFD_RELOC_8_PCREL
#endif
       )
      && fixP->fx_size == 1
      && (fixP->fx_addsy == NULL
	  || S_IS_DEFINED (fixP->fx_addsy))
      && (val == 0 || val == -1))
    as_bad_where (fixP->fx_file, fixP->fx_line, "invalid byte branch offset");
}

#ifdef BFD_ASSEMBLER
int
md_apply_fix (fixP, valp)
     fixS *fixP;
     valueT *valp;
{
  md_apply_fix_2 (fixP, (addressT) *valp);
  return 1;
}
#else
void md_apply_fix (fixP, val)
     fixS *fixP;
     long val;
{
  md_apply_fix_2 (fixP, (addressT) val);
}
#endif

/* *fragP has been relaxed to its final size, and now needs to have
   the bytes inside it modified to conform to the new size  There is UGLY
   MAGIC here. ..
   */
static void
md_convert_frag_1 (fragP)
     register fragS *fragP;
{
  long disp;
  long ext = 0;
  fixS *fixP;

  /* Address in object code of the displacement.  */
  register int object_address = fragP->fr_fix + fragP->fr_address;

  /* Address in gas core of the place to store the displacement.  */
  /* This convinces the native rs6000 compiler to generate the code we
     want. */
  register char *buffer_address = fragP->fr_literal;
  buffer_address += fragP->fr_fix;
  /* end ibm compiler workaround */

  /* The displacement of the address, from current location.  */
  disp = fragP->fr_symbol ? S_GET_VALUE (fragP->fr_symbol) : 0;
  disp = (disp + fragP->fr_offset) - object_address;

#ifdef BFD_ASSEMBLER
  disp += fragP->fr_symbol->sy_frag->fr_address;
#endif

  switch (fragP->fr_subtype)
    {
    case TAB (BCC68000, BYTE):
    case TAB (ABRANCH, BYTE):
      know (issbyte (disp));
      if (disp == 0)
	as_bad ("short branch with zero offset: use :w");
      fragP->fr_opcode[1] = disp;
      ext = 0;
      break;
    case TAB (DBCC, SHORT):
      know (issword (disp));
      ext = 2;
      break;
    case TAB (BCC68000, SHORT):
    case TAB (ABRANCH, SHORT):
      know (issword (disp));
      fragP->fr_opcode[1] = 0x00;
      ext = 2;
      break;
    case TAB (ABRANCH, LONG):
      if (!HAVE_LONG_BRANCH(current_architecture))
	{
	  if (fragP->fr_opcode[0] == 0x61)
	    /* BSR */
	    {
	      fragP->fr_opcode[0] = 0x4E;
	      fragP->fr_opcode[1] = (char) 0xB9; /* JBSR with MODE_EorB LONG offset */

	      fix_new (fragP,
		       fragP->fr_fix,
		       4,
		       fragP->fr_symbol,
		       fragP->fr_offset,
		       0,
		       NO_RELOC);

	      fragP->fr_fix += 4;
	      ext = 0;
	    }
	  /* BRA */
	  else if (fragP->fr_opcode[0] == 0x60)
	    {
	      fragP->fr_opcode[0] = 0x4E;
	      fragP->fr_opcode[1] = (char) 0xF9; /* JMP  with MODE_EorB LONG offset */
	      fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
		       fragP->fr_offset, 0, NO_RELOC);
	      fragP->fr_fix += 4;
	      ext = 0;
	    }
	  else
	    {
	      as_bad ("Long branch offset not supported.");
	    }
	}
      else
	{
	  fragP->fr_opcode[1] = (char) 0xff;
	  ext = 4;
	}
      break;
    case TAB (BCC68000, LONG):
      /* only Bcc 68000 instructions can come here */
      /* change bcc into b!cc/jmp absl long */
      fragP->fr_opcode[0] ^= 0x01;	/* invert bcc */
      fragP->fr_opcode[1] = 0x6;/* branch offset = 6 */

      /* JF: these used to be fr_opcode[2,3], but they may be in a
	   different frag, in which case refering to them is a no-no.
	   Only fr_opcode[0,1] are guaranteed to work. */
      *buffer_address++ = 0x4e;	/* put in jmp long (0x4ef9) */
      *buffer_address++ = (char) 0xf9;
      fragP->fr_fix += 2;	/* account for jmp instruction */
      fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
	       fragP->fr_offset, 0, NO_RELOC);
      fragP->fr_fix += 4;
      ext = 0;
      break;
    case TAB (DBCC, LONG):
      /* only DBcc 68000 instructions can come here */
      /* change dbcc into dbcc/jmp absl long */
      /* JF: these used to be fr_opcode[2-7], but that's wrong */
      *buffer_address++ = 0x00;	/* branch offset = 4 */
      *buffer_address++ = 0x04;
      *buffer_address++ = 0x60;	/* put in bra pc+6 */
      *buffer_address++ = 0x06;
      *buffer_address++ = 0x4e;	/* put in jmp long (0x4ef9) */
      *buffer_address++ = (char) 0xf9;

      fragP->fr_fix += 6;	/* account for bra/jmp instructions */
      fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
	       fragP->fr_offset, 0, NO_RELOC);
      fragP->fr_fix += 4;
      ext = 0;
      break;
    case TAB (FBRANCH, SHORT):
      know ((fragP->fr_opcode[1] & 0x40) == 0);
      ext = 2;
      break;
    case TAB (FBRANCH, LONG):
      fragP->fr_opcode[1] |= 0x40;	/* Turn on LONG bit */
      ext = 4;
      break;
    case TAB (PCREL, SHORT):
      ext = 2;
      break;
    case TAB (PCREL, LONG):
      /* The thing to do here is force it to ABSOLUTE LONG, since
	PCREL is really trying to shorten an ABSOLUTE address anyway */
      /* JF FOO This code has not been tested */
      fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol, fragP->fr_offset,
	       0, NO_RELOC);
      if ((fragP->fr_opcode[1] & 0x3F) != 0x3A)
	as_bad ("Internal error (long PC-relative operand) for insn 0x%04x at 0x%lx",
		(unsigned) fragP->fr_opcode[0],
		(unsigned long) fragP->fr_address);
      fragP->fr_opcode[1] &= ~0x3F;
      fragP->fr_opcode[1] |= 0x39;	/* Mode 7.1 */
      fragP->fr_fix += 4;
      ext = 0;
      break;
    case TAB (PCLEA, SHORT):
      fix_new (fragP, (int) (fragP->fr_fix), 2, fragP->fr_symbol,
	       fragP->fr_offset, 1, NO_RELOC);
      fragP->fr_opcode[1] &= ~0x3F;
      fragP->fr_opcode[1] |= 0x3A; /* 072 - mode 7.2 */
      ext = 2;
      break;
    case TAB (PCLEA, LONG):
      fixP = fix_new (fragP, (int) (fragP->fr_fix) + 2, 4, fragP->fr_symbol,
		      fragP->fr_offset, 1, NO_RELOC);
      fixP->fx_pcrel_adjust = 2;
      /* Already set to mode 7.3; this indicates: PC indirect with
	 suppressed index, 32-bit displacement.  */
      *buffer_address++ = 0x01;
      *buffer_address++ = 0x70;
      fragP->fr_fix += 2;
      ext = 4;
      break;

    case TAB (PCINDEX, BYTE):
      disp += 2;
      if (!issbyte (disp))
	{
	  as_bad ("displacement doesn't fit in one byte");
	  disp = 0;
	}
      assert (fragP->fr_fix >= 2);
      buffer_address[-2] &= ~1;
      buffer_address[-1] = disp;
      ext = 0;
      break;
    case TAB (PCINDEX, SHORT):
      disp += 2;
      assert (issword (disp));
      assert (fragP->fr_fix >= 2);
      buffer_address[-2] |= 0x1;
      buffer_address[-1] = 0x20;
      fixP = fix_new (fragP, (int) (fragP->fr_fix), 2, fragP->fr_symbol,
		      fragP->fr_offset, (fragP->fr_opcode[1] & 077) == 073,
		      NO_RELOC);
      fixP->fx_pcrel_adjust = 2;
      ext = 2;
      break;
    case TAB (PCINDEX, LONG):
      disp += 2;
      fixP = fix_new (fragP, (int) (fragP->fr_fix), 4, fragP->fr_symbol,
		      fragP->fr_offset, (fragP->fr_opcode[1] & 077) == 073,
		      NO_RELOC);
      fixP->fx_pcrel_adjust = 2;
      assert (fragP->fr_fix >= 2);
      buffer_address[-2] |= 0x1;
      buffer_address[-1] = 0x30;
      ext = 4;
      break;
    }

  if (ext)
    {
fprintf(stderr, "GAS needs FIX4\n");exit(3);
      BE_md_number_to_chars (buffer_address, (long) disp, (int) ext);
      fragP->fr_fix += ext;
    }
}

#ifndef BFD_ASSEMBLER

void
md_convert_frag (headers, sec, fragP)
     object_headers *headers;
     segT sec;
     fragS *fragP;
{
  md_convert_frag_1 (fragP);
}

#else

void
md_convert_frag (abfd, sec, fragP)
     bfd *abfd;
     segT sec;
     fragS *fragP;
{
  md_convert_frag_1 (fragP);
}
#endif

/* ************************ MIGHT BE OBSOLETE ********************* */

/* Force truly undefined symbols to their maximum size, and generally set up
   the frag list to be relaxed
   */
int
md_estimate_size_before_relax (fragP, segment)
     register fragS *fragP;
     segT segment;
{
  int old_fix;
  register char *buffer_address = fragP->fr_fix + fragP->fr_literal;

  old_fix = fragP->fr_fix;


  /* handle SZ_UNDEF first, it can be changed to BYTE or SHORT */
  switch (fragP->fr_subtype)
    {

    case TAB (ABRANCH, SZ_UNDEF):
      {
	if ((fragP->fr_symbol != NULL)	/* Not absolute */
	    && S_GET_SEGMENT (fragP->fr_symbol) == segment)
	  {
	    fragP->fr_subtype = TAB (TABTYPE (fragP->fr_subtype), BYTE);
	    break;
	  }
	else if ((fragP->fr_symbol == 0) || !HAVE_LONG_BRANCH(current_architecture))
	  {
	    /* On 68000, or for absolute value, switch to abs long */
	    /* FIXME, we should check abs val, pick short or long */
	    if (fragP->fr_opcode[0] == 0x61)
	      {
		fragP->fr_opcode[0] = 0x4E;
		fragP->fr_opcode[1] = (char) 0xB9; /* JBSR with MODE_EorB LONG offset */
		fix_new (fragP, fragP->fr_fix, 4,
			 fragP->fr_symbol, fragP->fr_offset, 0, NO_RELOC);
		fragP->fr_fix += 4;
		frag_wane (fragP);
	      }
	    else if (fragP->fr_opcode[0] == 0x60)
	      {
		fragP->fr_opcode[0] = 0x4E;
		fragP->fr_opcode[1] = (char) 0xF9; /* JMP  with MODE_EorB LONG offset */
		fix_new (fragP, fragP->fr_fix, 4,
			 fragP->fr_symbol, fragP->fr_offset, 0, NO_RELOC);
		fragP->fr_fix += 4;
		frag_wane (fragP);
	      }
	    else
	      {
		as_warn ("Long branch offset to extern symbol not supported.");
	      }
	  }
	else
	  {			/* Symbol is still undefined.  Make it simple */
	    fix_new (fragP, (int) (fragP->fr_fix), 4, fragP->fr_symbol,
		     fragP->fr_offset, 1, NO_RELOC);
	    fragP->fr_fix += 4;
	    fragP->fr_opcode[1] = (char) 0xff;
	    frag_wane (fragP);
	    break;
	  }

	break;
      }				/* case TAB(ABRANCH,SZ_UNDEF) */

    case TAB (FBRANCH, SZ_UNDEF):
      {
	if (S_GET_SEGMENT (fragP->fr_symbol) == segment || flag_short_refs)
	  {
	    fragP->fr_subtype = TAB (FBRANCH, SHORT);
	    fragP->fr_var += 2;
	  }
	else
	  {
	    fix_new (fragP, (int) fragP->fr_fix, 4, fragP->fr_symbol,
		     fragP->fr_offset, 1, NO_RELOC);
	    fragP->fr_fix += 4;
	    fragP->fr_opcode[1] |= 0x40; /* Turn on LONG bit */
	    frag_wane (fragP);
	  }
	break;
      }				/* TAB(FBRANCH,SZ_UNDEF) */

    case TAB (PCREL, SZ_UNDEF):
      {
	if (S_GET_SEGMENT (fragP->fr_symbol) == segment
	    || flag_short_refs)
	  {
	    fragP->fr_subtype = TAB (PCREL, SHORT);
	    fragP->fr_var += 2;
	  }
	else
	  {
	    fragP->fr_subtype = TAB (PCREL, LONG);
	    fragP->fr_var += 4;
	  }
	break;
      }				/* TAB(PCREL,SZ_UNDEF) */

    case TAB (BCC68000, SZ_UNDEF):
      {
	if ((fragP->fr_symbol != NULL)
	    && S_GET_SEGMENT (fragP->fr_symbol) == segment)
	  {
	    fragP->fr_subtype = TAB (BCC68000, BYTE);
	    break;
	  }
	/* only Bcc 68000 instructions can come here */
	/* change bcc into b!cc/jmp absl long */
	fragP->fr_opcode[0] ^= 0x01;	/* invert bcc */
	if (flag_short_refs)
	  {
	    fragP->fr_opcode[1] = 0x04;	/* branch offset = 6 */
	    /* JF: these were fr_opcode[2,3] */
	    buffer_address[0] = 0x4e;	/* put in jmp long (0x4ef9) */
	    buffer_address[1] = (char) 0xf8;
	    fragP->fr_fix += 2;	/* account for jmp instruction */
	    fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
		     fragP->fr_offset, 0, NO_RELOC);
	    fragP->fr_fix += 2;
	  }
	else
	  {
	    fragP->fr_opcode[1] = 0x06;	/* branch offset = 6 */
	    /* JF: these were fr_opcode[2,3] */
	    buffer_address[0] = 0x4e;	/* put in jmp long (0x4ef9) */
	    buffer_address[1] = (char) 0xf9;
	    fragP->fr_fix += 2;	/* account for jmp instruction */
	    fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
		     fragP->fr_offset, 0, NO_RELOC);
	    fragP->fr_fix += 4;
	  }
	frag_wane (fragP);
	break;
      }				/* case TAB(BCC68000,SZ_UNDEF) */

    case TAB (DBCC, SZ_UNDEF):
      {
	if (fragP->fr_symbol != NULL && S_GET_SEGMENT (fragP->fr_symbol) == segment)
	  {
	    fragP->fr_subtype = TAB (DBCC, SHORT);
	    fragP->fr_var += 2;
	    break;
	  }
	/* only DBcc 68000 instructions can come here */
	/* change dbcc into dbcc/jmp absl long */
	/* JF: these used to be fr_opcode[2-4], which is wrong. */
	buffer_address[0] = 0x00;	/* branch offset = 4 */
	buffer_address[1] = 0x04;
	buffer_address[2] = 0x60;	/* put in bra pc + ... */

	if (flag_short_refs)
	  {
	    /* JF: these were fr_opcode[5-7] */
	    buffer_address[3] = 0x04;	/* plus 4 */
	    buffer_address[4] = 0x4e;	/* Put in Jump Word */
	    buffer_address[5] = (char) 0xf8;
	    fragP->fr_fix += 6;	/* account for bra/jmp instruction */
	    fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
		     fragP->fr_offset, 0, NO_RELOC);
	    fragP->fr_fix += 2;
	  }
	else
	  {
	    /* JF: these were fr_opcode[5-7] */
	    buffer_address[3] = 0x06;	/* Plus 6 */
	    buffer_address[4] = 0x4e;	/* put in jmp long (0x4ef9) */
	    buffer_address[5] = (char) 0xf9;
	    fragP->fr_fix += 6;	/* account for bra/jmp instruction */
	    fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
		     fragP->fr_offset, 0, NO_RELOC);
	    fragP->fr_fix += 4;
	  }

	frag_wane (fragP);
	break;
      }				/* case TAB(DBCC,SZ_UNDEF) */

    case TAB (PCLEA, SZ_UNDEF):
      {
	if ((S_GET_SEGMENT (fragP->fr_symbol)) == segment
	    || flag_short_refs)
	  {
	    fragP->fr_subtype = TAB (PCLEA, SHORT);
	    fragP->fr_var += 2;
	  }
	else
	  {
	    fragP->fr_subtype = TAB (PCLEA, LONG);
	    fragP->fr_var += 6;
	  }
	break;
      }				/* TAB(PCLEA,SZ_UNDEF) */

    case TAB (PCINDEX, SZ_UNDEF):
      if (S_GET_SEGMENT (fragP->fr_symbol) == segment)
	{
	  fragP->fr_subtype = TAB (PCINDEX, BYTE);
	}
      else
	{
	  fragP->fr_subtype = TAB (PCINDEX, LONG);
	  fragP->fr_var += 4;
	}
      break;

    default:
      break;
    }

  /* now that SZ_UNDEF are taken care of, check others */
  switch (fragP->fr_subtype)
    {
    case TAB (BCC68000, BYTE):
    case TAB (ABRANCH, BYTE):
      /* We can't do a short jump to the next instruction, so in that
	 case we force word mode.  At this point S_GET_VALUE should
	 return the offset of the symbol within its frag.  If the
	 symbol is at the start of a frag, and it is the next frag
	 with any data in it (usually this is just the next frag, but
	 assembler listings may introduce empty frags), we must use
	 word mode.  */
      if (fragP->fr_symbol && S_GET_VALUE (fragP->fr_symbol) == 0)
	{
	  fragS *l;

	  for (l = fragP->fr_next;
	       l != fragP->fr_symbol->sy_frag;
	       l = l->fr_next)
	    if (l->fr_fix + l->fr_var != 0)
	      break;
	  if (l == fragP->fr_symbol->sy_frag)
	    {
	      fragP->fr_subtype = TAB (TABTYPE (fragP->fr_subtype), SHORT);
	      fragP->fr_var += 2;
	    }
	}
      break;
    default:
      break;
    }
  return fragP->fr_var + fragP->fr_fix - old_fix;
}

#if defined(OBJ_AOUT) | defined(OBJ_BOUT)
/* the bit-field entries in the relocation_info struct plays hell
   with the byte-order problems of cross-assembly.  So as a hack,
   I added this mach. dependent ri twiddler.  Ugly, but it gets
   you there. -KWK */
/* on icvm: first 4 bytes are normal unsigned long, next three bytes
   are symbolnum, most sig. byte first.  Last byte is broken up with
   bit 7 as pcrel, bits 6 & 5 as length, bit 4 as pcrel, and the lower
   nibble as nuthin. (on Sun 3 at least) */
/* Translate the internal relocation information into target-specific
   format. */
#ifdef comment
void
md_ri_to_chars (the_bytes, ri)
     char *the_bytes;
     struct reloc_info_generic *ri;
{
  /* this is easy */
fprintf(stderr, "GAS needs FIX5\n");exit(3);
  BE_md_number_to_chars (the_bytes, ri->r_address, 4);
  /* now the fun stuff */
  the_bytes[4] = (ri->r_symbolnum >> 16) & 0x0ff;
  the_bytes[5] = (ri->r_symbolnum >> 8) & 0x0ff;
  the_bytes[6] = ri->r_symbolnum & 0x0ff;
  the_bytes[7] = (((ri->r_pcrel << 7) & 0x80) | ((ri->r_length << 5) & 0x60) |
		  ((ri->r_extern << 4) & 0x10));
}

#endif /* comment */

/*  ************************* MIGHT BE OBSOLETE ******************** */
#ifndef BFD_ASSEMBLER
void
tc_aout_fix_to_chars (where, fixP, segment_address_in_file)
     char *where;
     fixS *fixP;
     relax_addressT segment_address_in_file;
{
  /*
   * In: length of relocation (or of address) in chars: 1, 2 or 4.
   * Out: GNU LD relocation length code: 0, 1, or 2.
   */

  static CONST unsigned char nbytes_r_length[] = {42, 0, 1, 42, 2};
  long r_symbolnum;

  know (fixP->fx_addsy != NULL);

fprintf(stderr, "GAS needs FIX6\n");exit(3);
  BE_md_number_to_chars (where,
       fixP->fx_frag->fr_address + fixP->fx_where - segment_address_in_file,
		      4);

  r_symbolnum = (S_IS_DEFINED (fixP->fx_addsy)
		 ? S_GET_TYPE (fixP->fx_addsy)
		 : fixP->fx_addsy->sy_number);

  where[4] = (r_symbolnum >> 16) & 0x0ff;
  where[5] = (r_symbolnum >> 8) & 0x0ff;
  where[6] = r_symbolnum & 0x0ff;
  where[7] = (((fixP->fx_pcrel << 7) & 0x80) | ((nbytes_r_length[fixP->fx_size] << 5) & 0x60) |
	      (((!S_IS_DEFINED (fixP->fx_addsy)) << 4) & 0x10));
}
#endif

#endif /* OBJ_AOUT or OBJ_BOUT */

/* ******************** MIGHT BE OBSOLETE ************************ */

#ifndef WORKING_DOT_WORD
CONST int md_short_jump_size = 4;
CONST int md_long_jump_size = 6;

void
md_create_short_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr, to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  valueT offset;

  offset = to_addr - (from_addr + 2);

fprintf(stderr, "GAS needs FIX7\n");exit(3);
  BE_md_number_to_chars (ptr, (valueT) 0x6000, 2);
  BE_md_number_to_chars (ptr + 2, (valueT) offset, 2);
}
/* ******************** MIGHT BE OBSOLETE ************************ */

void
md_create_long_jump (ptr, from_addr, to_addr, frag, to_symbol)
     char *ptr;
     addressT from_addr, to_addr;
     fragS *frag;
     symbolS *to_symbol;
{
  valueT offset;

  if (!HAVE_LONG_BRANCH(current_architecture))
    {
      offset = to_addr - S_GET_VALUE (to_symbol);
fprintf(stderr, "GAS needs FIX8\n");exit(3);
      BE_md_number_to_chars (ptr, (valueT) 0x4EF9, 2);
      BE_md_number_to_chars (ptr + 2, (valueT) offset, 4);
      fix_new (frag, (ptr + 2) - frag->fr_literal, 4, to_symbol, (offsetT) 0,
	       0, NO_RELOC);
    }
  else
    {
      offset = to_addr - (from_addr + 2);
fprintf(stderr, "GAS needs FIX9\n");exit(3);
      BE_md_number_to_chars (ptr, (valueT) 0x60ff, 2);
      BE_md_number_to_chars (ptr + 2, (valueT) offset, 4);
    }
}

#endif

/* Different values of OK tell what its OK to return.  Things that
   aren't OK are an error (what a shock, no?)

   0:  Everything is OK
   10:  Absolute 1:8	only
   20:  Absolute 0:7	only
   30:  absolute 0:15	only
   40:  Absolute 0:31	only
   50:  absolute 0:127	only
   55:  absolute -64:63    only
   60:  absolute -128:127	only
   70:  absolute 0:4095	only
   80:  No bignums

   */

static long long get_num (exp, ok)
     struct icvm_exp *exp;
     int ok;
{
  if (exp->exp.X_op == O_absent)
    {
      /* Do the same thing the VAX asm does */
      op (exp) = O_constant;
      adds (exp) = 0;
      subs (exp) = 0;
      offs (exp) = 0;
      if (ok == 10)
	{
	  as_warn ("expression out of range: defaulting to 1");
	  offs (exp) = 1;
	}
    }
  else if (exp->exp.X_op == O_constant)
    {
      switch (ok)
	{
	case 10:
	  if (offs (exp) < 1 || offs (exp) > 8)
	    {
	      as_warn ("expression out of range: defaulting to 1");
	      offs (exp) = 1;
	    }
	  break;
	case 20:
	  if (offs (exp) < 0 || offs (exp) > 7)
	    goto outrange;
	  break;
	case 30:
	  if (offs (exp) < 0 || offs (exp) > 15)
	    goto outrange;
	  break;
	case 40:
	  if (offs (exp) < 0 || offs (exp) > 32)
	    goto outrange;
	  break;
	case 50:
	  if (offs (exp) < 0 || offs (exp) > 127)
	    goto outrange;
	  break;
	case 55:
	  if (offs (exp) < -64 || offs (exp) > 63)
	    goto outrange;
	  break;
	case 60:
	  if (offs (exp) < -128 || offs (exp) > 127)
	    goto outrange;
	  break;
	case 70:
	  if (offs (exp) < 0 || offs (exp) > 4095)
	    {
	    outrange:
	      as_warn ("expression out of range: defaulting to 0");
	      offs (exp) = 0;
	    }
	  break;
	default:

	  break;
	}
    }
  else if (exp->exp.X_op == O_big)
    {
/* FLOATING POINT PRECISION IS SET HERE */
      if (offs (exp) <= 0	/* flonum */
	  && (ok == 80	|| ok==90 || ok==95	/* no bignums */
	      || (ok > 10	/* small-int ranges including 0 ok */
		  /* If we have a flonum zero, a zero integer should
		     do as well (e.g., in moveq).  */
		  && generic_floating_point_number.exponent == 0
		  && generic_floating_point_number.low[0] == 0)))
	{
	  /* HACK! Turn it into a long */
	  LITTLENUM_TYPE words[6];
      if(ok==90)
	    gen_to_words (words, 4, 11L);	/* These numbers are magic! */
      else if(ok==95)
	    gen_to_words (words, 6, 15L);	/* These numbers are magic! */
      else
	    gen_to_words (words, 2, 8L);	/* These numbers are magic! */
	  op (exp) = O_constant;
	  adds (exp) = 0;
	  subs (exp) = 0;
	  offs (exp) = words[1] | (words[0] << 16);
	}
      else if (ok != 0)
	{

	  long long l;
	  unsigned char *s=(unsigned char *)generic_bignum;
	  short endiantest=1;
	  char *endiantestp=(char *)&endiantest;	
	  op (exp) = O_constant;
	  adds (exp) = 0;
	  subs (exp) = 0;
	  offs (exp) = (ok == 10) ? 1 : 0;

/* we now assume we accept a long long int
 convert to correct endian for current endian
*/
  if(*endiantestp){
	 l=((unsigned long long)s[7])<<56;
	 l|=(((unsigned long long)s[6])<<48);
	 l|=(((unsigned long long)s[5])<<40);
	 l|=(((unsigned long long)s[4])<<32);
	 l|=(((unsigned long long)s[3])<<24);
	 l|=(((unsigned long long)s[2])<<16);
	 l|=(((unsigned long long)s[1])<<8);
	 l|=(((unsigned long long)s[0])<<0);
 	 
   }else{

	 l=((unsigned long long)s[0])<<56;
	 l|=(((unsigned long long)s[1])<<48);
	 l|=(((unsigned long long)s[2])<<40);
	 l|=(((unsigned long long)s[3])<<32);
	 l|=(((unsigned long long)s[4])<<24);
	 l|=(((unsigned long long)s[5])<<16);
	 l|=(((unsigned long long)s[6])<<8);
	 l|=(((unsigned long long)s[7])<<0);


   }
	return l;


	}

    }
  else
    {
      if (ok >= 10 && ok <= 70)
	{
	  op (exp) = O_constant;
	  adds (exp) = 0;
	  subs (exp) = 0;
	  offs (exp) = (ok == 10) ? 1 : 0;
	  as_warn ("Can't deal with expression; defaulting to %ld",
		   offs (exp));
	}
    }

  if (exp->size != SIZE_UNSPEC)
    {
      switch (exp->size)
	{
	case SIZE_UNSPEC:
	case SIZE_LONG:
	  break;
	case SIZE_BYTE:
	  if (!isbyte (offs (exp)))
	    as_warn ("expression doesn't fit in BYTE");
	  break;
	case SIZE_WORD:
	  if (!isword (offs (exp)))
	    as_warn ("expression doesn't fit in WORD");
	  break;
	}
    }
	if(xunsigned(exp))
		return offs(exp) & 0xffffffff;
  	return offs (exp);
}


/* place to dump relocation info for final output	*/
segT srelocationsection=0;
static void s_relocation(ignore)
{

	if(srelocationsection==0){
  		srelocationsection = subseg_new (".rlp", 0);
	    bfd_set_section_flags (stdoutput, srelocationsection,
				 SEC_ALLOC | SEC_LOAD | SEC_RELOC);
	}		
  subseg_set (srelocationsection,0);
}



static void s_readonly(ignore)
{

  subseg_set (rdata_section,0);

}

static void s_ctors(ignore)
{

  subseg_set (ctors_section,0);

}

static void s_dtors(ignore)
{

  subseg_set (dtors_section,0);

}


/* These are the back-ends for the various machine dependent pseudo-ops.  */

static void
s_data1 (ignore)
     int ignore;
{
  subseg_set (data_section, 1);
  demand_empty_rest_of_line ();
}

static void
s_data2 (ignore)
     int ignore;
{
  subseg_set (data_section, 2);
  demand_empty_rest_of_line ();
}

static void
s_bss (ignore)
     int ignore;
{
  /* We don't support putting frags in the BSS segment, we fake it
     by marking in_bss, then looking at s_skip for clues.  */

  subseg_set (bss_section, 0);
  demand_empty_rest_of_line ();
}

static void
s_even (ignore)
     int ignore;
{
  register int temp;
  register long temp_fill;

  temp = 1;			/* JF should be 2? */
  temp_fill = get_absolute_expression ();
  if (!need_pass_2)		/* Never make frag if expect extra pass. */
    frag_align (temp, (int) temp_fill, 0);
  demand_empty_rest_of_line ();
  record_alignment (now_seg, temp);
}

static void
s_proc (ignore)
     int ignore;
{
  demand_empty_rest_of_line ();
}

/* Pseudo-ops handled for MRI compatibility.  */

/* This function returns non-zero if the argument is a conditional
   pseudo-op.  This is called when checking whether a pending
   alignment is needed.  */

int
icvm_conditional_pseudoop (pop)
     pseudo_typeS *pop;
{
  return (pop->poc_handler == s_mri_if
	  || pop->poc_handler == s_mri_else);
}

/* Handle an MRI style chip specification.  */

static void
mri_chip ()
{
  char *s;
  char c;
  int i;

  s = input_line_pointer;
  /* We can't use get_symbol_end since the processor names are not proper
     symbols.  */
  while (is_part_of_name (c = *input_line_pointer++))
    ;
  *--input_line_pointer = 0;
  for (i = 0; i < n_archs; i++)
    if (strcasecmp (s, archs[i].name) == 0)
      break;
  if (i >= n_archs)
    {
      as_bad ("%s: unrecognized processor name", s);
      *input_line_pointer = c;
      ignore_rest_of_line ();
      return;
    }
  *input_line_pointer = c;

  current_architecture |= archs[i].arch;

  while (*input_line_pointer == '/')
    {
      ++input_line_pointer;
      s = input_line_pointer;
      /* We can't use get_symbol_end since the processor names are not
	 proper symbols.  */
      while (is_part_of_name (c = *input_line_pointer++))
	;
      *--input_line_pointer = 0;
      *input_line_pointer = c;
    }

}

/* The MRI CHIP pseudo-op.  */

static void
s_chip (ignore)
     int ignore;
{
  char *stop = NULL;
  char stopc;

  if (flag_mri)
    stop = mri_comment_field (&stopc);
  mri_chip ();
  if (flag_mri)
    mri_comment_end (stop, stopc);
  demand_empty_rest_of_line ();
}

/* The MRI FOPT pseudo-op.  */

static void
s_fopt (ignore)
     int ignore;
{
  SKIP_WHITESPACE ();

  if (strncasecmp (input_line_pointer, "ID=", 3) == 0)
    {
      int temp;

      input_line_pointer += 3;
      temp = get_absolute_expression ();
      if (temp < 0 || temp > 7)
	as_bad ("bad coprocessor id");
      else
	icvm_float_copnum = COP0 + temp;
    }
  else
    {
      as_bad ("unrecognized fopt option");
      ignore_rest_of_line ();
      return;
    }

  demand_empty_rest_of_line ();
}

/* The structure used to handle the MRI OPT pseudo-op.  */

struct opt_action
{
  /* The name of the option.  */
  const char *name;

  /* If this is not NULL, just call this function.  The first argument
     is the ARG field of this structure, the second argument is
     whether the option was negated.  */
  void (*pfn) PARAMS ((int arg, int on));

  /* If this is not NULL, and the PFN field is NULL, set the variable
     this points to.  Set it to the ARG field if the option was not
     negated, and the NOTARG field otherwise.  */
  int *pvar;

  /* The value to pass to PFN or to assign to *PVAR.  */
  int arg;

  /* The value to assign to *PVAR if the option is negated.  If PFN is
     NULL, and PVAR is not NULL, and ARG and NOTARG are the same, then
     the option may not be negated.  */
  int notarg;
};

/* The table used to handle the MRI OPT pseudo-op.  */

static void skip_to_comma PARAMS ((int, int));
static void opt_nest PARAMS ((int, int));
static void opt_chip PARAMS ((int, int));
static void opt_list PARAMS ((int, int));
static void opt_list_symbols PARAMS ((int, int));

static const struct opt_action opt_table[] =
{
  { "abspcadd", 0, &icvm_abspcadd, 1, 0 },

  /* We do relaxing, so there is little use for these options.  */
  { "b", 0, 0, 0, 0 },
  { "brs", 0, 0, 0, 0 },
  { "brb", 0, 0, 0, 0 },
  { "brl", 0, 0, 0, 0 },
  { "brw", 0, 0, 0, 0 },

  { "c", 0, 0, 0, 0 },
  { "cex", 0, 0, 0, 0 },
  { "case", 0, &symbols_case_sensitive, 1, 0 },
  { "cl", 0, 0, 0, 0 },
  { "cre", 0, 0, 0, 0 },
  { "d", 0, &flag_keep_locals, 1, 0 },
  { "e", 0, 0, 0, 0 },
  { "f", 0, &flag_short_refs, 1, 0 },
  { "frs", 0, &flag_short_refs, 1, 0 },
  { "frl", 0, &flag_short_refs, 0, 1 },
  { "g", 0, 0, 0, 0 },
  { "i", 0, 0, 0, 0 },
  { "m", 0, 0, 0, 0 },
  { "mex", 0, 0, 0, 0 },
  { "mc", 0, 0, 0, 0 },
  { "md", 0, 0, 0, 0 },
  { "nest", opt_nest, 0, 0, 0 },
  { "next", skip_to_comma, 0, 0, 0 },
  { "o", 0, 0, 0, 0 },
  { "old", 0, 0, 0, 0 },
  { "op", skip_to_comma, 0, 0, 0 },
  { "pco", 0, 0, 0, 0 },
  { "p", opt_chip, 0, 0, 0 },
  { "pcr", 0, 0, 0, 0 },
  { "pcs", 0, 0, 0, 0 },
  { "r", 0, 0, 0, 0 },
  { "quick", 0, &icvm_quick, 1, 0 },
  { "rel32", 0, &icvm_rel32, 1, 0 },
  { "s", opt_list, 0, 0, 0 },
  { "t", opt_list_symbols, 0, 0, 0 },
  { "w", 0, &flag_no_warnings, 0, 1 },
  { "x", 0, 0, 0, 0 }
};

#define OPTCOUNT (sizeof opt_table / sizeof opt_table[0])

/* The MRI OPT pseudo-op.  */

static void
s_opt (ignore)
     int ignore;
{
  do
    {
      int t;
      char *s;
      char c;
      int i;
      const struct opt_action *o;

      SKIP_WHITESPACE ();

      t = 1;
      if (*input_line_pointer == '-')
	{
	  ++input_line_pointer;
	  t = 0;
	}
      else if (strncasecmp (input_line_pointer, "NO", 2) == 0)
	{
	  input_line_pointer += 2;
	  t = 0;
	}

      s = input_line_pointer;
      c = get_symbol_end ();

      for (i = 0, o = opt_table; i < OPTCOUNT; i++, o++)
	{
	  if (strcasecmp (s, o->name) == 0)
	    {
	      if (o->pfn)
		{
		  /* Restore input_line_pointer now in case the option
		     takes arguments.  */
		  *input_line_pointer = c;
		  (*o->pfn) (o->arg, t);
		}
	      else if (o->pvar != NULL)
		{
		  if (! t && o->arg == o->notarg)
		    as_bad ("option `%s' may not be negated", s);
		  *input_line_pointer = c;
		  *o->pvar = t ? o->arg : o->notarg;
		}
	      else
		*input_line_pointer = c;
	      break;
	    }
	}
      if (i >= OPTCOUNT)
	{
	  as_bad ("option `%s' not recognized", s);
	  *input_line_pointer = c;
	}
    }
  while (*input_line_pointer++ == ',');

  /* Move back to terminating character.  */
  --input_line_pointer;
  demand_empty_rest_of_line ();
}

/* Skip ahead to a comma.  This is used for OPT options which we do
   not suppor tand which take arguments.  */

static void
skip_to_comma (arg, on)
     int arg;
     int on;
{
  while (*input_line_pointer != ','
	 && ! is_end_of_line[(unsigned char) *input_line_pointer])
    ++input_line_pointer;
}

/* Handle the OPT NEST=depth option.  */

static void
opt_nest (arg, on)
     int arg;
     int on;
{
  if (*input_line_pointer != '=')
    {
      as_bad ("bad format of OPT NEST=depth");
      return;
    }

  ++input_line_pointer;
  max_macro_nest = get_absolute_expression ();
}

/* Handle the OPT P=chip option.  */

static void
opt_chip (arg, on)
     int arg;
     int on;
{
  if (*input_line_pointer != '=')
    {
      /* This is just OPT P, which we do not support.  */
      return;
    }

  ++input_line_pointer;
  mri_chip ();
}

/* Handle the OPT S option.  */

static void
opt_list (arg, on)
     int arg;
     int on;
{
  listing_list (on);
}

/* Handle the OPT T option.  */

static void
opt_list_symbols (arg, on)
     int arg;
     int on;
{
  if (on)
    listing |= LISTING_SYMBOLS;
  else
    listing &=~ LISTING_SYMBOLS;
}

/* Handle the MRI REG pseudo-op.  */

static void
s_reg (ignore)
     int ignore;
{
  char *s;
  int c;
  struct icvm_op rop;
  int mask;
  char *stop = NULL;
  char stopc;

  if (line_label == NULL)
    {
      as_bad ("missing label");
      ignore_rest_of_line ();
      return;
    }

  if (flag_mri)
    stop = mri_comment_field (&stopc);

  SKIP_WHITESPACE ();

  s = input_line_pointer;
  while (isalnum ((unsigned char) *input_line_pointer)
#ifdef REGISTER_PREFIX
	 || *input_line_pointer == REGISTER_PREFIX
#endif
	 || *input_line_pointer == '/'
	 || *input_line_pointer == '-')
    ++input_line_pointer;
  c = *input_line_pointer;
  *input_line_pointer = '\0';

  if (icvm_ip_op (s, &rop) != 0)
    {
      if (rop.error == NULL)
	as_bad ("bad register list");
      else
	as_bad ("bad register list: %s", rop.error);
      *input_line_pointer = c;
      ignore_rest_of_line ();
      return;
    }

  *input_line_pointer = c;

  if (rop.mode == REGLST)
    mask = rop.mask;
  else if (rop.mode == MODE_r)
    mask = 1 << (rop.reg - DATA0);
  else if (rop.mode == AREG)
    mask = 1 << (rop.reg - ADDR0 + 8);
  else if (rop.mode == MODE_s)
    mask = 1 << (rop.reg - ADDR0 + 8);
  else if (rop.mode == MODE_b)
    mask = 1 << (rop.reg - ADDR0 + 8);
  else if (rop.mode == MODE_p)
    mask = 1 << (rop.reg - ADDR0 + 8);
  else if (rop.mode == MODE_F)
    mask = 1 << (rop.reg - FP0 + 16);
  else if (rop.mode == CONTROL
	   && rop.reg == FPI)
    mask = 1 << 24;
  else if (rop.mode == CONTROL
	   && rop.reg == FPS)
    mask = 1 << 25;
  else if (rop.mode == CONTROL
	   && rop.reg == FPC)
    mask = 1 << 26;
  else
    {
      as_bad ("bad register list");
      ignore_rest_of_line ();
      return;
    }

  S_SET_SEGMENT (line_label, reg_section);
  S_SET_VALUE (line_label, ~mask);
  line_label->sy_frag = &zero_address_frag;

  if (flag_mri)
    mri_comment_end (stop, stopc);

  demand_empty_rest_of_line ();
}

/* This structure is used for the MRI SAVE and RESTORE pseudo-ops.  */

struct save_opts
{
  struct save_opts *next;
  int abspcadd;
  int symbols_case_sensitive;
  int keep_locals;
  int short_refs;
  int architecture;
  int quick;
  int rel32;
  int listing;
  int no_warnings;
  /* FIXME: We don't save OPT S.  */
};

/* This variable holds the stack of saved options.  */

static struct save_opts *save_stack;

/* The MRI SAVE pseudo-op.  */

static void
s_save (ignore)
     int ignore;
{
  struct save_opts *s;

  s = (struct save_opts *) xmalloc (sizeof (struct save_opts));
  s->abspcadd = icvm_abspcadd;
  s->symbols_case_sensitive = symbols_case_sensitive;
  s->keep_locals = flag_keep_locals;
  s->short_refs = flag_short_refs;
  s->architecture = current_architecture;
  s->quick = icvm_quick;
  s->rel32 = icvm_rel32;
  s->listing = listing;
  s->no_warnings = flag_no_warnings;

  s->next = save_stack;
  save_stack = s;

  demand_empty_rest_of_line ();
}

/* The MRI RESTORE pseudo-op.  */

static void
s_restore (ignore)
     int ignore;
{
  struct save_opts *s;

  if (save_stack == NULL)
    {
      as_bad ("restore without save");
      ignore_rest_of_line ();
      return;
    }

  s = save_stack;
  save_stack = s->next;

  icvm_abspcadd = s->abspcadd;
  symbols_case_sensitive = s->symbols_case_sensitive;
  flag_keep_locals = s->keep_locals;
  flag_short_refs = s->short_refs;
  current_architecture = s->architecture;
  icvm_quick = s->quick;
  icvm_rel32 = s->rel32;
  listing = s->listing;
  flag_no_warnings = s->no_warnings;

  free (s);

  demand_empty_rest_of_line ();
}

/* Types of MRI structured control directives.  */

enum mri_control_type
{
  mri_for,
  mri_if,
  mri_repeat,
  mri_while
};

/* This structure is used to stack the MRI structured control
   directives.  */

struct mri_control_info
{
  /* The directive within which this one is enclosed.  */
  struct mri_control_info *outer;

  /* The type of directive.  */
  enum mri_control_type type;

  /* Whether an ELSE has been in an IF.  */
  int else_seen;

  /* The add or sub statement at the end of a FOR.  */
  char *incr;

  /* The label of the top of a FOR or REPEAT loop.  */
  char *top;

  /* The label to jump to for the next iteration, or the else
     expression of a conditional.  */
  char *next;

  /* The label to jump to to break out of the loop, or the label past
     the end of a conditional.  */
  char *bottom;
};

/* The stack of MRI structured control directives.  */

static struct mri_control_info *mri_control_stack;

/* The current MRI structured control directive index number, used to
   generate label names.  */

static int mri_control_index;

/* Some function prototypes.  */

static char *mri_control_label PARAMS ((void));
static struct mri_control_info *push_mri_control
  PARAMS ((enum mri_control_type));
static void pop_mri_control PARAMS ((void));
static int parse_mri_condition PARAMS ((int *));
static int parse_mri_control_operand
  PARAMS ((int *, char **, char **, char **, char **));
static int swap_mri_condition PARAMS ((int));
static int reverse_mri_condition PARAMS ((int));
static void build_mri_control_operand
  PARAMS ((int, int, char *, char *, char *, char *, const char *,
	   const char *, int));
static void parse_mri_control_expression
  PARAMS ((char *, int, const char *, const char *, int));

/* Generate a new MRI label structured control directive label name.  */

static char *
mri_control_label ()
{
  char *n;

  n = (char *) xmalloc (20);
  sprintf (n, "%smc%d", FAKE_LABEL_NAME, mri_control_index);
  ++mri_control_index;
  return n;
}

/* Create a new MRI structured control directive.  */

static struct mri_control_info *
push_mri_control (type)
     enum mri_control_type type;
{
  struct mri_control_info *n;

  n = (struct mri_control_info *) xmalloc (sizeof (struct mri_control_info));

  n->type = type;
  n->else_seen = 0;
  if (type == mri_if || type == mri_while)
    n->top = NULL;
  else
    n->top = mri_control_label ();
  n->next = mri_control_label ();
  n->bottom = mri_control_label ();

  n->outer = mri_control_stack;
  mri_control_stack = n;

  return n;
}

/* Pop off the stack of MRI structured control directives.  */

static void
pop_mri_control ()
{
  struct mri_control_info *n;

  n = mri_control_stack;
  mri_control_stack = n->outer;
  if (n->top != NULL)
    free (n->top);
  free (n->next);
  free (n->bottom);
  free (n);
}

/* Recognize a condition code in an MRI structured control expression.  */

static int
parse_mri_condition (pcc)
     int *pcc;
{
  char c1, c2;

  know (*input_line_pointer == '<');

  ++input_line_pointer;
  c1 = *input_line_pointer++;
  c2 = *input_line_pointer++;

  if (*input_line_pointer != '>')
    {
      as_bad ("syntax error in structured control directive");
      return 0;
    }

  ++input_line_pointer;
  SKIP_WHITESPACE ();

  if (isupper (c1))
    c1 = tolower (c1);
  if (isupper (c2))
    c2 = tolower (c2);

  *pcc = (c1 << 8) | c2;

  return 1;
}

/* Parse a single operand in an MRI structured control expression.  */

static int
parse_mri_control_operand (pcc, leftstart, leftstop, rightstart, rightstop)
     int *pcc;
     char **leftstart;
     char **leftstop;
     char **rightstart;
     char **rightstop;
{
  char *s;

  SKIP_WHITESPACE ();

  *pcc = -1;
  *leftstart = NULL;
  *leftstop = NULL;
  *rightstart = NULL;
  *rightstop = NULL;

  if (*input_line_pointer == '<')
    {
      /* It's just a condition code.  */
      return parse_mri_condition (pcc);
    }

  /* Look ahead for the condition code.  */
  for (s = input_line_pointer; *s != '\0'; ++s)
    {
      if (*s == '<' && s[1] != '\0' && s[2] != '\0' && s[3] == '>')
	break;
    }
  if (*s == '\0')
    {
      as_bad ("missing condition code in structured control directive");
      return 0;
    }

  *leftstart = input_line_pointer;
  *leftstop = s;
  if (*leftstop > *leftstart
      && ((*leftstop)[-1] == ' ' || (*leftstop)[-1] == '\t'))
    --*leftstop;

  input_line_pointer = s;
  if (! parse_mri_condition (pcc))
    return 0;

  /* Look ahead for AND or OR or end of line.  */
  for (s = input_line_pointer; *s != '\0'; ++s)
    {
      if ((strncasecmp (s, "AND", 3) == 0
	   && (s[3] == '.' || ! is_part_of_name (s[3])))
	  || (strncasecmp (s, "OR", 2) == 0
	      && (s[2] == '.' || ! is_part_of_name (s[2]))))
	break;
    }

  *rightstart = input_line_pointer;
  *rightstop = s;
  if (*rightstop > *rightstart
      && ((*rightstop)[-1] == ' ' || (*rightstop)[-1] == '\t'))
    --*rightstop;

  input_line_pointer = s;

  return 1;
}

#define MCC(b1, b2) (((b1) << 8) | (b2))

/* Swap the sense of a condition.  This changes the condition so that
   it generates the same result when the operands are swapped.  */

static int
swap_mri_condition (cc)
     int cc;
{
  switch (cc)
    {
    case MCC ('h', 'i'): return MCC ('c', 's');
    case MCC ('l', 's'): return MCC ('c', 'c');
    case MCC ('c', 'c'): return MCC ('l', 's');
    case MCC ('c', 's'): return MCC ('h', 'i');
    case MCC ('p', 'l'): return MCC ('m', 'i');
    case MCC ('m', 'i'): return MCC ('p', 'l');
    case MCC ('g', 'e'): return MCC ('l', 'e');
    case MCC ('l', 't'): return MCC ('g', 't');
    case MCC ('g', 't'): return MCC ('l', 't');
    case MCC ('l', 'e'): return MCC ('g', 'e');
    }
  return cc;
}

/* Reverse the sense of a condition.  */

static int
reverse_mri_condition (cc)
     int cc;
{
  switch (cc)
    {
    case MCC ('h', 'i'): return MCC ('l', 's');
    case MCC ('l', 's'): return MCC ('h', 'i');
    case MCC ('c', 'c'): return MCC ('c', 's');
    case MCC ('c', 's'): return MCC ('c', 'c');
    case MCC ('n', 'e'): return MCC ('e', 'q');
    case MCC ('e', 'q'): return MCC ('n', 'e');
    case MCC ('v', 'c'): return MCC ('v', 's');
    case MCC ('v', 's'): return MCC ('v', 'c');
    case MCC ('p', 'l'): return MCC ('m', 'i');
    case MCC ('m', 'i'): return MCC ('p', 'l');
    case MCC ('g', 'e'): return MCC ('l', 't');
    case MCC ('l', 't'): return MCC ('g', 'e');
    case MCC ('g', 't'): return MCC ('l', 'e');
    case MCC ('l', 'e'): return MCC ('g', 't');
    }
  return cc;
}

/* Build an MRI structured control expression.  This generates test
   and branch instructions.  It goes to TRUELAB if the condition is
   true, and to FALSELAB if the condition is false.  Exactly one of
   TRUELAB and FALSELAB will be NULL, meaning to fall through.  QUAL
   is the size qualifier for the expression.  EXTENT is the size to
   use for the branch.  */

static void
build_mri_control_operand (qual, cc, leftstart, leftstop, rightstart,
			   rightstop, truelab, falselab, extent)
     int qual;
     int cc;
     char *leftstart;
     char *leftstop;
     char *rightstart;
     char *rightstop;
     const char *truelab;
     const char *falselab;
     int extent;
{
  char *buf;
  char *s;


#ifdef NOT_NOW
  if (leftstart != NULL)
    {
      struct icvm_op leftop, rightop;
      char c;

      /* Swap the compare operands, if necessary, to produce a legal
	 icvm compare instruction.  Comparing a register operand with
	 a non-register operand requires the register to be on the
	 right (cmp, cmpa).  Comparing an immediate value with
	 anything requires the immediate value to be on the left
	 (cmpi).  */

      c = *leftstop;
      *leftstop = '\0';
      (void) icvm_ip_op (leftstart, &leftop);
      *leftstop = c;

      c = *rightstop;
      *rightstop = '\0';
      (void) icvm_ip_op (rightstart, &rightop);
      *rightstop = c;

/*  ****************** THIS CODE IS OBSOLETE BELOW AS WEE AS SWAP_MRI_COND */
      if (rightop.mode == MODE_IMMED
	  || ((leftop.mode == MODE_r || leftop.mode == AREG)
	      && (rightop.mode != MODE_r && rightop.mode != AREG)))
	{
	  char *temp;

	  cc = swap_mri_condition (cc);
	  temp = leftstart;
	  leftstart = rightstart;
	  rightstart = temp;
	  temp = leftstop;
	  leftstop = rightstop;
	  rightstop = temp;
	}
    }


#endif

  if (truelab == NULL)
    {
      cc = reverse_mri_condition (cc);
      truelab = falselab;
    }
      
  if (leftstart != NULL)
    {
      buf = (char *) xmalloc (20
			      + (leftstop - leftstart)
			      + (rightstop - rightstart));
      s = buf;
      *s++ = 'c';
      *s++ = 'm';
      *s++ = 'p';
      if (qual != '\0')
	*s++ = qual;
      *s++ = ' ';
      memcpy (s, leftstart, leftstop - leftstart);
      s += leftstop - leftstart;
      *s++ = ',';
      memcpy (s, rightstart, rightstop - rightstart);
      s += rightstop - rightstart;
      *s = '\0';
      md_assemble (buf);
      free (buf);
    }
      
  buf = (char *) xmalloc (20 + strlen (truelab));
  s = buf;
  *s++ = 'b';
  *s++ = cc >> 8;
  *s++ = cc & 0xff;
  if (extent != '\0')
    *s++ = extent;
  *s++ = ' ';
  strcpy (s, truelab);
  md_assemble (buf);
  free (buf);
}

/* Parse an MRI structured control expression.  This generates test
   and branch instructions.  STOP is where the expression ends.  It
   goes to TRUELAB if the condition is true, and to FALSELAB if the
   condition is false.  Exactly one of TRUELAB and FALSELAB will be
   NULL, meaning to fall through.  QUAL is the size qualifier for the
   expression.  EXTENT is the size to use for the branch.  */

static void
parse_mri_control_expression (stop, qual, truelab, falselab, extent)
     char *stop;
     int qual;
     const char *truelab;
     const char *falselab;
     int extent;
{
  int c;
  int cc;
  char *leftstart;
  char *leftstop;
  char *rightstart;
  char *rightstop;

  c = *stop;
  *stop = '\0';

  if (! parse_mri_control_operand (&cc, &leftstart, &leftstop,
				   &rightstart, &rightstop))
    {
      *stop = c;
      return;
    }

  if (strncasecmp (input_line_pointer, "AND", 3) == 0)
    {
      const char *flab;

      if (falselab != NULL)
	flab = falselab;
      else
	flab = mri_control_label ();

      build_mri_control_operand (qual, cc, leftstart, leftstop, rightstart,
				 rightstop, (const char *) NULL, flab, extent);

      input_line_pointer += 3;
      if (*input_line_pointer != '.'
	  || input_line_pointer[1] == '\0')
	qual = '\0';
      else
	{
	  qual = input_line_pointer[1];
	  input_line_pointer += 2;
	}

      if (! parse_mri_control_operand (&cc, &leftstart, &leftstop,
				       &rightstart, &rightstop))
	{
	  *stop = c;
	  return;
	}

      build_mri_control_operand (qual, cc, leftstart, leftstop, rightstart,
				 rightstop, truelab, falselab, extent);

      if (falselab == NULL)
	colon (flab);
    }
  else if (strncasecmp (input_line_pointer, "OR", 2) == 0)
    {
      const char *tlab;

      if (truelab != NULL)
	tlab = truelab;
      else
	tlab = mri_control_label ();

      build_mri_control_operand (qual, cc, leftstart, leftstop, rightstart,
				 rightstop, tlab, (const char *) NULL, extent);

      input_line_pointer += 2;
      if (*input_line_pointer != '.'
	  || input_line_pointer[1] == '\0')
	qual = '\0';
      else
	{
	  qual = input_line_pointer[1];
	  input_line_pointer += 2;
	}

      if (! parse_mri_control_operand (&cc, &leftstart, &leftstop,
				       &rightstart, &rightstop))
	{
	  *stop = c;
	  return;
	}

      build_mri_control_operand (qual, cc, leftstart, leftstop, rightstart,
				 rightstop, truelab, falselab, extent);

      if (truelab == NULL)
	colon (tlab);
    }
  else
    {
      build_mri_control_operand (qual, cc, leftstart, leftstop, rightstart,
				 rightstop, truelab, falselab, extent);
    }

  *stop = c;
  if (input_line_pointer != stop)
    as_bad ("syntax error in structured control directive");
}

/* Handle the MRI IF pseudo-op.  This may be a structured control
   directive, or it may be a regular assembler conditional, depending
   on its operands.  */

static void
s_mri_if (qual)
     int qual;
{
  char *s;
  int c;
  struct mri_control_info *n;

  /* A structured control directive must end with THEN with an
     optional qualifier.  */
  s = input_line_pointer;
  while (! is_end_of_line[(unsigned char) *s]
	 && (! flag_mri || *s != '*'))
    ++s;
  --s;
  while (s > input_line_pointer && (*s == ' ' || *s == '\t'))
    --s;

  if (s - input_line_pointer > 1
      && s[-1] == '.')
    s -= 2;

  if (s - input_line_pointer < 3
      || strncasecmp (s - 3, "THEN", 4) != 0)
    {
      if (qual != '\0')
	{
	  as_bad ("missing then");
	  ignore_rest_of_line ();
	  return;
	}

      /* It's a conditional.  */
      s_if (O_ne);
      return;
    }

  /* Since this might be a conditional if, this pseudo-op will be
     called even if we are supported to be ignoring input.  Double
     check now.  Clobber *input_line_pointer so that ignore_input
     thinks that this is not a special pseudo-op.  */
  c = *input_line_pointer;
  *input_line_pointer = 0;
  if (ignore_input ())
    {
      *input_line_pointer = c;
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
      demand_empty_rest_of_line ();
      return;
    }
  *input_line_pointer = c;

  n = push_mri_control (mri_if);

  parse_mri_control_expression (s - 3, qual, (const char *) NULL,
				n->next, s[1] == '.' ? s[2] : '\0');

  if (s[1] == '.')
    input_line_pointer = s + 3;
  else
    input_line_pointer = s + 1;

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI else pseudo-op.  If we are currently doing an MRI
   structured IF, associate the ELSE with the IF.  Otherwise, assume
   it is a conditional else.  */

static void
s_mri_else (qual)
     int qual;
{
  int c;
  char *buf;
  char q[2];

  if (qual == '\0'
      && (mri_control_stack == NULL
	  || mri_control_stack->type != mri_if
	  || mri_control_stack->else_seen))
    {
      s_else (0);
      return;
    }

  c = *input_line_pointer;
  *input_line_pointer = 0;
  if (ignore_input ())
    {
      *input_line_pointer = c;
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
      demand_empty_rest_of_line ();
      return;
    }
  *input_line_pointer = c;

  if (mri_control_stack == NULL
      || mri_control_stack->type != mri_if
      || mri_control_stack->else_seen)
    {
      as_bad ("else without matching if");
      ignore_rest_of_line ();
      return;
    }

  mri_control_stack->else_seen = 1;

  buf = (char *) xmalloc (20 + strlen (mri_control_stack->bottom));
  q[0] = qual;
  q[1] = '\0';
  sprintf (buf, "bra%s %s", q, mri_control_stack->bottom);
  md_assemble (buf);
  free (buf);

  colon (mri_control_stack->next);

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI ENDI pseudo-op.  */

static void
s_mri_endi (ignore)
     int ignore;
{
  if (mri_control_stack == NULL
      || mri_control_stack->type != mri_if)
    {
      as_bad ("endi without matching if");
      ignore_rest_of_line ();
      return;
    }

  /* ignore_input will not return true for ENDI, so we don't need to
     worry about checking it again here.  */

  if (! mri_control_stack->else_seen)
    colon (mri_control_stack->next);
  colon (mri_control_stack->bottom);

  pop_mri_control ();

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI BREAK pseudo-op.  */

static void
s_mri_break (extent)
     int extent;
{
  struct mri_control_info *n;
  char *buf;
  char ex[2];

  n = mri_control_stack;
  while (n != NULL
	 && n->type != mri_for
	 && n->type != mri_repeat
	 && n->type != mri_while)
    n = n->outer;
  if (n == NULL)
    {
      as_bad ("break outside of structured loop");
      ignore_rest_of_line ();
      return;
    }

  buf = (char *) xmalloc (20 + strlen (n->bottom));
  ex[0] = extent;
  ex[1] = '\0';
  sprintf (buf, "bra%s %s", ex, n->bottom);
  md_assemble (buf);
  free (buf);

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI NEXT pseudo-op.  */

static void
s_mri_next (extent)
     int extent;
{
  struct mri_control_info *n;
  char *buf;
  char ex[2];

  n = mri_control_stack;
  while (n != NULL
	 && n->type != mri_for
	 && n->type != mri_repeat
	 && n->type != mri_while)
    n = n->outer;
  if (n == NULL)
    {
      as_bad ("next outside of structured loop");
      ignore_rest_of_line ();
      return;
    }

  buf = (char *) xmalloc (20 + strlen (n->next));
  ex[0] = extent;
  ex[1] = '\0';
  sprintf (buf, "bra%s %s", ex, n->next);
  md_assemble (buf);
  free (buf);

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI FOR pseudo-op.  */

static void
s_mri_for (qual)
     int qual;
{
  const char *varstart, *varstop;
  const char *initstart, *initstop;
  const char *endstart, *endstop;
  const char *bystart, *bystop;
  int up;
  int by;
  int extent;
  struct mri_control_info *n;
  char *buf;
  char *s;
  char ex[2];

  /* The syntax is
       FOR.q var = init { TO | DOWNTO } end [ BY by ] DO.e
     */

  SKIP_WHITESPACE ();
  varstart = input_line_pointer;

  /* Look for the '='.  */
  while (! is_end_of_line[(unsigned char) *input_line_pointer]
	 && *input_line_pointer != '=')
    ++input_line_pointer;
  if (*input_line_pointer != '=')
    {
      as_bad ("missing =");
      ignore_rest_of_line ();
      return;
    }

  varstop = input_line_pointer;
  if (varstop > varstart
      && (varstop[-1] == ' ' || varstop[-1] == '\t'))
    --varstop;

  ++input_line_pointer;

  initstart = input_line_pointer;

  /* Look for TO or DOWNTO.  */
  up = 1;
  initstop = NULL;
  while (! is_end_of_line[(unsigned char) *input_line_pointer])
    {
      if (strncasecmp (input_line_pointer, "TO", 2) == 0
	  && ! is_part_of_name (input_line_pointer[2]))
	{
	  initstop = input_line_pointer;
	  input_line_pointer += 2;
	  break;
	}
      if (strncasecmp (input_line_pointer, "DOWNTO", 6) == 0
	  && ! is_part_of_name (input_line_pointer[6]))
	{
	  initstop = input_line_pointer;
	  up = 0;
	  input_line_pointer += 6;
	  break;
	}
      ++input_line_pointer;
    }
  if (initstop == NULL)
    {
      as_bad ("missing to or downto");
      ignore_rest_of_line ();
      return;
    }
  if (initstop > initstart
      && (initstop[-1] == ' ' || initstop[-1] == '\t'))
    --initstop;

  SKIP_WHITESPACE ();
  endstart = input_line_pointer;

  /* Look for BY or DO.  */
  by = 0;
  endstop = NULL;
  while (! is_end_of_line[(unsigned char) *input_line_pointer])
    {
      if (strncasecmp (input_line_pointer, "BY", 2) == 0
	  && ! is_part_of_name (input_line_pointer[2]))
	{
	  endstop = input_line_pointer;
	  by = 1;
	  input_line_pointer += 2;
	  break;
	}
      if (strncasecmp (input_line_pointer, "DO", 2) == 0
	  && (input_line_pointer[2] == '.'
	      || ! is_part_of_name (input_line_pointer[2])))
	{
	  endstop = input_line_pointer;
	  input_line_pointer += 2;
	  break;
	}
      ++input_line_pointer;
    }
  if (endstop == NULL)
    {
      as_bad ("missing do");
      ignore_rest_of_line ();
      return;
    }
  if (endstop > endstart
      && (endstop[-1] == ' ' || endstop[-1] == '\t'))
    --endstop;

  if (! by)
    {
      bystart = "#1";
      bystop = bystart + 2;
    }
  else
    {
      SKIP_WHITESPACE ();
      bystart = input_line_pointer;

      /* Look for DO.  */
      bystop = NULL;
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	{
	  if (strncasecmp (input_line_pointer, "DO", 2) == 0
	      && (input_line_pointer[2] == '.'
		  || ! is_part_of_name (input_line_pointer[2])))
	    {
	      bystop = input_line_pointer;
	      input_line_pointer += 2;
	      break;
	    }
	  ++input_line_pointer;
	}
      if (bystop == NULL)
	{
	  as_bad ("missing do");
	  ignore_rest_of_line ();
	  return;
	}
      if (bystop > bystart
	  && (bystop[-1] == ' ' || bystop[-1] == '\t'))
	--bystop;
    }

  if (*input_line_pointer != '.')
    extent = '\0';
  else
    {
      extent = input_line_pointer[1];
      input_line_pointer += 2;
    }

  /* We have fully parsed the FOR operands.  Now build the loop.  */

  n = push_mri_control (mri_for);

  buf = (char *) xmalloc (50 + (input_line_pointer - varstart));

  /* move init,var */
  s = buf;
  *s++ = 'm';
  *s++ = 'o';
  *s++ = 'v';
  *s++ = 'e';
  if (qual != '\0')
    *s++ = qual;
  *s++ = ' ';
  memcpy (s, initstart, initstop - initstart);
  s += initstop - initstart;
  *s++ = ',';
  memcpy (s, varstart, varstop - varstart);
  s += varstop - varstart;
  *s = '\0';
  md_assemble (buf);

  colon (n->top);

  /* cmp end,var */
  s = buf;
  *s++ = 'c';
  *s++ = 'm';
  *s++ = 'p';
  if (qual != '\0')
    *s++ = qual;
  *s++ = ' ';
  memcpy (s, endstart, endstop - endstart);
  s += endstop - endstart;
  *s++ = ',';
  memcpy (s, varstart, varstop - varstart);
  s += varstop - varstart;
  *s = '\0';
  md_assemble (buf);

  /* bcc bottom */
  ex[0] = extent;
  ex[1] = '\0';
  if (up)
    sprintf (buf, "blt%s %s", ex, n->bottom);
  else
    sprintf (buf, "bgt%s %s", ex, n->bottom);
  md_assemble (buf);

  /* Put together the add or sub instruction used by ENDF.  */
  s = buf;
  if (up)
    strcpy (s, "add");
  else
    strcpy (s, "sub");
  s += 3;
  if (qual != '\0')
    *s++ = qual;
  *s++ = ' ';
  memcpy (s, bystart, bystop - bystart);
  s += bystop - bystart;
  *s++ = ',';
  memcpy (s, varstart, varstop - varstart);
  s += varstop - varstart;
  *s = '\0';
  n->incr = buf;

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI ENDF pseudo-op.  */

static void
s_mri_endf (ignore)
     int ignore;
{
  if (mri_control_stack == NULL
      || mri_control_stack->type != mri_for)
    {
      as_bad ("endf without for");
      ignore_rest_of_line ();
      return;
    }

  colon (mri_control_stack->next);

  md_assemble (mri_control_stack->incr);

  sprintf (mri_control_stack->incr, "bra %s", mri_control_stack->top);
  md_assemble (mri_control_stack->incr);

  free (mri_control_stack->incr);

  colon (mri_control_stack->bottom);

  pop_mri_control ();

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI REPEAT pseudo-op.  */

static void
s_mri_repeat (ignore)
     int ignore;
{
  struct mri_control_info *n;

  n = push_mri_control (mri_repeat);
  colon (n->top);
  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }
  demand_empty_rest_of_line ();
}

/* Handle the MRI UNTIL pseudo-op.  */

static void
s_mri_until (qual)
     int qual;
{
  char *s;

  if (mri_control_stack == NULL
      || mri_control_stack->type != mri_repeat)
    {
      as_bad ("until without repeat");
      ignore_rest_of_line ();
      return;
    }

  colon (mri_control_stack->next);

  for (s = input_line_pointer; ! is_end_of_line[(unsigned char) *s]; s++)
    ;

  parse_mri_control_expression (s, qual, (const char *) NULL,
				mri_control_stack->top, '\0');

  colon (mri_control_stack->bottom);

  input_line_pointer = s;

  pop_mri_control ();

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI WHILE pseudo-op.  */

static void
s_mri_while (qual)
     int qual;
{
  char *s;

  struct mri_control_info *n;

  s = input_line_pointer;
  while (! is_end_of_line[(unsigned char) *s]
	 && (! flag_mri || *s != '*'))
    s++;
  --s;
  while (*s == ' ' || *s == '\t')
    --s;
  if (s - input_line_pointer > 1
      && s[-1] == '.')
    s -= 2;
  if (s - input_line_pointer < 2
      || strncasecmp (s - 1, "DO", 2) != 0)
    {
      as_bad ("missing do");
      ignore_rest_of_line ();
      return;
    }

  n = push_mri_control (mri_while);

  colon (n->next);

  parse_mri_control_expression (s - 1, qual, (const char *) NULL, n->bottom,
				s[1] == '.' ? s[2] : '\0');

  input_line_pointer = s + 1;
  if (*input_line_pointer == '.')
    input_line_pointer += 2;

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/* Handle the MRI ENDW pseudo-op.  */

static void
s_mri_endw (ignore)
     int ignore;
{
  char *buf;

  if (mri_control_stack == NULL
      || mri_control_stack->type != mri_while)
    {
      as_bad ("endw without while");
      ignore_rest_of_line ();
      return;
    }

  buf = (char *) xmalloc (20 + strlen (mri_control_stack->next));
  sprintf (buf, "bra %s", mri_control_stack->next);
  md_assemble (buf);
  free (buf);

  colon (mri_control_stack->bottom);

  pop_mri_control ();

  if (flag_mri)
    {
      while (! is_end_of_line[(unsigned char) *input_line_pointer])
	++input_line_pointer;
    }

  demand_empty_rest_of_line ();
}

/*
 * md_parse_option
 *	Invocation line includes a switch not recognized by the base assembler.
 *	See if it's a processor-specific option.  These are:
 *
 *	-[A]m[c]68000, -[A]m[c]68008, -[A]m[c]68010, -[A]m[c]68020, -[A]m[c]68030, -[A]m[c]68040
 *	-[A]m[c]68881, -[A]m[c]68882, -[A]m[c]68851
 *		Select the architecture.  Instructions or features not
 *		supported by the selected architecture cause fatal
 *		errors.  More than one may be specified.  The default is
 *		-m68020 -m68851 -m68881.  Note that -m68008 is a synonym
 *		for -m68000, and -m68882 is a synonym for -m68881.
 *	-[A]m[c]no-68851, -[A]m[c]no-68881
 *		Don't accept 688?1 instructions.  (The "c" is kind of silly,
 *		so don't use or document it, but that's the way the parsing
 *		works).
 *
 *	-pic	Indicates PIC.
 *	-k	Indicates PIC.  (Sun 3 only.)
 *
 *	--bitwise-or
 *		Permit `|' to be used in expressions.
 *
 */

#ifdef OBJ_ELF
CONST char *md_shortopts = "lSA:m:kQ:V";
#else
CONST char *md_shortopts = "lSA:m:k";
#endif

struct option md_longopts[] = {
#define OPTION_REGISTER_PREFIX_OPTIONAL (OPTION_MD_BASE)
  {"register-prefix-optional", no_argument, NULL,
     OPTION_REGISTER_PREFIX_OPTIONAL},
#define OPTION_BITWISE_OR (OPTION_MD_BASE + 1)
  {"bitwise-or", no_argument, NULL, OPTION_BITWISE_OR},
#define OPTION_LITTLE_ENDIAN (OPTION_MD_BASE + 2)
  {"little-endian", no_argument, NULL, OPTION_LITTLE_ENDIAN},
#define OPTION_BIG_ENDIAN (OPTION_MD_BASE + 3)
  {"big-endian", no_argument, NULL, OPTION_BIG_ENDIAN},
};
size_t md_longopts_size = sizeof(md_longopts);

int
md_parse_option (c, arg)
     int c;
     char *arg;
{
  switch (c)
    {
    case 'l':			/* -l means keep external to 2 bit offset
				   rather than 16 bit one */
      flag_short_refs = 1;
      break;

    case 'S':			/* -S means that jbsr's always turn into
				   jsr's.  */
      flag_long_jumps = 1;
      break;

    case 'A':
      if (*arg == 'm')
 	arg++;
      /* intentional fall-through */
    case 'm':

      if (arg[0] == 'n' && arg[1] == 'o' && arg[2] == '-')
	{
	  int i;
	  unsigned long arch;
	  const char *oarg = arg;

	  arg += 3;
	  if (*arg == 'm')
	    {
	      arg++;
	      if (arg[0] == 'c' && arg[1] == '6')
		arg++;
	    }
	  for (i = 0; i < n_archs; i++)
	    if (!strcmp (arg, archs[i].name))
	      break;
	  if (i == n_archs)
	    {
	    unknown:
	      as_bad ("unrecognized option `%s'", oarg);
	      return 0;
	    }
	  arch = archs[i].arch;
	  goto unknown;
	}
      else
	{
	  int i;

	  if (arg[0] == 'c' && arg[1] == '6')
	    arg++;

	  for (i = 0; i < n_archs; i++)
	    if (!strcmp (arg, archs[i].name))
	      {
		unsigned long arch = archs[i].arch;
		  /* ??? */
		  abort ();
		break;
	      }
	  if (i == n_archs)
	    {
	      as_bad ("unrecognized architecture specification `%s'", arg);
	      return 0;
	    }
	}
      break;

    case OPTION_LITTLE_ENDIAN:
      target_big_endian = 0;
      break;

    case OPTION_BIG_ENDIAN:
      target_big_endian = 1;
      break;


    case OPTION_REGISTER_PREFIX_OPTIONAL:
      flag_reg_prefix_optional = 1;
      reg_prefix_optional_seen = 1;
      break;

      /* -V: SVR4 argument to print version ID.  */
    case 'V':
      print_version_id ();
      break;

      /* -Qy, -Qn: SVR4 arguments controlling whether a .comment section
	 should be emitted or not.  FIXME: Not implemented.  */
    case 'Q':
      break;

    case OPTION_BITWISE_OR:
      {
	char *n, *t;
	const char *s;

	n = (char *) xmalloc (strlen (icvm_comment_chars) + 1);
	t = n;
	for (s = icvm_comment_chars; *s != '\0'; s++)
	*t = '\0';
	icvm_comment_chars = n;
      }
      break;

    default:
      return 0;
    }

  return 1;
}

void
md_show_usage (stream)
     FILE *stream;
{
  fprintf(stream, "\
options:\n\
--register-prefix-optional\n\
			recognize register names without prefix character\n\
--bitwise-or		do not treat `|' as a comment character\n\
--little-endian		compile for little -endian\n\
--big-endian		compile for big endian\n");
}

#ifdef TEST2

/* TEST2:  Test md_assemble() */
/* Warning, this routine probably doesn't work anymore */

main ()
{
  struct icvm_it the_ins;
  char buf[120];
  char *cp;
  int n;

  icvm_ip_begin ();
  for (;;)
    {
      if (!gets (buf) || !*buf)
	break;
      if (buf[1] == '.')
	continue;
      for (cp = buf; *cp; cp++)
	if (*cp == '\t')
	  *cp = ' ';
      if (is_label (buf))
	continue;
      memset (&the_ins, '\0', sizeof (the_ins));
      icvm_ip (&the_ins, buf);
      if (the_ins.error)
	{
	  printf ("Error %s in %s\n", the_ins.error, buf);
	}
      else
	{
	  printf ("Opcode(%d.%s): ", the_ins.numo, the_ins.args);
	  for (n = 0; n < the_ins.numo; n++)
	    printf (" 0x%x", the_ins.opcode[n] & 0xffff);
	  printf ("    ");
	  print_the_insn (&the_ins.opcode[0], stdout);
	  (void) putchar ('\n');
	}
      for (n = 0; n < strlen (the_ins.args) / 2; n++)
	{
	  if (the_ins.operands[n].error)
	    {
	      printf ("op%d Error %s in %s\n", n, the_ins.operands[n].error, buf);
	      continue;
	    }
	  printf ("mode %d, reg %d, ", the_ins.operands[n].mode, the_ins.operands[n].reg);
	  if (the_ins.operands[n].b_const)
	    printf ("Constant: '%.*s', ", 1 + the_ins.operands[n].e_const - the_ins.operands[n].b_const, the_ins.operands[n].b_const);
	  printf ("ireg %d, isiz %d, imul %d, ", the_ins.operands[n].ireg, the_ins.operands[n].isiz, the_ins.operands[n].imul);
	  if (the_ins.operands[n].b_iadd)
	    printf ("Iadd: '%.*s',", 1 + the_ins.operands[n].e_iadd - the_ins.operands[n].b_iadd, the_ins.operands[n].b_iadd);
	  (void) putchar ('\n');
	}
    }
  icvm_ip_end ();
  return 0;
}

is_label (str)
     char *str;
{
  while (*str == ' ')
    str++;
  while (*str && *str != ' ')
    str++;
  if (str[-1] == ':' || str[1] == '=')
    return 1;
  return 0;
}

#endif

/* Possible states for relaxation:

   0 0	branch offset	byte	(bra, etc)
   0 1			word
   0 2			long

   1 0	indexed offsets	byte	a0@(32,d4:w:1) etc
   1 1			word
   1 2			long

   2 0	two-offset index word-word a0@(32,d4)@(45) etc
   2 1			word-long
   2 2			long-word
   2 3			long-long

   */

/* We have no need to default values of symbols.  */

/* ARGSUSED */
symbolS *
md_undefined_symbol (name)
     char *name;
{
  return 0;
}

/* Round up a section size to the appropriate boundary.  */
valueT
md_section_align (segment, size)
     segT segment;
     valueT size;
{
#ifdef OBJ_AOUT
#ifdef BFD_ASSEMBLER
  /* For a.out, force the section size to be aligned.  If we don't do
     this, BFD will align it for us, but it will not write out the
     final bytes of the section.  This may be a bug in BFD, but it is
     easier to fix it here since that is how the other a.out targets
     work.  */
  int align;

  align = bfd_get_section_alignment (stdoutput, segment);
  size = ((size + (1 << align) - 1) & ((valueT) -1 << align));
#endif
#endif

  return size;
}

/* Exactly what point is a PC-relative offset relative TO?
   On the 68k, it is relative to the address of the first extension
   word.  The difference between the addresses of the offet and the
   first extension word is stored in fx_pcrel_adjust. */
long
md_pcrel_from (fixP)
     fixS *fixP;
{
  int adjust;

  /* Because fx_pcrel_adjust is a char, and may be unsigned, we store
     -1 as 64.  */
  adjust = fixP->fx_pcrel_adjust;
  if (adjust == 64)
    adjust = -1;
  return fixP->fx_where + fixP->fx_frag->fr_address - adjust;
}

#ifndef BFD_ASSEMBLER
#ifdef OBJ_COFF

/*ARGSUSED*/
void
tc_coff_symbol_emit_hook (ignore)
     symbolS *ignore;
{
}

int
tc_coff_sizemachdep (frag)
     fragS *frag;
{
  switch (frag->fr_subtype & 0x3)
    {
    case BYTE:
      return 1;
    case SHORT:
      return 2;
    case LONG:
      return 4;
    default:
      abort ();
      return 0;
    }
}

#endif
#endif

/* end of tc-icvm.c */
