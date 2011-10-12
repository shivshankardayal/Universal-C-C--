#ifndef CVM_H
#define CVM_H



// Copyright (C) 2000,2001 Bob Dayley

/* BASIC TYPES */

typedef char  					s8;
typedef unsigned char  			u8;
typedef short 					s16;
typedef unsigned short 			u16;
typedef int 					s32;
typedef unsigned int 			u32;
typedef long long   			s64;
typedef unsigned long long   	u64;
typedef float					f32;
typedef double					f64;
typedef s32						p32;


#ifdef BIGENDIAN
#define I_HI	0
#define I_LOW	1
#else
#define I_HI	1
#define I_LOW	0
#endif

// stuff to break down a word to optimization

typedef union {
	s8 b[1];
	u8 ub[1];
} g8;

typedef union {
	s16 w[1];
	u16 uw[1];
	s8 b[2];
	u8 ub[2];
	g8 gb[2];
} g16;

typedef union {
	s32 l[1];
	u32 ul[1];
	f32 f[1];
	s16 w[2];
	s16 uw[2];
	g16 gw[2];
	s8 	b[4];
	s8 	ub[4];
	
} g32;

typedef union {
	s64 q[1];
	u64 uq[1];
	f64 d[1];
	s32 l[2];
	u32 ul[2];
	g32 gl[2];
	f32 f[2]; 
	s16 w[4];
	s16 uw[4];
	s8 	b[8];
	s8 	ub[8];
} g64;


typedef union {
	s64 q[2];
	u64 uq[2];
	g64 gq[2];
	f64 d[2];
	s32 l[4];
	u32 ul[4];
	g32 gl[4];
	f32 f[4]; 
	s16 w[8];
	s16 uw[8];
	s8 	b[8];
	s8 	ub[8];
} g128;


typedef union {
	g128 go[2];
	s64 q[4];
	u64 uq[4];
	f64 d[4];
	g64 gq[4];
	s32 l[8];
	u32 ul[8];
	g32 gl[8];
	f32 f[8]; 
	s16 w[16];
	s16 uw[16];
	s8 	b[32];
	s8 	ub[32];
} g256;


#ifdef __WIN32__
#include <windows.h>

#endif




// #define I386 define this for I386 specifics
//#define USEMMX  // define this for MMX support
//#define USE3DNOW  // define this for K6/ATHLON 3DNOW MMX support



//#define BIGANDFAST	// define this if you want the fast model
				   // generates one of two models, with this approach,
				   // depending on USESHORTFUNCTIONDISPATCH.
					// Generates lots of fast functions.
					// if BIGANDFAST is NOT selected, separate address mode
					// routines are built, running much slower, but
					// very compact


#define USEHWREGS // define this to map native HW regs to VM
				  // used in all models



// ******** configuration needs
// #define BIGENDIAN - define if target is big endian

//#define NEEDSHIFTMASK31		// define this if your processor does NOT truncate
							// a shift amount by 5 bits  (mod 32)
							//	for 8,16, and 32 bit values.
							//  Pentium, 68k do not need this

//#define NEEDSHIFTMASK64		// define this if your processor does NOT truncate
							// a shift amount by 5 bits (mod 64) 
							// for 64 bit values. 
							// Pentium, 68k do not need this

// these alignments are NOT fully supported yet.

// set this if 32 bit alignment MUST be followed for memory access of 32 bits
//#define ICVM_NEEDALIGN32

// set this if 64 bit alignment MUST be followed for memory access of 64 bits
//#define ICVM_NEEDALIGN64

// turn this on to check for strict alignment violations for ICVM violation
// for testing only - will slow things down greatly!

// Checks for conformance with ICVM alignment specifications -- natural
// alignment to 16 bits in code memory and natural alignment to 32 bits in
// data memory.

// these macros apply checking ONLY to the RD .. WD ... RC ..
// WC ... macros below.
// you might want to enable this for checking final code on machines that
// do not fail on aligment violations, such as Pentium and PPC.
// This code will enforce strict ICVM alignment and report alignment violations.
// When not testing final code, this should be off for greatest speed.

//#define ICVM_FORCEDALIGNMENTCHECK

// enable one of these to check alignment conformance to a full natural aligned
// machine on the doREAD and doWRITE levels.

// this is mostly to allow testing of the alignment macros for processors
// like MIPS and SPARC.

// normally you should not enable these!!! It will generate VERY slow code.

//#define ICVM_NATURALALIGNMENTCHECK32 /* natural alignment to 32 bits */
//#define ICVM_NATURALALIGNMENTCHECK64 /* natural alignment to 64 bits */


//#define MONITORREADWRITES // internel - not supported


// define core machine



// 386 stuff


#ifndef I386

#if defined(M_I386) || defined(i386) || defined(_I386)
#define I386
#endif

#endif


// power pc stuff
#ifndef PPC

#if defined(__PPC__) || defined(mklinux)
#define PPC
#endif

#endif


#ifdef USE3DNOW
#ifndef USEMMX
#define USEMMX
#endif
#endif

#ifndef I386
#undef USEMMX // MMX only available on I386
#undef USE3DNOW // MMX only available on I386
#endif



#ifdef NEEDHWREGS // turn on high speed hardware registers
	// NEEDHWREGS is set by file that use them. DO NOT SET IT YOURSELF!!!

// THESE OPTIONS ARE BOTH MACHINE AND GNU DEPENDANT



#if defined(USEHWREGS)  &&\
	!defined(ICVM_FORCEDALIGNMENTCHECK) && \
	!defined(ICVM_NATURALALIGNMENTCHECK32) \
	  && !defined(ICVM_NATURALALIGNMENTCHECK64)
 

#ifdef __GNUC__ // only available with GNU compilers


#ifdef I386



// ************ I386 definitions for HW registers

// NOTE - these registers are normally preserved across function calls
// in GCC. If you get different results you might change one of these
// to another register normally saved by your compiler across function calls.

#ifdef BIGANDFAST
	register p32 _HW_R_FP asm("%edi");
#define HW_R_FP _HW_R_FP
#endif // BIGANDFAST

	register p32 _HW_R_PC asm("%esi");
#define HW_R_PC _HW_R_PC

#ifdef BIGANDFAST
#define DUMPHWREGS	{\
	_R_FP=_HW_R_FP;\
	_R_PC=_HW_R_PC;\
}
#define LOADHWREGS	{\
	_HW_R_FP=_R_FP;\
	_HW_R_PC=_R_PC;\
}

#else // BIGANDFAST

#define DUMPHWREGS	{\
	_R_PC=_HW_R_PC;\
}
#define LOADHWREGS	{\
	_HW_R_PC=_R_PC;\
}

#endif // BIGANDFAST




#endif // I386

#ifdef PPC


// ************ PPC definitions for HW registers

// NOTE - these registers are normally preserved across function calls
// in GCC. If you get different results you might change one of these
// to another register normally saved by your compiler across function calls.
	register int _HW_R_FP asm("14");
	register int _HW_R_PC asm("15");
	register int _HW_R_R0 asm("16");
	register int _HW_R_R1 asm("17");
	register int _HW_R_R2 asm("18");
	register int _HW_R_R3 asm("19");
	register int _HW_R_SP asm("20");

#define DUMPHWREGS	{\
	_R_FP=_HW_R_FP;\
	_R_SP=_HW_R_SP;\
	_R_PC=_HW_R_PC;\
	_R_R0=_HW_R_R0;\
	_R_R1=_HW_R_R1;\
	_R_R2=_HW_R_R2;\
	_R_R3=_HW_R_R3;\
}

#define LOADHWREGS	{\
	_HW_R_FP=_R_FP;\
	_HW_R_SP=_R_SP;\
	_HW_R_PC=_R_PC;\
	_HW_R_R0=_R_R0;\
	_HW_R_R1=_R_R1;\
	_HW_R_R2=_R_R2;\
	_HW_R_R3=_R_R3;\
}


#define HW_R_FP _HW_R_FP
#define HW_R_SP _HW_R_SP
#define HW_R_PC _HW_R_PC
#define HW_R_R0 _HW_R_R0
#define HW_R_R1 _HW_R_R1
#define HW_R_R2 _HW_R_R2
#define HW_R_R3 _HW_R_R3

#endif // PPC

// NOTE - 68030 or above required
#ifdef M68K

// ************ M68030 definitions for HW registers

// NOTE - these registers are normally preserved across function calls
// in GCC. If you get different results you might change one of these
// to another register normally saved by your compiler across function calls.

	register int _HW_R_FP asm("%a2");
	register int _HW_R_PC asm("%a3");
	register int _HW_R_R0 asm("%d2");
	register int _HW_R_R0 asm("%d3");
	register int _HW_R_R1 asm("%d4");
	register int _HW_R_R2 asm("%a4");
	register int _HW_R_R3 asm("%a5");
	register int _HW_R_SP asm("%a6");

#define DUMPHWREGS	{\
	_R_FP=_HW_R_FP;\
	_R_PC=_HW_R_PC;\
	_R_R0=_HW_R_R0;\
	_R_R1=_HW_R_R1;\
	_R_R2=_HW_R_R2;\
	_R_R3=_HW_R_R3;\
	_R_SP=_HW_R_SP;\
}

#define LOADHWREGS	{\
	_HW_R_FP=_R_FP;\
	_HW_R_PC=_R_PC;\
	_HW_R_R0=_R_R0;\
	_HW_R_R1=_R_R1;\
	_HW_R_R2=_R_R2;\
	_HW_R_R3=_R_R3;\
	_HW_R_SP=_R_SP;\
}


#define HW_R_FP _HW_R_FP
#define HW_R_PC _HW_R_PC
#define HW_R_R0 _HW_R_R0
#define HW_R_R1 _HW_R_R1
#define HW_R_R2 _HW_R_R2
#define HW_R_R3 _HW_R_R3
#define HW_R_SP _HW_R_SP

#endif // 68030


#endif // __GNUC__

#endif // USEHWREGS

#endif // NEEDHWREGS


#ifndef DUMPHWREGS
#define DUMPHWREGS
#endif

#ifndef LOADHWREGS
#define LOADHWREGS
#endif



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>



//***************** SPECIAL NOTES **********************

// ENDIAN:

// do not use #ifdef __BIGENDIAN__ etc, in your programs.
// instead, use #include <machine/endian.h> in your files.
// a macro called ISBIGENDIAN will tell you whether or not
// you are running in big endian mode.
//
// change your code to:
// if(ISBIGENDIAN) ..bigendianstuff... else ....littleendianstuff...
// or
//  (ISBIGENDIAN ? bigendianexpression : littleendianexpression)



//**************** END SPECIAL NOTES ******************************

#ifdef NEEDSHIFTMASK32
#define SHIFT32MASK(x) ((x) & 31)
#else
#define SHIFT32MASK(x) (x)
#endif

#ifdef NEEDSHIFTMASK64
#define SHIFT64MASK(x) ((x) & 63)
#else
#define SHIFT64MASK(x) (x)
#endif



#define I_ABS(x) ((x)>=0 ? (x) : -(x))




// development DEBUG options - development of VM stuff! 



#ifndef BIGENDIAN
#define LITTLEENDIAN
#endif

#ifdef BIGENDIAN
#define ISBIGENDIAN 1
#else
#define ISBIGENDIAN 0
#endif




#ifdef __cplusplus
extern "C" {
#endif




struct _SINFO;

typedef void OpcodeHandler(void);
typedef void VoidOpcodeHandler(void);



int ICVMMain(int argc,char **argv,p32 pc);


typedef struct _SINFO{
	OpcodeHandler *handler;
	s32 offset;
	const u8 *src1map;
	const u8 *src2map;
	const u8 *destmap;
	s32 genericopcode;
	s32 pad2;
	s32 pad3;
} SINFO;

#ifdef __WIN32__
#define sigjmp_buf jmp_buf
#define sigsetjmp(buf,flg) setjmp(buf)
#define siglongjmp(buf,val) longjmp(buf,val)
#endif // __WIN32__

typedef struct i_jmp_buf{
	struct i_jmp_buf *oreturn;
	struct i_jmp_buf *oattnjmp;
	sigjmp_buf buf;
}i_jmp_buf;


extern i_jmp_buf *icvmattnjmp;
extern i_jmp_buf *returnjmpbuf;

extern OpcodeHandler *icvmopcodehandlers[65536];
extern VoidOpcodeHandler * const shortfunctions[65536];

extern SINFO const sinfo[]; // sinfo structs for opcodes
extern int const sinfosize; // number of sinfo structs present at sinfo
extern u16 const sopcodetable[];


// register mapping macros

#ifdef 	HW_R_FP
#define ISHW_R_FP	1
#define R_FP 	HW_R_FP
#else
#define ISHW_R_FP	0
#define R_FP 	_R_FP
#endif

#ifdef HW_R_SP
#define ISHW_R_SP	1
#define R_SP 	HW_R_SP
#else
#define ISHW_R_SP	0
#define R_SP 	_R_SP
#endif

#ifdef HW_R_PC
#define ISHW_R_PC	1
#define R_PC 	HW_R_PC
#else
#define ISHW_R_SP	0
#define R_PC 	_R_PC
#endif


#ifdef HW_R_R0
#define ISHW_R_R0	1
#define R_R0 	HW_R_R0
#else
#define R_R0 	_R_R0
#define ISHW_R_R0	0
#endif

#ifdef HW_R_R1
#define ISHW_R_R1	1
#define R_R1 	HW_R_R1
#else
#define ISHW_R_R1	0
#define R_R1 	_R_R1
#endif

#ifdef HW_R_R2
#define ISHW_R_R2	1
#define R_R2 	HW_R_R2
#else
#define ISHW_R_R2	0
#define R_R2 	_R_R2
#endif

#ifdef HW_R_R3
#define ISHW_R_R3	1
#define R_R3 	HW_R_R3
#else
#define ISHW_R_R3	0
#define R_R3 	_R_R3
#endif

#ifdef HW_R_Q0
#define ISHW_R_Q0	1
#define R_Q0 	HW_R_Q0
#else
#define ISHW_R_Q0	0
#define R_Q0 	_R_Q0
#endif

#ifdef HW_R_Q1
#define ISHW_R_Q1	1
#define R_Q1 	HW_R_Q1
#else
#define ISHW_R_Q1	0
#define R_Q1 	_R_Q1
#endif

#define ISHW_R_FP0	0
#define R_FP0 	_R_FP0
#define ISHW_R_FP1	0
#define R_FP1 	_R_FP1
#define ISHW_R_FP2	0
#define R_FP2 	_R_FP2

#define R_LASTPC _R_LASTPC
#define R_LASTSP _R_LASTSP
#define ISHW_R_LASTPC	0

// macro to determine if an ICVM  register is mapped onto actual  hardware.
// use this in #if statments to determine how you may optimize

#define IsRegisterHardwareMapped(reg) ISHW_##reg




typedef struct ICVMMachine{
	p32 r_PC;
	p32	r_FP;
	p32 r_LASTPC;
	p32	r_SP;
	p32	r_LASTSP;
	g32	r_R0;
	g32	r_R1;
	g32	r_R2;
	g32	r_R3;
	g256 r_Q0;
	g256 r_Q1;
	g64	r_FP0;
	g64	r_FP1;
	g64	r_FP2;
	s32 r_scratch;
#ifndef BIGANDFAST
// temporary storage for opcode to avoid pushing this
	SINFO *r_sinfo;
	s32 r_soffset;
#define R_sinfo		(R_LM->r_sinfo)
#define R_soffset	(R_LM->r_soffset)
#define R_src1map	 R_sinfo->src1map[R_soffset]
#define R_src2map	 R_sinfo->src2map[R_soffset]
#define R_destmap	 R_sinfo->destmap[R_soffset]
#endif

#ifdef MONITORREADWRITES
	s32 r_rmtp,r_wmtp;
	p32 r_rmaddr,r_wmaddr;

	union R_MONITOR{
		s8	ms8;
		u8	mu8;
		s16	ms16;
		u16	mu16;
		s32	ms32;
		u32	mu32;
		s64	ms64;
		u64	mu64;
		f32	mf32;
		f64	mf64;
		g8	mg8;
		g16	mg16;
		g32	mg32;
		g64	mg64;
	}r_rm,r_wm;


#define R_rm		(R_LM->r_rm)
#define R_wm		(R_LM->r_wm)
#define R_rmtp		(R_LM->r_rmtp)
#define R_rmaddr	(R_LM->r_rmaddr)
#define R_rms8		(R_LM->r_rm.ms8)
#define R_rmu8		(R_LM->r_rm.mu8)
#define R_rms16		(R_LM->r_rm.ms16)
#define R_rmu16		(R_LM->r_rm.mu16)
#define R_rms32		(R_LM->r_rm.ms32)
#define R_rmu32		(R_LM->r_rm.mu32)
#define R_rms64		(R_LM->r_rm.ms64)
#define R_rmu64		(R_LM->r_rm.mu64)
#define R_rmf32		(R_LM->r_rm.mf32)
#define R_rmf64		(R_LM->r_rm.mf64)

#define R_rmg8		(R_LM->r_rm.mg8)
#define R_rmg16		(R_LM->r_rm.mg16)
#define R_rmg32		(R_LM->r_rm.mg32)
#define R_rmg64		(R_LM->r_rm.mg64)



#define R_wmtp		(R_LM->r_wmtp)
#define R_wmaddr	(R_LM->r_wmaddr)

#define R_wms8		(R_LM->r_wm.ms8)
#define R_wmu8		(R_LM->r_wm.mu8)
#define R_wms16		(R_LM->r_wm.ms16)
#define R_wmu16		(R_LM->r_wm.mu16)
#define R_wms32		(R_LM->r_wm.ms32)
#define R_wmu32		(R_LM->r_wm.mu32)
#define R_wms64		(R_LM->r_wm.ms64)
#define R_wmu64		(R_LM->r_wm.mu64)
#define R_wmf32		(R_LM->r_wm.mf32)
#define R_wmf64		(R_LM->r_wm.mf64)

#define R_wmg8		(R_LM->r_wm.mg8)
#define R_wmg16		(R_LM->r_wm.mg16)
#define R_wmg32		(R_LM->r_wm.mg32)
#define R_wmg64		(R_LM->r_wm.mg64)



#endif // MONITORREADWRITES


}ICVMMachine;


// *********** mapping of local machine register -- R_LM

#if	defined(__GNUC__) && defined(HW_LM) // only available with GNU compilers
#ifdef I386



static inline struct ICVMMachine *i_get_lm(void){
	register struct ICVMMachine *s asm("%ebx"); 
	return s;
};

static inline void i_set_lm(struct ICVMMachine *t){
	register struct ICVMMachine *s asm("%ebx"); 
	s=t;
	s->r_scratch=0; // to keep from optimizing out
};

#define SET_LM(x) i_set_lm(x)
#define R_LM i_get_lm() 



#define I_SWAP_u16(x) \
     (__extension__	\
      ({ register u16 v;\
	   __asm__ __volatile__ (\
		"rorw $8, %w0"\
		: "=r" (v)\
		: "0" ((u16) (x)));\
	 v; }))

#define I_SWAP_u32(x) \
     (__extension__	\
      ({ register u32 v;\
	   __asm__ __volatile__ (\
				"rorw $8, %w0;"\
				"rorl $16, %0;"\
				"rorw $8, %w0"\
				: "=r" (v)\
				: "0" ((u32) (x)));\
	 v; }))

static inline u64 _I_SWAP_u64(u64 x){
	return ((u64)(I_SWAP_u32((u32)x)) <<32) |
		(u64) I_SWAP_u32((u32)(x>>32));
};

#define I_SWAP_u64	_I_SWAP_u64


#endif // I386
#endif // __GNUC__


#ifndef R_LM

#define ISHW_R_LM	0

extern struct ICVMMachine *_R_LM;
#define SET_LM(x) _R_LM=x;
#define R_LM _R_LM 
#else
#define ISHW_R_LM	1
#endif


// ****************************************************
// layout of frame - register mapping etc. done in frame space
// these are offsets from frame pointer
// ****************************************************

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


// physical register space
// how this is mapped in LocalMachine memory and frame pointer memory	
// *** basically r1,r2,r3,fp1,fp2, and q1 are frame based registers
// allowing them to be used locally as permanent registers without
// saving them.

// r0,fp0,q0, sp, pc, and fp are all permanent registers, not frame based

#define _R_PC		(R_LM->r_PC)
#define _R_FP		(R_LM->r_FP)
#define _R_LASTPC	(R_LM->r_LASTPC)
#define _R_LASTSP	(R_LM->r_LASTSP)
#define _R_SP		(R_LM->r_SP)
#define _R_R0		(R_LM->r_R0.l[0])
//#define _R_R1		(R_LM->r_R1.l[0])
//#define _R_R2		(R_LM->r_R2.l[0])
//#define _R_R3		(R_LM->r_R3.l[0])
#define _R_R1		(gR_R1.l[0])
#define _R_R2		(gR_R2.l[0])
#define _R_R3		(gR_R3.l[0])
#define _R_Q0		(R_LM->r_Q0.q[0])
//#define _R_Q1		(R_LM->r_Q1.q[0])
#define _R_Q1		(gR_Q1.q[0])
#define _R_FP0		(R_LM->r_FP0.d[0])
//#define _R_FP1		(R_LM->r_FP1.d[0])
#define _R_FP1		(gR_FP1.d[0])
//#define _R_FP2		(R_LM->r_FP2.d[0])
#define _R_FP2		(gR_FP2.d[0])

#define R_ERRNO1	(*((s32 *)Nptr(R_FP-116)))
#define R_ERRNO2	(*((s32 *)Nptr(R_FP-120)))



#define gR_PC		(R_LM->r_PC)
#define gR_FP		(R_LM->r_FP)
#define gR_LASTPC	(R_LM->r_LASTPC)
#define gR_LASTSP	(R_LM->r_LASTSP)
#define gR_SP		(R_LM->r_SP)
#define gR_R0		(R_LM->r_R0)
//#define gR_R1		(R_LM->r_R1)
//#define gR_R2		(R_LM->r_R2)
//#define gR_R3		(R_LM->r_R3)
#define gR_R1		(*((g32 *)Nptr(R_FP+rSH_r1)))
#define gR_R2		(*((g32 *)Nptr(R_FP+rSH_r2)))
#define gR_R3		(*((g32 *)Nptr(R_FP+rSH_r3)))
#define gR_Q0		(R_LM->r_Q0)
//#define gR_Q1		(R_LM->r_Q1)
#define gR_Q1		(*((g256 *)Nptr(R_FP+rSH_q1)))
#define gR_FP0		(R_LM->r_FP0)
//#define gR_FP1		(R_LM->r_FP1)
#define gR_FP1		(*((g64 *)Nptr(R_FP+rSH_fp1)))
//#define gR_FP2		(R_LM->r_FP2)
#define gR_FP2		(*((g64 *)Nptr(R_FP+rSH_fp2)))


// stuff to copy stuff back and forth between frame transitions

// copy global pointer to new frame
#define iNEWFRAME(oldf) { WDs32(R_FP+rSH_globalptr,RDs32(oldf+rSH_globalptr,0),0);}

#define iDUMPFRAME(oldf) 


#ifdef NOT_NOW
// copy returns from old frame
#define iDUMPFRAME(oldf) {\
	 WDs64(R_FP+GP_qreturn,RDs64(oldf+GP_qreturn,0),0);\
	 WDs64(R_FP+GP_qreturn,RDs64(oldf+GP_qreturn,1),1);\
	 WDs64(R_FP+GP_qreturn,RDs64(oldf+GP_qreturn,2),2);\
	 WDs64(R_FP+GP_qreturn,RDs64(oldf+GP_qreturn,3),3);\
	 WDs64(R_FP+GP_fpreturn,RDs64(oldf+GP_fpreturn,0),0);\
	 WDs32(R_FP+GP_ireturn,RDs32(oldf+GP_ireturn,0),0);\
}
#endif


