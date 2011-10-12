#include "cvm.h"

// Copyright (C) 2000,2001 Bob Dayley


#include "icvmsys.h"
#include "icvmerr.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "proc.h"

/* these are functions waiting for 1.2 compatibility in windoze */
/* these work with mesa, but are left as NOPS until a windoze solution can be found */
/* tried to compile mesa 3.4.1 but could not get running version from either
	VC or mingw */

#ifndef __WIN32__
#define USE_GL_12 /* to enable opengl 1.2 features */
#endif

#ifdef USE_GL_12
#define V12(x) x
#define pV12(x) x
#else
#define V12(x)
#define pV12(x) 0
#endif

#ifndef __WIN32__

#include <sys/time.h>
#include <sys/uio.h>

#endif /* __WIN32__ */

struct i_glcallback{
	ICVMMachine *machine;
	p32 callback;
};

struct i_glcallbacklist{
	struct i_glcallbacklist *next;
	u32	id;
	ICVMMachine *machine;
	p32 callback;
};


/* find a callback based on id in a list */

static struct i_glcallbacklist 
		*FindCallback(struct i_glcallbacklist *l,int id){
	while(l){
		if(l->id==id)
			return l;
		l=l->next;
	}
	return 0;
};

/* deleate a callback based on id in a list */

static void DeleteCallback(struct i_glcallbacklist **lp,int id){
	struct i_glcallbacklist **op=0;
	while(*lp){
		if((*lp)->id==id){
			struct i_glcallbacklist *l=*lp;
			if(op)
				(*op)->next=l->next;
			free(l);
			return;
		}
		op=lp;
		lp= &((*lp)->next);
	};		
};

/* add a callback based on id in a list */

static void AddCallback(struct i_glcallbacklist **lp,int id,
		ICVMMachine *machine,p32 callback){
	struct i_glcallbacklist *l=malloc(sizeof(struct i_glcallbacklist));
	if(l==0)
		return; // *** not much error handling ... but ... oh well
	l->id=id;
	l->machine=machine;
	l->callback=callback;
	l->next=*lp;
	*lp=l;
};




/* macros to set up and build callbacks */

#define iSTARTCALLBACKBODY(ptrname)\
		p32 oldSP;\
		sigjmp_buf jmp;\
		struct ICVM_jmp_buf state;\
		if(ptrname.callback==0)\
			return;\
		if(sigsetjmp(jmp,0))\
			return;\
		SET_LM(ptrname.machine);\
		i_savecontext(&state,0);


#define iENDCALLBACKBODY(ptrname)\
		iPUSHNULLRETURN;\
		R_PC=ptrname.callback;\
		ICVMRunLoop();\
		i_restorecontext(&state);\
		longjmp(jmp,1);


static struct i_glcallbacklist *menulist=0;

/* menu callback tracking stuff  */

static struct i_glcallbacklist *menus;

static struct i_glcallbacklist 
		*FindMenuCallback(int id){
	return FindCallback(menus,id);
};


static void DeleteMenuCallback(int id){
	DeleteCallback(&menus,id);
};


static void AddMenuCallback(int id,ICVMMachine *machine,p32 callback){
	AddCallback(&menus,id,machine,callback);
};

static void menucallback(s32 arg0){
	struct i_glcallbacklist *g=FindMenuCallback(glutGetMenu());
	if(g==0)
		return;
	{
		iSTARTCALLBACKBODY((*g));
		iPUSHs32(arg0);
		iENDCALLBACKBODY((*g));
	}
};


static void ICVM_glutCreateMenu(void){
	R_R0=glutCreateMenu(
		rSTKs32(0) ? menucallback : (void *)0
	);
	if(rSTKs32(0)) // need a callback
		AddMenuCallback(R_R0,R_LM,rSTKs32(0));
};

static void ICVM_glutDestroyMenu(void){
	glutDestroyMenu(rSTKs32(0));
	DeleteMenuCallback(rSTKs32(0));
};



#define glutGLUT_STROKE_ROMAN()					GLUT_STROKE_ROMAN
#define glutGLUT_STROKE_MONO_ROMAN() 			GLUT_STROKE_MONO_ROMAN
#define glutGLUT_BITMAP_9_BY_15()				GLUT_BITMAP_9_BY_15
#define glutGLUT_BITMAP_8_BY_13()				GLUT_BITMAP_8_BY_13
#define glutGLUT_BITMAP_TIMES_ROMAN_10()		GLUT_BITMAP_TIMES_ROMAN_10
#define glutGLUT_BITMAP_TIMES_ROMAN_24()		GLUT_BITMAP_TIMES_ROMAN_24
#define glutGLUT_BITMAP_HELVETICA_10()			GLUT_BITMAP_HELVETICA_10
#define glutGLUT_BITMAP_HELVETICA_12()			GLUT_BITMAP_HELVETICA_12
#define glutGLUT_BITMAP_HELVETICA_18()			GLUT_BITMAP_HELVETICA_18


/* macros to build system calls into OpenGL/MesaGL */


#define iBLD_V_IIIPPP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),Nptr(rSTKs32(3)),Nptr(rSTKs32(4)),Nptr(rSTKs32(5)));};
#define iBLD_V_IIIIIIPP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),Nptr(rSTKs32(6)),Nptr(rSTKs32(7)));};
#define iBLD_V_IUUIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKu32(1),rSTKu32(2),rSTKs32(3),rSTKs32(4),Nptr(rSTKs32(5)));};
#define iBLD_V_IFFFF(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf32(1),rSTKf32(2),rSTKf32(3),rSTKf32(4));};
#define iBLD_V_IDDDD(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf64(1),rSTKf64(2),rSTKf64(3),rSTKf64(4));};
#define iBLD_V_IDDD(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf64(1),rSTKf64(2),rSTKf64(3));};
#define iBLD_V_ID(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf64(1));};
#define iBLD_V_IIIIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),Nptr(rSTKs32(5)));};
#define iBLD_V_IIIIIIIII(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),rSTKs32(6),rSTKs32(7),rSTKs32(8));};
#define iBLD_V_IIIIIIIIIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),rSTKs32(6),rSTKs32(7),rSTKs32(8),rSTKs32(9),Nptr(rSTKs32(10)));};
#define iBLD_V_IIIIIIIIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),rSTKs32(6),rSTKs32(7),rSTKs32(8),Nptr(rSTKs32(9)));};
#define iBLD_V_IFFIFF(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf32(1),rSTKf32(2),rSTKs32(3),rSTKf32(4),rSTKf32(5));};
#define iBLD_V_IDDIDD(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf64(1),rSTKf64(2),rSTKs32(3),rSTKf64(4),rSTKf64(5));};
#define iBLD_V_IFF(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf32(1),rSTKf32(2));};
#define iBLD_V_IDD(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf64(1),rSTKf64(2));};
#define iBLD_V_IFFIIFFIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf32(1),rSTKf32(2),rSTKs32(3),rSTKs32(4),rSTKf32(5),rSTKf32(6),rSTKs32(7),rSTKs32(8),Nptr(rSTKs32(9)));};
#define iBLD_V_IDDIIDDIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf64(1),rSTKf64(2),rSTKs32(3),rSTKs32(4),rSTKf64(5),rSTKf64(6),rSTKs32(7),rSTKs32(8),Nptr(rSTKs32(9)));};
#define iBLD_V_IFFIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf32(1),rSTKf32(2),rSTKs32(3),rSTKs32(4),Nptr(rSTKs32(5)));};
#define iBLD_V_IDDIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf64(1),rSTKf64(2),rSTKs32(3),rSTKs32(4),Nptr(rSTKs32(5)));};
#define iBLD_V_IIIIII(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5));};
#define iBLD_V_IIIIIIII(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),rSTKs32(6),rSTKs32(7));};
#define iBLD_V_IIIIIII(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),rSTKs32(6));};
#define iBLD_I_IPP(nm,cnm) static void nm(void){R_R0=cnm(rSTKs32(0),Nptr(rSTKs32(1)),Nptr(rSTKs32(2)));};
#define iBLD_V_IPP(nm,cnm) static void nm(void){cnm(rSTKs32(0),Nptr(rSTKs32(1)),Nptr(rSTKs32(2)));};
#define iBLD_V_IIIIIIIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),rSTKs32(6),rSTKs32(7),Nptr(rSTKs32(8)));};
#define iBLD_V_IIIIIIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),rSTKs32(6),Nptr(rSTKs32(7)));};
#define iBLD_V_IID(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKf64(2));};
#define iBLD_V_IIU(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKu32(2));};
#define iBLD_V_IIIII(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4));};
#define iBLD_V_IIIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),Nptr(rSTKs32(4)));};
#define iBLD_V_IIIIIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4),rSTKs32(5),Nptr(rSTKs32(6)));};
#define iBLD_V_IIFFFFP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKf32(2),rSTKf32(3),rSTKf32(4),rSTKf32(5),Nptr(rSTKs32(6)));};
#define iBLD_V_IIF(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKf32(2));};
#define iBLD_V_IIIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),Nptr(rSTKs32(3)));};
#define iBLD_V_UUUU(nm,cnm) static void nm(void){cnm(rSTKu32(0),rSTKu32(1),rSTKu32(2),rSTKu32(3));};
#define iBLD_V_UUU(nm,cnm) static void nm(void){cnm(rSTKu32(0),rSTKu32(1),rSTKu32(2));};
#define iBLD_V_III(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2));};
#define iBLD_V_IIP(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),Nptr(rSTKs32(2)));};
#define iBLD_U_I(nm,cnm) static void nm(void){R_R0=cnm(rSTKs32(0));};
#define iBLD_V_UI(nm,cnm) static void nm(void){cnm(rSTKu32(0),rSTKs32(1));};
#define iBLD_I_U(nm,cnm) static void nm(void){R_R0=cnm(rSTKu32(0));};
#define iBLD_V_FFF(nm,cnm) static void nm(void){cnm(rSTKf32(0),rSTKf32(1),rSTKf32(2));};
#define iBLD_V_DDD(nm,cnm) static void nm(void){cnm(rSTKf64(0),rSTKf64(1),rSTKf64(2));};
#define iBLD_V_DDDD(nm,cnm) static void nm(void){cnm(rSTKf64(0),rSTKf64(1),rSTKf64(2),rSTKf64(3));};
#define iBLD_V_DDDDDD(nm,cnm) static void nm(void){cnm(rSTKf64(0),rSTKf64(1),rSTKf64(2),rSTKf64(3),rSTKf64(4),rSTKf64(5));};
#define iBLD_V_DD(nm,cnm) static void nm(void){cnm(rSTKf64(0),rSTKf64(1));};
#define iBLD_P_I(nm,cnm) static void nm(void){R_R0=Iptr(cnm(rSTKs32(0)));};
#define iBLD_V_IP(nm,cnm) static void nm(void){cnm(rSTKs32(0),Nptr(rSTKs32(1)));};
#define iBLD_V_FF(nm,cnm) static void nm(void){cnm(rSTKf32(0),rSTKf32(1));};
#define iBLD_V_IU(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKu32(1));};
#define iBLD_V_IF(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf32(1));};
#define iBLD_V_FFFF(nm,cnm) static void nm(void){cnm(rSTKf32(0),rSTKf32(1),rSTKf32(2),rSTKf32(3));};
#define iBLD_V_F(nm,cnm) static void nm(void){cnm(rSTKf32(0));};
#define iBLD_V_IIII(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3));};
#define iBLD_V_D(nm,cnm) static void nm(void){cnm(rSTKf64(0));};
#define iBLD_V_DDII(nm,cnm) static void nm(void){cnm(rSTKf64(0),rSTKf64(1),rSTKs32(2),rSTKs32(3));};
#define iBLD_V_DII(nm,cnm) static void nm(void){cnm(rSTKf64(0),rSTKs32(1),rSTKs32(2));};
#define iBLD_I_PP(nm,cnm) static void nm(void){R_R0=cnm(Nptr(rSTKs32(0)),Nptr(rSTKs32(1)));};
#define iBLD_I_PI(nm,cnm) static void nm(void){R_R0=cnm(Nptr(rSTKs32(0)),rSTKs32(1));};
#define iBLD_I_I(nm,cnm) static void nm(void){R_R0=cnm(rSTKs32(0));};
#define iBLD_F_II(nm,cnm) static void nm(void){R_R0=cnm(rSTKs32(0),rSTKs32(1));};
#define iBLD_V_IFFF(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKf32(1),rSTKf32(2),rSTKf32(3));};
#define iBLD_V_IPI(nm,cnm) static void nm(void){cnm(rSTKs32(0),Nptr(rSTKs32(1)),rSTKs32(2));};
#define iBLD_V_PI(nm,cnm) static void nm(void){cnm(Nptr(rSTKs32(0)),rSTKs32(1));};
#define iBLD_I_V(nm,cnm) static void nm(void){R_R0=cnm();};
#define iBLD_V_I(nm,cnm) static void nm(void){cnm(rSTKs32(0));};
#define iBLD_I_IIIII(nm,cnm) static void nm(void){R_R0=cnm(rSTKs32(0),rSTKs32(1),rSTKs32(2),rSTKs32(3),rSTKs32(4));};
#define iBLD_I_P(nm,cnm) static void nm(void){R_R0=cnm(Nptr(rSTKs32(0)));};
#define iBLD_V_V(nm,cnm) static void nm(void){cnm();};
#define iBLD_V_II(nm,cnm) static void nm(void){cnm(rSTKs32(0),rSTKs32(1));};
#define iBLD_V_P(nm,cnm) static void nm(void){cnm(Nptr(rSTKs32(0)));};
#define iBLD_V_U(nm,cnm) static void nm(void){cnm(rSTKu32(0));};
#define iBLD_V_PP(nm,cnm) static void nm(void){cnm(Nptr(rSTKs32(0)),Nptr(rSTKs32(1)));};
#define iBLD_P_V(nm,cnm) static void nm(void){R_R0=Iptr(cnm());};

