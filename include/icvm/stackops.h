#ifndef STACKOPS_H
#define STACKOPS_H

// define various optimal stack operations used with ICVM
// these macros/functions allow the stack modes of ICVM to be used
// easily to create machine language statements at higher levels for
// better code in some (but not most or all) cases.

// strictly convenience ....

// ************ stack pointer ops *************************


#define _STK_getptr(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("movel %%sp,%w0"\
				 : "=g" ((void *)(x))\
				 );\
	  }))

#define _STK_putptr(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("movel %w1,%%sp"\
				 : "=g" ((void *)(junk))\
				 : "g" ((void *)(x))\
				 );\
	  }))


static inline void *STK_ptr(void){
	void *n;
	_STK_getptr(n);
	return n;
}


static inline void STK_saveptr(void **where){*where=STK_ptr();}
static inline void STK_restoreptr(void **where){_STK_putptr(*where);}

#define STK_addstkptr(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("addl %w1,%%sp"\
				 : "=g" ((long int)(junk))\
				 : "g" (( long int) (x))\
				 );\
	  }))


#define STK_generic2OP(op)	__asm__ __volatile__ (op " (%sp)+,-(%sp)")
#define STK_generic3OP(op)	__asm__ __volatile__ (op " (%sp)+,(%sp)+,-(%sp)")


// ****************  32 bit integer ops  ************************

#define STK_pushl(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("movel %w1,-(%%sp)"\
				 : "=g" ((long int)(junk))\
				 : "g" (( long int) (x))\
				 );\
	  }))


#define _STK_popl(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("movel (%%sp)+,%w0"\
				 : "=g" ((long int)(x))\
				 );\
	  }))



// pure stack ops


#define STK_addl() STK_generic3OP("addl")
#define STK_subl() STK_generic3OP("subl")
#define STK_mull() STK_generic3OP("mull")
#define STK_divl() STK_generic3OP("divl")
#define STK_modl() STK_generic3OP("modl")
#define STK_andl() STK_generic3OP("andl")
#define STK_orl()  STK_generic3OP("orl")
#define STK_xorl() STK_generic3OP("eorl")
#define STK_notl() STK_generic2OP("notl")
#define STK_negl() STK_generic2OP("negl")



// general purpose stack ops

static inline int STK_popl(void){int n;_STK_popl(n);return n;};
static inline void STK_dupl(void){int *np=STK_ptr();STK_pushl(*np);};
static inline void STK_swapl(void){int *np=STK_ptr();int n=*np;np[0]=np[1];np[1]=n;};
#define STK_discardl() __asm__ __volatile__ ("inc4l %sp")


// ****************  64 bit integer ops  ************************

#define STK_pushq(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("moveq %w1,-(%%sp)"\
				 : "=g" ((long int)(junk))\
				 : "g" (( long long int) (x))\
				 );\
	  }))


#define _STK_popq(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("moveq (%%sp)+,%w0"\
				 : "=g" ((long long int)(x))\
				 );\
	  }))



// pure stack ops


#define STK_addq() STK_generic3OP("addq")
#define STK_subq() STK_generic3OP("subq")
#define STK_mulq() STK_generic3OP("mulq")
#define STK_divq() STK_generic3OP("divq")
#define STK_modq() STK_generic3OP("modq")
#define STK_andq() STK_generic3OP("andq")
#define STK_orq()  STK_generic3OP("orq")
#define STK_xorq() STK_generic3OP("eorq")
#define STK_notq() STK_generic2OP("notq")
#define STK_negq() STK_generic2OP("negq")



// general purpose stack ops

static inline int STK_popq(void)
	{long long int n;_STK_popq(n);return n;};
static inline void STK_dupq(void)
	{long long int *np=STK_ptr();STK_pushq(*np);};
static inline void STK_swapq(void)
	{long long int *np=STK_ptr();long long int n=*np;np[0]=np[1];np[1]=n;};
#define STK_discardq() __asm__ __volatile__ ("inc8l %sp")

#endif


// ****************  32 bit float ops  ************************

#define STK_pushf(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("movef %w1,-(%%sp)"\
				 : "=g" ((float)(junk))\
				 : "g" (( float) (x))\
				 );\
	  }))


#define _STK_popf(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("movef (%%sp)+,%w0"\
				 : "=g" ((float)(x))\
				 );\
	  }))



// pure stack ops


#define STK_addf() STK_generic3OP("addf")
#define STK_subf() STK_generic3OP("subf")
#define STK_mulf() STK_generic3OP("mulf")
#define STK_divf() STK_generic3OP("divf")
#define STK_modf() STK_generic3OP("modf")
#define STK_andf() STK_generic3OP("andf")
#define STK_orf()  STK_generic3OP("orf")
#define STK_xorf() STK_generic3OP("eorf")
#define STK_negf() STK_generic2OP("negf")



// general purpose stack ops

static inline int STK_popf(void){float n;_STK_popf(n);return n;};
#define STK_dupf STK_dupl
#define STK_swapf STK_swapl
#define STK_discardf STK_discardl


// ****************  64 bit float ops  ************************

#define STK_pushd(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("moved %w1,-(%%sp)"\
				 : "=g" ((double)(junk))\
				 : "g" (( double) (x))\
				 );\
	  }))


#define _STK_popd(x)\
     (__extension__\
      ({ 	unsigned long junk;\
	   __asm__ __volatile__ ("moved (%%sp)+,%w0"\
				 : "=g" ((double)(x))\
				 );\
	  }))



// pure stack ops


#define STK_addd() STK_generic3OP("addd")
#define STK_subd() STK_generic3OP("subd")
#define STK_muld() STK_generic3OP("muld")
#define STK_divd() STK_generic3OP("divd")
#define STK_modd() STK_generic3OP("modd")
#define STK_andd() STK_generic3OP("andd")
#define STK_ord()  STK_generic3OP("ord")
#define STK_xord() STK_generic3OP("eord")
#define STK_negd() STK_generic2OP("negd")



// general purpose stack ops

static inline int STK_popd(void){double n;_STK_popd(n);return n;};
#define STK_dupd STK_dupq
#define STK_swapd STK_swapq
#define STK_discardd STK_discardq