// returning 64 bit long long  values to caller
#define iRETURNs64(what)	wQ0s64(what) 

// returning 32 bit integer values to caller
#define iRETURNs32(what) 	wQ0s32(what)

// returning 32 bit float  values to caller
#define iRETURNf32(what)    wFP0f32(what)

// returning 64 bit float  values to caller
#define iRETURNf64(what) 	wFP0f64(what)



#ifdef __WIN32__

#define fmodf fmod

#define powf pow
#define atan2f atan2
#define logf log
#define log10f log10
#define expf exp
#define sinf sin
#define cosf cos
#define tanf tan
#define asinf asin
#define acosf acos
#define atanf atan
#define sqrtf sqrt
#define fmodf fmod
#define fabsf fabs

/* missing in win95 ming */

#define iInterlockedIncrement(plong) (++(*(plong)))
#define iInterlockedDecrement(plong) (--(*(plong)))

static inline PVOID iInterlockedCompareExchange(
		PVOID *dest,PVOID exg,PVOID cmp){
	volatile PVOID *d=(volatile PVOID *)dest;
	PVOID old;
	if((old=*d)==cmp)
		*d=exg;
	return old;
};


// syncronized and
static inline void SyncAndValue(volatile PVOID *where,s32 mask){
	for(;;){
		s32 x=(s32)*where;
		s32 m=x&mask;
		s32 y;
		if(x==m)
			break;
		y=(s32) iInterlockedCompareExchange((PVOID *)where,(PVOID)m,(PVOID)x);
		if(x==y)
			break;
//		Sleep(0);
	}
};

/*** NOTE - this code is probably not valid -- need to be replaced
 with a machine inst that can do the exchange in one instruction 

In future releases this should be changed to call the NT kernel version.
It is not present in Win95, so this is the current implementation, until
all Win95/98/ME garbage is gone. Interestingly, it IS present in Win98
and probably WinME also

*/

static inline LONG iInterlockedExchangeAdd(PLONG paddr,LONG inc){
	LONG old=*paddr;
	*paddr+=inc;
	return old;
}

// syncronized or
static inline void SyncOrValue(volatile PVOID *where,s32 mask){
	for(;;){
		s32 x=(s32)*where;
		s32 m=x|mask;
		s32 y;
		if(x==m)
			break;
		y=(s32) iInterlockedCompareExchange((PVOID *)where,(PVOID)m,(PVOID)x);
		if(x==y)
			break;
//		Sleep(0);
	}
};


// syncronized set attempt - return 1 if successful, 0 if not
// the dest must be equal to compare for this to work
static inline s32 TrySyncSetValue(volatile PVOID *where,s32 value,s32 compare){
		s32 x=(s32)*where;
		s32 y=(s32) iInterlockedCompareExchange((PVOID *)where,
					(PVOID) value,(PVOID)compare);
//		fprintf(stderr,"x=%d y=%d\n",x,y);
		if(x==y)
			return 1;
		return 0;
};




#define ICVM_SETATTN(flag) SyncOrValue(icvmattnptr,flag)
#define ICVM_CLEARATTN(flag) SyncAndValue(icvmattnptr,~flag)

#define ICVMATTN ((s32)(*icvmattnptr))
typedef PVOID ICVMATTNTYPE;


#else // __WIN32__

typedef u32 ICVMATTNTYPE;
#define ICVMATTN (icvmattn)
#define ICVM_SETATTN(flag) (ICVMATTN|=flag)
#define ICVM_CLEARATTN(flag) (ICVMATTN&=~(flag))

#endif // !__WIN32__


#define ATTNLONGJMP siglongjmp(icvmattnjmp->buf,0)
#define RETURNLONGJMP siglongjmp(returnjmpbuf->buf,0)


#define ICVM_SETBREAK(flag) {ICVM_SETATTN(flag);ATTNLONGJMP;}
#define ICVM_SETRETURN(flag) {ICVM_SETATTN(flag);RETURNLONGJMP;}

extern volatile  ICVMATTNTYPE icvmattn; // attention flags
extern volatile  ICVMATTNTYPE *icvmattnptr; // attention flags ptr

// use these macro to flag attention to the loop process, or clear it

// cpu release and hold - for our primitive multithreading implementation

#define HOLDCPU
#define RELEASECPU


// force a break
#define ICVM_BREAK			(1<<0)

// is an exit
#define ICVM_EXIT			(1<<1)

// check  debug
#define ICVM_DEBUG			(1<<2)

// OS signals
#define ICVM_SIGNAL			(1<<3)

// so that we may call back to ICVM code
#define ICVM_ZERORETURN		(1<<4)


typedef void SysCallFunction(void);

#if defined(ICVM_FORCEDALIGNMENTCHECK) || \
	defined(ICVM_NATURALALIGNMENTCHECK32) ||\
	defined(ICVM_NATURALALIGNMENTCHECK64)
		p32 icvm_checkalign(p32 addr,int align);
#endif


#ifdef ICVM_FORCEDALIGNMENTCHECK
#define I_CKALGN(addr,align) icvm_checkalign((p32)(addr),align)
#else
#define I_CKALGN(addr,algn) (addr)
#endif

#ifdef ICVM_NATURALALIGNMENTCHECK32
#define I_CKNALGN(addr,align)\
	 (align> 1 ? icvm_checkalign((p32)(addr),align<=4 ? align : 4):(p32)(addr))
#else

#ifdef ICVM_NATURALALIGNMENTCHECK64
#define I_CKNALGN(addr,align)\
	 (align> 1 ? icvm_checkalign((p32)(addr),align):(p32)(addr))
#else
#define I_CKNALGN(addr,algn) (addr)
#endif

#endif

// ******** hooks for any wierd pointer conversions *******

// this macro converts a 32 bit ICVM pointer (p32,s32,u32) 
// to a native void pointer

#define Nptr(p) ((void *)(p))

// this macro converts a native pointer of any kind to
// a 32 bit ICVM pointer (p32)

#define Iptr(p) ((p32)(void *)(p)) 

// these macros read and write to/from data memory only
// data memory is  aligned to 32 bits

							
//#define doREADDATAMEM(tp,ptr) 		(*((tp *)(void *)(I_CKNALGN(ptr,sizeof(tp)))))
//#define doWRITEDATAMEM(tp,ptr,what) 	(*((tp *)(void *)(I_CKNALGN(ptr,sizeof(tp))))=(tp)(what))

#ifdef MONITORREADWRITES
// INTERNAL DEBUGGING - NOT SUPPORTED !!!	
			
#define _doREADDATAMEM(tp,ptr) 			(*((tp *)(I_CKNALGN(ptr,sizeof(tp)))))
#define _doWRITEDATAMEM(tp,ptr,what) 	(*((tp *)(I_CKNALGN(ptr,sizeof(tp))))=(tp)(what))

p32 m_rd(int sz,p32 addr);
p32  m_wr(int sz,p32 addr);
void  m_startwr(void);
void  m_startrd(void);
void m_finishwr(void);


#define doREADDATAMEM(tp,ptr) (\
	 m_startrd(),\
	 (*((tp *)m_rd(sizeof(tp),(p32)(ptr))))\
)

#define doWRITEDATAMEM(tp,ptr,what) (\
	 m_startwr(),\
	 (*((tp *)m_wr(sizeof(tp),(p32)(ptr)))=(tp)(what)),\
	 m_finishwr()\
)

#define doWRITECODEMEM  _doWRITEDATAMEM
#define doREADCODEMEM 	_doREADDATAMEM


#else // MONITOREADWRITES

#define doREADDATAMEM(tp,ptr) 			(*((tp *)(I_CKNALGN(ptr,sizeof(tp)))))
#define doWRITEDATAMEM(tp,ptr,what) 	(*((tp *)(I_CKNALGN(ptr,sizeof(tp))))=(tp)(what))

#endif // MONITORREADWRITES

// these macros read/write from code memory only
// code memory is aligned to 16 bits
// NOTE - puts in code mem are only allowed during load and relocation

#ifndef doREADCODEMEM
#define doREADCODEMEM doREADDATAMEM
#endif

#ifndef doWRITECODEMEM
#define doWRITECODEMEM doWRITEDATAMEM
#endif



// these macros read and write to/from data memory only
// any alignment/endian adjustments should be made in these macros

#define RDs8(ptr,n) 	doREADDATAMEM(s8,(ptr)+(n))
#define RDu8(ptr,n)		doREADDATAMEM(u8,(ptr)+(n))
#define RDs16(ptr,n)	doREADDATAMEM(s16,I_CKALGN((ptr)+(n*2),2))
#define RDu16(ptr,n)	doREADDATAMEM(u16,I_CKALGN((ptr)+(n*2),2))
#define RDs32(ptr,n)	doREADDATAMEM(s32,I_CKALGN((ptr)+(n*4),4))
#define RDu32(ptr,n)	doREADDATAMEM(u32,I_CKALGN((ptr)+(n*4),4))
#define RDs64(ptr,n)	doREADDATAMEM(s64,I_CKALGN((ptr)+(n*8),4))
#define RDu64(ptr,n)	doREADDATAMEM(u64,I_CKALGN((ptr)+(n*8),4))
#define RDf32(ptr,n)	doREADDATAMEM(f32,I_CKALGN((ptr)+(n*4),4))
#define RDf64(ptr,n)	doREADDATAMEM(f64,I_CKALGN((ptr)+(n*8),4))


// generic optimizing types
// direct access to byte members

#define RDg8(ptr,n)		doREADDATAMEM(g8,(ptr)+(n))
#define RDg16(ptr,n)	doREADDATAMEM(g16,I_CKALGN((ptr)+(n*2),2))
#define RDg32(ptr,n)	doREADDATAMEM(g32,I_CKALGN((ptr)+(n*4),4))
#define RDg64(ptr,n)	doREADDATAMEM(g64,I_CKALGN((ptr)+(n*8),4))


#define WDs8(ptr,what,n)	doWRITEDATAMEM(s8,(ptr)+(n),what)
#define WDu8(ptr,what,n)	doWRITEDATAMEM(u8,(ptr)+(n),what)
#define WDs16(ptr,what,n)	doWRITEDATAMEM(s16,I_CKALGN((ptr)+(n*2),2),what)
#define WDu16(ptr,what,n)	doWRITEDATAMEM(u16,I_CKALGN((ptr)+(n*2),2),what)
#define WDs32(ptr,what,n)	doWRITEDATAMEM(s32,I_CKALGN((ptr)+(n*4),4),what)
#define WDu32(ptr,what,n)	doWRITEDATAMEM(u32,I_CKALGN((ptr)+(n*4),4),what)
#define WDs64(ptr,what,n)	doWRITEDATAMEM(s64,I_CKALGN((ptr)+(n*8),4),what)
#define WDu64(ptr,what,n)	doWRITEDATAMEM(u64,I_CKALGN((ptr)+(n*8),4),what)
#define WDf32(ptr,what,n)	doWRITEDATAMEM(f32,I_CKALGN((ptr)+(n*4),4),what)
#define WDf64(ptr,what,n)	doWRITEDATAMEM(f64,I_CKALGN((ptr)+(n*8),4),what)

// these macros read and write to/from code memory only
// any alignment/endian adjustments should be made in these macros

#define RCs8(ptr,n)			doREADCODEMEM(s8,(ptr)+(n))
#define RCu8(ptr,n)			doREADCODEMEM(u8,(ptr)+(n))
#define RCs16(ptr,n)		doREADCODEMEM(s16,I_CKALGN((ptr)+(n*2),2))
#define RCu16(ptr,n)		doREADCODEMEM(u16,I_CKALGN((ptr)+(n*2),2))
#define RCs32(ptr,n)		doREADCODEMEM(s32,I_CKALGN((ptr)+(n*4),2))
#define RCu32(ptr,n)		doREADCODEMEM(u32,I_CKALGN((ptr)+(n*4),2))
#define RCs64(ptr,n)		doREADCODEMEM(s64,I_CKALGN((ptr)+(n*8),2))
#define RCu64(ptr,n)		doREADCODEMEM(u64,I_CKALGN((ptr)+(n*8),2))
#define RCf32(ptr,n)		doREADCODEMEM(f32,I_CKALGN((ptr)+(n*4),2))
#define RCf64(ptr,n)		doREADCODEMEM(f64,I_CKALGN((ptr)+(n*8),2))


// generic optimizing types
// direct access to byte members

#define RCg8(ptr,n)			doREADDATAMEM(g8,(ptr)+(n))
#define RCg16(ptr,n)		doREADDATAMEM(g16,I_CKALGN((ptr)+(n*2),2))
#define RCg32(ptr,n)		doREADDATAMEM(g32,I_CKALGN((ptr)+(n*4),4))
#define RCg64(ptr,n)		doREADDATAMEM(g64,I_CKALGN((ptr)+(n*8),4))



#define WCs8(ptr,what,n)	doWRITECODEMEM(s8,(ptr)+(n),what)
#define WCu8(ptr,what,n)	doWRITECODEMEM(u8,(ptr)+(n),what)
#define WCs16(ptr,what,n)	doWRITECODEMEM(s16,I_CKALGN((ptr)+(n*2),2),what)
#define WCu16(ptr,what,n)	doWRITECODEMEM(u16,I_CKALGN((ptr)+(n*2),2),what)
#define WCs32(ptr,what,n)	doWRITECODEMEM(s32,I_CKALGN((ptr)+(n*4),2),what)
#define WCu32(ptr,what,n)	doWRITECODEMEM(u32,I_CKALGN((ptr)+(n*4),2),what)
#define WCs64(ptr,what,n)	doWRITECODEMEM(s64,I_CKALGN((ptr)+(n*8),2),what)
#define WCu64(ptr,what,n)	doWRITECODEMEM(u64,I_CKALGN((ptr)+(n*8),2),what)
#define WCf32(ptr,what,n)	doWRITECODEMEM(f32,I_CKALGN((ptr)+(n*4),2),what)
#define WCf64(ptr,what,n)	doWRITECODEMEM(f64,I_CKALGN((ptr)+(n*8),2),what)


// 64 bit alignment macros and inlines

#if defined(ICVM_NEEDALIGN32) || defined(ICVM_NEEDALIGN64)

#undef RCu32
#undef RCs32
#undef RCu64
#undef RCs64
#undef RCf32
#undef RCf64

#undef WCu32
#undef WCs32
#undef WCu64
#undef WCs64
#undef WCf32
#undef WCf64

#define RCu32(ptr,n) 		I_GetCU32((p32)(ptr)+(n*4))
#define RCs32(ptr,n) 		((s32)RCu32(ptr)+(n*4))
#define RCu64(ptr,n) 		I_GetCU64((p32)(ptr)+(n*8))
#define RCs64(ptr,n) 		((s64)RCu64(ptr)+(n*8))
#define RCf32(ptr,n) 		I_GetCF32((p32)(ptr)+(n*4))
#define RCf64(ptr,n) 		I_GetCF64((p32)(ptr)+(n*8))

#define WCu32(ptr,what,n) 	I_PutCU32((p32)(ptr)+(n*4),what)
#define WCs32(ptr,what,n) 	WCu32(ptr+(n*4),(u32)(what))
#define WCu64(ptr,what,n) 	I_PutCU64((p32)(ptr)+(n*8),what)
#define WCs64(ptr,what,n) 	WCu64(ptr+(n*8),(u64)(what))
#define WCf32(ptr,what,n) 	I_PutCF32((p32)(ptr)+(n*4),what)
#define WCf64(ptr,what,n) 	I_PutCF64((p32)(ptr)+(n*8),what)



static inline u32 I_GetCU32(p32 ptr){
#ifdef BIGENDIAN
	return ( ((u32)RCu16(ptr)) << 16) + (u32) RCu16(ptr+2);
#else
	return (u32) RCu16(ptr) + ( ((u32)RCu16(ptr+2)) << 16);
#endif
}

static inline f32 I_GetCF32(p32 ptr){
	union{
		u32 i;
		f32 f;
	}x;
	x.i=RCu32(ptr);
	return x.f;
}


static inline u64 I_GetCU64(p32 ptr){
#ifdef BIGENDIAN
	return ( ((u64)RCu32(ptr)) << 32) + (u64) RCu32(ptr+4);
#else
	return (u64) RCu32(ptr) + ( ((u64)RCu32(ptr+4)) << 32);
#endif
}


static inline f64 I_GetCF64(p32 ptr){
	union{
		u64 i;
		f64 f;
	}x;
	x.i=RCu64(ptr);
	return x.f;
}

static inline void I_PutCU32(p32 ptr,u32 what){
#ifdef BIGENDIAN
	WCu16(ptr,what>>16);
	WCu16(ptr+2,what);
#else
	WCu16(ptr,what);
	WCu16(ptr+2,what>>16);
#endif
}

static inline void I_PutCU64(p32 ptr,u64 what){
#ifdef BIGENDIAN
	WCu32(ptr,what>>32);
	WCu32(ptr+4,what);
#else
	WCu32(ptr,what);
	WCu32(ptr+4,what>>32);
#endif
}

static inline void I_PutCF32(p32 ptr,f32 what){
	union{
		u32 i;
		f32 f;
	}x;
	x.f=what;
	WCu32(ptr,x.i);
}

static inline void I_PutCF64(p32 ptr,f64 what){
	union{
		u64 i;
		f64 f;
	}x;
	x.f=what;
	WCu64(ptr,x.i);
}




#endif


// **** these macros are support for old macros

#define GETDATAMEM_s8	RDs8
#define GETDATAMEM_u8	RDu8
#define GETDATAMEM_s16	RDs16
#define GETDATAMEM_u16	RDu16
#define GETDATAMEM_s32	RDs32
#define GETDATAMEM_u32	RDu32
#define GETDATAMEM_s64	RDs64
#define GETDATAMEM_u64	RDu64
#define GETDATAMEM_f32	RDf32
#define GETDATAMEM_f64	RDf64

#define PUTDATAMEM_s8	WDs8
#define PUTDATAMEM_u8	WDu8
#define PUTDATAMEM_s16	WDs16

#define PUTDATAMEM_u16	WDu16
#define PUTDATAMEM_s32	WDs32
#define PUTDATAMEM_u32	WDu32
#define PUTDATAMEM_s64	WDs64
#define PUTDATAMEM_u64	WDu64
#define PUTDATAMEM_f32	WDf32
#define PUTDATAMEM_f64	WDf64


#define GETCODEMEM_s8	RCs8
#define GETCODEMEM_u8	RCu8
#define GETCODEMEM_s16	RCs16
#define GETCODEMEM_u16	RCu16
#define GETCODEMEM_s32	RCs32
#define GETCODEMEM_u32	RCu32
#define GETCODEMEM_s64	RCs64
#define GETCODEMEM_u64	RCu64
#define GETCODEMEM_f32	RCf32
#define GETCODEMEM_f64	RCf64

#define PUTCODEMEM_s8	WCs8
#define PUTCODEMEM_u8	WCu8
#define PUTCODEMEM_s16	WCs16
#define PUTCODEMEM_u16	WCu16
#define PUTCODEMEM_s32	WCs32
#define PUTCODEMEM_u32	WCu32
#define PUTCODEMEM_s64	WCs64
#define PUTCODEMEM_u64	WCu64
#define PUTCODEMEM_f32	WCf32
#define PUTCODEMEM_f64	WCf64

/* macros to read from the stack */

#define iSTACKALIGN		8

#define rSTKs8(n) 		RDs8(R_SP+((n)*iSTACKALIGN),0)
#define rSTKu8(n) 		RDu8(R_SP+((n)*iSTACKALIGN),0)
#define rSTKs16(n) 		RDs16(R_SP+((n)*iSTACKALIGN),0)
#define rSTKu16(n) 		RDu16(R_SP+((n)*iSTACKALIGN),0)
#define rSTKs32(n) 		RDs32(R_SP+((n)*iSTACKALIGN),0)
#define rSTKu32(n) 		RDu32(R_SP+((n)*iSTACKALIGN),0)
#define rSTKs64(n) 		RDs64(R_SP+((n)*iSTACKALIGN),0)
#define rSTKu64(n) 		RDu64(R_SP+((n)*iSTACKALIGN),0)
#define rSTKf32(n) 		RDf32(R_SP+((n)*iSTACKALIGN),0)
#define rSTKf64(n) 		RDf64(R_SP+((n)*iSTACKALIGN),0)

/* pushes for call backs made from native code into ICVM code */
/* when calling a ICVM function from native code, you need to push the args
   in reverse order. */
/* NOTE - bytes and words not supported here -- use the integer calls which 
will perform the correct conversion before handing them to ICVM */


#define iPUSHs32(n)	(R_SP-=iSTACKALIGN,WDs32(R_SP,n,0))
#define iPUSHu32(n)	(R_SP-=iSTACKALIGN,WDu32(R_SP,n,0))
#define iPUSHs64(n)	(R_SP-=iSTACKALIGN,WDs64(R_SP,n,0))
#define iPUSHu64(n)	(R_SP-=iSTACKALIGN,WDu64(R_SP,n,0))
#define iPUSHf32(n)	(R_SP-=iSTACKALIGN,WDf32(R_SP,n,0))
#define iPUSHf64(n)	(R_SP-=iSTACKALIGN,WDf64(R_SP,n,0))

/* pushes zero return which to ICVM causes the main loop to return to native
   caller */
#define iPUSHNULLRETURN iPUSHs32(0)


// bit swapping macros
// used in ICVM bswap implementation

#ifndef I_SWAP_u16
#define I_SWAP_u16(x) ((((x)>>8) & 0xff) | (((x)<<8) & 0xff00))
#endif

#ifndef I_SWAP_u32
#define I_SWAP_u32(x) (\
	 (((x)>>24) &0xff) |   (((x)<<24) & 0xff0000) | \
	 (((x)>>8) & 0xff00) | (((x)<<8) & 0xff0000)\
 )
#endif

#ifndef I_SWAP_u64

#define I_SWAP_u64(x) ( \
	(((x)>>56) &  (u64) 0xff) |  (((x)<<56) & 	(((u64)0xff)<<56)) | \
	(((x)>>40) & (u64)0xff00) |  (((x)<<40) &    (((u64)0xff)<<48)) |\
	(((x)>>24) & (u64)0xff0000) |  (((x)<<24) &  (((u64)0xff)<<40)) |\
	(((x)>>8) &  (((u64)0xff))<<24) |  (((x)<<8) & (((u64)0xff)<<32))\
)

#endif

#define I_SWAP_s16(x) I_SWAP_u16(((u16)(x)))
#define I_SWAP_s32(x) I_SWAP_u32(((u32)(x)))
#define I_SWAP_s64(x) I_SWAP_u64(((u64)(x)))

#ifdef CHECKILLEGALREADWRITES

// diagnostics -- just for my own use in writing the messy macros below 

#define ILLEGAL_READ	i_illegalread()
#define ILLEGAL_WRITE	i_illegalwrite()

extern int i_illegalread(void);
extern int i_illegalwrite(void);

#else

#define ILLEGAL_READ	0
#define ILLEGAL_WRITE


#endif//  CHECKILLEGALREADWRITES



// **************************************************************
// ************** SPECIALIZED icvmsup SUPPORT *******************
// **************************************************************



// empty string meaning "No Pointer". Used for saveptr in many cases
#define nP


// read next immediate from code

// effective address

#define EAi(pcoff)	(R_PC+pcoff)