/*********************/

V12(iBLD_V_II(ICVM_glHintPGI,glHintPGI);)
V12(iBLD_V_V(ICVM_glUnlockArraysEXT,glUnlockArraysEXT);)
V12(iBLD_V_II(ICVM_glLockArraysEXT,glLockArraysEXT);)
V12(iBLD_V_IIIPPP(ICVM_glGetSeparableFilter,glGetSeparableFilter);)
V12(iBLD_V_IIIIIIPP(ICVM_glSeparableFilter2D,glSeparableFilter2D);)
V12(iBLD_V_IIP(ICVM_glGetConvolutionParameteriv,glGetConvolutionParameteriv);)
V12(iBLD_V_IIP(ICVM_glGetConvolutionParameterfv,glGetConvolutionParameterfv);)
V12(iBLD_V_IIIP(ICVM_glGetConvolutionFilter,glGetConvolutionFilter);)
V12(iBLD_V_IIIIII(ICVM_glCopyConvolutionFilter2D,glCopyConvolutionFilter2D);)
V12(iBLD_V_IIIII(ICVM_glCopyConvolutionFilter1D,glCopyConvolutionFilter1D);)
V12(iBLD_V_IIP(ICVM_glConvolutionParameteriv,glConvolutionParameteriv);)
V12(iBLD_V_III(ICVM_glConvolutionParameteri,glConvolutionParameteri);)
V12(iBLD_V_IIP(ICVM_glConvolutionParameterfv,glConvolutionParameterfv);)
V12(iBLD_V_IIF(ICVM_glConvolutionParameterf,glConvolutionParameterf);)
V12(iBLD_V_IIIIIIP(ICVM_glConvolutionFilter2D,glConvolutionFilter2D);)
V12(iBLD_V_IIIIIP(ICVM_glConvolutionFilter1D,glConvolutionFilter1D);)
V12(iBLD_V_IIP(ICVM_glGetMinmaxParameteriv,glGetMinmaxParameteriv);)
V12(iBLD_V_IIP(ICVM_glGetMinmaxParameterfv,glGetMinmaxParameterfv);)
V12(iBLD_V_IIIIP(ICVM_glGetMinmax,glGetMinmax);)
V12(iBLD_V_I(ICVM_glResetMinmax,glResetMinmax);)
V12(iBLD_V_III(ICVM_glMinmax,glMinmax);)
V12(iBLD_V_IIP(ICVM_glGetHistogramParameteriv,glGetHistogramParameteriv);)
V12(iBLD_V_IIP(ICVM_glGetHistogramParameterfv,glGetHistogramParameterfv);)
V12(iBLD_V_IIIIP(ICVM_glGetHistogram,glGetHistogram);)
V12(iBLD_V_I(ICVM_glResetHistogram,glResetHistogram);)
V12(iBLD_V_IIII(ICVM_glHistogram,glHistogram);)
V12(iBLD_V_FFFF(ICVM_glBlendColor,glBlendColor);)
V12(iBLD_V_I(ICVM_glBlendEquation,glBlendEquation);)
V12(iBLD_V_IIP(ICVM_glGetColorTableParameteriv,glGetColorTableParameteriv);)
V12(iBLD_V_IIP(ICVM_glGetColorTableParameterfv,glGetColorTableParameterfv);)
V12(iBLD_V_IIIP(ICVM_glGetColorTable,glGetColorTable);)
V12(iBLD_V_IIIII(ICVM_glCopyColorTable,glCopyColorTable);)
V12(iBLD_V_IIIII(ICVM_glCopyColorSubTable,glCopyColorSubTable);)
V12(iBLD_V_IIP(ICVM_glColorTableParameterfv,glColorTableParameterfv);)
V12(iBLD_V_IIP(ICVM_glColorTableParameteriv,glColorTableParameteriv);)
V12(iBLD_V_IIIIIP(ICVM_glColorSubTable,glColorSubTable);)
V12(iBLD_V_IIIIIP(ICVM_glColorTable,glColorTable);)
V12(iBLD_V_IIIIIIIII(ICVM_glCopyTexSubImage3D,glCopyTexSubImage3D);)
V12(iBLD_V_IIIIIIIIIIP(ICVM_glTexSubImage3D,glTexSubImage3D);)
V12(iBLD_V_IIIIIIIIIP(ICVM_glTexImage3D,glTexImage3D);)
V12(iBLD_V_IUUIIP(ICVM_glDrawRangeElements,glDrawRangeElements);)
//iBLD_V_IIII(ICVM_glBlendFuncSeparateINGR,glBlendFuncSeparateINGR);
V12(iBLD_V_IP(ICVM_glPointParameterfvEXT,glPointParameterfvEXT);)
V12(iBLD_V_IF(ICVM_glPointParameterfEXT,glPointParameterfEXT);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord4svARB,glMultiTexCoord4svARB);)
V12(iBLD_V_IIIII(ICVM_glMultiTexCoord4sARB,glMultiTexCoord4sARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord4ivARB,glMultiTexCoord4ivARB);)
V12(iBLD_V_IIIII(ICVM_glMultiTexCoord4iARB,glMultiTexCoord4iARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord4fvARB,glMultiTexCoord4fvARB);)
V12(iBLD_V_IFFFF(ICVM_glMultiTexCoord4fARB,glMultiTexCoord4fARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord4dvARB,glMultiTexCoord4dvARB);)
V12(iBLD_V_IDDDD(ICVM_glMultiTexCoord4dARB,glMultiTexCoord4dARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord3svARB,glMultiTexCoord3svARB);)
V12(iBLD_V_IIII(ICVM_glMultiTexCoord3sARB,glMultiTexCoord3sARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord3ivARB,glMultiTexCoord3ivARB);)
V12(iBLD_V_IIII(ICVM_glMultiTexCoord3iARB,glMultiTexCoord3iARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord3fvARB,glMultiTexCoord3fvARB);)
V12(iBLD_V_IFFF(ICVM_glMultiTexCoord3fARB,glMultiTexCoord3fARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord3dvARB,glMultiTexCoord3dvARB);)
V12(iBLD_V_IDDD(ICVM_glMultiTexCoord3dARB,glMultiTexCoord3dARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord2svARB,glMultiTexCoord2svARB);)
V12(iBLD_V_III(ICVM_glMultiTexCoord2sARB,glMultiTexCoord2sARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord2ivARB,glMultiTexCoord2ivARB);)
V12(iBLD_V_III(ICVM_glMultiTexCoord2iARB,glMultiTexCoord2iARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord2fvARB,glMultiTexCoord2fvARB);)
V12(iBLD_V_IFF(ICVM_glMultiTexCoord2fARB,glMultiTexCoord2fARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord2dvARB,glMultiTexCoord2dvARB);)
V12(iBLD_V_IDD(ICVM_glMultiTexCoord2dARB,glMultiTexCoord2dARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord1svARB,glMultiTexCoord1svARB);)
V12(iBLD_V_II(ICVM_glMultiTexCoord1sARB,glMultiTexCoord1sARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord1ivARB,glMultiTexCoord1ivARB);)
V12(iBLD_V_II(ICVM_glMultiTexCoord1iARB,glMultiTexCoord1iARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord1fvARB,glMultiTexCoord1fvARB);)
V12(iBLD_V_IF(ICVM_glMultiTexCoord1fARB,glMultiTexCoord1fARB);)
V12(iBLD_V_IP(ICVM_glMultiTexCoord1dvARB,glMultiTexCoord1dvARB);)
V12(iBLD_V_ID(ICVM_glMultiTexCoord1dARB,glMultiTexCoord1dARB);)
V12(iBLD_V_I(ICVM_glClientActiveTextureARB,glClientActiveTextureARB);)
V12(iBLD_V_I(ICVM_glActiveTextureARB,glActiveTextureARB);)
V12(iBLD_V_IIP(ICVM_glGetColorTableParameterivEXT,glGetColorTableParameterivEXT);)
V12(iBLD_V_IIP(ICVM_glGetColorTableParameterfvEXT,glGetColorTableParameterfvEXT);)
V12(iBLD_V_IIIP(ICVM_glGetColorTableEXT,glGetColorTableEXT);)
V12(iBLD_V_IIIIIP(ICVM_glColorSubTableEXT,glColorSubTableEXT);)
V12(iBLD_V_IIIIIP(ICVM_glColorTableEXT,glColorTableEXT);)
V12(iBLD_V_IIIIIIIII(ICVM_glCopyTexSubImage3DEXT,glCopyTexSubImage3DEXT);)
V12(iBLD_V_IIIIIIIIIIP(ICVM_glTexSubImage3DEXT,glTexSubImage3DEXT);)
V12(iBLD_V_IIIIIIIIIP(ICVM_glTexImage3DEXT,glTexImage3DEXT);)
V12(iBLD_I_U(ICVM_glIsTextureEXT,glIsTextureEXT);)
V12(iBLD_I_IPP(ICVM_glAreTexturesResidentEXT,glAreTexturesResidentEXT);)
V12(iBLD_V_IPP(ICVM_glPrioritizeTexturesEXT,glPrioritizeTexturesEXT);)
V12(iBLD_V_IU(ICVM_glBindTextureEXT,glBindTextureEXT);)
V12(iBLD_V_IP(ICVM_glDeleteTexturesEXT,glDeleteTexturesEXT);)
V12(iBLD_V_IP(ICVM_glGenTexturesEXT,glGenTexturesEXT);)
V12(iBLD_V_III(ICVM_glDrawArraysEXT,glDrawArraysEXT);)
V12(iBLD_V_I(ICVM_glArrayElementEXT,glArrayElementEXT);)
V12(iBLD_V_IP(ICVM_glGetPointervEXT,glGetPointervEXT);)
V12(iBLD_V_IIP(ICVM_glEdgeFlagPointerEXT,glEdgeFlagPointerEXT);)
V12(iBLD_V_IIIIP(ICVM_glTexCoordPointerEXT,glTexCoordPointerEXT);)
V12(iBLD_V_IIIP(ICVM_glIndexPointerEXT,glIndexPointerEXT);)
V12(iBLD_V_IIIIP(ICVM_glColorPointerEXT,glColorPointerEXT);)
V12(iBLD_V_IIIP(ICVM_glNormalPointerEXT,glNormalPointerEXT);)
V12(iBLD_V_IIIIP(ICVM_glVertexPointerEXT,glVertexPointerEXT);)
V12(iBLD_V_FF(ICVM_glPolygonOffsetEXT,glPolygonOffsetEXT);)
V12(iBLD_V_FFFF(ICVM_glBlendColorEXT,glBlendColorEXT);)
V12(iBLD_V_I(ICVM_glBlendEquationEXT,glBlendEquationEXT);)
iBLD_V_V(ICVM_glPopName,glPopName);
iBLD_V_U(ICVM_glPushName,glPushName);
iBLD_V_U(ICVM_glLoadName,glLoadName);
iBLD_V_V(ICVM_glInitNames,glInitNames);
iBLD_V_IP(ICVM_glSelectBuffer,glSelectBuffer);
iBLD_V_F(ICVM_glPassThrough,glPassThrough);
iBLD_V_IIP(ICVM_glFeedbackBuffer,glFeedbackBuffer);
iBLD_V_IP(ICVM_glFogiv,glFogiv);
iBLD_V_IP(ICVM_glFogfv,glFogfv);
iBLD_V_II(ICVM_glFogi,glFogi);
iBLD_V_IF(ICVM_glFogf,glFogf);
iBLD_V_IIIII(ICVM_glEvalMesh2,glEvalMesh2);
iBLD_V_III(ICVM_glEvalMesh1,glEvalMesh1);
iBLD_V_II(ICVM_glEvalPoint2,glEvalPoint2);
iBLD_V_I(ICVM_glEvalPoint1,glEvalPoint1);
iBLD_V_IFFIFF(ICVM_glMapGrid2f,glMapGrid2f);
iBLD_V_IDDIDD(ICVM_glMapGrid2d,glMapGrid2d);
iBLD_V_IFF(ICVM_glMapGrid1f,glMapGrid1f);
iBLD_V_IDD(ICVM_glMapGrid1d,glMapGrid1d);
iBLD_V_P(ICVM_glEvalCoord2fv,glEvalCoord2fv);
iBLD_V_P(ICVM_glEvalCoord2dv,glEvalCoord2dv);
iBLD_V_FF(ICVM_glEvalCoord2f,glEvalCoord2f);
iBLD_V_DD(ICVM_glEvalCoord2d,glEvalCoord2d);
iBLD_V_P(ICVM_glEvalCoord1fv,glEvalCoord1fv);
iBLD_V_P(ICVM_glEvalCoord1dv,glEvalCoord1dv);
iBLD_V_F(ICVM_glEvalCoord1f,glEvalCoord1f);
iBLD_V_D(ICVM_glEvalCoord1d,glEvalCoord1d);
iBLD_V_IIP(ICVM_glGetMapiv,glGetMapiv);
iBLD_V_IIP(ICVM_glGetMapfv,glGetMapfv);
iBLD_V_IIP(ICVM_glGetMapdv,glGetMapdv);
iBLD_V_IFFIIFFIIP(ICVM_glMap2f,glMap2f);
iBLD_V_IDDIIDDIIP(ICVM_glMap2d,glMap2d);
iBLD_V_IFFIIP(ICVM_glMap1f,glMap1f);
iBLD_V_IDDIIP(ICVM_glMap1d,glMap1d);
iBLD_V_IIIIIIII(ICVM_glCopyTexSubImage2D,glCopyTexSubImage2D);
iBLD_V_IIIIII(ICVM_glCopyTexSubImage1D,glCopyTexSubImage1D);
iBLD_V_IIIIIIII(ICVM_glCopyTexImage2D,glCopyTexImage2D);
iBLD_V_IIIIIII(ICVM_glCopyTexImage1D,glCopyTexImage1D);
iBLD_V_IIIIIIIIP(ICVM_glTexSubImage2D,glTexSubImage2D);
iBLD_V_IIIIIIP(ICVM_glTexSubImage1D,glTexSubImage1D);
iBLD_I_U(ICVM_glIsTexture,glIsTexture);
iBLD_I_IPP(ICVM_glAreTexturesResident,glAreTexturesResident);
iBLD_V_IPP(ICVM_glPrioritizeTextures,glPrioritizeTextures);
iBLD_V_IU(ICVM_glBindTexture,glBindTexture);
iBLD_V_IP(ICVM_glDeleteTextures,glDeleteTextures);
iBLD_V_IP(ICVM_glGenTextures,glGenTextures);
iBLD_V_IIIIP(ICVM_glGetTexImage,glGetTexImage);
iBLD_V_IIIIIIIIP(ICVM_glTexImage2D,glTexImage2D);
iBLD_V_IIIIIIIP(ICVM_glTexImage1D,glTexImage1D);
iBLD_V_IIIP(ICVM_glGetTexLevelParameteriv,glGetTexLevelParameteriv);
iBLD_V_IIIP(ICVM_glGetTexLevelParameterfv,glGetTexLevelParameterfv);
iBLD_V_IIP(ICVM_glGetTexParameteriv,glGetTexParameteriv);
iBLD_V_IIP(ICVM_glGetTexParameterfv,glGetTexParameterfv);
iBLD_V_IIP(ICVM_glTexParameteriv,glTexParameteriv);
iBLD_V_IIP(ICVM_glTexParameterfv,glTexParameterfv);
iBLD_V_III(ICVM_glTexParameteri,glTexParameteri);
iBLD_V_IIF(ICVM_glTexParameterf,glTexParameterf);
iBLD_V_IIP(ICVM_glGetTexEnviv,glGetTexEnviv);
iBLD_V_IIP(ICVM_glGetTexEnvfv,glGetTexEnvfv);
iBLD_V_IIP(ICVM_glTexEnviv,glTexEnviv);
iBLD_V_IIP(ICVM_glTexEnvfv,glTexEnvfv);
iBLD_V_III(ICVM_glTexEnvi,glTexEnvi);
iBLD_V_IIF(ICVM_glTexEnvf,glTexEnvf);
iBLD_V_IIP(ICVM_glGetTexGeniv,glGetTexGeniv);
iBLD_V_IIP(ICVM_glGetTexGenfv,glGetTexGenfv);
iBLD_V_IIP(ICVM_glGetTexGendv,glGetTexGendv);
iBLD_V_IIP(ICVM_glTexGeniv,glTexGeniv);
iBLD_V_IIP(ICVM_glTexGenfv,glTexGenfv);
iBLD_V_IIP(ICVM_glTexGendv,glTexGendv);
iBLD_V_III(ICVM_glTexGeni,glTexGeni);
iBLD_V_IIF(ICVM_glTexGenf,glTexGenf);
iBLD_V_IID(ICVM_glTexGend,glTexGend);
iBLD_V_I(ICVM_glClearStencil,glClearStencil);
iBLD_V_III(ICVM_glStencilOp,glStencilOp);
iBLD_V_U(ICVM_glStencilMask,glStencilMask);
iBLD_V_IIU(ICVM_glStencilFunc,glStencilFunc);
iBLD_V_IIIII(ICVM_glCopyPixels,glCopyPixels);
iBLD_V_IIIIP(ICVM_glDrawPixels,glDrawPixels);
iBLD_V_IIIIIIP(ICVM_glReadPixels,glReadPixels);
iBLD_V_IIFFFFP(ICVM_glBitmap,glBitmap);
iBLD_V_IP(ICVM_glGetPixelMapusv,glGetPixelMapusv);
iBLD_V_IP(ICVM_glGetPixelMapuiv,glGetPixelMapuiv);
iBLD_V_IP(ICVM_glGetPixelMapfv,glGetPixelMapfv);
iBLD_V_IIP(ICVM_glPixelMapusv,glPixelMapusv);
iBLD_V_IIP(ICVM_glPixelMapuiv,glPixelMapuiv);
iBLD_V_IIP(ICVM_glPixelMapfv,glPixelMapfv);
iBLD_V_II(ICVM_glPixelTransferi,glPixelTransferi);
iBLD_V_IF(ICVM_glPixelTransferf,glPixelTransferf);
iBLD_V_II(ICVM_glPixelStorei,glPixelStorei);
iBLD_V_IF(ICVM_glPixelStoref,glPixelStoref);
iBLD_V_FF(ICVM_glPixelZoom,glPixelZoom);
iBLD_V_II(ICVM_glColorMaterial,glColorMaterial);
iBLD_V_IIP(ICVM_glGetMaterialiv,glGetMaterialiv);
iBLD_V_IIP(ICVM_glGetMaterialfv,glGetMaterialfv);
iBLD_V_IIP(ICVM_glMaterialiv,glMaterialiv);
iBLD_V_IIP(ICVM_glMaterialfv,glMaterialfv);
iBLD_V_III(ICVM_glMateriali,glMateriali);
iBLD_V_IIF(ICVM_glMaterialf,glMaterialf);
iBLD_V_IP(ICVM_glLightModeliv,glLightModeliv);
iBLD_V_IP(ICVM_glLightModelfv,glLightModelfv);
iBLD_V_II(ICVM_glLightModeli,glLightModeli);
iBLD_V_IF(ICVM_glLightModelf,glLightModelf);
iBLD_V_IIP(ICVM_glGetLightiv,glGetLightiv);
iBLD_V_IIP(ICVM_glGetLightfv,glGetLightfv);
iBLD_V_IIP(ICVM_glLightiv,glLightiv);
iBLD_V_IIP(ICVM_glLightfv,glLightfv);
iBLD_V_III(ICVM_glLighti,glLighti);
iBLD_V_IIF(ICVM_glLightf,glLightf);
iBLD_V_I(ICVM_glShadeModel,glShadeModel);
iBLD_V_IIP(ICVM_glInterleavedArrays,glInterleavedArrays);
iBLD_V_IIIP(ICVM_glDrawElements,glDrawElements);
iBLD_V_III(ICVM_glDrawArrays,glDrawArrays);
iBLD_V_I(ICVM_glArrayElement,glArrayElement);
iBLD_V_IP(ICVM_glGetPointerv,glGetPointerv);
iBLD_V_IP(ICVM_glEdgeFlagPointer,glEdgeFlagPointer);
iBLD_V_IIIP(ICVM_glTexCoordPointer,glTexCoordPointer);
iBLD_V_IIP(ICVM_glIndexPointer,glIndexPointer);
iBLD_V_IIIP(ICVM_glColorPointer,glColorPointer);
iBLD_V_IIP(ICVM_glNormalPointer,glNormalPointer);
iBLD_V_IIIP(ICVM_glVertexPointer,glVertexPointer);
iBLD_V_PP(ICVM_glRectsv,glRectsv);
iBLD_V_PP(ICVM_glRectiv,glRectiv);
iBLD_V_PP(ICVM_glRectfv,glRectfv);
iBLD_V_PP(ICVM_glRectdv,glRectdv);
iBLD_V_IIII(ICVM_glRects,glRects);
iBLD_V_IIII(ICVM_glRecti,glRecti);
iBLD_V_FFFF(ICVM_glRectf,glRectf);
iBLD_V_DDDD(ICVM_glRectd,glRectd);
iBLD_V_P(ICVM_glRasterPos4sv,glRasterPos4sv);
iBLD_V_P(ICVM_glRasterPos4iv,glRasterPos4iv);
iBLD_V_P(ICVM_glRasterPos4fv,glRasterPos4fv);
iBLD_V_P(ICVM_glRasterPos4dv,glRasterPos4dv);
iBLD_V_P(ICVM_glRasterPos3sv,glRasterPos3sv);
iBLD_V_P(ICVM_glRasterPos3iv,glRasterPos3iv);
iBLD_V_P(ICVM_glRasterPos3fv,glRasterPos3fv);
iBLD_V_P(ICVM_glRasterPos3dv,glRasterPos3dv);
iBLD_V_P(ICVM_glRasterPos2sv,glRasterPos2sv);
iBLD_V_P(ICVM_glRasterPos2iv,glRasterPos2iv);
iBLD_V_P(ICVM_glRasterPos2fv,glRasterPos2fv);
iBLD_V_P(ICVM_glRasterPos2dv,glRasterPos2dv);
iBLD_V_IIII(ICVM_glRasterPos4s,glRasterPos4s);
iBLD_V_IIII(ICVM_glRasterPos4i,glRasterPos4i);
iBLD_V_FFFF(ICVM_glRasterPos4f,glRasterPos4f);
iBLD_V_DDDD(ICVM_glRasterPos4d,glRasterPos4d);
iBLD_V_III(ICVM_glRasterPos3s,glRasterPos3s);
iBLD_V_III(ICVM_glRasterPos3i,glRasterPos3i);
iBLD_V_FFF(ICVM_glRasterPos3f,glRasterPos3f);
iBLD_V_DDD(ICVM_glRasterPos3d,glRasterPos3d);
iBLD_V_II(ICVM_glRasterPos2s,glRasterPos2s);
iBLD_V_II(ICVM_glRasterPos2i,glRasterPos2i);
iBLD_V_FF(ICVM_glRasterPos2f,glRasterPos2f);
iBLD_V_DD(ICVM_glRasterPos2d,glRasterPos2d);
iBLD_V_P(ICVM_glTexCoord4sv,glTexCoord4sv);
iBLD_V_P(ICVM_glTexCoord4iv,glTexCoord4iv);
iBLD_V_P(ICVM_glTexCoord4fv,glTexCoord4fv);
iBLD_V_P(ICVM_glTexCoord4dv,glTexCoord4dv);
iBLD_V_P(ICVM_glTexCoord3sv,glTexCoord3sv);
iBLD_V_P(ICVM_glTexCoord3iv,glTexCoord3iv);
iBLD_V_P(ICVM_glTexCoord3fv,glTexCoord3fv);
iBLD_V_P(ICVM_glTexCoord3dv,glTexCoord3dv);
iBLD_V_P(ICVM_glTexCoord2sv,glTexCoord2sv);
iBLD_V_P(ICVM_glTexCoord2iv,glTexCoord2iv);
iBLD_V_P(ICVM_glTexCoord2fv,glTexCoord2fv);
iBLD_V_P(ICVM_glTexCoord2dv,glTexCoord2dv);
iBLD_V_P(ICVM_glTexCoord1sv,glTexCoord1sv);
iBLD_V_P(ICVM_glTexCoord1iv,glTexCoord1iv);
iBLD_V_P(ICVM_glTexCoord1fv,glTexCoord1fv);
iBLD_V_P(ICVM_glTexCoord1dv,glTexCoord1dv);
iBLD_V_IIII(ICVM_glTexCoord4s,glTexCoord4s);
iBLD_V_IIII(ICVM_glTexCoord4i,glTexCoord4i);
iBLD_V_FFFF(ICVM_glTexCoord4f,glTexCoord4f);
iBLD_V_DDDD(ICVM_glTexCoord4d,glTexCoord4d);
iBLD_V_III(ICVM_glTexCoord3s,glTexCoord3s);
iBLD_V_III(ICVM_glTexCoord3i,glTexCoord3i);
iBLD_V_FFF(ICVM_glTexCoord3f,glTexCoord3f);
iBLD_V_DDD(ICVM_glTexCoord3d,glTexCoord3d);
iBLD_V_II(ICVM_glTexCoord2s,glTexCoord2s);
iBLD_V_II(ICVM_glTexCoord2i,glTexCoord2i);
iBLD_V_FF(ICVM_glTexCoord2f,glTexCoord2f);
iBLD_V_DD(ICVM_glTexCoord2d,glTexCoord2d);
iBLD_V_I(ICVM_glTexCoord1s,glTexCoord1s);
iBLD_V_I(ICVM_glTexCoord1i,glTexCoord1i);
iBLD_V_F(ICVM_glTexCoord1f,glTexCoord1f);
iBLD_V_D(ICVM_glTexCoord1d,glTexCoord1d);
iBLD_V_P(ICVM_glColor4usv,glColor4usv);
iBLD_V_P(ICVM_glColor4uiv,glColor4uiv);
iBLD_V_P(ICVM_glColor4ubv,glColor4ubv);
iBLD_V_P(ICVM_glColor4sv,glColor4sv);
iBLD_V_P(ICVM_glColor4iv,glColor4iv);
iBLD_V_P(ICVM_glColor4fv,glColor4fv);
iBLD_V_P(ICVM_glColor4dv,glColor4dv);
iBLD_V_P(ICVM_glColor4bv,glColor4bv);
iBLD_V_P(ICVM_glColor3usv,glColor3usv);
iBLD_V_P(ICVM_glColor3uiv,glColor3uiv);
iBLD_V_P(ICVM_glColor3ubv,glColor3ubv);
iBLD_V_P(ICVM_glColor3sv,glColor3sv);
iBLD_V_P(ICVM_glColor3iv,glColor3iv);
iBLD_V_P(ICVM_glColor3fv,glColor3fv);
iBLD_V_P(ICVM_glColor3dv,glColor3dv);
iBLD_V_P(ICVM_glColor3bv,glColor3bv);
iBLD_V_UUUU(ICVM_glColor4us,glColor4us);
iBLD_V_UUUU(ICVM_glColor4ui,glColor4ui);
iBLD_V_UUUU(ICVM_glColor4ub,glColor4ub);
iBLD_V_IIII(ICVM_glColor4s,glColor4s);
iBLD_V_IIII(ICVM_glColor4i,glColor4i);
iBLD_V_FFFF(ICVM_glColor4f,glColor4f);
iBLD_V_DDDD(ICVM_glColor4d,glColor4d);
iBLD_V_IIII(ICVM_glColor4b,glColor4b);
iBLD_V_UUU(ICVM_glColor3us,glColor3us);
iBLD_V_UUU(ICVM_glColor3ui,glColor3ui);
iBLD_V_UUU(ICVM_glColor3ub,glColor3ub);
iBLD_V_III(ICVM_glColor3s,glColor3s);
iBLD_V_III(ICVM_glColor3i,glColor3i);
iBLD_V_FFF(ICVM_glColor3f,glColor3f);
iBLD_V_DDD(ICVM_glColor3d,glColor3d);
iBLD_V_III(ICVM_glColor3b,glColor3b);
iBLD_V_P(ICVM_glIndexubv,glIndexubv);
iBLD_V_P(ICVM_glIndexsv,glIndexsv);
iBLD_V_P(ICVM_glIndexiv,glIndexiv);
iBLD_V_P(ICVM_glIndexfv,glIndexfv);
iBLD_V_P(ICVM_glIndexdv,glIndexdv);
iBLD_V_U(ICVM_glIndexub,glIndexub);
iBLD_V_I(ICVM_glIndexs,glIndexs);
iBLD_V_I(ICVM_glIndexi,glIndexi);
iBLD_V_F(ICVM_glIndexf,glIndexf);
iBLD_V_D(ICVM_glIndexd,glIndexd);
iBLD_V_P(ICVM_glNormal3sv,glNormal3sv);
iBLD_V_P(ICVM_glNormal3iv,glNormal3iv);
iBLD_V_P(ICVM_glNormal3fv,glNormal3fv);
iBLD_V_P(ICVM_glNormal3dv,glNormal3dv);
iBLD_V_P(ICVM_glNormal3bv,glNormal3bv);
iBLD_V_III(ICVM_glNormal3s,glNormal3s);
iBLD_V_III(ICVM_glNormal3i,glNormal3i);
iBLD_V_FFF(ICVM_glNormal3f,glNormal3f);
iBLD_V_DDD(ICVM_glNormal3d,glNormal3d);
iBLD_V_III(ICVM_glNormal3b,glNormal3b);
iBLD_V_P(ICVM_glVertex4sv,glVertex4sv);
iBLD_V_P(ICVM_glVertex4iv,glVertex4iv);
iBLD_V_P(ICVM_glVertex4fv,glVertex4fv);
iBLD_V_P(ICVM_glVertex4dv,glVertex4dv);
iBLD_V_P(ICVM_glVertex3sv,glVertex3sv);
iBLD_V_P(ICVM_glVertex3iv,glVertex3iv);
iBLD_V_P(ICVM_glVertex3fv,glVertex3fv);
iBLD_V_P(ICVM_glVertex3dv,glVertex3dv);
iBLD_V_P(ICVM_glVertex2sv,glVertex2sv);
iBLD_V_P(ICVM_glVertex2iv,glVertex2iv);
iBLD_V_P(ICVM_glVertex2fv,glVertex2fv);
iBLD_V_P(ICVM_glVertex2dv,glVertex2dv);
iBLD_V_IIII(ICVM_glVertex4s,glVertex4s);
iBLD_V_IIII(ICVM_glVertex4i,glVertex4i);
iBLD_V_FFFF(ICVM_glVertex4f,glVertex4f);
iBLD_V_DDDD(ICVM_glVertex4d,glVertex4d);
iBLD_V_III(ICVM_glVertex3s,glVertex3s);
iBLD_V_III(ICVM_glVertex3i,glVertex3i);
iBLD_V_FFF(ICVM_glVertex3f,glVertex3f);
iBLD_V_DDD(ICVM_glVertex3d,glVertex3d);
iBLD_V_II(ICVM_glVertex2s,glVertex2s);
iBLD_V_II(ICVM_glVertex2i,glVertex2i);
iBLD_V_FF(ICVM_glVertex2f,glVertex2f);
iBLD_V_DD(ICVM_glVertex2d,glVertex2d);
iBLD_V_V(ICVM_glEnd,glEnd);
iBLD_V_I(ICVM_glBegin,glBegin);
iBLD_V_U(ICVM_glListBase,glListBase);
iBLD_V_IIP(ICVM_glCallLists,glCallLists);
iBLD_V_U(ICVM_glCallList,glCallList);
iBLD_V_V(ICVM_glEndList,glEndList);
iBLD_V_UI(ICVM_glNewList,glNewList);
iBLD_U_I(ICVM_glGenLists,glGenLists);
iBLD_V_UI(ICVM_glDeleteLists,glDeleteLists);
iBLD_I_U(ICVM_glIsList,glIsList);
iBLD_V_FFF(ICVM_glTranslatef,glTranslatef);
iBLD_V_DDD(ICVM_glTranslated,glTranslated);
iBLD_V_FFF(ICVM_glScalef,glScalef);
iBLD_V_DDD(ICVM_glScaled,glScaled);
iBLD_V_FFFF(ICVM_glRotatef,glRotatef);
iBLD_V_DDDD(ICVM_glRotated,glRotated);
iBLD_V_P(ICVM_glMultMatrixf,glMultMatrixf);
iBLD_V_P(ICVM_glMultMatrixd,glMultMatrixd);
iBLD_V_P(ICVM_glLoadMatrixf,glLoadMatrixf);
iBLD_V_P(ICVM_glLoadMatrixd,glLoadMatrixd);
iBLD_V_V(ICVM_glLoadIdentity,glLoadIdentity);
iBLD_V_V(ICVM_glPopMatrix,glPopMatrix);
iBLD_V_V(ICVM_glPushMatrix,glPushMatrix);
iBLD_V_IIII(ICVM_glViewport,glViewport);
iBLD_V_DDDDDD(ICVM_glFrustum,glFrustum);
iBLD_V_DDDDDD(ICVM_glOrtho,glOrtho);
iBLD_V_I(ICVM_glMatrixMode,glMatrixMode);
iBLD_V_IF(ICVM_glAccum,glAccum);
iBLD_V_FFFF(ICVM_glClearAccum,glClearAccum);
iBLD_V_DD(ICVM_glDepthRange,glDepthRange);
iBLD_V_I(ICVM_glDepthMask,glDepthMask);
iBLD_V_I(ICVM_glDepthFunc,glDepthFunc);
iBLD_V_D(ICVM_glClearDepth,glClearDepth);
iBLD_V_II(ICVM_glHint,glHint);
iBLD_V_V(ICVM_glFlush,glFlush);
iBLD_V_V(ICVM_glFinish,glFinish);
iBLD_P_I(ICVM_glGetString,glGetString);
iBLD_I_V(ICVM_glGetError,glGetError);
iBLD_I_I(ICVM_glRenderMode,glRenderMode);
iBLD_V_V(ICVM_glPopClientAttrib,glPopClientAttrib);
iBLD_V_U(ICVM_glPushClientAttrib,glPushClientAttrib);
iBLD_V_V(ICVM_glPopAttrib,glPopAttrib);
iBLD_V_U(ICVM_glPushAttrib,glPushAttrib);
iBLD_V_IP(ICVM_glGetIntegerv,glGetIntegerv);
iBLD_V_IP(ICVM_glGetFloatv,glGetFloatv);
iBLD_V_IP(ICVM_glGetDoublev,glGetDoublev);
iBLD_V_IP(ICVM_glGetBooleanv,glGetBooleanv);
iBLD_V_I(ICVM_glDisableClientState,glDisableClientState);
iBLD_V_I(ICVM_glEnableClientState,glEnableClientState);
iBLD_I_I(ICVM_glIsEnabled,glIsEnabled);
iBLD_V_I(ICVM_glDisable,glDisable);
iBLD_V_I(ICVM_glEnable,glEnable);
iBLD_V_I(ICVM_glReadBuffer,glReadBuffer);
iBLD_V_I(ICVM_glDrawBuffer,glDrawBuffer);
iBLD_V_IP(ICVM_glGetClipPlane,glGetClipPlane);
iBLD_V_IP(ICVM_glClipPlane,glClipPlane);
iBLD_V_IIII(ICVM_glScissor,glScissor);
iBLD_V_P(ICVM_glEdgeFlagv,glEdgeFlagv);
iBLD_V_I(ICVM_glEdgeFlag,glEdgeFlag);
iBLD_V_P(ICVM_glGetPolygonStipple,glGetPolygonStipple);
iBLD_V_P(ICVM_glPolygonStipple,glPolygonStipple);
iBLD_V_FF(ICVM_glPolygonOffset,glPolygonOffset);
iBLD_V_II(ICVM_glPolygonMode,glPolygonMode);
iBLD_V_IU(ICVM_glLineStipple,glLineStipple);
iBLD_V_F(ICVM_glLineWidth,glLineWidth);
iBLD_V_F(ICVM_glPointSize,glPointSize);
iBLD_V_I(ICVM_glFrontFace,glFrontFace);
iBLD_V_I(ICVM_glCullFace,glCullFace);
iBLD_V_I(ICVM_glLogicOp,glLogicOp);
iBLD_V_II(ICVM_glBlendFunc,glBlendFunc);
iBLD_V_IF(ICVM_glAlphaFunc,glAlphaFunc);
iBLD_V_IIII(ICVM_glColorMask,glColorMask);
iBLD_V_U(ICVM_glIndexMask,glIndexMask);
iBLD_V_U(ICVM_glClear,glClear);
iBLD_V_FFFF(ICVM_glClearColor,glClearColor);
iBLD_V_F(ICVM_glClearIndex,glClearIndex);
iBLD_I_I(ICVM_glutGameModeGet,glutGameModeGet);
iBLD_V_V(ICVM_glutLeaveGameMode,glutLeaveGameMode);
iBLD_I_V(ICVM_glutEnterGameMode,glutEnterGameMode);
iBLD_V_P(ICVM_glutGameModeString,glutGameModeString);
iBLD_V_V(ICVM_glutForceJoystickFunc,glutForceJoystickFunc);
iBLD_V_I(ICVM_glutSetKeyRepeat,glutSetKeyRepeat);
iBLD_V_I(ICVM_glutIgnoreKeyRepeat,glutIgnoreKeyRepeat);
iBLD_V_V(ICVM_glutReportErrors,glutReportErrors);
iBLD_V_IIII(ICVM_glutVideoPan,glutVideoPan);
iBLD_V_IIII(ICVM_glutVideoResize,glutVideoResize);
iBLD_V_V(ICVM_glutStopVideoResizing,glutStopVideoResizing);
iBLD_V_V(ICVM_glutSetupVideoResizing,glutSetupVideoResizing);
iBLD_I_I(ICVM_glutVideoResizeGet,glutVideoResizeGet);
iBLD_V_V(ICVM_glutSolidIcosahedron,glutSolidIcosahedron);
iBLD_V_V(ICVM_glutWireIcosahedron,glutWireIcosahedron);
iBLD_V_V(ICVM_glutSolidTetrahedron,glutSolidTetrahedron);
iBLD_V_V(ICVM_glutWireTetrahedron,glutWireTetrahedron);
iBLD_V_V(ICVM_glutSolidOctahedron,glutSolidOctahedron);
iBLD_V_V(ICVM_glutWireOctahedron,glutWireOctahedron);
iBLD_V_D(ICVM_glutSolidTeapot,glutSolidTeapot);
iBLD_V_D(ICVM_glutWireTeapot,glutWireTeapot);
iBLD_V_V(ICVM_glutSolidDodecahedron,glutSolidDodecahedron);
iBLD_V_V(ICVM_glutWireDodecahedron,glutWireDodecahedron);
iBLD_V_DDII(ICVM_glutSolidTorus,glutSolidTorus);
iBLD_V_DDII(ICVM_glutWireTorus,glutWireTorus);
iBLD_V_D(ICVM_glutSolidCube,glutSolidCube);
iBLD_V_D(ICVM_glutWireCube,glutWireCube);
iBLD_V_DDII(ICVM_glutSolidCone,glutSolidCone);
iBLD_V_DDII(ICVM_glutWireCone,glutWireCone);
iBLD_V_DII(ICVM_glutSolidSphere,glutSolidSphere);
iBLD_V_DII(ICVM_glutWireSphere,glutWireSphere);
iBLD_I_PP(ICVM_glutStrokeLength,glutStrokeLength);
iBLD_I_PP(ICVM_glutBitmapLength,glutBitmapLength);
iBLD_I_PI(ICVM_glutStrokeWidth,glutStrokeWidth);
iBLD_V_PI(ICVM_glutStrokeCharacter,glutStrokeCharacter);
iBLD_I_PI(ICVM_glutBitmapWidth,glutBitmapWidth);
iBLD_V_PI(ICVM_glutBitmapCharacter,glutBitmapCharacter);
iBLD_I_I(ICVM_glutLayerGet,glutLayerGet);
iBLD_I_V(ICVM_glutGetModifiers,glutGetModifiers);
iBLD_I_P(ICVM_glutExtensionSupported,glutExtensionSupported);
iBLD_I_I(ICVM_glutDeviceGet,glutDeviceGet);
iBLD_I_I(ICVM_glutGet,glutGet);
iBLD_V_I(ICVM_glutCopyColormap,glutCopyColormap);
iBLD_F_II(ICVM_glutGetColor,glutGetColor);
iBLD_V_IFFF(ICVM_glutSetColor,glutSetColor);

