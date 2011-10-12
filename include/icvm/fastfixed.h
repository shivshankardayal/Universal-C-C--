#ifndef FASTFIXED_H
#define FASTFIXED_H
#include <icvm/fastsyscall.h>



// these numbers all multiply two fixed numbers together.
// no error or range checking is performed on these



#define ICVM_fixed32mul(x,y) (__extension__ ({ \
		int __result; \
		__asm__ __volatile__("fixed32mull  %2,%0" \
			:"=d" (__result) :"0" ((int) (x)), \
			"d" ((int) (y))); \
		__result; }))




#define ICVM_fixed32div(x,y) ( __extension__ ({ \
		int __result; \
		__asm__ __volatile__("fixed32divl  %2,%0" \
			:"=d" (__result) :"0" ((int) (x)), \
			"d" ((int) (y))); \
		__result; }))



#ifdef NOT_NOW
extern int  ICVM_fixed32mul(int,int)
	ICVMDIRECT("fixed32mul");
extern unsigned int  ICVM_fixed32mulu(unsigned int,unsigned int)
	ICVMDIRECT("fixed32mulu");

extern int  ICVM_fixed32div(int,int)
	ICVMDIRECT("fixed32div");
extern unsigned int  ICVM_fixed32divu(unsigned int,unsigned int)
	ICVMDIRECT("fixed32divu");

#endif

#endif