#define iu8(pcoff)	((u8)RCu16(EAi(pcoff),0)) 
#define iu16(pcoff)	RCu16(EAi(pcoff),0)
#define iu32(pcoff)	RCu32(EAi(pcoff),0)
#define iu64(pcoff)	RCu64(EAi(pcoff),0) 
#define is8(pcoff)	((s8)RCs16(EAi(pcoff),0)) 
#define is16(pcoff)	RCs16(EAi(pcoff),0)
#define is32(pcoff)	RCs32(EAi(pcoff),0)
#define is64(pcoff)	RCs64(EAi(pcoff),0) 
#define if32(pcoff)	RCf32(EAi(pcoff),0) 
#define if64(pcoff)	RCf64(EAi(pcoff),0) 

#define ig8(pcoff)	(RCg16(EAi(pcoff),0).gb[I_LOW]) 
#define ig16(pcoff)	RCg16(EAi(pcoff),0)
#define ig32(pcoff)	RCg32(EAi(pcoff),0)
#define ig64(pcoff)	RCg64(EAi(pcoff),0) 


// reads and writes from/to absolute address

// effective address

#define EAa(pcoff)				(is32(pcoff))


#define rau8(pcoff,saveptr,n)	RDu8(saveptr EAa(pcoff),n)
#define rau16(pcoff,saveptr,n)	RDu16(saveptr EAa(pcoff),n)
#define rau32(pcoff,saveptr,n)	RDu32(saveptr EAa(pcoff),n)
#define rau64(pcoff,saveptr,n)	RDu64(saveptr EAa(pcoff),n)
#define ras8(pcoff,saveptr,n)	RDs8(saveptr EAa(pcoff),n)
#define ras16(pcoff,saveptr,n)	RDs16(saveptr EAa(pcoff),n)
#define ras32(pcoff,saveptr,n)	RDs32(saveptr EAa(pcoff),n)
#define ras64(pcoff,saveptr,n)	RDs64(saveptr EAa(pcoff),n)
#define raf32(pcoff,saveptr,n)	RDf32(saveptr EAa(pcoff),n)
#define raf64(pcoff,saveptr,n)	RDf64(saveptr EAa(pcoff),n)

#define rag8(pcoff,saveptr,n)	RDg8(saveptr EAa(pcoff),n)
#define rag16(pcoff,saveptr,n)	RDg16(saveptr EAa(pcoff),n)
#define rag32(pcoff,saveptr,n)	RDg32(saveptr EAa(pcoff),n)
#define rag64(pcoff,saveptr,n)	RDg64(saveptr EAa(pcoff),n)

#define wau8(pcoff,what,n)	WDu8(EAa(pcoff),what,n)
#define wau16(pcoff,what,n)	WDu16(EAa(pcoff),what,n)
#define wau32(pcoff,what,n)	WDu32(EAa(pcoff),what,n)
#define wau64(pcoff,what,n)	WDu64(EAa(pcoff),what,n)
#define was8(pcoff,what,n)	WDs8(EAa(pcoff),what,n)
#define was16(pcoff,what,n)	WDs16(EAa(pcoff),what,n)
#define was32(pcoff,what,n)	WDs32(EAa(pcoff),what,n)
#define was64(pcoff,what,n)	WDs64(EAa(pcoff),what,n)
#define waf32(pcoff,what,n)	WDf32(EAa(pcoff),what,n)
#define waf64(pcoff,what,n)	WDf64(EAa(pcoff),what,n)



// reads and writes from/to relative address
// THIS ONE USES LASTPC for base

// effective address

#define EArL(pcoff)	(R_LASTPC+(is32(pcoff)))


#define rru8L(pcoff,saveptr,n)	RDu8(saveptr EArL(pcoff),n)
#define rru16L(pcoff,saveptr,n)	RDu16(saveptr EArL(pcoff),n)
#define rru32L(pcoff,saveptr,n)	RDu32(saveptr EArL(pcoff),n)
#define rru64L(pcoff,saveptr,n)	RDu64(saveptr EArL(pcoff),n)
#define rrs8L(pcoff,saveptr,n)	RDs8(saveptr EArL(pcoff),n)
#define rrs16L(pcoff,saveptr,n)	RDs16(saveptr EArL(pcoff),n)
#define rrs32L(pcoff,saveptr,n)	RDs32(saveptr EArL(pcoff),n)
#define rrs64L(pcoff,saveptr,n)	RDs64(saveptr EArL(pcoff),n)
#define rrf32L(pcoff,saveptr,n)	RDf32(saveptr EArL(pcoff),n)
#define rrf64L(pcoff,saveptr,n)	RDf64(saveptr EArL(pcoff),n)

#define rrg8L(pcoff,saveptr,n)	RDg8(saveptr EArL(pcoff),n)
#define rrg16L(pcoff,saveptr,n)	RDg16(saveptr EArL(pcoff),n)
#define rrg32L(pcoff,saveptr,n)	RDg32(saveptr EArL(pcoff),n)
#define rrg64L(pcoff,saveptr,n)	RDg64(saveptr EArL(pcoff),n)

#define wru8L(pcoff,what,n)		WDu8(EArL(pcoff),what,n)
#define wru16L(pcoff,what,n)	WDu16(EArL(pcoff),what,n)
#define wru32L(pcoff,what,n)	WDu32(EArL(pcoff),what,n)
#define wru64L(pcoff,what,n)	WDu64(EArL(pcoff),what,n)
#define wrs8L(pcoff,what,n)		WDs8(EArL(pcoff),what,n)
#define wrs16L(pcoff,what,n)	WDs16(EArL(pcoff),what,n)
#define wrs32L(pcoff,what,n)	WDs32(EArL(pcoff),what,n)
#define wrs64L(pcoff,what,n)	WDs64(EArL(pcoff),what,n)
#define wrf32L(pcoff,what,n)	WDf32(EArL(pcoff),what,n)
#define wrf64L(pcoff,what,n)	WDf64(EArL(pcoff),what,n)

// reads and writes from/to relative address
// THIS ONE USES PC for base

// effective address
#define EAr(pcoff)	(R_PC+(is32(pcoff)))

#define rru8(pcoff,saveptr,n)	RDu8(saveptr EAr(pcoff),n)
#define rru16(pcoff,saveptr,n)	RDu16(saveptr EAr(pcoff),n)
#define rru32(pcoff,saveptr,n)	RDu32(saveptr EAr(pcoff),n)
#define rru64(pcoff,saveptr,n)	RDu64(saveptr EAr(pcoff),n)
#define rrs8(pcoff,saveptr,n)	RDs8(saveptr EAr(pcoff),n)
#define rrs16(pcoff,saveptr,n)	RDs16(saveptr EAr(pcoff),n)
#define rrs32(pcoff,saveptr,n)	RDs32(saveptr EAr(pcoff),n)
#define rrs64(pcoff,saveptr,n)	RDs64(saveptr EAr(pcoff),n)
#define rrf32(pcoff,saveptr,n)	RDf32(saveptr EAr(pcoff),n)
#define rrf64(pcoff,saveptr,n)	RDf64(saveptr EAr(pcoff),n)

#define rrg8(pcoff,saveptr,n)	RDg8(saveptr EAr(pcoff),n)
#define rrg16(pcoff,saveptr,n)	RDg16(saveptr EAr(pcoff),n)
#define rrg32(pcoff,saveptr,n)	RDg32(saveptr EAr(pcoff),n)
#define rrg64(pcoff,saveptr,n)	RDg64(saveptr EAr(pcoff),n)

#define wru8(pcoff,what,n)	WDu8(EAr(pcoff),what,n)
#define wru16(pcoff,what,n)	WDu16(EAr(pcoff),what,n)
#define wru32(pcoff,what,n)	WDu32(EAr(pcoff),what,n)
#define wru64(pcoff,what,n)	WDu64(EAr(pcoff),what,n)
#define wrs8(pcoff,what,n)	WDs8(EAr(pcoff),what,n)
#define wrs16(pcoff,what,n)	WDs16(EAr(pcoff),what,n)
#define wrs32(pcoff,what,n)	WDs32(EAr(pcoff),what,n)
#define wrs64(pcoff,what,n)	WDs64(EAr(pcoff),what,n)
#define wrf32(pcoff,what,n)	WDf32(EAr(pcoff),what,n)
#define wrf64(pcoff,what,n)	WDf64(EAr(pcoff),what,n)


// read/write  from offset+stackpointer

// effective address
#define EAoSP(spoff)		(R_SP+spoff) 

#define roSPu8(spoff,n)		RDu8(EAoSP(spoff),n) 
#define roSPu16(spoff,n)	RDu16(EAoSP(spoff),n)
#define roSPu32(spoff,n)	RDu32(EAoSP(spoff),n)
#define roSPu64(spoff,n)	RDu64(EAoSP(spoff),n) 
#define roSPs8(spoff,n)		RDs8(EAoSP(spoff),n) 
#define roSPs16(spoff,n)	RDs16(EAoSP(spoff),n)
#define roSPs32(spoff,n)	RDs32(EAoSP(spoff),n)
#define roSPs64(spoff,n)	RDs64(EAoSP(spoff),n) 
#define roSPf32(spoff,n)	RDf32(EAoSP(spoff),n) 
#define roSPf64(spoff,n)	RDf64(EAoSP(spoff),n) 

#define roSPg8(spoff,n)		RDg8(EAoSP(spoff),n) 
#define roSPg16(spoff,n)	RDg16(EAoSP(spoff),n)
#define roSPg32(spoff,n)	RDg32(EAoSP(spoff),n)
#define roSPg64(spoff,n)	RDg64(EAoSP(spoff),n) 

#define woSPu8(spoff,what,n)	WDu8(EAoSP(spoff),what,n) 
#define woSPu16(spoff,what,n)	WDu16(EAoSP(spoff),what,n)
#define woSPu32(spoff,what,n)	WDu32(EAoSP(spoff),what,n)
#define woSPu64(spoff,what,n)	WDu64(EAoSP(spoff),what,n) 
#define woSPs8(spoff,what,n)	WDs8(EAoSP(spoff),what,n) 
#define woSPs16(spoff,what,n)	WDs16(EAoSP(spoff),what,n)
#define woSPs32(spoff,what,n)	WDs32(EAoSP(spoff),what,n)
#define woSPs64(spoff,what,n)	WDs64(EAoSP(spoff),what,n) 
#define woSPf32(spoff,what,n)	WDf32(EAoSP(spoff),what,n) 
#define woSPf64(spoff,what,n)	WDf64(EAoSP(spoff),what,n) 

// reads and writes from/to (n,%fp)


// effective address
#define EArOFP(pcoff)		(R_FP+(s32)(is16(pcoff)))


#define rOFPu8(pcoff,saveptr,n)		RDu8(saveptr EArOFP(pcoff),n)
#define rOFPu16(pcoff,saveptr,n)	RDu16(saveptr EArOFP(pcoff),n)
#define rOFPu32(pcoff,saveptr,n)	RDu32(saveptr EArOFP(pcoff),n)
#define rOFPu64(pcoff,saveptr,n)	RDu64(saveptr EArOFP(pcoff),n)
#define rOFPs8(pcoff,saveptr,n)		RDs8(saveptr EArOFP(pcoff),n)
#define rOFPs16(pcoff,saveptr,n)	RDs16(saveptr EArOFP(pcoff),n)
#define rOFPs32(pcoff,saveptr,n)	RDs32(saveptr EArOFP(pcoff),n)
#define rOFPs64(pcoff,saveptr,n)	RDs64(saveptr EArOFP(pcoff),n)
#define rOFPf32(pcoff,saveptr,n)	RDf32(saveptr EArOFP(pcoff),n)
#define rOFPf64(pcoff,saveptr,n)	RDf64(saveptr EArOFP(pcoff),n)

#define rOFPg8(pcoff,saveptr,n)		RDg8(saveptr EArOFP(pcoff),n)
#define rOFPg16(pcoff,saveptr,n)	RDg16(saveptr EArOFP(pcoff),n)
#define rOFPg32(pcoff,saveptr,n)	RDg32(saveptr EArOFP(pcoff),n)
#define rOFPg64(pcoff,saveptr,n)	RDg64(saveptr EArOFP(pcoff),n)


#define wOFPu8(pcoff,what,n)	WDu8(EArOFP(pcoff),what,n)
#define wOFPu16(pcoff,what,n)	WDu16(EArOFP(pcoff),what,n)
#define wOFPu32(pcoff,what,n)	WDu32(EArOFP(pcoff),what,n)
#define wOFPu64(pcoff,what,n)	WDu64(EArOFP(pcoff),what,n)
#define wOFPs8(pcoff,what,n)	WDs8(EArOFP(pcoff),what,n)
#define wOFPs16(pcoff,what,n)	WDs16(EArOFP(pcoff),what,n)
#define wOFPs32(pcoff,what,n)	WDs32(EArOFP(pcoff),what,n)
#define wOFPs64(pcoff,what,n)	WDs64(EArOFP(pcoff),what,n)
#define wOFPf32(pcoff,what,n)	WDf32(EArOFP(pcoff),what,n)
#define wOFPf64(pcoff,what,n)	WDf64(EArOFP(pcoff),what,n)


// reads and writes from/to @(n,%fp)

// effective address
#define EAriFP(pcoff)		(rOFPs32(pcoff,nP,0))

#define riFPu8(pcoff,saveptr,n)		RDu8(saveptr EAriFP(pcoff),n)
#define riFPu16(pcoff,saveptr,n)	RDu16(saveptr EAriFP(pcoff),n)
#define riFPu32(pcoff,saveptr,n)	RDu32(saveptr EAriFP(pcoff),n)
#define riFPu64(pcoff,saveptr,n)	RDu64(saveptr EAriFP(pcoff),n)
#define riFPs8(pcoff,saveptr,n)		RDs8(saveptr EAriFP(pcoff),n)
#define riFPs16(pcoff,saveptr,n)	RDs16(saveptr EAriFP(pcoff),n)
#define riFPs32(pcoff,saveptr,n)	RDs32(saveptr EAriFP(pcoff),n)
#define riFPs64(pcoff,saveptr,n)	RDs64(saveptr EAriFP(pcoff),n)
#define riFPf32(pcoff,saveptr,n)	RDf32(saveptr EAriFP(pcoff),n)
#define riFPf64(pcoff,saveptr,n)	RDf64(saveptr EAriFP(pcoff),n)

#define riFPg8(pcoff,saveptr,n)		RDg8(saveptr EAriFP(pcoff),n)
#define riFPg16(pcoff,saveptr,n)	RDg16(saveptr EAriFP(pcoff),n)
#define riFPg32(pcoff,saveptr,n)	RDg32(saveptr EAriFP(pcoff),n)
#define riFPg64(pcoff,saveptr,n)	RDg64(saveptr EAriFP(pcoff),n)


#define wiFPu8(pcoff,what,n)	WDu8(EAriFP(pcoff),what,n)
#define wiFPu16(pcoff,what,n)	WDu16(EAriFP(pcoff),what,n)
#define wiFPu32(pcoff,what,n)	WDu32(EAriFP(pcoff),what,n)
#define wiFPu64(pcoff,what,n)	WDu64(EAriFP(pcoff),what,n)
#define wiFPs8(pcoff,what,n)	WDs8(EAriFP(pcoff),what,n)
#define wiFPs16(pcoff,what,n)	WDs16(EAriFP(pcoff),what,n)
#define wiFPs32(pcoff,what,n)	WDs32(EAriFP(pcoff),what,n)
#define wiFPs64(pcoff,what,n)	WDs64(EAriFP(pcoff),what,n)
#define wiFPf32(pcoff,what,n)	WDf32(EAriFP(pcoff),what,n)
#define wiFPf64(pcoff,what,n)	WDf64(EAriFP(pcoff),what,n)


// reads and writes from/to (n,(n,%fp))

// effective address
#define EAriOFP(pcoff) ((s32)is16(pcoff)+rOFPs32(pcoff+2,nP,0))

#define riOFPu8(pcoff,saveptr,n) 	RDu8(saveptr EAriOFP(pcoff),n)
#define riOFPu16(pcoff,saveptr,n) 	RDu16(saveptr EAriOFP(pcoff),n)
#define riOFPu32(pcoff,saveptr,n) 	RDu32(saveptr EAriOFP(pcoff),n)
#define riOFPu64(pcoff,saveptr,n) 	RDu64(saveptr EAriOFP(pcoff),n)
#define riOFPs8(pcoff,saveptr,n) 	RDs8(saveptr EAriOFP(pcoff),n)
#define riOFPs16(pcoff,saveptr,n) 	RDs16(saveptr EAriOFP(pcoff),n)
#define riOFPs32(pcoff,saveptr,n) 	RDs32(saveptr EAriOFP(pcoff),n)
#define riOFPs64(pcoff,saveptr,n) 	RDs64(saveptr EAriOFP(pcoff),n)
#define riOFPf32(pcoff,saveptr,n) 	RDf32(saveptr EAriOFP(pcoff),n)
#define riOFPf64(pcoff,saveptr,n) 	RDf64(saveptr EAriOFP(pcoff),n)


#define riOFPg8(pcoff,saveptr,n) 	RDg8(saveptr EAriOFP(pcoff),n)
#define riOFPg16(pcoff,saveptr,n) 	RDg16(saveptr EAriOFP(pcoff),n)
#define riOFPg32(pcoff,saveptr,n) 	RDg32(saveptr EAriOFP(pcoff),n)
#define riOFPg64(pcoff,saveptr,n) 	RDg64(saveptr EAriOFP(pcoff),n)


#define wiOFPu8(pcoff,what,n)	WDu8(EAriOFP(pcoff),what,n)
#define wiOFPu16(pcoff,what,n)	WDu16(EAriOFP(pcoff),what,n)
#define wiOFPu32(pcoff,what,n)	WDu32(EAriOFP(pcoff),what,n)
#define wiOFPu64(pcoff,what,n)	WDu64(EAriOFP(pcoff),what,n)
#define wiOFPs8(pcoff,what,n)	WDs8(EAriOFP(pcoff),what,n)
#define wiOFPs16(pcoff,what,n)	WDs16(EAriOFP(pcoff),what,n)
#define wiOFPs32(pcoff,what,n)	WDs32(EAriOFP(pcoff),what,n)
#define wiOFPs64(pcoff,what,n)	WDs64(EAriOFP(pcoff),what,n)
#define wiOFPf32(pcoff,what,n)	WDf32(EAriOFP(pcoff),what,n)
#define wiOFPf64(pcoff,what,n)	WDf64(EAriOFP(pcoff),what,n)


// reads and writes from/to (@(n,%fp),(n,%fp))

// effective address
#define EAriFFP(pcoff) (rOFPs32(pcoff,nP,0)+rOFPs32(pcoff+2,nP,0))


#define riFFPu8(pcoff,saveptr,n) 	RDu8(saveptr EAriFFP(pcoff),n)
#define riFFPu16(pcoff,saveptr,n) 	RDu16(saveptr EAriFFP(pcoff),n)
#define riFFPu32(pcoff,saveptr,n) 	RDu32(saveptr EAriFFP(pcoff),n)
#define riFFPu64(pcoff,saveptr,n) 	RDu64(saveptr EAriFFP(pcoff),n)
#define riFFPs8(pcoff,saveptr,n) 	RDs8(saveptr EAriFFP(pcoff),n)
#define riFFPs16(pcoff,saveptr,n) 	RDs16(saveptr EAriFFP(pcoff),n)
#define riFFPs32(pcoff,saveptr,n) 	RDs32(saveptr EAriFFP(pcoff),n)
#define riFFPs64(pcoff,saveptr,n) 	RDs64(saveptr EAriFFP(pcoff),n)
#define riFFPf32(pcoff,saveptr,n) 	RDf32(saveptr EAriFFP(pcoff),n)
#define riFFPf64(pcoff,saveptr,n) 	RDf64(saveptr EAriFFP(pcoff),n)

#define riFFPg8(pcoff,saveptr,n) 	RDg8(saveptr EAriFFP(pcoff),n)
#define riFFPg16(pcoff,saveptr,n) 	RDg16(saveptr EAriFFP(pcoff),n)
#define riFFPg32(pcoff,saveptr,n) 	RDg32(saveptr EAriFFP(pcoff),n)
#define riFFPg64(pcoff,saveptr,n) 	RDg64(saveptr EAriFFP(pcoff),n)


#define wiFFPu8(pcoff,what,n)	WDu8(EAriFFP(pcoff),what,n)
#define wiFFPu16(pcoff,what,n)	WDu16(EAriFFP(pcoff),what,n)
#define wiFFPu32(pcoff,what,n)	WDu32(EAriFFP(pcoff),what,n)
#define wiFFPu64(pcoff,what,n)	WDu64(EAriFFP(pcoff),what,n)
#define wiFFPs8(pcoff,what,n)	WDs8(EAriFFP(pcoff),what,n)
#define wiFFPs16(pcoff,what,n)	WDs16(EAriFFP(pcoff),what,n)
#define wiFFPs32(pcoff,what,n)	WDs32(EAriFFP(pcoff),what,n)
#define wiFFPs64(pcoff,what,n)	WDs64(EAriFFP(pcoff),what,n)
#define wiFFPf32(pcoff,what,n)	WDf32(EAriFFP(pcoff),what,n)
#define wiFFPf64(pcoff,what,n)	WDf64(EAriFFP(pcoff),what,n)




// reads and writes from/to (n,(n,%fp),(n,%fp))

// effective address
#define EAriOFFP(pcoff)\
	((s32)is16(pcoff)+rOFPs32(pcoff+2,nP,0)+rOFPs32(pcoff+4,nP,0))


#define riOFFPu8(pcoff,saveptr,n) 	RDu8(saveptr EAriOFFP(pcoff),n)
#define riOFFPu16(pcoff,saveptr,n) 	RDu16(saveptr EAriOFFP(pcoff),n)
#define riOFFPu32(pcoff,saveptr,n) 	RDu32(saveptr EAriOFFP(pcoff),n)
#define riOFFPu64(pcoff,saveptr,n) 	RDu64(saveptr EAriOFFP(pcoff),n)
#define riOFFPs8(pcoff,saveptr,n) 	RDs8(saveptr EAriOFFP(pcoff),n)
#define riOFFPs16(pcoff,saveptr,n) 	RDs16(saveptr EAriOFFP(pcoff),n)
#define riOFFPs32(pcoff,saveptr,n) 	RDs32(saveptr EAriOFFP(pcoff),n)
#define riOFFPs64(pcoff,saveptr,n) 	RDs64(saveptr EAriOFFP(pcoff),n)
#define riOFFPf32(pcoff,saveptr,n) 	RDf32(saveptr EAriOFFP(pcoff),n)
#define riOFFPf64(pcoff,saveptr,n) 	RDf64(saveptr EAriOFFP(pcoff),n)


#define riOFFPg8(pcoff,saveptr,n) 	RDg8(saveptr EAriOFFP(pcoff),n)
#define riOFFPg16(pcoff,saveptr,n) 	RDg16(saveptr EAriOFFP(pcoff),n)
#define riOFFPg32(pcoff,saveptr,n) 	RDg32(saveptr EAriOFFP(pcoff),n)
#define riOFFPg64(pcoff,saveptr,n) 	RDg64(saveptr EAriOFFP(pcoff),n)