static struct i_glcallback glutJoystickFunc_callbackp;
static void glutJoystickFunc_callback(u32 arg0,s32 arg1,s32 arg2,s32 arg3){
	iSTARTCALLBACKBODY(glutJoystickFunc_callbackp);
	iPUSHs32(arg3);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHu32(arg0);
	iENDCALLBACKBODY(glutJoystickFunc_callbackp);
};
static void ICVM_glutJoystickFunc(void){
	glutJoystickFunc_callbackp.machine=R_LM;
	glutJoystickFunc(
		(glutJoystickFunc_callbackp.callback=rSTKs32(0)) ? glutJoystickFunc_callback : (void *)0,
		rSTKs32(1)
	);
};



static struct i_glcallback glutSpecialUpFunc_callbackp;
static void glutSpecialUpFunc_callback(s32 arg0,s32 arg1,s32 arg2){
	iSTARTCALLBACKBODY(glutSpecialUpFunc_callbackp);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutSpecialUpFunc_callbackp);
};
static void ICVM_glutSpecialUpFunc(void){
	glutSpecialUpFunc_callbackp.machine=R_LM;
	glutSpecialUpFunc(
		(glutSpecialUpFunc_callbackp.callback=rSTKs32(0)) ? glutSpecialUpFunc_callback : (void *)0
	);
};



static struct i_glcallback glutKeyboardUpFunc_callbackp;
static void glutKeyboardUpFunc_callback(u32 arg0,s32 arg1,s32 arg2){
	iSTARTCALLBACKBODY(glutKeyboardUpFunc_callbackp);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHu32(arg0);
	iENDCALLBACKBODY(glutKeyboardUpFunc_callbackp);
};
static void ICVM_glutKeyboardUpFunc(void){
	glutKeyboardUpFunc_callbackp.machine=R_LM;
	glutKeyboardUpFunc(
		(glutKeyboardUpFunc_callbackp.callback=rSTKs32(0)) ? glutKeyboardUpFunc_callback : (void *)0
	);
};



static struct i_glcallback glutWindowStatusFunc_callbackp;
static void glutWindowStatusFunc_callback(s32 arg0){
	iSTARTCALLBACKBODY(glutWindowStatusFunc_callbackp);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutWindowStatusFunc_callbackp);
};
static void ICVM_glutWindowStatusFunc(void){
	glutWindowStatusFunc_callbackp.machine=R_LM;
	glutWindowStatusFunc(
		(glutWindowStatusFunc_callbackp.callback=rSTKs32(0)) ? glutWindowStatusFunc_callback : (void *)0
	);
};



static struct i_glcallback glutOverlayDisplayFunc_callbackp;
static void glutOverlayDisplayFunc_callback(void){
	iSTARTCALLBACKBODY(glutOverlayDisplayFunc_callbackp);
	iENDCALLBACKBODY(glutOverlayDisplayFunc_callbackp);
};
static void ICVM_glutOverlayDisplayFunc(void){
	glutOverlayDisplayFunc_callbackp.machine=R_LM;
	glutOverlayDisplayFunc(
		(glutOverlayDisplayFunc_callbackp.callback=rSTKs32(0)) ? glutOverlayDisplayFunc_callback : (void *)0
	);
};