#define wiOFFPu8(pcoff,what,n)	WDu8(EAriOFFP(pcoff),what,n)
#define wiOFFPu16(pcoff,what,n)	WDu16(EAriOFFP(pcoff),what,n)
#define wiOFFPu32(pcoff,what,n)	WDu32(EAriOFFP(pcoff),what,n)
#define wiOFFPu64(pcoff,what,n)	WDu64(EAriOFFP(pcoff),what,n)
#define wiOFFPs8(pcoff,what,n)	WDs8(EAriOFFP(pcoff),what,n)
#define wiOFFPs16(pcoff,what,n)	WDs16(EAriOFFP(pcoff),what,n)
#define wiOFFPs32(pcoff,what,n)	WDs32(EAriOFFP(pcoff),what,n)
#define wiOFFPs64(pcoff,what,n)	WDs64(EAriOFFP(pcoff),what,n)
#define wiOFFPf32(pcoff,what,n)	WDf32(EAriOFFP(pcoff),what,n)
#define wiOFFPf64(pcoff,what,n)	WDf64(EAriOFFP(pcoff),what,n)


// reads and writes from/to (@(n,%fp),(n,%fp)@)

// effective address
#define EAriFFPS(pcoff,scale)\
 	(rOFPs32(pcoff,nP,0)+rOFPs32(pcoff+2,nP,0)*scale)


#define riFFPu16S(pcoff,saveptr,n) 	RDu16(saveptr EAriFFPS(pcoff,2),n)
#define riFFPu32S(pcoff,saveptr,n) 	RDu32(saveptr EAriFFPS(pcoff,4),n)
#define riFFPu64S(pcoff,saveptr,n) 	RDu64(saveptr EAriFFPS(pcoff,8),n)
#define riFFPs16S(pcoff,saveptr,n) 	RDs16(saveptr EAriFFPS(pcoff,2),n)
#define riFFPs32S(pcoff,saveptr,n) 	RDs32(saveptr EAriFFPS(pcoff,4),n)
#define riFFPs64S(pcoff,saveptr,n) 	RDs64(saveptr EAriFFPS(pcoff,8),n)
#define riFFPf32S(pcoff,saveptr,n) 	RDf32(saveptr EAriFFPS(pcoff,4),n)
#define riFFPf64S(pcoff,saveptr,n) 	RDf64(saveptr EAriFFPS(pcoff,8),n)


#define riFFPg16S(pcoff,saveptr,n) 	RDg16(saveptr EAriFFPS(pcoff,2),n)
#define riFFPg32S(pcoff,saveptr,n) 	RDg32(saveptr EAriFFPS(pcoff,4),n)
#define riFFPg64S(pcoff,saveptr,n) 	RDg64(saveptr EAriFFPS(pcoff,8),n)


#define wiFFPu16S(pcoff,what,n)	WDu16(EAriFFPS(pcoff,2),what,n)
#define wiFFPu32S(pcoff,what,n)	WDu32(EAriFFPS(pcoff,4),what,n)
#define wiFFPu64S(pcoff,what,n)	WDu64(EAriFFPS(pcoff,8),what,n)
#define wiFFPs16S(pcoff,what,n)	WDs16(EAriFFPS(pcoff,2),what,n)
#define wiFFPs32S(pcoff,what,n)	WDs32(EAriFFPS(pcoff,4),what,n)
#define wiFFPs64S(pcoff,what,n)	WDs64(EAriFFPS(pcoff,8),what,n)
#define wiFFPf32S(pcoff,what,n)	WDf32(EAriFFPS(pcoff,4),what,n)
#define wiFFPf64S(pcoff,what,n)	WDf64(EAriFFPS(pcoff,8),what,n)

// reads and writes from/to (n,(n,%fp),(n,%fp)@)

//effective address
#define EAriOFFPS(pcoff,scale)\
 	((s32)is16(pcoff)+rOFPs32(pcoff+2,nP,0)+rOFPs32(pcoff+4,nP,0)*scale)


#define riOFFPu16S(pcoff,saveptr,n) 	RDu16(saveptr EAriOFFPS(pcoff,2),n)
#define riOFFPu32S(pcoff,saveptr,n) 	RDu32(saveptr EAriOFFPS(pcoff,4),n)
#define riOFFPu64S(pcoff,saveptr,n) 	RDu64(saveptr EAriOFFPS(pcoff,8),n)
#define riOFFPs16S(pcoff,saveptr,n) 	RDs16(saveptr EAriOFFPS(pcoff,2),n)
#define riOFFPs32S(pcoff,saveptr,n) 	RDs32(saveptr EAriOFFPS(pcoff,4),n)
#define riOFFPs64S(pcoff,saveptr,n) 	RDs64(saveptr EAriOFFPS(pcoff,8),n)
#define riOFFPf32S(pcoff,saveptr,n) 	RDf32(saveptr EAriOFFPS(pcoff,4),n)
#define riOFFPf64S(pcoff,saveptr,n) 	RDf64(saveptr EAriOFFPS(pcoff,8),n)

#define riOFFPg16S(pcoff,saveptr,n) 	RDg16(saveptr EAriOFFPS(pcoff,2),n)
#define riOFFPg32S(pcoff,saveptr,n) 	RDg32(saveptr EAriOFFPS(pcoff,4),n)
#define riOFFPg64S(pcoff,saveptr,n) 	RDg64(saveptr EAriOFFPS(pcoff,8),n)


#define wiOFFPu16S(pcoff,what,n)	WDu16(EAriOFFPS(pcoff,2),what,n)
#define wiOFFPu32S(pcoff,what,n)	WDu32(EAriOFFPS(pcoff,4),what,n)
#define wiOFFPu64S(pcoff,what,n)	WDu64(EAriOFFPS(pcoff,8),what,n)
#define wiOFFPs16S(pcoff,what,n)	WDs16(EAriOFFPS(pcoff,2),what,n)
#define wiOFFPs32S(pcoff,what,n)	WDs32(EAriOFFPS(pcoff,4),what,n)
#define wiOFFPs64S(pcoff,what,n)	WDs64(EAriOFFPS(pcoff,8),what,n)
#define wiOFFPf32S(pcoff,what,n)	WDf32(EAriOFFPS(pcoff,4),what,n)
#define wiOFFPf64S(pcoff,what,n)	WDf64(EAriOFFPS(pcoff,8),what,n)





// reads and writes from/to (R2,(n,%fp))

// effective address
#define EAriR2FP(pcoff) 	((s32)R_R2+rOFPs32(pcoff,nP,0))


#define riR2FPu8(pcoff,saveptr,n) 	RDu8(saveptr EAriR2FP(pcoff),n)
#define riR2FPu16(pcoff,saveptr,n) 	RDu16(saveptr EAriR2FP(pcoff),n)
#define riR2FPu32(pcoff,saveptr,n) 	RDu32(saveptr EAriR2FP(pcoff),n)
#define riR2FPu64(pcoff,saveptr,n) 	RDu64(saveptr EAriR2FP(pcoff),n)
#define riR2FPs8(pcoff,saveptr,n) 	RDs8(saveptr EAriR2FP(pcoff),n)
#define riR2FPs16(pcoff,saveptr,n) 	RDs16(saveptr EAriR2FP(pcoff),n)
#define riR2FPs32(pcoff,saveptr,n) 	RDs32(saveptr EAriR2FP(pcoff),n)
#define riR2FPs64(pcoff,saveptr,n) 	RDs64(saveptr EAriR2FP(pcoff),n)
#define riR2FPf32(pcoff,saveptr,n) 	RDf32(saveptr EAriR2FP(pcoff),n)
#define riR2FPf64(pcoff,saveptr,n) 	RDf64(saveptr EAriR2FP(pcoff),n)


#define riR2FPg8(pcoff,saveptr,n) 	RDg8(saveptr EAriR2FP(pcoff),n)
#define riR2FPg16(pcoff,saveptr,n) 	RDg16(saveptr EAriR2FP(pcoff),n)
#define riR2FPg32(pcoff,saveptr,n) 	RDg32(saveptr EAriR2FP(pcoff),n)
#define riR2FPg64(pcoff,saveptr,n) 	RDg64(saveptr EAriR2FP(pcoff),n)


#define wiR2FPu8(pcoff,what,n)	WDu8(EAriR2FP(pcoff),what,n)
#define wiR2FPu16(pcoff,what,n)	WDu16(EAriR2FP(pcoff),what,n)
#define wiR2FPu32(pcoff,what,n)	WDu32(EAriR2FP(pcoff),what,n)
#define wiR2FPu64(pcoff,what,n)	WDu64(EAriR2FP(pcoff),what,n)
#define wiR2FPs8(pcoff,what,n)	WDs8(EAriR2FP(pcoff),what,n)
#define wiR2FPs16(pcoff,what,n)	WDs16(EAriR2FP(pcoff),what,n)
#define wiR2FPs32(pcoff,what,n)	WDs32(EAriR2FP(pcoff),what,n)
#define wiR2FPs64(pcoff,what,n)	WDs64(EAriR2FP(pcoff),what,n)
#define wiR2FPf32(pcoff,what,n)	WDf32(EAriR2FP(pcoff),what,n)
#define wiR2FPf64(pcoff,what,n)	WDf64(EAriR2FP(pcoff),what,n)


// reads and writes from/to (R3,(n,%fp))

#define EAriR3FP(pcoff) 	((s32)R_R3+rOFPs32(pcoff,nP,0))


#define riR3FPu8(pcoff,saveptr,n) 	RDu8(saveptr EAriR3FP(pcoff),n)
#define riR3FPu16(pcoff,saveptr,n) 	RDu16(saveptr EAriR3FP(pcoff),n)
#define riR3FPu32(pcoff,saveptr,n) 	RDu32(saveptr EAriR3FP(pcoff),n)
#define riR3FPu64(pcoff,saveptr,n) 	RDu64(saveptr EAriR3FP(pcoff),n)
#define riR3FPs8(pcoff,saveptr,n) 	RDs8(saveptr EAriR3FP(pcoff),n)
#define riR3FPs16(pcoff,saveptr,n) 	RDs16(saveptr EAriR3FP(pcoff),n)
#define riR3FPs32(pcoff,saveptr,n) 	RDs32(saveptr EAriR3FP(pcoff),n)
#define riR3FPs64(pcoff,saveptr,n) 	RDs64(saveptr EAriR3FP(pcoff),n)
#define riR3FPf32(pcoff,saveptr,n) 	RDf32(saveptr EAriR3FP(pcoff),n)
#define riR3FPf64(pcoff,saveptr,n) 	RDf64(saveptr EAriR3FP(pcoff),n)

#define riR3FPg8(pcoff,saveptr,n) 	RDg8(saveptr EAriR3FP(pcoff),n)
#define riR3FPg16(pcoff,saveptr,n) 	RDg16(saveptr EAriR3FP(pcoff),n)
#define riR3FPg32(pcoff,saveptr,n) 	RDg32(saveptr EAriR3FP(pcoff),n)
#define riR3FPg64(pcoff,saveptr,n) 	RDg64(saveptr EAriR3FP(pcoff),n)


#define wiR3FPu8(pcoff,what,n)	WDu8(EAriR3FP(pcoff),what,n)
#define wiR3FPu16(pcoff,what,n)	WDu16(EAriR3FP(pcoff),what,n)
#define wiR3FPu32(pcoff,what,n)	WDu32(EAriR3FP(pcoff),what,n)
#define wiR3FPu64(pcoff,what,n)	WDu64(EAriR3FP(pcoff),what,n)
#define wiR3FPs8(pcoff,what,n)	WDs8(EAriR3FP(pcoff),what,n)
#define wiR3FPs16(pcoff,what,n)	WDs16(EAriR3FP(pcoff),what,n)
#define wiR3FPs32(pcoff,what,n)	WDs32(EAriR3FP(pcoff),what,n)
#define wiR3FPs64(pcoff,what,n)	WDs64(EAriR3FP(pcoff),what,n)
#define wiR3FPf32(pcoff,what,n)	WDf32(EAriR3FP(pcoff),what,n)
#define wiR3FPf64(pcoff,what,n)	WDf64(EAriR3FP(pcoff),what,n)


// reads and writes from/to (R2,(n,%fp)@)


// effective address
#define EAriR2FPS(pcoff,scale) 	((s32)R_R2+rOFPs32(pcoff,nP,0)*scale)

#define riR2FPu16S(pcoff,saveptr,n) 	RDu16(saveptr EAriR2FPS(pcoff,2),n)
#define riR2FPu32S(pcoff,saveptr,n) 	RDu32(saveptr EAriR2FPS(pcoff,4),n)
#define riR2FPu64S(pcoff,saveptr,n) 	RDu64(saveptr EAriR2FPS(pcoff,8),n)
#define riR2FPs16S(pcoff,saveptr,n) 	RDs16(saveptr EAriR2FPS(pcoff,2),n)
#define riR2FPs32S(pcoff,saveptr,n) 	RDs32(saveptr EAriR2FPS(pcoff,4),n)
#define riR2FPs64S(pcoff,saveptr,n) 	RDs64(saveptr EAriR2FPS(pcoff,8),n)
#define riR2FPf32S(pcoff,saveptr,n) 	RDf32(saveptr EAriR2FPS(pcoff,4),n)
#define riR2FPf64S(pcoff,saveptr,n) 	RDf64(saveptr EAriR2FPS(pcoff,8),n)

#define riR2FPg16S(pcoff,saveptr,n) 	RDg16(saveptr EAriR2FPS(pcoff,2),n)
#define riR2FPg32S(pcoff,saveptr,n) 	RDg32(saveptr EAriR2FPS(pcoff,4),n)
#define riR2FPg64S(pcoff,saveptr,n) 	RDg64(saveptr EAriR2FPS(pcoff,8),n)


#define wiR2FPu16S(pcoff,what,n)	WDu16(EAriR2FPS(pcoff,2),what,n)
#define wiR2FPu32S(pcoff,what,n)	WDu32(EAriR2FPS(pcoff,4),what,n)
#define wiR2FPu64S(pcoff,what,n)	WDu64(EAriR2FPS(pcoff,8),what,n)
#define wiR2FPs16S(pcoff,what,n)	WDs16(EAriR2FPS(pcoff,2),what,n)
#define wiR2FPs32S(pcoff,what,n)	WDs32(EAriR2FPS(pcoff,4),what,n)
#define wiR2FPs64S(pcoff,what,n)	WDs64(EAriR2FPS(pcoff,8),what,n)
#define wiR2FPf32S(pcoff,what,n)	WDf32(EAriR2FPS(pcoff,4),what,n)
#define wiR2FPf64S(pcoff,what,n)	WDf64(EAriR2FPS(pcoff,8),what,n)

// reads and writes from/to (R3,(n,%fp)@)

// effective address
#define EAriR3FPS(pcoff,scale) 	((s32)R_R3+rOFPs32(pcoff,nP,0)*scale)


#define riR3FPu16S(pcoff,saveptr,n) 	RDu16(saveptr EAriR3FPS(pcoff,2),n)
#define riR3FPu32S(pcoff,saveptr,n) 	RDu32(saveptr EAriR3FPS(pcoff,4),n)
#define riR3FPu64S(pcoff,saveptr,n) 	RDu64(saveptr EAriR3FPS(pcoff,8),n)
#define riR3FPs16S(pcoff,saveptr,n) 	RDs16(saveptr EAriR3FPS(pcoff,2),n)
#define riR3FPs32S(pcoff,saveptr,n) 	RDs32(saveptr EAriR3FPS(pcoff,4),n)
#define riR3FPs64S(pcoff,saveptr,n) 	RDs64(saveptr EAriR3FPS(pcoff,8),n)
#define riR3FPf32S(pcoff,saveptr,n) 	RDf32(saveptr EAriR3FPS(pcoff,4),n)
#define riR3FPf64S(pcoff,saveptr,n) 	RDf64(saveptr EAriR3FPS(pcoff,8),n)

#define riR3FPg16S(pcoff,saveptr,n) 	RDg16(saveptr EAriR3FPS(pcoff,2),n)
#define riR3FPg32S(pcoff,saveptr,n) 	RDg32(saveptr EAriR3FPS(pcoff,4),n)
#define riR3FPg64S(pcoff,saveptr,n) 	RDg64(saveptr EAriR3FPS(pcoff,8),n)


#define wiR3FPu16S(pcoff,what,n)	WDu16(EAriR3FPS(pcoff,2),what,n)
#define wiR3FPu32S(pcoff,what,n)	WDu32(EAriR3FPS(pcoff,4),what,n)
#define wiR3FPu64S(pcoff,what,n)	WDu64(EAriR3FPS(pcoff,8),what,n)
#define wiR3FPs16S(pcoff,what,n)	WDs16(EAriR3FPS(pcoff,2),what,n)
#define wiR3FPs32S(pcoff,what,n)	WDs32(EAriR3FPS(pcoff,4),what,n)
#define wiR3FPs64S(pcoff,what,n)	WDs64(EAriR3FPS(pcoff,8),what,n)
#define wiR3FPf32S(pcoff,what,n)	WDf32(EAriR3FPS(pcoff,4),what,n)
#define wiR3FPf64S(pcoff,what,n)	WDf64(EAriR3FPS(pcoff,8),what,n)


// reads and writes from/to (n,R2,(n,%fp))

// effective address
#define EAriOR2FP(pcoff)\
 	((s32)is16(pcoff)+(s32)R_R2+rOFPs32(pcoff+2,nP,0))

#define riOR2FPu8(pcoff,saveptr,n) 		RDu8(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPu16(pcoff,saveptr,n) 	RDu16(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPu32(pcoff,saveptr,n) 	RDu32(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPu64(pcoff,saveptr,n) 	RDu64(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPs8(pcoff,saveptr,n) 		RDs8(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPs16(pcoff,saveptr,n) 	RDs16(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPs32(pcoff,saveptr,n) 	RDs32(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPs64(pcoff,saveptr,n) 	RDs64(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPf32(pcoff,saveptr,n) 	RDf32(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPf64(pcoff,saveptr,n) 	RDf64(saveptr EAriOR2FP(pcoff),n)

#define riOR2FPg8(pcoff,saveptr,n) 		RDg8(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPg16(pcoff,saveptr,n) 	RDg16(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPg32(pcoff,saveptr,n) 	RDg32(saveptr EAriOR2FP(pcoff),n)
#define riOR2FPg64(pcoff,saveptr,n) 	RDg64(saveptr EAriOR2FP(pcoff),n)


#define wiOR2FPu8(pcoff,what,n)		WDu8(EAriOR2FP(pcoff),what,n)
#define wiOR2FPu16(pcoff,what,n)	WDu16(EAriOR2FP(pcoff),what,n)
#define wiOR2FPu32(pcoff,what,n)	WDu32(EAriOR2FP(pcoff),what,n)
#define wiOR2FPu64(pcoff,what,n)	WDu64(EAriOR2FP(pcoff),what,n)
#define wiOR2FPs8(pcoff,what,n)		WDs8(EAriOR2FP(pcoff),what,n)
#define wiOR2FPs16(pcoff,what,n)	WDs16(EAriOR2FP(pcoff),what,n)
#define wiOR2FPs32(pcoff,what,n)	WDs32(EAriOR2FP(pcoff),what,n)
#define wiOR2FPs64(pcoff,what,n)	WDs64(EAriOR2FP(pcoff),what,n)
#define wiOR2FPf32(pcoff,what,n)	WDf32(EAriOR2FP(pcoff),what,n)
#define wiOR2FPf64(pcoff,what,n)	WDf64(EAriOR2FP(pcoff),what,n)

// reads and writes from/to (n,R3,(n,%fp))

// effective address
#define EAriOR3FP(pcoff)\
 	((s32)is16(pcoff)+(s32)R_R3+rOFPs32(pcoff+2,nP,0))


#define riOR3FPu8(pcoff,saveptr,n) 		RDu8(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPu16(pcoff,saveptr,n) 	RDu16(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPu32(pcoff,saveptr,n) 	RDu32(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPu64(pcoff,saveptr,n) 	RDu64(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPs8(pcoff,saveptr,n) 		RDs8(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPs16(pcoff,saveptr,n) 	RDs16(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPs32(pcoff,saveptr,n) 	RDs32(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPs64(pcoff,saveptr,n) 	RDs64(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPf32(pcoff,saveptr,n) 	RDf32(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPf64(pcoff,saveptr,n) 	RDf64(saveptr EAriOR3FP(pcoff),n)

#define riOR3FPg8(pcoff,saveptr,n) 		RDg8(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPg16(pcoff,saveptr,n) 	RDg16(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPg32(pcoff,saveptr,n) 	RDg32(saveptr EAriOR3FP(pcoff),n)
#define riOR3FPg64(pcoff,saveptr,n) 	RDg64(saveptr EAriOR3FP(pcoff),n)


#define wiOR3FPu8(pcoff,what,n)		WDu8(EAriOR3FP(pcoff),what,n)
#define wiOR3FPu16(pcoff,what,n)	WDu16(EAriOR3FP(pcoff),what,n)
#define wiOR3FPu32(pcoff,what,n)	WDu32(EAriOR3FP(pcoff),what,n)
#define wiOR3FPu64(pcoff,what,n)	WDu64(EAriOR3FP(pcoff),what,n)
#define wiOR3FPs8(pcoff,what,n)		WDs8(EAriOR3FP(pcoff),what,n)
#define wiOR3FPs16(pcoff,what,n)	WDs16(EAriOR3FP(pcoff),what,n)
#define wiOR3FPs32(pcoff,what,n)	WDs32(EAriOR3FP(pcoff),what,n)
#define wiOR3FPs64(pcoff,what,n)	WDs64(EAriOR3FP(pcoff),what,n)
#define wiOR3FPf32(pcoff,what,n)	WDf32(EAriOR3FP(pcoff),what,n)
#define wiOR3FPf64(pcoff,what,n)	WDf64(EAriOR3FP(pcoff),what,n)

// reads and writes from/to (n,R2,(n,%fp)@)

// effective address 
#define EAriOR2FPS(pcoff,scale)\
 	((s32)is16(pcoff)+(s32)R_R2+rOFPs32(pcoff+2,nP,0)*scale)


#define riOR2FPu16S(pcoff,saveptr,n) 	RDu16(saveptr EAriOR2FPS(pcoff,2),n)
#define riOR2FPu32S(pcoff,saveptr,n) 	RDu32(saveptr EAriOR2FPS(pcoff,4),n)
#define riOR2FPu64S(pcoff,saveptr,n) 	RDu64(saveptr EAriOR2FPS(pcoff,8),n)
#define riOR2FPs16S(pcoff,saveptr,n) 	RDs16(saveptr EAriOR2FPS(pcoff,2),n)
#define riOR2FPs32S(pcoff,saveptr,n) 	RDs32(saveptr EAriOR2FPS(pcoff,4),n)
#define riOR2FPs64S(pcoff,saveptr,n) 	RDs64(saveptr EAriOR2FPS(pcoff,8),n)
#define riOR2FPf32S(pcoff,saveptr,n) 	RDf32(saveptr EAriOR2FPS(pcoff,4),n)
#define riOR2FPf64S(pcoff,saveptr,n) 	RDf64(saveptr EAriOR2FPS(pcoff,8),n)

#define riOR2FPg16S(pcoff,saveptr,n) 	RDg16(saveptr EAriOR2FPS(pcoff,2),n)
#define riOR2FPg32S(pcoff,saveptr,n) 	RDg32(saveptr EAriOR2FPS(pcoff,4),n)
#define riOR2FPg64S(pcoff,saveptr,n) 	RDg64(saveptr EAriOR2FPS(pcoff,8),n)


#define wiOR2FPu16S(pcoff,what,n)	WDu16(EAriOR2FPS(pcoff,2),what,n)
#define wiOR2FPu32S(pcoff,what,n)	WDu32(EAriOR2FPS(pcoff,4),what,n)
#define wiOR2FPu64S(pcoff,what,n)	WDu64(EAriOR2FPS(pcoff,8),what,n)
#define wiOR2FPs16S(pcoff,what,n)	WDs16(EAriOR2FPS(pcoff,2),what,n)
#define wiOR2FPs32S(pcoff,what,n)	WDs32(EAriOR2FPS(pcoff,4),what,n)
#define wiOR2FPs64S(pcoff,what,n)	WDs64(EAriOR2FPS(pcoff,8),what,n)
#define wiOR2FPf32S(pcoff,what,n)	WDf32(EAriOR2FPS(pcoff,4),what,n)
#define wiOR2FPf64S(pcoff,what,n)	WDf64(EAriOR2FPS(pcoff,8),what,n)

// reads and writes from/to (n,R3,(n,%fp)@)

// effective address 
#define EAriOR3FPS(pcoff,scale)\
 	((s32)is16(pcoff)+(s32)R_R3+rOFPs32(pcoff+2,nP,0)*scale)


#define riOR3FPu16S(pcoff,saveptr,n) 	RDu16(saveptr EAriOR3FPS(pcoff,2),n)
#define riOR3FPu32S(pcoff,saveptr,n) 	RDu32(saveptr EAriOR3FPS(pcoff,4),n)
#define riOR3FPu64S(pcoff,saveptr,n) 	RDu64(saveptr EAriOR3FPS(pcoff,8),n)
#define riOR3FPs16S(pcoff,saveptr,n) 	RDs16(saveptr EAriOR3FPS(pcoff,2),n)
#define riOR3FPs32S(pcoff,saveptr,n) 	RDs32(saveptr EAriOR3FPS(pcoff,4),n)
#define riOR3FPs64S(pcoff,saveptr,n) 	RDs64(saveptr EAriOR3FPS(pcoff,8),n)
#define riOR3FPf32S(pcoff,saveptr,n) 	RDf32(saveptr EAriOR3FPS(pcoff,4),n)
#define riOR3FPf64S(pcoff,saveptr,n) 	RDf64(saveptr EAriOR3FPS(pcoff,8),n)

#define riOR3FPg16S(pcoff,saveptr,n) 	RDg16(saveptr EAriOR3FPS(pcoff,2),n)
#define riOR3FPg32S(pcoff,saveptr,n) 	RDg32(saveptr EAriOR3FPS(pcoff,4),n)
#define riOR3FPg64S(pcoff,saveptr,n) 	RDg64(saveptr EAriOR3FPS(pcoff,8),n)


#define wiOR3FPu16S(pcoff,what,n)	WDu16(EAriOR3FPS(pcoff,2),what,n)
#define wiOR3FPu32S(pcoff,what,n)	WDu32(EAriOR3FPS(pcoff,4),what,n)
#define wiOR3FPu64S(pcoff,what,n)	WDu64(EAriOR3FPS(pcoff,8),what,n)
#define wiOR3FPs16S(pcoff,what,n)	WDs16(EAriOR3FPS(pcoff,2),what,n)
#define wiOR3FPs32S(pcoff,what,n)	WDs32(EAriOR3FPS(pcoff,4),what,n)
#define wiOR3FPs64S(pcoff,what,n)	WDs64(EAriOR3FPS(pcoff,8),what,n)
#define wiOR3FPf32S(pcoff,what,n)	WDf32(EAriOR3FPS(pcoff,4),what,n)
#define wiOR3FPf64S(pcoff,what,n)	WDf64(EAriOR3FPS(pcoff,8),what,n)


// reads and writes from/to (@(n,%fp),R2@)

// effective address
#define EAriFPR2S(pcoff,scale) 	(scale*(s32)R_R2+rOFPs32(pcoff,nP,0))

#define riFPR2u16S(pcoff,saveptr,n) 	RDu16(saveptr EAriFPR2S(pcoff,2),n)
#define riFPR2u32S(pcoff,saveptr,n) 	RDu32(saveptr EAriFPR2S(pcoff,4),n)
#define riFPR2u64S(pcoff,saveptr,n) 	RDu64(saveptr EAriFPR2S(pcoff,8),n)
#define riFPR2s16S(pcoff,saveptr,n) 	RDs16(saveptr EAriFPR2S(pcoff,2),n)
#define riFPR2s32S(pcoff,saveptr,n) 	RDs32(saveptr EAriFPR2S(pcoff,4),n)
#define riFPR2s64S(pcoff,saveptr,n) 	RDs64(saveptr EAriFPR2S(pcoff,8),n)
#define riFPR2f32S(pcoff,saveptr,n) 	RDf32(saveptr EAriFPR2S(pcoff,4),n)
#define riFPR2f64S(pcoff,saveptr,n) 	RDf64(saveptr EAriFPR2S(pcoff,8),n)

#define riFPR2g16S(pcoff,saveptr,n) 	RDg16(saveptr EAriFPR2S(pcoff,2),n)
#define riFPR2g32S(pcoff,saveptr,n) 	RDg32(saveptr EAriFPR2S(pcoff,4),n)
#define riFPR2g64S(pcoff,saveptr,n) 	RDg64(saveptr EAriFPR2S(pcoff,8),n)


#define wiFPR2u16S(pcoff,what,n)	WDu16(EAriFPR2S(pcoff,2),what,n)
#define wiFPR2u32S(pcoff,what,n)	WDu32(EAriFPR2S(pcoff,4),what,n)
#define wiFPR2u64S(pcoff,what,n)	WDu64(EAriFPR2S(pcoff,8),what,n)
#define wiFPR2s16S(pcoff,what,n)	WDs16(EAriFPR2S(pcoff,2),what,n)
#define wiFPR2s32S(pcoff,what,n)	WDs32(EAriFPR2S(pcoff,4),what,n)
#define wiFPR2s64S(pcoff,what,n)	WDs64(EAriFPR2S(pcoff,8),what,n)
#define wiFPR2f32S(pcoff,what,n)	WDf32(EAriFPR2S(pcoff,4),what,n)
#define wiFPR2f64S(pcoff,what,n)	WDf64(EAriFPR2S(pcoff,8),what,n)

// reads and writes from/to (@(n,%fp),R3@)

// effective address
#define EAriFPR3S(pcoff,scale) 	(scale*(s32)R_R3+rOFPs32(pcoff,nP,0))


#define riFPR3u16S(pcoff,saveptr,n) 	RDu16(saveptr EAriFPR3S(pcoff,2),n)
#define riFPR3u32S(pcoff,saveptr,n) 	RDu32(saveptr EAriFPR3S(pcoff,4),n)
#define riFPR3u64S(pcoff,saveptr,n) 	RDu64(saveptr EAriFPR3S(pcoff,8),n)
#define riFPR3s16S(pcoff,saveptr,n) 	RDs16(saveptr EAriFPR3S(pcoff,2),n)
#define riFPR3s32S(pcoff,saveptr,n) 	RDs32(saveptr EAriFPR3S(pcoff,4),n)
#define riFPR3s64S(pcoff,saveptr,n) 	RDs64(saveptr EAriFPR3S(pcoff,8),n)
#define riFPR3f32S(pcoff,saveptr,n) 	RDf32(saveptr EAriFPR3S(pcoff,4),n)
#define riFPR3f64S(pcoff,saveptr,n) 	RDf64(saveptr EAriFPR3S(pcoff,8),n)

#define riFPR3g16S(pcoff,saveptr,n) 	RDg16(saveptr EAriFPR3S(pcoff,2),n)
#define riFPR3g32S(pcoff,saveptr,n) 	RDg32(saveptr EAriFPR3S(pcoff,4),n)
#define riFPR3g64S(pcoff,saveptr,n) 	RDg64(saveptr EAriFPR3S(pcoff,8),n)


#define wiFPR3u16S(pcoff,what,n)	WDu16(EAriFPR3S(pcoff,2),what,n)
#define wiFPR3u32S(pcoff,what,n)	WDu32(EAriFPR3S(pcoff,4),what,n)
#define wiFPR3u64S(pcoff,what,n)	WDu64(EAriFPR3S(pcoff,8),what,n)
#define wiFPR3s16S(pcoff,what,n)	WDs16(EAriFPR3S(pcoff,2),what,n)
#define wiFPR3s32S(pcoff,what,n)	WDs32(EAriFPR3S(pcoff,4),what,n)
#define wiFPR3s64S(pcoff,what,n)	WDs64(EAriFPR3S(pcoff,8),what,n)
#define wiFPR3f32S(pcoff,what,n)	WDf32(EAriFPR3S(pcoff,4),what,n)
#define wiFPR3f64S(pcoff,what,n)	WDf64(EAriFPR3S(pcoff,8),what,n)




// reads and writes from/to (n,(n,%fp),R2@)

// effective address
#define EAriOFPR2S(pcoff,scale)\
 	((s32)is16(pcoff)+scale*(s32)R_R2+rOFPs32(pcoff+2,nP,0))


#define riOFPR2u16S(pcoff,saveptr,n) 	RDu16(saveptr EAriOFPR2S(pcoff,2),n)
#define riOFPR2u32S(pcoff,saveptr,n) 	RDu32(saveptr EAriOFPR2S(pcoff,4),n)
#define riOFPR2u64S(pcoff,saveptr,n) 	RDu64(saveptr EAriOFPR2S(pcoff,8),n)
#define riOFPR2s16S(pcoff,saveptr,n) 	RDs16(saveptr EAriOFPR2S(pcoff,2),n)
#define riOFPR2s32S(pcoff,saveptr,n) 	RDs32(saveptr EAriOFPR2S(pcoff,4),n)
#define riOFPR2s64S(pcoff,saveptr,n) 	RDs64(saveptr EAriOFPR2S(pcoff,8),n)
#define riOFPR2f32S(pcoff,saveptr,n) 	RDf32(saveptr EAriOFPR2S(pcoff,4),n)
#define riOFPR2f64S(pcoff,saveptr,n) 	RDf64(saveptr EAriOFPR2S(pcoff,8),n)

#define riOFPR2g16S(pcoff,saveptr,n) 	RDg16(saveptr EAriOFPR2S(pcoff,2),n)
#define riOFPR2g32S(pcoff,saveptr,n) 	RDg32(saveptr EAriOFPR2S(pcoff,4),n)
#define riOFPR2g64S(pcoff,saveptr,n) 	RDg64(saveptr EAriOFPR2S(pcoff,8),n)


#define wiOFPR2u16S(pcoff,what,n)	WDu16(EAriOFPR2S(pcoff,2),what,n)
#define wiOFPR2u32S(pcoff,what,n)	WDu32(EAriOFPR2S(pcoff,4),what,n)
#define wiOFPR2u64S(pcoff,what,n)	WDu64(EAriOFPR2S(pcoff,8),what,n)
#define wiOFPR2s16S(pcoff,what,n)	WDs16(EAriOFPR2S(pcoff,2),what,n)
#define wiOFPR2s32S(pcoff,what,n)	WDs32(EAriOFPR2S(pcoff,4),what,n)
#define wiOFPR2s64S(pcoff,what,n)	WDs64(EAriOFPR2S(pcoff,8),what,n)
#define wiOFPR2f32S(pcoff,what,n)	WDf32(EAriOFPR2S(pcoff,4),what,n)
#define wiOFPR2f64S(pcoff,what,n)	WDf64(EAriOFPR2S(pcoff,8),what,n)

// reads and writes from/to (n,(n,%fp),R3@)

// effective address
#define EAriOFPR3S(pcoff,scale) \
	((s32)is16(pcoff)+scale*(s32)R_R3+rOFPs32(pcoff+2,nP,0))


#define riOFPR3u16S(pcoff,saveptr,n) 	RDu16(saveptr EAriOFPR3S(pcoff,2),n)
#define riOFPR3u32S(pcoff,saveptr,n) 	RDu32(saveptr EAriOFPR3S(pcoff,4),n)
#define riOFPR3u64S(pcoff,saveptr,n) 	RDu64(saveptr EAriOFPR3S(pcoff,8),n)
#define riOFPR3s16S(pcoff,saveptr,n) 	RDs16(saveptr EAriOFPR3S(pcoff,2),n)
#define riOFPR3s32S(pcoff,saveptr,n) 	RDs32(saveptr EAriOFPR3S(pcoff,4),n)
#define riOFPR3s64S(pcoff,saveptr,n) 	RDs64(saveptr EAriOFPR3S(pcoff,8),n)
#define riOFPR3f32S(pcoff,saveptr,n) 	RDf32(saveptr EAriOFPR3S(pcoff,4),n)
#define riOFPR3f64S(pcoff,saveptr,n) 	RDf64(saveptr EAriOFPR3S(pcoff,8),n)

#define riOFPR3g16S(pcoff,saveptr,n) 	RDg16(saveptr EAriOFPR3S(pcoff,2),n)
#define riOFPR3g32S(pcoff,saveptr,n) 	RDg32(saveptr EAriOFPR3S(pcoff,4),n)
#define riOFPR3g64S(pcoff,saveptr,n) 	RDg64(saveptr EAriOFPR3S(pcoff,8),n)


#define wiOFPR3u16S(pcoff,what,n)	WDu16(EAriOFPR3S(pcoff,2),what,n)
#define wiOFPR3u32S(pcoff,what,n)	WDu32(EAriOFPR3S(pcoff,4),what,n)
#define wiOFPR3u64S(pcoff,what,n)	WDu64(EAriOFPR3S(pcoff,8),what,n)
#define wiOFPR3s16S(pcoff,what,n)	WDs16(EAriOFPR3S(pcoff,2),what,n)
#define wiOFPR3s32S(pcoff,what,n)	WDs32(EAriOFPR3S(pcoff,4),what,n)
#define wiOFPR3s64S(pcoff,what,n)	WDs64(EAriOFPR3S(pcoff,8),what,n)
#define wiOFPR3f32S(pcoff,what,n)	WDf32(EAriOFPR3S(pcoff,4),what,n)
#define wiOFPR3f64S(pcoff,what,n)	WDf64(EAriOFPR3S(pcoff,8),what,n)

// reads and writes from/to (%R2)

#define riR2u8(saveptr,n)	RDu8(saveptr R_R2,n)
#define riR2u16(saveptr,n)	RDu16(saveptr R_R2,n)
#define riR2u32(saveptr,n)	RDu32(saveptr R_R2,n)
#define riR2u64(saveptr,n)	RDu64(saveptr R_R2,n)
#define riR2s8(saveptr,n)	RDs8(saveptr R_R2,n)
#define riR2s16(saveptr,n)	RDs16(saveptr R_R2,n)
#define riR2s32(saveptr,n)	RDs32(saveptr R_R2,n)
#define riR2s64(saveptr,n)	RDs64(saveptr R_R2,n)
#define riR2f32(saveptr,n)	RDf32(saveptr R_R2,n)
#define riR2f64(saveptr,n)	RDf64(saveptr R_R2,n)

#define riR2g8(saveptr,n)	RDg8(saveptr R_R2,n)
#define riR2g16(saveptr,n)	RDg16(saveptr R_R2,n)
#define riR2g32(saveptr,n)	RDg32(saveptr R_R2,n)
#define riR2g64(saveptr,n)	RDg64(saveptr R_R2,n)


#define wiR2u8(what,n)	WDu8(R_R2,what,n)
#define wiR2u16(what,n)	WDu16(R_R2,what,n)
#define wiR2u32(what,n)	WDu32(R_R2,what,n)
#define wiR2u64(what,n)	WDu64(R_R2,what,n)
#define wiR2s8(what,n)	WDs8(R_R2,what,n)
#define wiR2s16(what,n)	WDs16(R_R2,what,n)
#define wiR2s32(what,n)	WDs32(R_R2,what,n)
#define wiR2s64(what,n)	WDs64(R_R2,what,n)
#define wiR2f32(what,n)	WDf32(R_R2,what,n)
#define wiR2f64(what,n)	WDf64(R_R2,what,n)

// reads and writes from/to (%R3)

#define riR3u8(saveptr,n)	RDu8(saveptr R_R3,n)
#define riR3u16(saveptr,n)	RDu16(saveptr R_R3,n)
#define riR3u32(saveptr,n)	RDu32(saveptr R_R3,n)
#define riR3u64(saveptr,n)	RDu64(saveptr R_R3,n)
#define riR3s8(saveptr,n)	RDs8(saveptr R_R3,n)
#define riR3s16(saveptr,n)	RDs16(saveptr R_R3,n)
#define riR3s32(saveptr,n)	RDs32(saveptr R_R3,n)
#define riR3s64(saveptr,n)	RDs64(saveptr R_R3,n)
#define riR3f32(saveptr,n)	RDf32(saveptr R_R3,n)
#define riR3f64(saveptr,n)	RDf64(saveptr R_R3,n)

#define riR3g8(saveptr,n)	RDg8(saveptr R_R3,n)
#define riR3g16(saveptr,n)	RDg16(saveptr R_R3,n)
#define riR3g32(saveptr,n)	RDg32(saveptr R_R3,n)
#define riR3g64(saveptr,n)	RDg64(saveptr R_R3,n)


#define wiR3u8(what,n)	WDu8(R_R3,what,n)
#define wiR3u16(what,n)	WDu16(R_R3,what,n)
#define wiR3u32(what,n)	WDu32(R_R3,what,n)
#define wiR3u64(what,n)	WDu64(R_R3,what,n)
#define wiR3s8(what,n)	WDs8(R_R3,what,n)
#define wiR3s16(what,n)	WDs16(R_R3,what,n)
#define wiR3s32(what,n)	WDs32(R_R3,what,n)
#define wiR3s64(what,n)	WDs64(R_R3,what,n)
#define wiR3f32(what,n)	WDf32(R_R3,what,n)
#define wiR3f64(what,n)	WDf64(R_R3,what,n)


// reads and writes from/to (n,%R2)

#define riOR2u8(pcoff,saveptr,n)	RDu8(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2u16(pcoff,saveptr,n)	RDu16(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2u32(pcoff,saveptr,n)	RDu32(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2u64(pcoff,saveptr,n)	RDu64(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2s8(pcoff,saveptr,n)	RDs8(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2s16(pcoff,saveptr,n)	RDs16(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2s32(pcoff,saveptr,n)	RDs32(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2s64(pcoff,saveptr,n)	RDs64(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2f32(pcoff,saveptr,n)	RDf32(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2f64(pcoff,saveptr,n)	RDf64(saveptr (s32)is16(pcoff)+R_R2,n)

#define riOR2g8(pcoff,saveptr,n)	RDg8(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2g16(pcoff,saveptr,n)	RDg16(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2g32(pcoff,saveptr,n)	RDg32(saveptr (s32)is16(pcoff)+R_R2,n)
#define riOR2g64(pcoff,saveptr,n)	RDg64(saveptr (s32)is16(pcoff)+R_R2,n)


#define wiOR2u8(pcoff,what,n)	WDu8((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2u16(pcoff,what,n)	WDu16((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2u32(pcoff,what,n)	WDu32((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2u64(pcoff,what,n)	WDu64((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2s8(pcoff,what,n)	WDs8((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2s16(pcoff,what,n)	WDs16((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2s32(pcoff,what,n)	WDs32((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2s64(pcoff,what,n)	WDs64((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2f32(pcoff,what,n)	WDf32((s32)is16(pcoff)+R_R2,what,n)
#define wiOR2f64(pcoff,what,n)	WDf64((s32)is16(pcoff)+R_R2,what,n)

// reads and writes from/to (n,%R3)

#define riOR3u8(pcoff,saveptr,n)	RDu8(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3u16(pcoff,saveptr,n)	RDu16(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3u32(pcoff,saveptr,n)	RDu32(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3u64(pcoff,saveptr,n)	RDu64(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3s8(pcoff,saveptr,n)	RDs8(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3s16(pcoff,saveptr,n)	RDs16(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3s32(pcoff,saveptr,n)	RDs32(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3s64(pcoff,saveptr,n)	RDs64(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3f32(pcoff,saveptr,n)	RDf32(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3f64(pcoff,saveptr,n)	RDf64(saveptr (s32)is16(pcoff)+R_R3,n)

#define riOR3g8(pcoff,saveptr,n)	RDg8(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3g16(pcoff,saveptr,n)	RDg16(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3g32(pcoff,saveptr,n)	RDg32(saveptr (s32)is16(pcoff)+R_R3,n)
#define riOR3g64(pcoff,saveptr,n)	RDg64(saveptr (s32)is16(pcoff)+R_R3,n)


#define wiOR3u8(pcoff,what,n)	WDu8((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3u16(pcoff,what,n)	WDu16((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3u32(pcoff,what,n)	WDu32((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3u64(pcoff,what,n)	WDu64((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3s8(pcoff,what,n)	WDs8((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3s16(pcoff,what,n)	WDs16((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3s32(pcoff,what,n)	WDs32((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3s64(pcoff,what,n)	WDs64((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3f32(pcoff,what,n)	WDf32((s32)is16(pcoff)+R_R3,what,n)
#define wiOR3f64(pcoff,what,n)	WDf64((s32)is16(pcoff)+R_R3,what,n)

// reads and writes from/to (%R2,%R3)

#define riR2R3u8(saveptr,n)		RDu8(saveptr R_R2+R_R3,n)
#define riR2R3u16(saveptr,n)	RDu16(saveptr R_R2+R_R3,n)
#define riR2R3u32(saveptr,n)	RDu32(saveptr R_R2+R_R3,n)
#define riR2R3u64(saveptr,n)	RDu64(saveptr R_R2+R_R3,n)
#define riR2R3s8(saveptr,n)		RDs8(saveptr R_R2+R_R3,n)
#define riR2R3s16(saveptr,n)	RDs16(saveptr R_R2+R_R3,n)
#define riR2R3s32(saveptr,n)	RDs32(saveptr R_R2+R_R3,n)
#define riR2R3s64(saveptr,n)	RDs64(saveptr R_R2+R_R3,n)
#define riR2R3f32(saveptr,n)	RDf32(saveptr R_R2+R_R3,n)
#define riR2R3f64(saveptr,n)	RDf64(saveptr R_R2+R_R3,n)

#define riR2R3g8(saveptr,n)		RDg8(saveptr R_R2+R_R3,n)
#define riR2R3g16(saveptr,n)	RDg16(saveptr R_R2+R_R3,n)
#define riR2R3g32(saveptr,n)	RDg32(saveptr R_R2+R_R3,n)
#define riR2R3g64(saveptr,n)	RDg64(saveptr R_R2+R_R3,n)


#define wiR2R3u8(what,n)	WDu8(R_R2+R_R3,what,n)
#define wiR2R3u16(what,n)	WDu16(R_R2+R_R3,what,n)
#define wiR2R3u32(what,n)	WDu32(R_R2+R_R3,what,n)
#define wiR2R3u64(what,n)	WDu64(R_R2+R_R3,what,n)
#define wiR2R3s8(what,n)	WDs8(R_R2+R_R3,what,n)
#define wiR2R3s16(what,n)	WDs16(R_R2+R_R3,what,n)
#define wiR2R3s32(what,n)	WDs32(R_R2+R_R3,what,n)
#define wiR2R3s64(what,n)	WDs64(R_R2+R_R3,what,n)
#define wiR2R3f32(what,n)	WDf32(R_R2+R_R3,what,n)
#define wiR2R3f64(what,n)	WDf64(R_R2+R_R3,what,n)


// reads and writes from/to (n,%R2,%R3)

#define riOR2R3u8(pcoff,saveptr,n)	RDu8(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3u16(pcoff,saveptr,n)	RDu16(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3u32(pcoff,saveptr,n)	RDu32(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3u64(pcoff,saveptr,n)	RDu64(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3s8(pcoff,saveptr,n)	RDs8(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3s16(pcoff,saveptr,n)	RDs16(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3s32(pcoff,saveptr,n)	RDs32(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3s64(pcoff,saveptr,n)	RDs64(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3f32(pcoff,saveptr,n)	RDf32(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3f64(pcoff,saveptr,n)	RDf64(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)