static struct i_glcallback glutMenuStatusFunc_callbackp;
static void glutMenuStatusFunc_callback(s32 arg0,s32 arg1,s32 arg2){
	iSTARTCALLBACKBODY(glutMenuStatusFunc_callbackp);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutMenuStatusFunc_callbackp);
};
static void ICVM_glutMenuStatusFunc(void){
	glutMenuStatusFunc_callbackp.machine=R_LM;
	glutMenuStatusFunc(
		(glutMenuStatusFunc_callbackp.callback=rSTKs32(0)) ? glutMenuStatusFunc_callback : (void *)0
	);
};



static struct i_glcallback glutTabletButtonFunc_callbackp;
static void glutTabletButtonFunc_callback(s32 arg0,s32 arg1,s32 arg2,s32 arg3){
	iSTARTCALLBACKBODY(glutTabletButtonFunc_callbackp);
	iPUSHs32(arg3);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutTabletButtonFunc_callbackp);
};
static void ICVM_glutTabletButtonFunc(void){
	glutTabletButtonFunc_callbackp.machine=R_LM;
	glutTabletButtonFunc(
		(glutTabletButtonFunc_callbackp.callback=rSTKs32(0)) ? glutTabletButtonFunc_callback : (void *)0
	);
};



static struct i_glcallback glutTabletMotionFunc_callbackp;
static void glutTabletMotionFunc_callback(s32 arg0,s32 arg1){
	iSTARTCALLBACKBODY(glutTabletMotionFunc_callbackp);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutTabletMotionFunc_callbackp);
};
static void ICVM_glutTabletMotionFunc(void){
	glutTabletMotionFunc_callbackp.machine=R_LM;
	glutTabletMotionFunc(
		(glutTabletMotionFunc_callbackp.callback=rSTKs32(0)) ? glutTabletMotionFunc_callback : (void *)0
	);
};



static struct i_glcallback glutDialsFunc_callbackp;
static void glutDialsFunc_callback(s32 arg0,s32 arg1){
	iSTARTCALLBACKBODY(glutDialsFunc_callbackp);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutDialsFunc_callbackp);
};
static void ICVM_glutDialsFunc(void){
	glutDialsFunc_callbackp.machine=R_LM;
	glutDialsFunc(
		(glutDialsFunc_callbackp.callback=rSTKs32(0)) ? glutDialsFunc_callback : (void *)0
	);
};



static struct i_glcallback glutButtonBoxFunc_callbackp;
static void glutButtonBoxFunc_callback(s32 arg0,s32 arg1){
	iSTARTCALLBACKBODY(glutButtonBoxFunc_callbackp);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutButtonBoxFunc_callbackp);
};
static void ICVM_glutButtonBoxFunc(void){
	glutButtonBoxFunc_callbackp.machine=R_LM;
	glutButtonBoxFunc(
		(glutButtonBoxFunc_callbackp.callback=rSTKs32(0)) ? glutButtonBoxFunc_callback : (void *)0
	);
};



static struct i_glcallback glutSpaceballButtonFunc_callbackp;
static void glutSpaceballButtonFunc_callback(s32 arg0,s32 arg1){
	iSTARTCALLBACKBODY(glutSpaceballButtonFunc_callbackp);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutSpaceballButtonFunc_callbackp);
};
static void ICVM_glutSpaceballButtonFunc(void){
	glutSpaceballButtonFunc_callbackp.machine=R_LM;
	glutSpaceballButtonFunc(
		(glutSpaceballButtonFunc_callbackp.callback=rSTKs32(0)) ? glutSpaceballButtonFunc_callback : (void *)0
	);
};



static struct i_glcallback glutSpaceballRotateFunc_callbackp;
static void glutSpaceballRotateFunc_callback(s32 arg0,s32 arg1,s32 arg2){
	iSTARTCALLBACKBODY(glutSpaceballRotateFunc_callbackp);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutSpaceballRotateFunc_callbackp);
};
static void ICVM_glutSpaceballRotateFunc(void){
	glutSpaceballRotateFunc_callbackp.machine=R_LM;
	glutSpaceballRotateFunc(
		(glutSpaceballRotateFunc_callbackp.callback=rSTKs32(0)) ? glutSpaceballRotateFunc_callback : (void *)0
	);
};



static struct i_glcallback glutSpaceballMotionFunc_callbackp;
static void glutSpaceballMotionFunc_callback(s32 arg0,s32 arg1,s32 arg2){
	iSTARTCALLBACKBODY(glutSpaceballMotionFunc_callbackp);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutSpaceballMotionFunc_callbackp);
};
static void ICVM_glutSpaceballMotionFunc(void){
	glutSpaceballMotionFunc_callbackp.machine=R_LM;
	glutSpaceballMotionFunc(
		(glutSpaceballMotionFunc_callbackp.callback=rSTKs32(0)) ? glutSpaceballMotionFunc_callback : (void *)0
	);
};



static struct i_glcallback glutSpecialFunc_callbackp;
static void glutSpecialFunc_callback(s32 arg0,s32 arg1,s32 arg2){
	iSTARTCALLBACKBODY(glutSpecialFunc_callbackp);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutSpecialFunc_callbackp);
};
static void ICVM_glutSpecialFunc(void){
	glutSpecialFunc_callbackp.machine=R_LM;
	glutSpecialFunc(
		(glutSpecialFunc_callbackp.callback=rSTKs32(0)) ? glutSpecialFunc_callback : (void *)0
	);
};



static struct i_glcallback glutMenuStateFunc_callbackp;
static void glutMenuStateFunc_callback(s32 arg0){
	iSTARTCALLBACKBODY(glutMenuStateFunc_callbackp);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutMenuStateFunc_callbackp);
};
static void ICVM_glutMenuStateFunc(void){
	glutMenuStateFunc_callbackp.machine=R_LM;
	glutMenuStateFunc(
		(glutMenuStateFunc_callbackp.callback=rSTKs32(0)) ? glutMenuStateFunc_callback : (void *)0
	);
};



static struct i_glcallback glutTimerFunc_callbackp;
static void glutTimerFunc_callback(s32 arg0){
	iSTARTCALLBACKBODY(glutTimerFunc_callbackp);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutTimerFunc_callbackp);
};
static void ICVM_glutTimerFunc(void){
	glutTimerFunc_callbackp.machine=R_LM;
	glutTimerFunc(
		rSTKu32(0),
		(glutTimerFunc_callbackp.callback=rSTKs32(1)) ? glutTimerFunc_callback : (void *)0,
		rSTKs32(2)
	);
};



static struct i_glcallback glutIdleFunc_callbackp;
static void glutIdleFunc_callback(void){
	iSTARTCALLBACKBODY(glutIdleFunc_callbackp);
	iENDCALLBACKBODY(glutIdleFunc_callbackp);
};
static void ICVM_glutIdleFunc(void){
	glutIdleFunc_callbackp.machine=R_LM;
	glutIdleFunc(
		(glutIdleFunc_callbackp.callback=rSTKs32(0)) ? glutIdleFunc_callback : (void *)0
	);
};



static struct i_glcallback glutVisibilityFunc_callbackp;
static void glutVisibilityFunc_callback(s32 arg0){
	iSTARTCALLBACKBODY(glutVisibilityFunc_callbackp);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutVisibilityFunc_callbackp);
};
static void ICVM_glutVisibilityFunc(void){
	glutVisibilityFunc_callbackp.machine=R_LM;
	glutVisibilityFunc(
		(glutVisibilityFunc_callbackp.callback=rSTKs32(0)) ? glutVisibilityFunc_callback : (void *)0
	);
};



static struct i_glcallback glutEntryFunc_callbackp;
static void glutEntryFunc_callback(s32 arg0){
	iSTARTCALLBACKBODY(glutEntryFunc_callbackp);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutEntryFunc_callbackp);
};
static void ICVM_glutEntryFunc(void){
	glutEntryFunc_callbackp.machine=R_LM;
	glutEntryFunc(
		(glutEntryFunc_callbackp.callback=rSTKs32(0)) ? glutEntryFunc_callback : (void *)0
	);
};



static struct i_glcallback glutPassiveMotionFunc_callbackp;
static void glutPassiveMotionFunc_callback(s32 arg0,s32 arg1){
	iSTARTCALLBACKBODY(glutPassiveMotionFunc_callbackp);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutPassiveMotionFunc_callbackp);
};
static void ICVM_glutPassiveMotionFunc(void){
	glutPassiveMotionFunc_callbackp.machine=R_LM;
	glutPassiveMotionFunc(
		(glutPassiveMotionFunc_callbackp.callback=rSTKs32(0)) ? glutPassiveMotionFunc_callback : (void *)0
	);
};