#define riOR2R3g8(pcoff,saveptr,n)	RDg8(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3g16(pcoff,saveptr,n)	RDg16(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3g32(pcoff,saveptr,n)	RDg32(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)
#define riOR2R3g64(pcoff,saveptr,n)	RDg64(saveptr (s32)is16(pcoff)+R_R2+R_R3,n)


#define wiOR2R3u8(pcoff,what,n)		WDu8((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3u16(pcoff,what,n)	WDu16((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3u32(pcoff,what,n)	WDu32((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3u64(pcoff,what,n)	WDu64((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3s8(pcoff,what,n)		WDs8((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3s16(pcoff,what,n)	WDs16((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3s32(pcoff,what,n)	WDs32((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3s64(pcoff,what,n)	WDs64((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3f32(pcoff,what,n)	WDf32((s32)is16(pcoff)+R_R2+R_R3,what,n)
#define wiOR2R3f64(pcoff,what,n)	WDf64((s32)is16(pcoff)+R_R2+R_R3,what,n)

// reads and writes from/to (%R2,%R3@)

#define riR2R3u16S(saveptr,n)	RDu16(saveptr R_R2+R_R3*2,n)
#define riR2R3u32S(saveptr,n)	RDu32(saveptr R_R2+R_R3*4,n)
#define riR2R3u64S(saveptr,n)	RDu64(saveptr R_R2+R_R3*8,n)
#define riR2R3s16S(saveptr,n)	RDs16(saveptr R_R2+R_R3*2,n)
#define riR2R3s32S(saveptr,n)	RDs32(saveptr R_R2+R_R3*4,n)
#define riR2R3s64S(saveptr,n)	RDs64(saveptr R_R2+R_R3*8,n)
#define riR2R3f32S(saveptr,n)	RDf32(saveptr R_R2+R_R3*4,n)
#define riR2R3f64S(saveptr,n)	RDf64(saveptr R_R2+R_R3*8,n)

#define riR2R3g16S(saveptr,n)	RDg16(saveptr R_R2+R_R3*2,n)
#define riR2R3g32S(saveptr,n)	RDg32(saveptr R_R2+R_R3*4,n)
#define riR2R3g64S(saveptr,n)	RDg64(saveptr R_R2+R_R3*8,n)


#define wiR2R3u16S(what,n)	WDu16(R_R2+R_R3*2,what,n)
#define wiR2R3u32S(what,n)	WDu32(R_R2+R_R3*4,what,n)
#define wiR2R3u64S(what,n)	WDu64(R_R2+R_R3*8,what,n)
#define wiR2R3s16S(what,n)	WDs16(R_R2+R_R3*2,what,n)
#define wiR2R3s32S(what,n)	WDs32(R_R2+R_R3*4,what,n)
#define wiR2R3s64S(what,n)	WDs64(R_R2+R_R3*8,what,n)
#define wiR2R3f32S(what,n)	WDf32(R_R2+R_R3*4,what,n)
#define wiR2R3f64S(what,n)	WDf64(R_R2+R_R3*8,what,n)


// reads and writes from/to (%R3,%R2@)

#define riR3R2u16S(saveptr,n)	RDu16(saveptr R_R3+R_R2*2,n)
#define riR3R2u32S(saveptr,n)	RDu32(saveptr R_R3+R_R2*4,n)
#define riR3R2u64S(saveptr,n)	RDu64(saveptr R_R3+R_R2*8,n)
#define riR3R2s16S(saveptr,n)	RDs16(saveptr R_R3+R_R2*2,n)
#define riR3R2s32S(saveptr,n)	RDs32(saveptr R_R3+R_R2*4,n)
#define riR3R2s64S(saveptr,n)	RDs64(saveptr R_R3+R_R2*8,n)
#define riR3R2f32S(saveptr,n)	RDf32(saveptr R_R3+R_R2*4,n)
#define riR3R2f64S(saveptr,n)	RDf64(saveptr R_R3+R_R2*8,n)

#define riR3R2g16S(saveptr,n)	RDg16(saveptr R_R3+R_R2*2,n)
#define riR3R2g32S(saveptr,n)	RDg32(saveptr R_R3+R_R2*4,n)
#define riR3R2g64S(saveptr,n)	RDg64(saveptr R_R3+R_R2*8,n)


#define wiR3R2u16S(what,n)	WDu16(R_R3+R_R2*2,what,n)
#define wiR3R2u32S(what,n)	WDu32(R_R3+R_R2*4,what,n)
#define wiR3R2u64S(what,n)	WDu64(R_R3+R_R2*8,what,n)
#define wiR3R2s16S(what,n)	WDs16(R_R3+R_R2*2,what,n)
#define wiR3R2s32S(what,n)	WDs32(R_R3+R_R2*4,what,n)
#define wiR3R2s64S(what,n)	WDs64(R_R3+R_R2*8,what,n)
#define wiR3R2f32S(what,n)	WDf32(R_R3+R_R2*4,what,n)
#define wiR3R2f64S(what,n)	WDf64(R_R3+R_R2*8,what,n)

// reads and writes from/to (n,%R2,%R3@)

#define riOR2R3u16S(pcoff,saveptr,n)	RDu16(saveptr (s32)is16(pcoff)+R_R2+R_R3*2,n)
#define riOR2R3u32S(pcoff,saveptr,n)	RDu32(saveptr (s32)is16(pcoff)+R_R2+R_R3*4,n)
#define riOR2R3u64S(pcoff,saveptr,n)	RDu64(saveptr (s32)is16(pcoff)+R_R2+R_R3*8,n)
#define riOR2R3s16S(pcoff,saveptr,n)	RDs16(saveptr (s32)is16(pcoff)+R_R2+R_R3*2,n)
#define riOR2R3s32S(pcoff,saveptr,n)	RDs32(saveptr (s32)is16(pcoff)+R_R2+R_R3*4,n)
#define riOR2R3s64S(pcoff,saveptr,n)	RDs64(saveptr (s32)is16(pcoff)+R_R2+R_R3*8,n)
#define riOR2R3f32S(pcoff,saveptr,n)	RDf32(saveptr (s32)is16(pcoff)+R_R2+R_R3*4,n)
#define riOR2R3f64S(pcoff,saveptr,n)	RDf64(saveptr (s32)is16(pcoff)+R_R2+R_R3*8,n)

#define riOR2R3g16S(pcoff,saveptr,n)	RDg16(saveptr (s32)is16(pcoff)+R_R2+R_R3*2,n)
#define riOR2R3g32S(pcoff,saveptr,n)	RDg32(saveptr (s32)is16(pcoff)+R_R2+R_R3*4,n)
#define riOR2R3g64S(pcoff,saveptr,n)	RDg64(saveptr (s32)is16(pcoff)+R_R2+R_R3*8,n)


#define wiOR2R3u16S(pcoff,what,n)	WDu16((s32)is16(pcoff)+R_R2+R_R3*2,what,n)
#define wiOR2R3u32S(pcoff,what,n)	WDu32((s32)is16(pcoff)+R_R2+R_R3*4,what,n)
#define wiOR2R3u64S(pcoff,what,n)	WDu64((s32)is16(pcoff)+R_R2+R_R3*8,what,n)
#define wiOR2R3s16S(pcoff,what,n)	WDs16((s32)is16(pcoff)+R_R2+R_R3*2,what,n)
#define wiOR2R3s32S(pcoff,what,n)	WDs32((s32)is16(pcoff)+R_R2+R_R3*4,what,n)
#define wiOR2R3s64S(pcoff,what,n)	WDs64((s32)is16(pcoff)+R_R2+R_R3*8,what,n)
#define wiOR2R3f32S(pcoff,what,n)	WDf32((s32)is16(pcoff)+R_R2+R_R3*4,what,n)
#define wiOR2R3f64S(pcoff,what,n)	WDf64((s32)is16(pcoff)+R_R2+R_R3*8,what,n)


// reads and writes from/to (n,%R3,%R2@)

#define riOR3R2u16S(pcoff,saveptr,n)	RDu16(saveptr (s32)is16(pcoff)+R_R3+R_R2*2,n)
#define riOR3R2u32S(pcoff,saveptr,n)	RDu32(saveptr (s32)is16(pcoff)+R_R3+R_R2*4,n)
#define riOR3R2u64S(pcoff,saveptr,n)	RDu64(saveptr (s32)is16(pcoff)+R_R3+R_R2*8,n)
#define riOR3R2s16S(pcoff,saveptr,n)	RDs16(saveptr (s32)is16(pcoff)+R_R3+R_R2*2,n)
#define riOR3R2s32S(pcoff,saveptr,n)	RDs32(saveptr (s32)is16(pcoff)+R_R3+R_R2*4,n)
#define riOR3R2s64S(pcoff,saveptr,n)	RDs64(saveptr (s32)is16(pcoff)+R_R3+R_R2*8,n)
#define riOR3R2f32S(pcoff,saveptr,n)	RDf32(saveptr (s32)is16(pcoff)+R_R3+R_R2*4,n)
#define riOR3R2f64S(pcoff,saveptr,n)	RDf64(saveptr (s32)is16(pcoff)+R_R3+R_R2*8,n)

#define riOR3R2g16S(pcoff,saveptr,n)	RDg16(saveptr (s32)is16(pcoff)+R_R3+R_R2*2,n)
#define riOR3R2g32S(pcoff,saveptr,n)	RDg32(saveptr (s32)is16(pcoff)+R_R3+R_R2*4,n)
#define riOR3R2g64S(pcoff,saveptr,n)	RDg64(saveptr (s32)is16(pcoff)+R_R3+R_R2*8,n)


#define wiOR3R2u16S(pcoff,what,n)	WDu16((s32)is16(pcoff)+R_R3+R_R2*2,what,n)
#define wiOR3R2u32S(pcoff,what,n)	WDu32((s32)is16(pcoff)+R_R3+R_R2*4,what,n)
#define wiOR3R2u64S(pcoff,what,n)	WDu64((s32)is16(pcoff)+R_R3+R_R2*8,what,n)
#define wiOR3R2s16S(pcoff,what,n)	WDs16((s32)is16(pcoff)+R_R3+R_R2*2,what,n)
#define wiOR3R2s32S(pcoff,what,n)	WDs32((s32)is16(pcoff)+R_R3+R_R2*4,what,n)
#define wiOR3R2s64S(pcoff,what,n)	WDs64((s32)is16(pcoff)+R_R3+R_R2*8,what,n)
#define wiOR3R2f32S(pcoff,what,n)	WDf32((s32)is16(pcoff)+R_R3+R_R2*4,what,n)
#define wiOR3R2f64S(pcoff,what,n)	WDf64((s32)is16(pcoff)+R_R3+R_R2*8,what,n)

// reads and writes from/to %R0

#define rR0u8	(gR_R0.gw[I_LOW].ub[I_LOW])
#define rR0u16	(gR_R0.uw[I_LOW])
#define rR0u32	(gR_R0.ul[0])
#define rR0u64	ILLEGAL_READ
#define rR0s8	(gR_R0.gw[I_LOW].b[I_LOW])
#define rR0s16	(gR_R0.w[I_LOW])
#define rR0s32	(gR_R0.l[0])
#define rR0s64	ILLEGAL_READ
#define rR0f32	(gR_R0.f[0])
#define rR0f64	ILLEGAL_READ

#define rR0g8	(gR_R0.gw[I_LOW].gb[I_LOW])
#define rR0g16	(gR_R0.gw[I_LOW])
#define rR0g32	(gR_R0)

#define BASErR0g8	(gR_R0)
#define BASErR0g16	(gR_R0)
#define BASErR0g32	(gR_R0)


#define wR0u8(what) 	(gR_R0.gw[I_LOW].ub[I_LOW]=(u8)(what))
#define wR0u16(what) 	(gR_R0.uw[I_LOW]=(u16)(what))
#define wR0u32(what) 	(gR_R0.ul[0]=(u32)(what))
#define wR0u64(what) 	ILLEGAL_WRITE
#define wR0s8(what) 	(gR_R0.gw[I_LOW].b[I_LOW]=(s8)(what))
#define wR0s16(what) 	(gR_R0.w[I_LOW]=(s16)(what))
#define wR0s32(what) 	(gR_R0.l[0]=(s32)(what))
#define wR0s64(what) 	ILLEGAL_WRITE
#define wR0f32(what) 	(gR_R0.f[0]=(f32)(what))
#define wR0f64(what)	ILLEGAL_WRITE


// reads and writes from/to %R1

#define rR1u8	(gR_R1.gw[I_LOW].ub[I_LOW])
#define rR1u16	(gR_R1.uw[I_LOW])
#define rR1u32	(gR_R1.ul[0])
#define rR1u64	ILLEGAL_READ
#define rR1s8	(gR_R1.gw[I_LOW].b[I_LOW])
#define rR1s16	(gR_R1.w[I_LOW])
#define rR1s32	(gR_R1.l[0])
#define rR1s64	ILLEGAL_READ
#define rR1f32	(gR_R1.f[0])
#define rR1f64	ILLEGAL_READ

#define rR1g8	(gR_R1.gw[I_LOW].gb[I_LOW])
#define rR1g16	(gR_R1.gw[I_LOW])
#define rR1g32	(gR_R1)

#define BASErR1g8	(gR_R1)
#define BASErR1g16	(gR_R1)
#define BASErR1g32	(gR_R1)


#define wR1u8(what) 	(gR_R1.gw[I_LOW].ub[I_LOW]=(u8)(what))
#define wR1u16(what) 	(gR_R1.uw[I_LOW]=(u16)(what))
#define wR1u32(what) 	(gR_R1.ul[0]=(u32)(what))
#define wR1u64(what) 	ILLEGAL_WRITE
#define wR1s8(what) 	(gR_R1.gw[I_LOW].b[I_LOW]=(s8)(what))
#define wR1s16(what) 	(gR_R1.w[I_LOW]=(s16)(what))
#define wR1s32(what) 	(gR_R1.l[0]=(s32)(what))
#define wR1s64(what) 	ILLEGAL_WRITE
#define wR1f32(what) 	(gR_R1.f[0]=(f32)(what))
#define wR1f64(what)	ILLEGAL_WRITE


// reads and writes from/to %R2

#define rR2u8	(gR_R2.gw[I_LOW].ub[I_LOW])
#define rR2u16	(gR_R2.uw[I_LOW])
#define rR2u32	(gR_R2.ul[0])
#define rR2u64	ILLEGAL_READ
#define rR2s8	(gR_R2.gw[I_LOW].b[I_LOW])
#define rR2s16	(gR_R2.w[I_LOW])
#define rR2s32	(gR_R2.l[0])
#define rR2s64	ILLEGAL_READ
#define rR2f32	(gR_R2.f[0])
#define rR2f64	ILLEGAL_READ

#define rR2g8	(gR_R2.gw[I_LOW].gb[I_LOW])
#define rR2g16	(gR_R2.gw[I_LOW])
#define rR2g32	(gR_R2)

#define BASErR2g8	(gR_R2)
#define BASErR2g16	(gR_R2)
#define BASErR2g32	(gR_R2)


#define wR2u8(what) 	(gR_R2.gw[I_LOW].ub[I_LOW]=(u8)(what))
#define wR2u16(what) 	(gR_R2.uw[I_LOW]=(u16)(what))
#define wR2u32(what) 	(gR_R2.ul[0]=(u32)(what))
#define wR2u64(what) 	ILLEGAL_WRITE
#define wR2s8(what) 	(gR_R2.gw[I_LOW].b[I_LOW]=(s8)(what))
#define wR2s16(what) 	(gR_R2.w[I_LOW]=(s16)(what))
#define wR2s32(what) 	(gR_R2.l[0]=(s32)(what))
#define wR2s64(what) 	ILLEGAL_WRITE
#define wR2f32(what) 	(gR_R2.f[0]=(f32)(what))
#define wR2f64(what)	ILLEGAL_WRITE



// reads and writes from/to %R3

#define rR3u8	(gR_R3.gw[I_LOW].ub[I_LOW])
#define rR3u16	(gR_R3.uw[I_LOW])
#define rR3u32	(gR_R3.ul[0])
#define rR3u64	ILLEGAL_READ
#define rR3s8	(gR_R3.gw[I_LOW].b[I_LOW])
#define rR3s16	(gR_R3.w[I_LOW])
#define rR3s32	(gR_R3.l[0])
#define rR3s64	ILLEGAL_READ
#define rR3f32	(gR_R3.f[0])
#define rR3f64	ILLEGAL_READ

#define rR3g8	(gR_R3.gw[I_LOW].gb[I_LOW])
#define rR3g16	(gR_R3.gw[I_LOW])
#define rR3g32	(gR_R3)

#define BASErR3g8	(gR_R3)
#define BASErR3g16	(gR_R3)
#define BASErR3g32	(gR_R3)


#define wR3u8(what) 	(gR_R3.gw[I_LOW].ub[I_LOW]=(u8)(what))
#define wR3u16(what) 	(gR_R3.uw[I_LOW]=(u16)(what))
#define wR3u32(what) 	(gR_R3.ul[0]=(u32)(what))
#define wR3u64(what) 	ILLEGAL_WRITE
#define wR3s8(what) 	(gR_R3.gw[I_LOW].b[I_LOW]=(s8)(what))
#define wR3s16(what) 	(gR_R3.w[I_LOW]=(s16)(what))
#define wR3s32(what) 	(gR_R3.l[0]=(s32)(what))
#define wR3s64(what) 	ILLEGAL_WRITE
#define wR3f32(what) 	(gR_R3.f[0]=(f32)(what))
#define wR3f64(what)	ILLEGAL_WRITE


// reads and writes from/to %SP

#define rSPu8	ILLEGAL_READ
#define rSPu16	ILLEGAL_READ
#define rSPu32	((u32)R_SP)
#define rSPu64	ILLEGAL_READ
#define rSPs8	ILLEGAL_READ
#define rSPs16	ILLEGAL_READ
#define rSPs32	((s32)R_SP)
#define rSPs64	ILLEGAL_READ
#define rSPf32	ILLEGAL_READ
#define rSPf64	ILLEGAL_READ

#define rSPg32	(gR_SP)
#define BASErSPg32	(gR_SP)


#define wSPu8(what)		ILLEGAL_WRITE
#define wSPu16(what)	ILLEGAL_WRITE
#define wSPu32(what) 	(R_SP=(u32)(what))
#define wSPu64(what)	ILLEGAL_WRITE
#define wSPs8(what)		ILLEGAL_WRITE
#define wSPs16(what)	ILLEGAL_WRITE
#define wSPs32(what) 	(R_SP=(s32)(what))
#define wSPs64(what)	ILLEGAL_WRITE
#define wSPf32(what)	ILLEGAL_WRITE
#define wSPf64(what)	ILLEGAL_WRITE

// reads from  %LASTSP

#define rLASTSPu8	ILLEGAL_READ
#define rLASTSPu16	ILLEGAL_READ
#define rLASTSPu32	((u32)R_LASTSP)
#define rLASTSPu64	ILLEGAL_READ
#define rLASTSPs8	ILLEGAL_READ
#define rLASTSPs16	ILLEGAL_READ
#define rLASTSPs32	((s32)R_LASTSP)
#define rLASTSPs64	ILLEGAL_READ
#define rLASTSPf32	ILLEGAL_READ
#define rLASTSPf64	ILLEGAL_READ

#define rLASTSPg32	(gR_LASTSP)
#define BASErLASTSPg32	(gR_LASTSP)



// reads and writes from/to %PC

#define rPCu8	ILLEGAL_READ
#define rPCu16	ILLEGAL_READ
#define rPCu32	((u32)R_PC)
#define rPCu64	ILLEGAL_READ
#define rPCs8	ILLEGAL_READ
#define rPCs16	ILLEGAL_READ
#define rPCs32	((s32)R_PC)
#define rPCs64	ILLEGAL_READ
#define rPCf32	ILLEGAL_READ
#define rPCf64	ILLEGAL_READ

#define rPCg32	(gR_PC)
#define BASErPCg32	(gR_PC)


#define wPCu8(what)		ILLEGAL_WRITE
#define wPCu16(what)	ILLEGAL_WRITE
#define wPCu32(what) 	(R_PC=(u32)(what))
#define wPCu64(what)	ILLEGAL_WRITE
#define wPCs8(what)		ILLEGAL_WRITE
#define wPCs16(what)	ILLEGAL_WRITE
#define wPCs32(what) 	(R_PC=(s32)(what))
#define wPCs64(what)	ILLEGAL_WRITE
#define wPCf32(what)	ILLEGAL_WRITE
#define wPCf64(what)	ILLEGAL_WRITE

// reads from  %LASTPC

#define rLASTPCu8		ILLEGAL_READ
#define rLASTPCu16	ILLEGAL_READ
#define rLASTPCu32	((u32)R_LASTPC)
#define rLASTPCu64	ILLEGAL_READ
#define rLASTPCs8		ILLEGAL_READ
#define rLASTPCs16	ILLEGAL_READ
#define rLASTPCs32	((s32)R_LASTPC)
#define rLASTPCs64	ILLEGAL_READ
#define rLASTPCf32	ILLEGAL_READ
#define rLASTPCf64	ILLEGAL_READ

#define rLASTPCg32	(gR_LASTPC)
#define BASErLASTPCg32	(gR_LASTPC)


// reads and writes from/to %FP

#define rFPu8	ILLEGAL_READ
#define rFPu16	ILLEGAL_READ
#define rFPu32	((u32)R_FP)
#define rFPu64	ILLEGAL_READ
#define rFPs8	ILLEGAL_READ
#define rFPs16	ILLEGAL_READ
#define rFPs32	((s32)R_FP)
#define rFPs64	ILLEGAL_READ
#define rFPf32	ILLEGAL_READ
#define rFPf64	ILLEGAL_READ

#define rFPg32	(gR_FP)
#define BASErFPg32	(gR_FP)


#define wFPu8(what)		ILLEGAL_WRITE
#define wFPu16(what)	ILLEGAL_WRITE
#define wFPu32(what)	(R_FP=(u32)(what))
#define wFPu64(what)	ILLEGAL_WRITE
#define wFPs8(what)		ILLEGAL_WRITE
#define wFPs16(what)	ILLEGAL_WRITE
#define wFPs32(what) 	(R_FP=(s32)(what))
#define wFPs64(what)	ILLEGAL_WRITE
#define wFPf32(what)	ILLEGAL_WRITE
#define wFPf64(what)	ILLEGAL_WRITE

// reads and writes from/to %Q0

#define rQ0u8	ILLEGAL_READ
#define rQ0u16	ILLEGAL_READ
#define rQ0u32	(gR_Q0.ul[I_LOW])
#define rQ0u64	(gR_Q0.uq[0])
#define rQ0s8	ILLEGAL_READ
#define rQ0s16	ILLEGAL_READ
#define rQ0s32	(gR_Q0.l[I_LOW])
#define rQ0s64	(gR_Q0.q[0])
#define rQ0f32	(gR_Q0.f[I_LOW])
#define rQ0f64	(gR_Q0.d[0])

#define rQ0g64	(gR_Q0.gq[0])
#define BASErQ0g64	(gR_Q0)


#define wQ0u8(what) 	ILLEGAL_WRITE
#define wQ0u16(what)	ILLEGAL_WRITE
#define wQ0u32(what) 	(gR_Q0.ul[I_LOW]=(u32)(what))
#define wQ0u64(what) 	(gR_Q0.uq[0]=(u64)(what))
#define wQ0s8(what) 	ILLEGAL_WRITE
#define wQ0s16(what)	ILLEGAL_WRITE
#define wQ0s32(what) 	(gR_Q0.l[I_LOW]=(s32)(what))
#define wQ0s64(what) 	(gR_Q0.q[0]=(s64)(what))
#define wQ0f32(what) 	(gR_Q0.f[I_LOW]=(f32)(what))
#define wQ0f64(what) 	(gR_Q0.d[0]=(f64)(what))