static struct i_glcallback glutMotionFunc_callbackp;
static void glutMotionFunc_callback(s32 arg0,s32 arg1){
	iSTARTCALLBACKBODY(glutMotionFunc_callbackp);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutMotionFunc_callbackp);
};
static void ICVM_glutMotionFunc(void){
	glutMotionFunc_callbackp.machine=R_LM;
	glutMotionFunc(
		(glutMotionFunc_callbackp.callback=rSTKs32(0)) ? glutMotionFunc_callback : (void *)0
	);
};



static struct i_glcallback glutMouseFunc_callbackp;
static void glutMouseFunc_callback(s32 arg0,s32 arg1,s32 arg2,s32 arg3){
	iSTARTCALLBACKBODY(glutMouseFunc_callbackp);
	iPUSHs32(arg3);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutMouseFunc_callbackp);
};
static void ICVM_glutMouseFunc(void){
	glutMouseFunc_callbackp.machine=R_LM;
	glutMouseFunc(
		(glutMouseFunc_callbackp.callback=rSTKs32(0)) ? glutMouseFunc_callback : (void *)0
	);
};



static struct i_glcallback glutKeyboardFunc_callbackp;
static void glutKeyboardFunc_callback(u32 arg0,s32 arg1,s32 arg2){
	iSTARTCALLBACKBODY(glutKeyboardFunc_callbackp);
	iPUSHs32(arg2);
	iPUSHs32(arg1);
	iPUSHu32(arg0);
	iENDCALLBACKBODY(glutKeyboardFunc_callbackp);
};
static void ICVM_glutKeyboardFunc(void){
	glutKeyboardFunc_callbackp.machine=R_LM;
	glutKeyboardFunc(
		(glutKeyboardFunc_callbackp.callback=rSTKs32(0)) ? glutKeyboardFunc_callback : (void *)0
	);
};



static struct i_glcallback glutReshapeFunc_callbackp;
static void glutReshapeFunc_callback(s32 arg0,s32 arg1){
	iSTARTCALLBACKBODY(glutReshapeFunc_callbackp);
	iPUSHs32(arg1);
	iPUSHs32(arg0);
	iENDCALLBACKBODY(glutReshapeFunc_callbackp);
};
static void ICVM_glutReshapeFunc(void){
	glutReshapeFunc_callbackp.machine=R_LM;
	glutReshapeFunc(
		(glutReshapeFunc_callbackp.callback=rSTKs32(0)) ? glutReshapeFunc_callback : (void *)0
	);
};



static struct i_glcallback glutDisplayFunc_callbackp;
static void glutDisplayFunc_callback(void){
	iSTARTCALLBACKBODY(glutDisplayFunc_callbackp);
	iENDCALLBACKBODY(glutDisplayFunc_callbackp);
};
static void ICVM_glutDisplayFunc(void){
	glutDisplayFunc_callbackp.machine=R_LM;
	glutDisplayFunc(
		(glutDisplayFunc_callbackp.callback=rSTKs32(0)) ? glutDisplayFunc_callback : (void *)0
	);
};


iBLD_V_I(ICVM_glutDetachMenu,glutDetachMenu);
iBLD_V_I(ICVM_glutAttachMenu,glutAttachMenu);
iBLD_V_I(ICVM_glutRemoveMenuItem,glutRemoveMenuItem);
iBLD_V_IPI(ICVM_glutChangeToSubMenu,glutChangeToSubMenu);
iBLD_V_IPI(ICVM_glutChangeToMenuEntry,glutChangeToMenuEntry);
iBLD_V_PI(ICVM_glutAddSubMenu,glutAddSubMenu);
iBLD_V_PI(ICVM_glutAddMenuEntry,glutAddMenuEntry);
iBLD_V_I(ICVM_glutSetMenu,glutSetMenu);
iBLD_I_V(ICVM_glutGetMenu,glutGetMenu);
iBLD_V_V(ICVM_glutHideOverlay,glutHideOverlay);
iBLD_V_V(ICVM_glutShowOverlay,glutShowOverlay);
iBLD_V_I(ICVM_glutPostWindowOverlayRedisplay,glutPostWindowOverlayRedisplay);
iBLD_V_V(ICVM_glutPostOverlayRedisplay,glutPostOverlayRedisplay);
iBLD_V_I(ICVM_glutUseLayer,glutUseLayer);
iBLD_V_V(ICVM_glutRemoveOverlay,glutRemoveOverlay);
iBLD_V_V(ICVM_glutEstablishOverlay,glutEstablishOverlay);
iBLD_V_II(ICVM_glutWarpPointer,glutWarpPointer);
iBLD_V_I(ICVM_glutSetCursor,glutSetCursor);
iBLD_V_V(ICVM_glutFullScreen,glutFullScreen);
iBLD_V_V(ICVM_glutHideWindow,glutHideWindow);
iBLD_V_V(ICVM_glutShowWindow,glutShowWindow);
iBLD_V_V(ICVM_glutIconifyWindow,glutIconifyWindow);
iBLD_V_V(ICVM_glutPushWindow,glutPushWindow);
iBLD_V_V(ICVM_glutPopWindow,glutPopWindow);
iBLD_V_II(ICVM_glutReshapeWindow,glutReshapeWindow);
iBLD_V_II(ICVM_glutPositionWindow,glutPositionWindow);
iBLD_V_P(ICVM_glutSetIconTitle,glutSetIconTitle);
iBLD_V_P(ICVM_glutSetWindowTitle,glutSetWindowTitle);
iBLD_V_I(ICVM_glutSetWindow,glutSetWindow);
iBLD_I_V(ICVM_glutGetWindow,glutGetWindow);
iBLD_V_V(ICVM_glutSwapBuffers,glutSwapBuffers);
iBLD_V_I(ICVM_glutPostWindowRedisplay,glutPostWindowRedisplay);
iBLD_V_V(ICVM_glutPostRedisplay,glutPostRedisplay);
iBLD_V_I(ICVM_glutDestroyWindow,glutDestroyWindow);
iBLD_I_IIIII(ICVM_glutCreateSubWindow,glutCreateSubWindow);
iBLD_I_P(ICVM_glutCreateWindow,glutCreateWindow);
iBLD_V_V(ICVM_glutMainLoop,glutMainLoop);
iBLD_V_II(ICVM_glutInitWindowSize,glutInitWindowSize);
iBLD_V_II(ICVM_glutInitWindowPosition,glutInitWindowPosition);
iBLD_V_P(ICVM_glutInitDisplayString,glutInitDisplayString);
iBLD_V_U(ICVM_glutInitDisplayMode,glutInitDisplayMode);
iBLD_V_PP(ICVM_glutInit,glutInit);
iBLD_P_V(ICVM_glutGLUT_BITMAP_HELVETICA_18,glutGLUT_BITMAP_HELVETICA_18);
iBLD_P_V(ICVM_glutGLUT_BITMAP_HELVETICA_12,glutGLUT_BITMAP_HELVETICA_12);
iBLD_P_V(ICVM_glutGLUT_BITMAP_HELVETICA_10,glutGLUT_BITMAP_HELVETICA_10);
iBLD_P_V(ICVM_glutGLUT_BITMAP_TIMES_ROMAN_24,glutGLUT_BITMAP_TIMES_ROMAN_24);
iBLD_P_V(ICVM_glutGLUT_BITMAP_TIMES_ROMAN_10,glutGLUT_BITMAP_TIMES_ROMAN_10);
iBLD_P_V(ICVM_glutGLUT_BITMAP_8_BY_13,glutGLUT_BITMAP_8_BY_13);
iBLD_P_V(ICVM_glutGLUT_BITMAP_9_BY_15,glutGLUT_BITMAP_9_BY_15);
iBLD_P_V(ICVM_glutGLUT_STROKE_MONO_ROMAN,glutGLUT_STROKE_MONO_ROMAN);
iBLD_P_V(ICVM_glutGLUT_STROKE_ROMAN,glutGLUT_STROKE_ROMAN);









// base X system calls always hardwired into ICVM

SysCallFunction *OpenGLSysCalls[1024]={
	ICVM_glutDestroyMenu, // 3072
	ICVM_glutCreateMenu, // 3073
	0,	// 3074
	0,	// 3075
	0,	// 3076
	0,	// 3077
	0,	// 3078
	0,	// 3079
	0,	// 3080
	0,	// 3081
	0,	// 3082
	0,	// 3083
	0,	// 3084
	0,	// 3085
	0,	// 3086
	0,	// 3087
	0,	// 3088
	0,	// 3089
	0,	// 3090
	0,	// 3091
	0,	// 3092
	0,	// 3093
	0,	// 3094
	0,	// 3095
	0,	// 3096
	0,	// 3097
	0,	// 3098
	0,	// 3099
	0,	// 3100
	0,	// 3101
	0,	// 3102
	0,	// 3103
	0,	// 3104
	0,	// 3105
	0,	// 3106
	0,	// 3107
	0,	// 3108
	0,	// 3109
	0,	// 3110
	0,	// 3111
	0,	// 3112
	0,	// 3113
	0,	// 3114
	0,	// 3115
	0,	// 3116
	0,	// 3117
	0,	// 3118
	0,	// 3119
	0,	// 3120
	0,	// 3121
	0,	// 3122
	0,	// 3123
	0,	// 3124
	0,	// 3125
	0,	// 3126
	0,	// 3127
	0,	// 3128
	0,	// 3129
	0,	// 3130
	0,	// 3131
	0,	// 3132
	0,	// 3133
	0,	// 3134
	0,	// 3135
	0,	// 3136
	0,	// 3137
	0,	// 3138
	0,	// 3139
	0,	// 3140
	0,	// 3141
	0,	// 3142
	0,	// 3143
	0,	// 3144
	0,	// 3145
	0,	// 3146
	0,	// 3147
	0,	// 3148
	0,	// 3149
	0,	// 3150
	0,	// 3151
	0,	// 3152
	0,	// 3153
	0,	// 3154
	0,	// 3155
	0,	// 3156
	0,	// 3157
	0,	// 3158
	0,	// 3159
	0,	// 3160
	0,	// 3161
	0,	// 3162
	0,	// 3163
	0,	// 3164
	0,	// 3165
	0,	// 3166
	0,	// 3167
	0,	// 3168
	0,	// 3169
	0,	// 3170
	0,	// 3171
	0,	// 3172
	0,	// 3173
	0,	// 3174
	0,	// 3175
	0,	// 3176
	0,	// 3177
	0,	// 3178
	0,	// 3179
	0,	// 3180
	0,	// 3181
	0,	// 3182
	0,	// 3183
	0,	// 3184
	0,	// 3185
	0,	// 3186
	0,	// 3187
	0,	// 3188
	0,	// 3189
	0,	// 3190
	0,	// 3191
	0,	// 3192
	0,	// 3193
	0,	// 3194
	0,	// 3195
	0,	// 3196
	0,	// 3197
	0,	// 3198
	0,	// 3199



	pV12(ICVM_glHintPGI), // 3200
	pV12(ICVM_glUnlockArraysEXT), // 3201
	pV12(ICVM_glLockArraysEXT), // 3202
	pV12(ICVM_glGetSeparableFilter), // 3203
	pV12(ICVM_glSeparableFilter2D), // 3204
	pV12(ICVM_glGetConvolutionParameteriv), // 3205
	pV12(ICVM_glGetConvolutionParameterfv), // 3206
	pV12(ICVM_glGetConvolutionFilter), // 3207
	pV12(ICVM_glCopyConvolutionFilter2D), // 3208
	pV12(ICVM_glCopyConvolutionFilter1D), // 3209
	pV12(ICVM_glConvolutionParameteriv), // 3210
	pV12(ICVM_glConvolutionParameteri), // 3211
	pV12(ICVM_glConvolutionParameterfv), // 3212
	pV12(ICVM_glConvolutionParameterf), // 3213
	pV12(ICVM_glConvolutionFilter2D), // 3214
	pV12(ICVM_glConvolutionFilter1D), // 3215
	pV12(ICVM_glGetMinmaxParameteriv), // 3216
	pV12(ICVM_glGetMinmaxParameterfv), // 3217
	pV12(ICVM_glGetMinmax), // 3218
	pV12(ICVM_glResetMinmax), // 3219
	pV12(ICVM_glMinmax), // 3220
	pV12(ICVM_glGetHistogramParameteriv), // 3221
	pV12(ICVM_glGetHistogramParameterfv), // 3222
	pV12(ICVM_glGetHistogram), // 3223
	pV12(ICVM_glResetHistogram), // 3224
	pV12(ICVM_glHistogram), // 3225
	pV12(ICVM_glBlendColor), // 3226
	pV12(ICVM_glBlendEquation), // 3227
	pV12(ICVM_glGetColorTableParameteriv), // 3228
	pV12(ICVM_glGetColorTableParameterfv), // 3229
	pV12(ICVM_glGetColorTable), // 3230
	pV12(ICVM_glCopyColorTable), // 3231
	pV12(ICVM_glCopyColorSubTable), // 3232
	pV12(ICVM_glColorTableParameterfv), // 3233
	pV12(ICVM_glColorTableParameteriv), // 3234
	pV12(ICVM_glColorSubTable), // 3235
	pV12(ICVM_glColorTable), // 3236
	pV12(ICVM_glCopyTexSubImage3D), // 3237
	pV12(ICVM_glTexSubImage3D), // 3238
	pV12(ICVM_glTexImage3D), // 3239
	pV12(ICVM_glDrawRangeElements), // 3240
		0, // -- MISSING ???	ICVM_glBlendFuncSeparateINGR, // 3241
	pV12(ICVM_glPointParameterfvEXT), // 3242
	pV12(ICVM_glPointParameterfEXT), // 3243
	pV12(ICVM_glMultiTexCoord4svARB), // 3244
	pV12(ICVM_glMultiTexCoord4sARB), // 3245
	pV12(ICVM_glMultiTexCoord4ivARB), // 3246
	pV12(ICVM_glMultiTexCoord4iARB), // 3247
	pV12(ICVM_glMultiTexCoord4fvARB), // 3248
	pV12(ICVM_glMultiTexCoord4fARB), // 3249
	pV12(ICVM_glMultiTexCoord4dvARB), // 3250
	pV12(ICVM_glMultiTexCoord4dARB), // 3251
	pV12(ICVM_glMultiTexCoord3svARB), // 3252
	pV12(ICVM_glMultiTexCoord3sARB), // 3253
	pV12(ICVM_glMultiTexCoord3ivARB), // 3254
	pV12(ICVM_glMultiTexCoord3iARB), // 3255
	pV12(ICVM_glMultiTexCoord3fvARB), // 3256
	pV12(ICVM_glMultiTexCoord3fARB), // 3257
	pV12(ICVM_glMultiTexCoord3dvARB), // 3258
	pV12(ICVM_glMultiTexCoord3dARB), // 3259
	pV12(ICVM_glMultiTexCoord2svARB), // 3260
	pV12(ICVM_glMultiTexCoord2sARB), // 3261
	pV12(ICVM_glMultiTexCoord2ivARB), // 3262
	pV12(ICVM_glMultiTexCoord2iARB), // 3263
	pV12(ICVM_glMultiTexCoord2fvARB), // 3264
	pV12(ICVM_glMultiTexCoord2fARB), // 3265
	pV12(ICVM_glMultiTexCoord2dvARB), // 3266
	pV12(ICVM_glMultiTexCoord2dARB), // 3267
	pV12(ICVM_glMultiTexCoord1svARB), // 3268
	pV12(ICVM_glMultiTexCoord1sARB), // 3269
	pV12(ICVM_glMultiTexCoord1ivARB), // 3270
	pV12(ICVM_glMultiTexCoord1iARB), // 3271
	pV12(ICVM_glMultiTexCoord1fvARB), // 3272
	pV12(ICVM_glMultiTexCoord1fARB), // 3273
	pV12(ICVM_glMultiTexCoord1dvARB), // 3274
	pV12(ICVM_glMultiTexCoord1dARB), // 3275
	pV12(ICVM_glClientActiveTextureARB), // 3276
	pV12(ICVM_glActiveTextureARB), // 3277
	pV12(ICVM_glGetColorTableParameterivEXT), // 3278
	pV12(ICVM_glGetColorTableParameterfvEXT), // 3279
	pV12(ICVM_glGetColorTableEXT), // 3280
	pV12(ICVM_glColorSubTableEXT), // 3281
	pV12(ICVM_glColorTableEXT), // 3282
	pV12(ICVM_glCopyTexSubImage3DEXT), // 3283
	pV12(ICVM_glTexSubImage3DEXT), // 3284
	pV12(ICVM_glTexImage3DEXT), // 3285
	pV12(ICVM_glIsTextureEXT), // 3286
	pV12(ICVM_glAreTexturesResidentEXT), // 3287
	pV12(ICVM_glPrioritizeTexturesEXT), // 3288
	pV12(ICVM_glBindTextureEXT), // 3289
	pV12(ICVM_glDeleteTexturesEXT), // 3290
	pV12(ICVM_glGenTexturesEXT), // 3291
	pV12(ICVM_glDrawArraysEXT), // 3292
	pV12(ICVM_glArrayElementEXT), // 3293
	pV12(ICVM_glGetPointervEXT), // 3294
	pV12(ICVM_glEdgeFlagPointerEXT), // 3295
	pV12(ICVM_glTexCoordPointerEXT), // 3296
	pV12(ICVM_glIndexPointerEXT), // 3297
	pV12(ICVM_glColorPointerEXT), // 3298
	pV12(ICVM_glNormalPointerEXT), // 3299
	pV12(ICVM_glVertexPointerEXT), // 3300
	pV12(ICVM_glPolygonOffsetEXT), // 3301
	pV12(ICVM_glBlendColorEXT), // 3302
	pV12(ICVM_glBlendEquationEXT), // 3303
	ICVM_glPopName, // 3304
	ICVM_glPushName, // 3305
	ICVM_glLoadName, // 3306
	ICVM_glInitNames, // 3307
	ICVM_glSelectBuffer, // 3308
	ICVM_glPassThrough, // 3309
	ICVM_glFeedbackBuffer, // 3310
	ICVM_glFogiv, // 3311
	ICVM_glFogfv, // 3312
	ICVM_glFogi, // 3313
	ICVM_glFogf, // 3314
	ICVM_glEvalMesh2, // 3315
	ICVM_glEvalMesh1, // 3316
	ICVM_glEvalPoint2, // 3317
	ICVM_glEvalPoint1, // 3318
	ICVM_glMapGrid2f, // 3319
	ICVM_glMapGrid2d, // 3320
	ICVM_glMapGrid1f, // 3321
	ICVM_glMapGrid1d, // 3322
	ICVM_glEvalCoord2fv, // 3323
	ICVM_glEvalCoord2dv, // 3324
	ICVM_glEvalCoord2f, // 3325
	ICVM_glEvalCoord2d, // 3326
	ICVM_glEvalCoord1fv, // 3327
	ICVM_glEvalCoord1dv, // 3328
	ICVM_glEvalCoord1f, // 3329
	ICVM_glEvalCoord1d, // 3330
	ICVM_glGetMapiv, // 3331
	ICVM_glGetMapfv, // 3332
	ICVM_glGetMapdv, // 3333
	ICVM_glMap2f, // 3334
	ICVM_glMap2d, // 3335
	ICVM_glMap1f, // 3336
	ICVM_glMap1d, // 3337
	ICVM_glCopyTexSubImage2D, // 3338
	ICVM_glCopyTexSubImage1D, // 3339
	ICVM_glCopyTexImage2D, // 3340
	ICVM_glCopyTexImage1D, // 3341
	ICVM_glTexSubImage2D, // 3342
	ICVM_glTexSubImage1D, // 3343
	ICVM_glIsTexture, // 3344
	ICVM_glAreTexturesResident, // 3345
	ICVM_glPrioritizeTextures, // 3346
	ICVM_glBindTexture, // 3347
	ICVM_glDeleteTextures, // 3348
	ICVM_glGenTextures, // 3349
	ICVM_glGetTexImage, // 3350
	ICVM_glTexImage2D, // 3351
	ICVM_glTexImage1D, // 3352
	ICVM_glGetTexLevelParameteriv, // 3353
	ICVM_glGetTexLevelParameterfv, // 3354
	ICVM_glGetTexParameteriv, // 3355
	ICVM_glGetTexParameterfv, // 3356
	ICVM_glTexParameteriv, // 3357
	ICVM_glTexParameterfv, // 3358
	ICVM_glTexParameteri, // 3359
	ICVM_glTexParameterf, // 3360
	ICVM_glGetTexEnviv, // 3361
	ICVM_glGetTexEnvfv, // 3362
	ICVM_glTexEnviv, // 3363
	ICVM_glTexEnvfv, // 3364
	ICVM_glTexEnvi, // 3365
	ICVM_glTexEnvf, // 3366
	ICVM_glGetTexGeniv, // 3367
	ICVM_glGetTexGenfv, // 3368
	ICVM_glGetTexGendv, // 3369
	ICVM_glTexGeniv, // 3370
	ICVM_glTexGenfv, // 3371
	ICVM_glTexGendv, // 3372
	ICVM_glTexGeni, // 3373
	ICVM_glTexGenf, // 3374
	ICVM_glTexGend, // 3375
	ICVM_glClearStencil, // 3376
	ICVM_glStencilOp, // 3377
	ICVM_glStencilMask, // 3378
	ICVM_glStencilFunc, // 3379
	ICVM_glCopyPixels, // 3380
	ICVM_glDrawPixels, // 3381
	ICVM_glReadPixels, // 3382
	ICVM_glBitmap, // 3383
	ICVM_glGetPixelMapusv, // 3384
	ICVM_glGetPixelMapuiv, // 3385
	ICVM_glGetPixelMapfv, // 3386
	ICVM_glPixelMapusv, // 3387
	ICVM_glPixelMapuiv, // 3388
	ICVM_glPixelMapfv, // 3389
	ICVM_glPixelTransferi, // 3390
	ICVM_glPixelTransferf, // 3391
	ICVM_glPixelStorei, // 3392
	ICVM_glPixelStoref, // 3393
	ICVM_glPixelZoom, // 3394
	ICVM_glColorMaterial, // 3395
	ICVM_glGetMaterialiv, // 3396
	ICVM_glGetMaterialfv, // 3397
	ICVM_glMaterialiv, // 3398
	ICVM_glMaterialfv, // 3399
	ICVM_glMateriali, // 3400
	ICVM_glMaterialf, // 3401
	ICVM_glLightModeliv, // 3402
	ICVM_glLightModelfv, // 3403
	ICVM_glLightModeli, // 3404
	ICVM_glLightModelf, // 3405
	ICVM_glGetLightiv, // 3406
	ICVM_glGetLightfv, // 3407
	ICVM_glLightiv, // 3408
	ICVM_glLightfv, // 3409
	ICVM_glLighti, // 3410
	ICVM_glLightf, // 3411
	ICVM_glShadeModel, // 3412
	ICVM_glInterleavedArrays, // 3413
	ICVM_glDrawElements, // 3414
	ICVM_glDrawArrays, // 3415
	ICVM_glArrayElement, // 3416
	ICVM_glGetPointerv, // 3417
	ICVM_glEdgeFlagPointer, // 3418
	ICVM_glTexCoordPointer, // 3419
	ICVM_glIndexPointer, // 3420
	ICVM_glColorPointer, // 3421
	ICVM_glNormalPointer, // 3422
	ICVM_glVertexPointer, // 3423
	ICVM_glRectsv, // 3424
	ICVM_glRectiv, // 3425
	ICVM_glRectfv, // 3426
	ICVM_glRectdv, // 3427
	ICVM_glRects, // 3428
	ICVM_glRecti, // 3429
	ICVM_glRectf, // 3430
	ICVM_glRectd, // 3431
	ICVM_glRasterPos4sv, // 3432
	ICVM_glRasterPos4iv, // 3433
	ICVM_glRasterPos4fv, // 3434
	ICVM_glRasterPos4dv, // 3435
	ICVM_glRasterPos3sv, // 3436
	ICVM_glRasterPos3iv, // 3437
	ICVM_glRasterPos3fv, // 3438
	ICVM_glRasterPos3dv, // 3439
	ICVM_glRasterPos2sv, // 3440
	ICVM_glRasterPos2iv, // 3441
	ICVM_glRasterPos2fv, // 3442
	ICVM_glRasterPos2dv, // 3443
	ICVM_glRasterPos4s, // 3444
	ICVM_glRasterPos4i, // 3445
	ICVM_glRasterPos4f, // 3446
	ICVM_glRasterPos4d, // 3447
	ICVM_glRasterPos3s, // 3448
	ICVM_glRasterPos3i, // 3449
	ICVM_glRasterPos3f, // 3450
	ICVM_glRasterPos3d, // 3451
	ICVM_glRasterPos2s, // 3452
	ICVM_glRasterPos2i, // 3453
	ICVM_glRasterPos2f, // 3454
	ICVM_glRasterPos2d, // 3455
	ICVM_glTexCoord4sv, // 3456
	ICVM_glTexCoord4iv, // 3457
	ICVM_glTexCoord4fv, // 3458
	ICVM_glTexCoord4dv, // 3459
	ICVM_glTexCoord3sv, // 3460
	ICVM_glTexCoord3iv, // 3461
	ICVM_glTexCoord3fv, // 3462
	ICVM_glTexCoord3dv, // 3463
	ICVM_glTexCoord2sv, // 3464
	ICVM_glTexCoord2iv, // 3465
	ICVM_glTexCoord2fv, // 3466
	ICVM_glTexCoord2dv, // 3467
	ICVM_glTexCoord1sv, // 3468
	ICVM_glTexCoord1iv, // 3469
	ICVM_glTexCoord1fv, // 3470
	ICVM_glTexCoord1dv, // 3471
	ICVM_glTexCoord4s, // 3472
	ICVM_glTexCoord4i, // 3473
	ICVM_glTexCoord4f, // 3474
	ICVM_glTexCoord4d, // 3475
	ICVM_glTexCoord3s, // 3476
	ICVM_glTexCoord3i, // 3477
	ICVM_glTexCoord3f, // 3478
	ICVM_glTexCoord3d, // 3479
	ICVM_glTexCoord2s, // 3480
	ICVM_glTexCoord2i, // 3481
	ICVM_glTexCoord2f, // 3482
	ICVM_glTexCoord2d, // 3483
	ICVM_glTexCoord1s, // 3484
	ICVM_glTexCoord1i, // 3485
	ICVM_glTexCoord1f, // 3486
	ICVM_glTexCoord1d, // 3487
	ICVM_glColor4usv, // 3488
	ICVM_glColor4uiv, // 3489
	ICVM_glColor4ubv, // 3490
	ICVM_glColor4sv, // 3491
	ICVM_glColor4iv, // 3492
	ICVM_glColor4fv, // 3493
	ICVM_glColor4dv, // 3494
	ICVM_glColor4bv, // 3495
	ICVM_glColor3usv, // 3496
	ICVM_glColor3uiv, // 3497
	ICVM_glColor3ubv, // 3498
	ICVM_glColor3sv, // 3499
	ICVM_glColor3iv, // 3500
	ICVM_glColor3fv, // 3501
	ICVM_glColor3dv, // 3502
	ICVM_glColor3bv, // 3503
	ICVM_glColor4us, // 3504
	ICVM_glColor4ui, // 3505
	ICVM_glColor4ub, // 3506
	ICVM_glColor4s, // 3507
	ICVM_glColor4i, // 3508
	ICVM_glColor4f, // 3509
	ICVM_glColor4d, // 3510
	ICVM_glColor4b, // 3511
	ICVM_glColor3us, // 3512
	ICVM_glColor3ui, // 3513
	ICVM_glColor3ub, // 3514
	ICVM_glColor3s, // 3515
	ICVM_glColor3i, // 3516
	ICVM_glColor3f, // 3517
	ICVM_glColor3d, // 3518
	ICVM_glColor3b, // 3519
	ICVM_glIndexubv, // 3520
	ICVM_glIndexsv, // 3521
	ICVM_glIndexiv, // 3522
	ICVM_glIndexfv, // 3523
	ICVM_glIndexdv, // 3524
	ICVM_glIndexub, // 3525
	ICVM_glIndexs, // 3526
	ICVM_glIndexi, // 3527
	ICVM_glIndexf, // 3528
	ICVM_glIndexd, // 3529
	ICVM_glNormal3sv, // 3530
	ICVM_glNormal3iv, // 3531
	ICVM_glNormal3fv, // 3532
	ICVM_glNormal3dv, // 3533
	ICVM_glNormal3bv, // 3534
	ICVM_glNormal3s, // 3535
	ICVM_glNormal3i, // 3536
	ICVM_glNormal3f, // 3537
	ICVM_glNormal3d, // 3538
	ICVM_glNormal3b, // 3539
	ICVM_glVertex4sv, // 3540
	ICVM_glVertex4iv, // 3541
	ICVM_glVertex4fv, // 3542
	ICVM_glVertex4dv, // 3543
	ICVM_glVertex3sv, // 3544
	ICVM_glVertex3iv, // 3545
	ICVM_glVertex3fv, // 3546
	ICVM_glVertex3dv, // 3547
	ICVM_glVertex2sv, // 3548
	ICVM_glVertex2iv, // 3549
	ICVM_glVertex2fv, // 3550
	ICVM_glVertex2dv, // 3551
	ICVM_glVertex4s, // 3552
	ICVM_glVertex4i, // 3553
	ICVM_glVertex4f, // 3554
	ICVM_glVertex4d, // 3555
	ICVM_glVertex3s, // 3556
	ICVM_glVertex3i, // 3557
	ICVM_glVertex3f, // 3558
	ICVM_glVertex3d, // 3559
	ICVM_glVertex2s, // 3560
	ICVM_glVertex2i, // 3561
	ICVM_glVertex2f, // 3562
	ICVM_glVertex2d, // 3563
	ICVM_glEnd, // 3564
	ICVM_glBegin, // 3565
	ICVM_glListBase, // 3566
	ICVM_glCallLists, // 3567
	ICVM_glCallList, // 3568
	ICVM_glEndList, // 3569
	ICVM_glNewList, // 3570
	ICVM_glGenLists, // 3571
	ICVM_glDeleteLists, // 3572
	ICVM_glIsList, // 3573
	ICVM_glTranslatef, // 3574
	ICVM_glTranslated, // 3575
	ICVM_glScalef, // 3576
	ICVM_glScaled, // 3577
	ICVM_glRotatef, // 3578
	ICVM_glRotated, // 3579
	ICVM_glMultMatrixf, // 3580
	ICVM_glMultMatrixd, // 3581
	ICVM_glLoadMatrixf, // 3582
	ICVM_glLoadMatrixd, // 3583
	ICVM_glLoadIdentity, // 3584
	ICVM_glPopMatrix, // 3585
	ICVM_glPushMatrix, // 3586
	ICVM_glViewport, // 3587
	ICVM_glFrustum, // 3588
	ICVM_glOrtho, // 3589
	ICVM_glMatrixMode, // 3590
	ICVM_glAccum, // 3591
	ICVM_glClearAccum, // 3592
	ICVM_glDepthRange, // 3593
	ICVM_glDepthMask, // 3594
	ICVM_glDepthFunc, // 3595
	ICVM_glClearDepth, // 3596
	ICVM_glHint, // 3597
	ICVM_glFlush, // 3598
	ICVM_glFinish, // 3599
	ICVM_glGetString, // 3600
	ICVM_glGetError, // 3601
	ICVM_glRenderMode, // 3602
	ICVM_glPopClientAttrib, // 3603
	ICVM_glPushClientAttrib, // 3604
	ICVM_glPopAttrib, // 3605
	ICVM_glPushAttrib, // 3606
	ICVM_glGetIntegerv, // 3607
	ICVM_glGetFloatv, // 3608
	ICVM_glGetDoublev, // 3609
	ICVM_glGetBooleanv, // 3610
	ICVM_glDisableClientState, // 3611
	ICVM_glEnableClientState, // 3612
	ICVM_glIsEnabled, // 3613
	ICVM_glDisable, // 3614
	ICVM_glEnable, // 3615
	ICVM_glReadBuffer, // 3616
	ICVM_glDrawBuffer, // 3617
	ICVM_glGetClipPlane, // 3618
	ICVM_glClipPlane, // 3619
	ICVM_glScissor, // 3620
	ICVM_glEdgeFlagv, // 3621
	ICVM_glEdgeFlag, // 3622
	ICVM_glGetPolygonStipple, // 3623
	ICVM_glPolygonStipple, // 3624
	ICVM_glPolygonOffset, // 3625
	ICVM_glPolygonMode, // 3626
	ICVM_glLineStipple, // 3627
	ICVM_glLineWidth, // 3628
	ICVM_glPointSize, // 3629
	ICVM_glFrontFace, // 3630
	ICVM_glCullFace, // 3631
	ICVM_glLogicOp, // 3632
	ICVM_glBlendFunc, // 3633
	ICVM_glAlphaFunc, // 3634
	ICVM_glColorMask, // 3635
	ICVM_glIndexMask, // 3636
	ICVM_glClear, // 3637
	ICVM_glClearColor, // 3638
	ICVM_glClearIndex, // 3639
	ICVM_glutGameModeGet, // 3640
	ICVM_glutLeaveGameMode, // 3641
	ICVM_glutEnterGameMode, // 3642
	ICVM_glutGameModeString, // 3643
	ICVM_glutForceJoystickFunc, // 3644
	ICVM_glutSetKeyRepeat, // 3645
	ICVM_glutIgnoreKeyRepeat, // 3646
	ICVM_glutReportErrors, // 3647
	ICVM_glutVideoPan, // 3648
	ICVM_glutVideoResize, // 3649
	ICVM_glutStopVideoResizing, // 3650
	ICVM_glutSetupVideoResizing, // 3651
	ICVM_glutVideoResizeGet, // 3652
	ICVM_glutSolidIcosahedron, // 3653
	ICVM_glutWireIcosahedron, // 3654
	ICVM_glutSolidTetrahedron, // 3655
	ICVM_glutWireTetrahedron, // 3656
	ICVM_glutSolidOctahedron, // 3657
	ICVM_glutWireOctahedron, // 3658
	ICVM_glutSolidTeapot, // 3659
	ICVM_glutWireTeapot, // 3660
	ICVM_glutSolidDodecahedron, // 3661
	ICVM_glutWireDodecahedron, // 3662
	ICVM_glutSolidTorus, // 3663
	ICVM_glutWireTorus, // 3664
	ICVM_glutSolidCube, // 3665
	ICVM_glutWireCube, // 3666
	ICVM_glutSolidCone, // 3667
	ICVM_glutWireCone, // 3668
	ICVM_glutSolidSphere, // 3669
	ICVM_glutWireSphere, // 3670
	ICVM_glutStrokeLength, // 3671
	ICVM_glutBitmapLength, // 3672
	ICVM_glutStrokeWidth, // 3673
	ICVM_glutStrokeCharacter, // 3674
	ICVM_glutBitmapWidth, // 3675
	ICVM_glutBitmapCharacter, // 3676
	ICVM_glutLayerGet, // 3677
	ICVM_glutGetModifiers, // 3678
	ICVM_glutExtensionSupported, // 3679
	ICVM_glutDeviceGet, // 3680
	ICVM_glutGet, // 3681
	ICVM_glutCopyColormap, // 3682
	ICVM_glutGetColor, // 3683
	ICVM_glutSetColor, // 3684
	ICVM_glutJoystickFunc, // 3685
	ICVM_glutSpecialUpFunc, // 3686
	ICVM_glutKeyboardUpFunc, // 3687
	ICVM_glutWindowStatusFunc, // 3688
	ICVM_glutOverlayDisplayFunc, // 3689
	ICVM_glutMenuStatusFunc, // 3690
	ICVM_glutTabletButtonFunc, // 3691
	ICVM_glutTabletMotionFunc, // 3692
	ICVM_glutDialsFunc, // 3693
	ICVM_glutButtonBoxFunc, // 3694
	ICVM_glutSpaceballButtonFunc, // 3695
	ICVM_glutSpaceballRotateFunc, // 3696
	ICVM_glutSpaceballMotionFunc, // 3697
	ICVM_glutSpecialFunc, // 3698
	ICVM_glutMenuStateFunc, // 3699
	ICVM_glutTimerFunc, // 3700
	ICVM_glutIdleFunc, // 3701
	ICVM_glutVisibilityFunc, // 3702
	ICVM_glutEntryFunc, // 3703
	ICVM_glutPassiveMotionFunc, // 3704
	ICVM_glutMotionFunc, // 3705
	ICVM_glutMouseFunc, // 3706
	ICVM_glutKeyboardFunc, // 3707
	ICVM_glutReshapeFunc, // 3708
	ICVM_glutDisplayFunc, // 3709
	ICVM_glutDetachMenu, // 3710
	ICVM_glutAttachMenu, // 3711
	ICVM_glutRemoveMenuItem, // 3712
	ICVM_glutChangeToSubMenu, // 3713
	ICVM_glutChangeToMenuEntry, // 3714
	ICVM_glutAddSubMenu, // 3715
	ICVM_glutAddMenuEntry, // 3716
	ICVM_glutSetMenu, // 3717
	ICVM_glutGetMenu, // 3718
	ICVM_glutHideOverlay, // 3719
	ICVM_glutShowOverlay, // 3720
	ICVM_glutPostWindowOverlayRedisplay, // 3721
	ICVM_glutPostOverlayRedisplay, // 3722
	ICVM_glutUseLayer, // 3723
	ICVM_glutRemoveOverlay, // 3724
	ICVM_glutEstablishOverlay, // 3725
	ICVM_glutWarpPointer, // 3726
	ICVM_glutSetCursor, // 3727
	ICVM_glutFullScreen, // 3728
	ICVM_glutHideWindow, // 3729
	ICVM_glutShowWindow, // 3730
	ICVM_glutIconifyWindow, // 3731
	ICVM_glutPushWindow, // 3732
	ICVM_glutPopWindow, // 3733
	ICVM_glutReshapeWindow, // 3734
	ICVM_glutPositionWindow, // 3735
	ICVM_glutSetIconTitle, // 3736
	ICVM_glutSetWindowTitle, // 3737
	ICVM_glutSetWindow, // 3738
	ICVM_glutGetWindow, // 3739
	ICVM_glutSwapBuffers, // 3740
	ICVM_glutPostWindowRedisplay, // 3741
	ICVM_glutPostRedisplay, // 3742
	ICVM_glutDestroyWindow, // 3743
	ICVM_glutCreateSubWindow, // 3744
	ICVM_glutCreateWindow, // 3745
	ICVM_glutMainLoop, // 3746
	ICVM_glutInitWindowSize, // 3747
	ICVM_glutInitWindowPosition, // 3748
	ICVM_glutInitDisplayString, // 3749
	ICVM_glutInitDisplayMode, // 3750
	ICVM_glutInit, // 3751
	ICVM_glutGLUT_BITMAP_HELVETICA_18, // 3752
	ICVM_glutGLUT_BITMAP_HELVETICA_12, // 3753
	ICVM_glutGLUT_BITMAP_HELVETICA_10, // 3754
	ICVM_glutGLUT_BITMAP_TIMES_ROMAN_24, // 3755
	ICVM_glutGLUT_BITMAP_TIMES_ROMAN_10, // 3756
	ICVM_glutGLUT_BITMAP_8_BY_13, // 3757
	ICVM_glutGLUT_BITMAP_9_BY_15, // 3758
	ICVM_glutGLUT_STROKE_MONO_ROMAN, // 3759
	ICVM_glutGLUT_STROKE_ROMAN, // 3760



};