// reads and writes from/to %Q1

#define rQ1u8	ILLEGAL_READ
#define rQ1u16	ILLEGAL_READ
#define rQ1u32	(gR_Q1.ul[I_LOW])
#define rQ1u64	(gR_Q1.uq[0])
#define rQ1s8	ILLEGAL_READ
#define rQ1s16	ILLEGAL_READ
#define rQ1s32	(gR_Q1.l[I_LOW])
#define rQ1s64	(gR_Q1.q[0])
#define rQ1f32	(gR_Q1.f[I_LOW])
#define rQ1f64	(gR_Q1.d[0])

#define rQ1g64	(gR_Q1.gq[0])
#define BASErQ1g64	(gR_Q1)


#define wQ1u8(what) 	ILLEGAL_WRITE
#define wQ1u16(what)	ILLEGAL_WRITE
#define wQ1u32(what) 	(gR_Q1.ul[I_LOW]=(u32)(what))
#define wQ1u64(what) 	(gR_Q1.uq[0]=(u64)(what))
#define wQ1s8(what) 	ILLEGAL_WRITE
#define wQ1s16(what)	ILLEGAL_WRITE
#define wQ1s32(what) 	(gR_Q1.l[I_LOW]=(s32)(what))
#define wQ1s64(what) 	(gR_Q1.q[0]=(s64)(what))
#define wQ1f32(what) 	(gR_Q1.f[I_LOW]=(f32)(what))
#define wQ1f64(what) 	(gR_Q1.d[0]=(f64)(what))



// reads and writes from/to %FP0

#define rFP0u8		ILLEGAL_READ
#define rFP0u16		ILLEGAL_READ
#define rFP0u32	(gR_FP0.ul[0])
#define rFP0u64	(gR_FP0.uq[0])
#define rFP0s8		ILLEGAL_READ
#define rFP0s16		ILLEGAL_READ
#define rFP0s32	(gR_FP0.l[0])
#define rFP0s64	(gR_FP0.q[0])
#define rFP0f32	(gR_FP0.f[0])
#define rFP0f64	(gR_FP0.d[0])

#define rFP0g64		(gR_FP0)
#define BASErFP0g64	(gR_FP0)
#define rFP0g32		(gR_FP0)
#define BASErFP0g32	(gR_FP0)


#define wFP0u8(what)	ILLEGAL_WRITE
#define wFP0u16(what)	ILLEGAL_WRITE
#define wFP0u32(what) 	(gR_FP0.ul[0]=(u32)(what))
#define wFP0u64(what) 	(gR_FP0.uq[0]=(u64)(what))
#define wFP0s8(what)	ILLEGAL_WRITE
#define wFP0s16(what)	ILLEGAL_WRITE
#define wFP0s32(what) 	(gR_FP0.l[0]=(s32)(what))
#define wFP0s64(what) 	(gR_FP0.q[0]=(s64)(what))
#define wFP0f32(what) 	(gR_FP0.f[0]=(f32)(what))
#define wFP0f64(what) 	(gR_FP0.d[0]=(f64)(what))



// reads and writes from/to %FP1

#define rFP1u8		ILLEGAL_READ
#define rFP1u16		ILLEGAL_READ
#define rFP1u32	(gR_FP1.ul[0])
#define rFP1u64	(gR_FP1.uq[0])
#define rFP1s8		ILLEGAL_READ
#define rFP1s16		ILLEGAL_READ
#define rFP1s32	(gR_FP1.l[0])
#define rFP1s64	(gR_FP1.q[0])
#define rFP1f32	(gR_FP1.f[0])
#define rFP1f64	(gR_FP1.d[0])

#define rFP1g64		(gR_FP1)
#define BASErFP1g64	(gR_FP1)
#define rFP1g32		(gR_FP1)
#define BASErFP1g32	(gR_FP1)


#define wFP1u8(what)	ILLEGAL_WRITE
#define wFP1u16(what)	ILLEGAL_WRITE
#define wFP1u32(what) 	(gR_FP1.ul[0]=(u32)(what))
#define wFP1u64(what) 	(gR_FP1.uq[0]=(u64)(what))
#define wFP1s8(what)	ILLEGAL_WRITE
#define wFP1s16(what)	ILLEGAL_WRITE
#define wFP1s32(what) 	(gR_FP1.l[0]=(s32)(what))
#define wFP1s64(what) 	(gR_FP1.q[0]=(s64)(what))
#define wFP1f32(what) 	(gR_FP1.f[0]=(f32)(what))
#define wFP1f64(what) 	(gR_FP1.d[0]=(f64)(what))

// reads and writes from/to %FP2

#define rFP2u8		ILLEGAL_READ
#define rFP2u16		ILLEGAL_READ
#define rFP2u32	(gR_FP2.ul[0])
#define rFP2u64	(gR_FP2.uq[0])
#define rFP2s8		ILLEGAL_READ
#define rFP2s16		ILLEGAL_READ
#define rFP2s32	(gR_FP2.l[0])
#define rFP2s64	(gR_FP2.q[0])
#define rFP2f32	(gR_FP2.f[0])
#define rFP2f64	(gR_FP2.d[0])

#define rFP2g64		(gR_FP2)
#define BASErFP2g64	(gR_FP2)
#define rFP2g32		(gR_FP2)
#define BASErFP2g32	(gR_FP2)


#define wFP2u8(what)	ILLEGAL_WRITE
#define wFP2u16(what)	ILLEGAL_WRITE
#define wFP2u32(what) 	(gR_FP2.ul[0]=(u32)(what))
#define wFP2u64(what) 	(gR_FP2.uq[0]=(u64)(what))
#define wFP2s8(what)	ILLEGAL_WRITE
#define wFP2s16(what)	ILLEGAL_WRITE
#define wFP2s32(what) 	(gR_FP2.l[0]=(s32)(what))
#define wFP2s64(what) 	(gR_FP2.q[0]=(s64)(what))
#define wFP2f32(what) 	(gR_FP2.f[0]=(f32)(what))
#define wFP2f64(what) 	(gR_FP2.d[0]=(f64)(what))






// ***************** operations

#define oUNOP(s1,op,tp) 	((tp)( op (tp)(s1)))
#define oBINOP(s1,s2,op,tp) 	((tp)((tp)(s2) op (tp)(s1)))


// negate number  -s1

#define oNEGu8			oNEGs8
#define oNEGu16			oNEGs16
#define oNEGu32			oNEGs32
#define oNEGu64			oNEGs64
#define oNEGs8(s1)	 	oUNOP(s1,-,s8)
#define oNEGs16(s1) 	oUNOP(s1,-,s16)
#define oNEGs32(s1) 	oUNOP(s1,-,s32)
#define oNEGs64(s1) 	oUNOP(s1,-,s64)
#define oNEGf32(s1) 	oUNOP(s1,-,f32)
#define oNEGf64(s1) 	oUNOP(s1,-,f64)


// complement number  ~s1

#define oNOTu8			oNOTs8
#define oNOTu16			oNOTs16
#define oNOTu32			oNOTs32
#define oNOTu64			oNOTs64
#define oNOTs8(s1)	 	oUNOP(s1,~,s8)
#define oNOTs16(s1) 	oUNOP(s1,~,s16)
#define oNOTs32(s1) 	oUNOP(s1,~,s32)
#define oNOTs64(s1) 	oUNOP(s1,~,s64)
#define oNOTf32(s1) 	oUNOP(s1,~,f32)
#define oNOTf64(s1) 	oUNOP(s1,~,f64)



// add two numbers d += s
// add two numbers d = s1 + s2

#define oADD2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oADD2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oADD2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oADD2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oADD2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oADD2s16s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oADD2f32f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oADD2f64f64(s,d,srm,drm,defaultop) 				(defaultop)

#define oADD3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oADD3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oADD3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oADD3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oADD3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oADD3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oADD3f32f32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oADD3f64f64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oADDs8(s1,s2) 	oBINOP(s1,s2,+,s8)
#define oADDs16(s1,s2) 	oBINOP(s1,s2,+,s16)
#define oADDs32(s1,s2) 	oBINOP(s1,s2,+,s32)
#define oADDs64(s1,s2) 	oBINOP(s1,s2,+,s64)
#define oADDf32(s1,s2) 	oBINOP(s1,s2,+,f32)
#define oADDf64(s1,s2) 	oBINOP(s1,s2,+,f64)


// add two numbers d += s - system registers PC,FP, or SP involved
// add two numbers d = s1 + s2

#define oADDSYS2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oADDSYS2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oADDSYS3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oADDSYS3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)


// subtract two numbers d -= s
// subtract two numbers d = s2 - s1

#define oSUB2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oSUB2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oSUB2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSUB2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oSUB2f32f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSUB2f64f64(s,d,srm,drm,defaultop) 				(defaultop)

#define oSUB3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oSUB3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSUB3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSUB3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSUB3f32f32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSUB3f64f64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oSUBs8(s1,s2) 	oBINOP(s1,s2,-,s8)
#define oSUBs16(s1,s2) 	oBINOP(s1,s2,-,s16)
#define oSUBs32(s1,s2) 	oBINOP(s1,s2,-,s32)
#define oSUBs64(s1,s2) 	oBINOP(s1,s2,-,s64)
#define oSUBf32(s1,s2) 	oBINOP(s1,s2,-,f32)
#define oSUBf64(s1,s2) 	oBINOP(s1,s2,-,f64)

// subtract two numbers d -= s	system registers PC,FP, or SP involved
// subtract two numbers d = s2 - s1

#define oSUBSYS2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSUBSYS3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)




// and two numbers d &= s
// and two numbers d = s1 & s2

#define oAND2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oAND2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oAND2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oAND2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oAND2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oAND2s16s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oAND3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oAND3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oAND3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oAND3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oAND3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oAND3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oANDs8(s1,s2) 	oBINOP(s1,s2,&,s8)
#define oANDs16(s1,s2) 	oBINOP(s1,s2,&,s16)
#define oANDs32(s1,s2) 	oBINOP(s1,s2,&,s32)
#define oANDs64(s1,s2) 	oBINOP(s1,s2,&,s64)



// or two numbers  d |= s
// or two numbers  d = s1 | s2

#define oOR2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oOR2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oOR2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oOR2s64s64(s,d,srm,drm,defaultop)	 			(defaultop)
#define oOR2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oOR2s16s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oOR3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oOR3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oOR3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oOR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oOR3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oOR3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oORs8(s1,s2) 	oBINOP(s1,s2,|,s8)
#define oORs16(s1,s2) 	oBINOP(s1,s2,|,s16)
#define oORs32(s1,s2) 	oBINOP(s1,s2,|,s32)
#define oORs64(s1,s2) 	oBINOP(s1,s2,|,s64)



// xor two numbers d ^= s
// xor two numbers d = s1 ^ s2

#define oEOR2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oEOR2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oEOR2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oEOR2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oEOR2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oEOR2s16s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oEOR3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oEOR3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oEOR3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oEOR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oEOR3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oEOR3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oEORs8(s1,s2) 	oBINOP(s1,s2,^,s8)
#define oEORs16(s1,s2) 	oBINOP(s1,s2,^,s16)
#define oEORs32(s1,s2) 	oBINOP(s1,s2,^,s32)
#define oEORs64(s1,s2) 	oBINOP(s1,s2,^,s64)


// multiply two numbers d *= s
// multiply two numbers d = s1 * s2

#define oMUL2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oMUL2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMUL2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMUL2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oMUL2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMUL2s16s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oMUL2f32f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMUL2f64f64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMUL3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oMUL3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMUL3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMUL3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMUL3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMUL3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMUL3f32f32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMUL3f64f64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oMULs8(s1,s2) 	oBINOP(s1,s2,*,s8)
#define oMULs16(s1,s2) 	oBINOP(s1,s2,*,s16)
#define oMULs32(s1,s2) 	oBINOP(s1,s2,*,s32)
#define oMULs64(s1,s2) 	oBINOP(s1,s2,*,s64)
#define oMULf32(s1,s2) 	oBINOP(s1,s2,*,f32)
#define oMULf64(s1,s2) 	oBINOP(s1,s2,*,f64)



// divide two numbers d /= s
// divide two numbers d = s2 / s1

#define oDIV2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2s16s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2f32f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2f64f64(s,d,srm,drm,defaultop) 				(defaultop)

#define oDIV3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oDIV3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oDIV3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oDIV3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oDIV3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oDIV3f32f32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oDIV3f64f64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oDIV2u8u8(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2u16u16(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2u32u32(s,d,srm,drm,defaultop) 				(defaultop)
#define oDIV2u64u64(s,d,srm,drm,defaultop) 				(defaultop)

#define oDIV3u8u8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oDIV3u16u16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oDIV3u32u32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oDIV3u64u64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oDIVu8(s1,s2) 	oBINOP(s1,s2,/,u8)
#define oDIVu16(s1,s2) 	oBINOP(s1,s2,/,u16)
#define oDIVu32(s1,s2) 	oBINOP(s1,s2,/,u32)
#define oDIVu64(s1,s2) 	oBINOP(s1,s2,/,u64)
#define oDIVs8(s1,s2) 	oBINOP(s1,s2,/,s8)
#define oDIVs16(s1,s2) 	oBINOP(s1,s2,/,s16)
#define oDIVs32(s1,s2) 	oBINOP(s1,s2,/,s32)
#define oDIVs64(s1,s2) 	oBINOP(s1,s2,/,s64)
#define oDIVf32(s1,s2) 	oBINOP(s1,s2,/,f32)
#define oDIVf64(s1,s2) 	oBINOP(s1,s2,/,f64)


// modulas two numbers d %= s
// modulas two numbers d = s2 % s1

#define oMOD2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2s16s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2f32f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2f64f64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOD3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oMOD3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMOD3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMOD3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMOD3f32f32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMOD3f64f64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oMOD2u8u8(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2u16u16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2u32u32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOD2u64u64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOD3u8u8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oMOD3u16u16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMOD3u32u32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oMOD3u64u64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oMODu8(s1,s2) 	oBINOP(s1,s2,%,u8)
#define oMODu16(s1,s2) 	oBINOP(s1,s2,%,u16)
#define oMODu32(s1,s2) 	oBINOP(s1,s2,%,u32)
#define oMODu64(s1,s2) 	oBINOP(s1,s2,%,u64)
#define oMODs8(s1,s2) 	oBINOP(s1,s2,%,s8)
#define oMODs16(s1,s2) 	oBINOP(s1,s2,%,s16)
#define oMODs32(s1,s2) 	oBINOP(s1,s2,%,s32)
#define oMODs64(s1,s2) 	oBINOP(s1,s2,%,s64)
#define oMODf32(s1,s2) 	oBINOP(s1,s2,%,f32)
#define oMODf64(s1,s2) 	oBINOP(s1,s2,%,f64)





#define oshiftOP(s1,s2,op,tp,mask) ((tp)((tp)(s2) op mask(s1)))

// shiftleft two numbers  - s2<<s1

#define oSHL2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHL2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHL2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHL2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHL2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHL2s16s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oSHL3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oSHL3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHL3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHL3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHL3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHL3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)

#define oSHLs8(s1,s2) 	oshiftOP(s1,s2,<<,s8,(s32)SHIFT32MASK)
#define oSHLs16(s1,s2) 	oshiftOP(s1,s2,<<,s16,(s32)SHIFT32MASK)
#define oSHLs32(s1,s2) 	oshiftOP(s1,s2,<<,s32,(s32)SHIFT32MASK)
#define oSHLs64(s1,s2) 	oshiftOP(s1,s2,<<,s64,(s32)SHIFT64MASK)



// shiftright two numbers  - s2>>s1

#define oSHR2s8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2s16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2s32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2s64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2s16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2s16s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oSHR3s8s8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oSHR3s16s16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)


#define oSHR2u8u8(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2u16u16(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2u32u32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2u64u64(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2u16u32(s,d,srm,drm,defaultop) 				(defaultop)
#define oSHR2u16u64(s,d,srm,drm,defaultop) 				(defaultop)

#define oSHR3u8u8(s1,s2,d,s1rm,s2rm,drm,defaultop) 		(defaultop)
#define oSHR3u16u16(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3u32u32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3u64u64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3u16u32(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)
#define oSHR3u16u64(s1,s2,d,s1rm,s2rm,drm,defaultop) 	(defaultop)



#define oSHRu8(s1,s2) 	oshiftOP(s1,s2,>>,u8,(u32)SHIFT32MASK)
#define oSHRu16(s1,s2) 	oshiftOP(s1,s2,>>,u16,(u32)SHIFT32MASK)
#define oSHRu32(s1,s2) 	oshiftOP(s1,s2,>>,u32,(u32)SHIFT32MASK)
#define oSHRu64(s1,s2) 	oshiftOP(s1,s2,>>,u64,(u32)SHIFT64MASK)

#define oSHRs8(s1,s2) 	oshiftOP(s1,s2,>>,s8,(s32)SHIFT32MASK)
#define oSHRs16(s1,s2) 	oshiftOP(s1,s2,>>,s16,(s32)SHIFT32MASK)
#define oSHRs32(s1,s2) 	oshiftOP(s1,s2,>>,s32,(s32)SHIFT32MASK)
#define oSHRs64(s1,s2) 	oshiftOP(s1,s2,>>,s64,(s32)SHIFT64MASK)

// fixed multiply

#ifndef oFIXMULs32
#define oFIXMULs32(s1,s2) ((s32)((s64)(((s64)(s1) * (s64)(s2))>>16)))
#endif

#ifndef oFIXDIVs32
static inline s32 _oFIXDIVs32(s32 a,s32 b){
		return (I_ABS(b)>>14) >= I_ABS(a) ?
			 ((b^a)>>31) ^ 0x7fffffff :
			(s32)(((s64) b << 16) / a);
		
};

#define oFIXDIVs32 _oFIXDIVs32
#endif




#define ocmpOP(s1,s2,op,tp) ((tp)(s1) op (tp)(s2))

// compare two numbers  - s1==s2

#define oEQu8(s1,s2) 	ocmpOP(s1,s2,==,u8)
#define oEQu16(s1,s2) 	ocmpOP(s1,s2,==,u16)
#define oEQu32(s1,s2) 	ocmpOP(s1,s2,==,u32)
#define oEQu64(s1,s2) 	ocmpOP(s1,s2,==,u64)
#define oEQs8(s1,s2) 	ocmpOP(s1,s2,==,s8)
#define oEQs16(s1,s2) 	ocmpOP(s1,s2,==,s16)
#define oEQs32(s1,s2) 	ocmpOP(s1,s2,==,s32)
#define oEQs64(s1,s2) 	ocmpOP(s1,s2,==,s64)
#define oEQf32(s1,s2) 	ocmpOP(s1,s2,==,f32)
#define oEQf64(s1,s2) 	ocmpOP(s1,s2,==,f64)

// compare two numbers  - s1!=s2

#define oNEu8(s1,s2) 	ocmpOP(s1,s2,!=,u8)
#define oNEu16(s1,s2) 	ocmpOP(s1,s2,!=,u16)
#define oNEu32(s1,s2) 	ocmpOP(s1,s2,!=,u32)
#define oNEu64(s1,s2) 	ocmpOP(s1,s2,!=,u64)
#define oNEs8(s1,s2) 	ocmpOP(s1,s2,!=,s8)
#define oNEs16(s1,s2) 	ocmpOP(s1,s2,!=,s16)
#define oNEs32(s1,s2) 	ocmpOP(s1,s2,!=,s32)
#define oNEs64(s1,s2) 	ocmpOP(s1,s2,!=,s64)
#define oNEf32(s1,s2) 	ocmpOP(s1,s2,!=,f32)
#define oNEf64(s1,s2) 	ocmpOP(s1,s2,!=,f64)

// compare two numbers  - s1<s2

#define oLTu8(s1,s2) 	ocmpOP(s1,s2,<,u8)
#define oLTu16(s1,s2) 	ocmpOP(s1,s2,<,u16)
#define oLTu32(s1,s2) 	ocmpOP(s1,s2,<,u32)
#define oLTu64(s1,s2) 	ocmpOP(s1,s2,<,u64)
#define oLTs8(s1,s2) 	ocmpOP(s1,s2,<,s8)
#define oLTs16(s1,s2) 	ocmpOP(s1,s2,<,s16)
#define oLTs32(s1,s2) 	ocmpOP(s1,s2,<,s32)
#define oLTs64(s1,s2) 	ocmpOP(s1,s2,<,s64)
#define oLTf32(s1,s2) 	ocmpOP(s1,s2,<,f32)
#define oLTf64(s1,s2) 	ocmpOP(s1,s2,<,f64)



// compare two numbers  - s1<=s2

#define oLEu8(s1,s2) 	ocmpOP(s1,s2,<=,u8)
#define oLEu16(s1,s2) 	ocmpOP(s1,s2,<=,u16)
#define oLEu32(s1,s2) 	ocmpOP(s1,s2,<=,u32)
#define oLEu64(s1,s2) 	ocmpOP(s1,s2,<=,u64)
#define oLEs8(s1,s2) 	ocmpOP(s1,s2,<=,s8)
#define oLEs16(s1,s2) 	ocmpOP(s1,s2,<=,s16)
#define oLEs32(s1,s2) 	ocmpOP(s1,s2,<=,s32)
#define oLEs64(s1,s2) 	ocmpOP(s1,s2,<=,s64)
#define oLEf32(s1,s2) 	ocmpOP(s1,s2,<=,f32)
#define oLEf64(s1,s2) 	ocmpOP(s1,s2,<=,f64)


// compare two numbers  - s1>s2

#define oGTu8(s1,s2) 	ocmpOP(s1,s2,>,u8)
#define oGTu16(s1,s2) 	ocmpOP(s1,s2,>,u16)
#define oGTu32(s1,s2) 	ocmpOP(s1,s2,>,u32)
#define oGTu64(s1,s2) 	ocmpOP(s1,s2,>,u64)
#define oGTs8(s1,s2) 	ocmpOP(s1,s2,>,s8)
#define oGTs16(s1,s2) 	ocmpOP(s1,s2,>,s16)
#define oGTs32(s1,s2) 	ocmpOP(s1,s2,>,s32)
#define oGTs64(s1,s2) 	ocmpOP(s1,s2,>,s64)
#define oGTf32(s1,s2) 	ocmpOP(s1,s2,>,f32)
#define oGTf64(s1,s2) 	ocmpOP(s1,s2,>,f64)



// compare two numbers  - s1>=s2

#define oGEu8(s1,s2) 	ocmpOP(s1,s2,>=,u8)
#define oGEu16(s1,s2) 	ocmpOP(s1,s2,>=,u16)
#define oGEu32(s1,s2) 	ocmpOP(s1,s2,>=,u32)
#define oGEu64(s1,s2) 	ocmpOP(s1,s2,>=,u64)
#define oGEs8(s1,s2) 	ocmpOP(s1,s2,>=,s8)
#define oGEs16(s1,s2) 	ocmpOP(s1,s2,>=,s16)
#define oGEs32(s1,s2) 	ocmpOP(s1,s2,>=,s32)
#define oGEs64(s1,s2) 	ocmpOP(s1,s2,>=,s64)
#define oGEf32(s1,s2) 	ocmpOP(s1,s2,>=,f32)
#define oGEf64(s1,s2) 	ocmpOP(s1,s2,>=,f64)



// move operations - d=s

#define oMOVs8s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs16s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs64s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOVs8s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs8s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs8s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOVu8s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVu8s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVu8s64(s,d,srm,drm,defaultop) 				(defaultop)


#define oMOVs16s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs16s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOVu16s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVu16s64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOVs32s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs32s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs32s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs32f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs32f64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOVu32s64(s,d,srm,drm,defaultop) 				(defaultop)


#define oMOVs64s8(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs64s16(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs64s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs64f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVs64f64(s,d,srm,drm,defaultop) 				(defaultop)


#define oMOVf32s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVf32s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVf32f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVf32f64(s,d,srm,drm,defaultop) 				(defaultop)

#define oMOVf64s32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVf64s64(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVf64f32(s,d,srm,drm,defaultop) 				(defaultop)
#define oMOVf64f64(s,d,srm,drm,defaultop) 				(defaultop)


extern void ICVMRunLoop(void);
extern int ICVMDisassemble(void *addr,char *outbuf);


// format of ICVM binary in memory:

// text - all code and read only material
// data - all writeable, preinitialized data
// bss  - all writeable zero initializaed data

struct ICVMBin{
	void *data; // start of binary
	u32  size; // size of binary
    u32  dataoffset; // offset to start of data segment
	u32  bssoffset; // offset to start of bss segment

};


#define NOT_IMPLEMENTED {fprintf(stderr,"NOT IMPLEMENTED ERROR IN %s at %d\n",__FILE__,__LINE__);exit(3);}




#if defined(__GNUC__)  && !defined(NOOPTIMIZE)// only available with GNU compilers
#ifdef I386

// ********* THIS section is a very undocumented section that demonstrates
// the usage of the macro hooks to optimize a virtual machine 
// **** sorry about the lack of documents, World, ... there is only one of me!
// ***********
#define iASM	 __asm__ __volatile__


#ifdef USEMMX


#ifdef USE3DNOW
#define IEMMS		iASM("femms\n") 
#else
#define IEMMS		iASM("emms\n")
#endif // USE3DNOW

#endif // USEMMX



// ********* generic MMX selector macros

#ifdef USEMMX


// C-version - faster for many register ref combinations
#define ommxBIN2s32s32C(s,d,opcode,defaultop) defaultop
#define ommxBIN2u32u32C(s,d,opcode,defaultop) defaultop


// mmx version - faster for many memory ref combinations
#define ommxBIN2s32s32X(s,d,opcode,defaultop)\
	iASM("movd %0,%%mm1\n" :: "g" (s));\
	iASM("movd %0,%%mm0\n" :: "g" (d));\
	iASM(opcode " %mm1,%mm0\n" );\
	iASM("movd %%mm0,%0\n" :: "g" (d));\
	IEMMS;


// split optimization up so MMX only applied where advantageous

#define ommxBIN2s32s32MM	ommxBIN2s32s32X
#define ommxBIN2s32s32RM	ommxBIN2s32s32X
#define ommxBIN2s32s32MR	ommxBIN2s32s32C
#define ommxBIN2s32s32RR	ommxBIN2s32s32C

#define ommxBIN2u32u32MM	ommxBIN2s32s32MM
#define ommxBIN2u32u32RM	ommxBIN2s32s32RM
#define ommxBIN2u32u32MR	ommxBIN2s32s32MR
#define ommxBIN2u32u32RR	ommxBIN2s32s32RR

#define ommxBIN2s32s32(s,d,srm,drm,opcode,defaultop)\
	 ommxBIN2s32s32##srm##drm(s,d,opcode,defaultop)

#define ommxBIN2u32u32(s,d,srm,drm,opcode,defaultop)\
	 ommxBIN2u32u32##srm##drm(s,d,opcode,defaultop)



// C-version - faster for many register ref combinations
#define ommxBIN3s32s32C(s1,s2,d,opcode,defaultop) defaultop
#define ommxBIN3u32u32C(s1,s2,d,opcode,defaultop) defaultop

// mmx version - faster for many memory ref combinations
#define ommxBIN3s32s32X(s1,s2,d,opcode,defaultop)\
	iASM("movd %0,%%mm0\n" :: "g" (s2));\
	iASM("movd %0,%%mm1\n" :: "g" (s1));\
	iASM(opcode " %mm1,%mm0\n" );\
	iASM("movd %%mm0,%0\n" :: "g" (d));\
	IEMMS;

// split optimization up so MMX only applied where advantageous

#define ommxBIN3s32s32MMM	ommxBIN3s32s32X
#define ommxBIN3s32s32RMM	ommxBIN3s32s32X
#define ommxBIN3s32s32MRM	ommxBIN3s32s32C
#define ommxBIN3s32s32RRM	ommxBIN3s32s32X
#define ommxBIN3s32s32MMR	ommxBIN3s32s32X
#define ommxBIN3s32s32RMR	ommxBIN3s32s32C
#define ommxBIN3s32s32MRR	ommxBIN3s32s32C
#define ommxBIN3s32s32RRR	ommxBIN3s32s32C


#define ommxBIN3u32u32MMM	ommxBIN3s32s32MMM
#define ommxBIN3u32u32RMM	ommxBIN3s32s32RMM
#define ommxBIN3u32u32MRM	ommxBIN3s32s32MRM
#define ommxBIN3u32u32RRM	ommxBIN3s32s32RRM
#define ommxBIN3u32u32MMR	ommxBIN3s32s32MMR
#define ommxBIN3u32u32RMR	ommxBIN3s32s32RMR
#define ommxBIN3u32u32MRR	ommxBIN3s32s32MRR
#define ommxBIN3u32u32RRR	ommxBIN3s32s32RRR

#define ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,opcode,defaultop)\
	 ommxBIN3s32s32##s1rm##s2rm##drm(s1,s2,d,opcode,defaultop)

#define ommxBIN3u32u32(s1,s2,d,s1rm,s2rm,drm,opcode,defaultop)\
	 ommxBIN3u32u32##s1rm##s2rm##drm(s1,s2,d,opcode,defaultop)




#else // USEMMX

#define ommxBIN2s32s32(s,d,srm,drm,opcode,defaultop) defaultop
#define ommxBIN2u32u32(s,d,srm,drm,opcode,defaultop) defaultop

#define ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,opcode,defaultop) defaultop
#define ommxBIN3u32u32(s1,s2,d,s1rm,s2rm,drm,opcode,defaultop) defaultop

#endif // USEMMX



// ******************************************************
// *************** AND optimizations ********************
// ******************************************************

#undef oAND2s32s32
#undef oAND2s64s64


#undef oAND3s32s32
#undef oAND3s64s64

#define oAND2s32s32(s,d,srm,drm,defaultop)\
	ommxBIN2s32s32(s,d,srm,drm,"pand",defaultop)


#ifdef USEMMX
#define oAND2s64s64(s,d,srm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s));\
	iASM("pand %0,%%mm0\n" :: "g" (d));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;
#else // USEMMX
#define oAND2s64s64(s,d,srm,drm,defaultop)	defaultop
#endif // USEMMX


#define oAND3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,"pand",defaultop)


#ifdef USEMMX
#define oAND3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s1));\
	iASM("movq %0,%%mm1\n" :: "g" (s2));\
	iASM("pand  %mm1,%mm0\n" );\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;
#else // USEMMX
#define oAND3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) defaultop
#endif // USEMMX



// ******************************************************
// *************** EOR optimizations ********************
// ******************************************************

#undef oEOR2s32s32
#undef oEOR2s64s64

#undef oEOR3s32s32
#undef oEOR3s64s64



#define oEOR2s32s32(s,d,srm,drm,defaultop)\
	ommxBIN2s32s32(s,d,srm,drm,"pxor",defaultop)

#ifdef USEMMX
#define oEOR2s64s64(s,d,srm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s));\
	iASM("pxor %0,%%mm0\n" :: "g" (d));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;
#else // USEMMX
#define oEOR2s64s64(s,d,srm,drm,defaultop) defaultop
#endif // USEMMX


#define oEOR3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,"pxor",defaultop)


#ifdef USEMMX
#define oEOR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s1));\
	iASM("movq %0,%%mm1\n" :: "g" (s2));\
	iASM("pxor  %mm1,%mm0\n" );\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;
#else // USEMMX
#define oEOR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) defaultop
#endif // USEMMX



// ******************************************************
// *************** OR optimizations ********************
// ******************************************************

#undef oOR2s32s32
#undef oOR2s64s64

#undef oOR3s32s32
#undef oOR3s64s64


#define oOR2s32s32(s,d,srm,drm,defaultop)\
	ommxBIN2s32s32(s,d,srm,drm,"por",defaultop)


#ifdef USEMMX
#define oOR2s64s64(s,d,srm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s));\
	iASM("por %0,%%mm0\n" :: "g" (d));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;
#else // USEMMX
#define oOR2s64s64(s,d,srm,drm,defaultop) defaultop
#endif // USEMMX



#define oOR3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,"por",defaultop)


#ifdef USEMMX
#define oOR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s1));\
	iASM("movq %0,%%mm1\n" :: "g" (s2));\
	iASM("por  %mm1,%mm0\n" );\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;
#else // USEMMX
#define oOR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop) defaultop
#endif // USEMMX





#ifdef NOT_NOW_BOB

// ******************************************************
// *************** signed SHR optimizations *************
// ******************************************************

#undef oSHR2s8s8
#undef oSHR2s16s16
#undef oSHR2s32s32
#undef oSHR2s64s64
#undef oSHR2s16s32
#undef oSHR2s16s64


#undef oSHR3s32s32

#undef oSHR3s64s64

#undef oSHR3s16s32
#undef oSHR3s16s64



#define oSHR2s8s8(s,d,srm,drm,defaultop) (d.b[0] >>= s.b[0])
#define oSHR2s16s16(s,d,srm,drm,defaultop) (d.w[0] >>= s.w[0])

#define oSHR2s16s32(s,d,srm,drm,defaultop) (d.l[0] >>= s.w[0])

#define oSHR2s32s32(s,d,srm,drm,defaultop)\
	ommxBIN2s32s32(s,d,srm,drm,>>=,"psrad")

#define oSHR2s64s64(s,d,srm,drm,defaultop){\
		register u32 shft=s.l[I_LOW];\
		if(shft<32){\
			d.l[I_LOW]=(d.ul[I_LOW]>>shft) | (d.ul[I_LOW]<<(32-shft));\
			d.l[I_HI]>>=(s32)shft;\
		}else{\
			d.l[I_LOW]=d.l[I_HI]>>(s32)(shft & 31);\
			if(d.l[I_HI]<0)\
				d.l[I_HI]=-1;\
			else\
				d.l[I_HI]=0;\
		}\
	}

#define oSHR2s16s64(s,d,srm,drm,defaultop){\
		register u32 shft=s.w[0];\
		if(shft<32){\
			d.l[I_LOW]=(d.ul[I_LOW]>>shft) | (d.ul[I_HI]<<(32-shft));\
			d.l[I_HI]>>=(s32)shft;\
		}else{\
			d.l[I_LOW]=d.l[I_HI]>>(s32)(shft & 31);\
			if(d.l[I_HI]<0)\
				d.l[I_HI]=-1;\
			else\
				d.l[I_HI]=0;\
		}\
	}


#define oSHR3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,>>,"psrad")

#define oSHR3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	 (d.l[0] = s2.l[0] >> s1.w[0])


#define oSHR3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
		register u32 shft=s1.l[I_LOW];\
		if(shft<32){\
			d.l[I_LOW]=(s2.ul[I_LOW]>>shft) | (s2.ul[I_HI]<<(32-shft));\
			d.l[I_HI]= s2.l[I_HI]>>(s32)shft;\
		}else{\
			d.l[I_LOW]=s2.l[I_HI]>>(s32)(shft & 31);\
			if(s2.l[I_HI]<0)\
				d.l[I_HI]=-1;\
			else\
				d.l[I_HI]=0;\
		}\
	}

#define oSHR3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
		register u32 shft=s1.w[0];\
		if(shft<32){\
			d.l[I_LOW]=(s2.ul[I_LOW]>>shft) | (s2.ul[I_HI]<<(32-shft));\
			d.l[I_HI]= s2.l[I_HI]>>shft;\
		}else{\
			d.l[I_LOW]=s2.l[I_HI]>>(shft & 31);\
			if(s2.l[I_HI]<0)\
				d.l[I_HI]=-1;\
			else\
				d.l[I_HI]=0;\
		}\
	}

// ******************************************************
// *************** unsigned SHR optimizations *************
// ******************************************************

#undef oSHR2u8u8
#undef oSHR2u16u16
#undef oSHR2u32u32
#undef oSHR2u64u64
#undef oSHR2u16u32
#undef oSHR2u16u64



#undef oSHR3u32u32
#undef oSHR3u64u64

#undef oSHR3u16u32
#undef oSHR3u16u64



#define oSHR2u8u8(s,d,srm,drm,defaultop) (d.ub[0] >>= s.ub[0])
#define oSHR2u16u16(s,d,srm,drm,defaultop) (d.uw[0] >>= s.uw[0])

#define oSHR2u16u32(s,d,srm,drm,defaultop) (d.ul[0] >>= s.uw[0])

#define oSHR2u32u32(s,d,srm,drm,defaultop)\
	ommxBIN2u32u32(s,d,srm,drm,>>=,"psrld")

#ifdef USEMMX
#define oSHR2u64u64(s,d,srm,drm,defaultop) {\
	iASM(\
		"movq %0,%%mm1\n" :: "g" (s));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psrlq %%mm1,%%mm0\n"\
			 :: "g" (d));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHR2u64u64(s,d,srm,drm,defaultop){\
		register u32 shft=s.ul[I_LOW];\
		if(shft<32){\
			d.ul[I_LOW]=(d.ul[I_LOW]>>shft) | (d.ul[I_LOW]<<(32-shft));\
			d.ul[I_HI]>>=shft;\
		}else{\
			d.ul[I_LOW]=d.ul[I_HI]>>(shft & 31);\
			d.ul[I_HI]=0;\
		}\
	}
#endif // USEMMX

#ifdef USEMMX
#define oSHR2u16u64(s,d,srm,drm,defaultop) {\
	register s32 shft;\
	iASM(\
		"movswl %1,%0\n"\
		"movd %0,%%mm1\n"\
			:"=r" (shft): "g" (s));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psrlq %%mm1,%%mm0\n"\
			 :: "g" (d));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHR2u16u64(s,d,srm,drm,defaultop){\
		register u32 shft=s.uw[0];\
		if(shft<32){\
			d.ul[I_LOW]=(d.ul[I_LOW]>>shft) | (d.ul[I_HI]<<(32-shft));\
			d.ul[I_HI]>>=shft;\
		}else{\
			d.ul[I_LOW]=d.ul[I_HI]>>(shft & 31);\
			d.ul[I_HI]=0;\
		}\
	}

#endif // USEMMX

#define oSHR3u32u32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3u32u32(s1,s2,d,s1rm,s2rm,drm,>>,"psrld")

#define oSHR3u16u32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	 (d.ul[0] = s2.ul[0] >> s1.uw[0])


#ifdef USEMMX
#define oSHR3u64u64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
	iASM(\
		"movq %0,%%mm1\n" :: "g" (s1));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psrlq %%mm1,%%mm0\n"\
			 :: "g" (s1));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHR3u64u64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
		register u32 shft=s1.ul[I_LOW];\
		if(shft<32){\
			d.ul[I_LOW]=(s2.ul[I_LOW]>>shft) | (s2.ul[I_HI]<<(32-shft));\
			d.ul[I_HI]= s2.ul[I_HI]>>shft;\
		}else{\
			d.ul[I_LOW]=s2.ul[I_HI]>>(shft & 31);\
			d.ul[I_HI]=0;\
		}\
	}
#endif // USEMMX

#ifdef USEMMX
#define oSHR3u16u64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
	register s32 shft;\
	iASM(\
		"movswl %1,%0\n"\
		"movd %0,%%mm1\n"\
			:"=r" (shft): "g" (s1));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psrlq %%mm1,%%mm0\n"\
			 :: "g" (s2));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHR3u16u64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
		register u32 shft=s1.uw[0];\
		if(shft<32){\
			d.ul[I_LOW]=(s2.ul[I_LOW]>>shft) | (s2.ul[I_HI]<<(32-shft));\
			d.ul[I_HI]= s2.ul[I_HI]>>shft;\
		}else{\
			d.ul[I_LOW]=s2.ul[I_HI]>>(shft & 31);\
			d.ul[I_HI]=0;\
		}\
	}
#endif // USEMMX



// ******************************************************
// *************** SHL optimizations ********************
// ******************************************************

#undef oSHL2s8s8
#undef oSHL2s16s16
#undef oSHL2s32s32
#undef oSHL2s64s64
#undef oSHL2s16s32
#undef oSHL2s16s64

#undef oSHL3s32s32
#undef oSHL3s64s64

#undef oSHL3s16s32
#undef oSHL3s16s64




#define oSHL2s8s8(s,d,srm,drm,defaultop) (d.b[0] <<= s.b[0])
#define oSHL2s16s16(s,d,srm,drm,defaultop) (d.w[0] <<= s.w[0])

#define oSHL2s16s32(s,d,srm,drm,defaultop) (d.l[0] <<= s.w[0])

#define oSHL2s32s32(s,d,srm,drm,defaultop)\
	ommxBIN2s32s32(s,d,srm,drm,<<=,"pslld")


#ifdef USEMMX
#define oSHL2s16s64(s,d,srm,drm,defaultop) {\
	register s32 shft;\
	iASM(\
		"movswl %1,%0\n"\
		"movd %0,%%mm1\n"\
			:"=r" (shft): "g" (s));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psllq %%mm1,%%mm0\n"\
			 :: "g" (d));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHL2s16s64(s,d,srm,drm,defaultop){\
		register u32 shft=s.w[0];\
		if(shft<32){\
			d.l[I_HI]=(d.l[I_HI]<<(s32)shft) | (d.ul[I_LOW]>>(32 - shft));\
			d.ul[I_LOW]<<=shft;\
		}else{\
			d.l[I_HI]=d.l[I_LOW]<<(s32)(shft & 31);\
			d.ul[I_LOW]=0;\
		}\
	}
#endif // USEMMX


#ifdef USEMMX
#define oSHL2s64s64(s,d,srm,drm,defaultop) {\
	iASM(\
		"movq %0,%%mm1\n" :: "g" (s));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psllq %%mm1,%%mm0\n"\
			 :: "g" (d));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHL2s64s64(s,d,srm,drm,defaultop){\
		register u32 shft=s.l[I_LOW];\
		if(shft<32){\
			d.l[I_HI]=(d.l[I_HI]<<(s32)shft) | (d.ul[I_LOW]>>(32-shft));\
			d.ul[I_LOW]<<=shft;\
		}else{\
			d.l[I_HI]=d.l[I_LOW]<<(s32)(shft & 31);\
			d.ul[I_LOW]=0;\
		}\
	}
#endif // USEMMX


#define oSHL3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,<<,"pslld")

#define oSHL3s16s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	(d.l[0] = s2.l[0] << s1.w[0])


#ifdef USEMMX
#define oSHL3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
	register s32 shft;\
	iASM(\
		"movswl %1,%0\n"\
		"movd %0,%%mm1\n"\
			:"=r" (shft): "g" (s1));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psllq %%mm1,%%mm0\n"\
			 :: "g" (s2));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHL3s16s64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
		register u32 shft=s1.uw[0];\
		if(shft<32){\
			d.ul[I_HI]=(s2.l[I_HI]<<(s32)shft) | (s2.ul[I_LOW]>>(32-shft));\
			d.ul[I_LOW]=s2.ul[I_LOW]<<shft;\
		}else{\
			d.l[I_HI]=s2.l[I_LOW]<<(s32)(shft&31);\
			d.ul[I_LOW]=0;\
		}\
	}
#endif // USEMMX

#ifdef USEMMX
#define oSHL3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
	iASM(\
		"movq %0,%%mm1\n" :: "g" (s1));\
	iASM(\
		"movq %0,%%mm0\n"\
		"psllq %%mm1,%%mm0\n"\
			 :: "g" (s1));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;\
}
#else // USEMMX
#define oSHL3s64s64(s1,s2,d,s1rm,s2rm,drm,defaultop){\
		register s32 shft=s1.l[I_LOW];\
		if(shft<32){\
			d.l[I_HI]=(s2.l[I_HI]<<shft) | (s2.ul[I_LOW]>>(32-shft));\
			d.ul[I_LOW]=s2.ul[I_LOW]<<shft;\
		}else{\
			d.l[I_HI]=s2.l[I_LOW]<<(shft & 31);\
			d.ul[I_LOW]=0;\
		}\
	}
#endif // USEMMX





// ******************************************************
// *************** ADD optimizations ********************
// ******************************************************

#undef oADD2s32s32
#undef oADD3s32s32


#define oADD2s32s32(s,d,srm,drm,defaultop)\
	ommxBIN2s32s32(s,d,srm,drm,"paddd",defaultop)

#define oADD3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,"paddd",defaultop)


// ******************************************************
// ***************  SUB optimizations *************
// ******************************************************

#undef oSUB2s32s32
#undef oSUB3s32s32

#define oSUB2s32s32(s,d,srm,drm,defaultop)\
	ommxBIN2s32s32(s,d,srm,drm,"psubd",defaultop)

#define oSUB3s32s32(s1,s2,d,s1rm,s2rm,drm,defaultop)\
	ommxBIN3s32s32(s1,s2,d,s1rm,s2rm,drm,"psubd",defaultop)

#endif // NOT_NOW_BOB


// ******************************************************
// ***************  MOV optimizations *************
// ******************************************************


#ifdef USEMMX

#undef oMOVu32s64
#undef oMOVs32s32
#undef oMOVs64s64
#undef oMOVs64s32
#undef oMOVf32f32
#undef oMOVf64f64

#define oMOVs32s32C(s,d,defaultop) defaultop

#define oMOVf32f32C(s,d,defaultop) defaultop


// mmx version - faster for many memory ref combinations
#define oMOVs32s32X(s,d,defaultop)\
	iASM("movd %0,%%mm0\n" :: "g" (s));\
	iASM("movd %%mm0,%0\n" :: "g" (d));\
	IEMMS;

#define oMOVf32f32X oMOVs32s32X

// split optimization up so MMX only applied where advantageous

#define oMOVs32s32MM	oMOVs32s32X
#define oMOVs32s32RM	oMOVs32s32X
#define oMOVs32s32MR	oMOVs32s32C
#define oMOVs32s32RR	oMOVs32s32C

#define oMOVf32f32MM	oMOVf32f32X
#define oMOVf32f32RM	oMOVf32f32X
#define oMOVf32f32MR	oMOVf32f32C
#define oMOVf32f32RR	oMOVf32f32C


#define oMOVs32s32(s,d,srm,drm,defaultop)\
	 oMOVs32s32##srm##drm(s,d,defaultop)

#define oMOVf32f32(s,d,srm,drm,defaultop)\
	 oMOVf32f32##srm##drm(s,d,defaultop)



#define oMOVs64s64(s,d,srm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;


#define oMOVu32s64(s,d,srm,drm,defaultop)\
	iASM("movd %0,%%mm0\n" :: "g" (s));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;

#define oMOVs64s32(s,d,srm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s));\
	iASM("movd %%mm0,%0\n" :: "g" (d));\
	IEMMS;


#define oMOVf64f64(s,d,srm,drm,defaultop)\
	iASM("movq %0,%%mm0\n" :: "g" (s));\
	iASM("movq %%mm0,%0\n" :: "g" (d));\
	IEMMS;




#endif // USEMMX



#endif // __I386__
#endif // __GNUC__


#ifdef __cplusplus
}
#endif




#endif



