
#define MAKESYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");

#define MAKEERRSYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-116,%fp),_errno"); \
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");



#include <syscall.h>
#include <unistd.h>


/* surrounded with a dummy call so we can access system header files.
   these actually turn to jumps into the Host handlers 
   so we just need to compile
   the jump vectors.   */


/* basic operating level system calls. 
*/

static void dummy(void){


// sqrt call
	  asm("\n_sqrt:");
	  asm("\n__builtin_fsqrt:");
	  asm(".globl _sqrt");
	  asm("\n__builtin_sqrt:");
	  asm(".globl __builtin_sqrt");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("sqrtd (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// sqrtf call
	  asm("\n_sqrtf:");
	  asm("\n__builtin_sqrtf:");
	  asm("\n__builtin_sqrtf:");
	  asm(".globl __builtin_sqrtf");
	  asm(".globl _sqrtf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("sqrtf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// sin call
	  asm("\n_sin:");
	  asm(".globl _sin");
	  asm("\n__builtin_sin:");
	  asm(".globl __builtin_sin");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("sind (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// sinf call
	  asm("\n_sinf:");
	  asm(".globl _sinf");
	  asm("\n__builtin_sinf:");
	  asm(".globl __builtin_sinf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("sinf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// cos call
	  asm("\n_cos:");
	  asm(".globl _cos");
	  asm("\n__builtin_cos:");
	  asm(".globl __builtin_cos");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("cosd (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// cosf call
	  asm("\n_cosf:");
	  asm(".globl _cosf");
	  asm("\n__builtin_cosf:");
	  asm(".globl __builtin_cosf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("cosf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// tan call
	  asm("\n_tan:");
	  asm(".globl _tan");
	  asm("\n__builtin_tan:");
	  asm(".globl __builtin_tan");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("tand (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// tanf call
	  asm("\n_tanf:");
	  asm(".globl _tanf");
	  asm("\n__builtin_tanf:");
	  asm(".globl __builtin_tanf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("tanf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// asin call
	  asm("\n_asin:");
	  asm(".globl _asin");
	  asm("\n__builtin_asin:");
	  asm(".globl __builtin_asin");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("asind (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// asinf call
	  asm("\n_asinf:");
	  asm(".globl _asinf");
	  asm("\n__builtin_asinf:");
	  asm(".globl __builtin_asinf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("asinf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// acos call
	  asm("\n_acos:");
	  asm(".globl _acos");
	  asm("\n__builtin_acos:");
	  asm(".globl __builtin_acos");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("acosd (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// acosf call
	  asm("\n_acosf:");
	  asm(".globl _acosf");
	  asm("\n__builtin_acosf:");
	  asm(".globl __builtin_acosf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("acosf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// atan call
	  asm("\n_atan:");
	  asm(".globl _atan");
	  asm("\n__builtin_atan:");
	  asm(".globl __builtin_atan");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("atand (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// atanf call
	  asm("\n_atanf:");
	  asm(".globl _atanf");
	  asm("\n__builtin_atanf:");
	  asm(".globl __builtin_atanf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("atanf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


	MAKEERRSYSCALL("ceil",SYS_ceil);
	MAKEERRSYSCALL("floor",SYS_floor);
	MAKEERRSYSCALL("hypot",SYS_hypot);


// log call
	  asm("\n_log:");
	  asm(".globl _log");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("logd (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// logf call
	  asm("\n_logf:");
	  asm(".globl _logf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("logf (%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// log10 call
	  asm("\n_log10:");
	  asm(".globl _log10");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("log10d (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// log10f call
	  asm("\n_log10f:");
	  asm(".globl _log10f");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("log10f (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// exp call
	  asm("\n_exp:");
	  asm(".globl _exp");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("expd (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec8l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// expf call
	  asm("\n_expf:");
	  asm(".globl _expf");
	  asm("movel (%sp)+,(-112,%fp)");
	  asm("expf (%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("dec4l %sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// pow call
	  asm("\n_pow:");
	  asm(".globl _pow");
	  asm("movel (%sp)+,(-112,%fp)");

	  asm("moved (%sp)+,%fp0"); // backwards order, reverse it
	  asm("moved (%sp)+,%fp1"); 
	  asm("moved %fp0,-(%sp)"); 
	  asm("moved %fp1,-(%sp)"); 


	  asm("powd (%sp)+,(%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("addwl #-16,%sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// powf call
	  asm("\n_powf:");
	  asm(".globl _powf");
	  asm("movel (%sp)+,(-112,%fp)");

	  asm("movef (%sp)+,%fp0"); // backwards order, reverse it
	  asm("movef (%sp)+,%fp1"); 
	  asm("movef %fp0,-(%sp)"); 
	  asm("movef %fp1,-(%sp)"); 


	  asm("powf (%sp)+,(%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("addwl #-8,%sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");



// atan2 call
	  asm("\n_atan2:");
	  asm(".globl _atan2");
	  asm("movel (%sp)+,(-112,%fp)");

	  asm("moved (%sp)+,%fp0"); // backwards order, reverse it
	  asm("moved (%sp)+,%fp1"); 
	  asm("moved %fp0,-(%sp)"); 
	  asm("moved %fp1,-(%sp)"); 


	  asm("atan2d (%sp)+,(%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("addwl #-16,%sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");


// atan2f call
	  asm("\n_atan2f:");
	  asm(".globl _atan2f");
	  asm("movel (%sp)+,(-112,%fp)");

	  asm("movef (%sp)+,%fp0"); // backwards order, reverse it
	  asm("movef (%sp)+,%fp1"); 
	  asm("movef %fp0,-(%sp)"); 
	  asm("movef %fp1,-(%sp)"); 


	  asm("atan2f (%sp)+,(%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("addwl #-8,%sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// fmod call
	  asm("\n_fmod:");
	  asm(".globl _fmod");
	  asm("movel (%sp)+,(-112,%fp)");

	  asm("moved (%sp)+,%fp0"); // backwards order, reverse it
	  asm("moved (%sp)+,%fp1"); 
	  asm("moved %fp0,-(%sp)"); 
	  asm("moved %fp1,-(%sp)"); 

	  asm("modd (%sp)+,(%sp)+,-(%sp)");
	  asm("moved (%sp)+,%fp0");
	  asm("addwl #-16,%sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");

// fmodf call
	  asm("\n_fmodf:");
	  asm(".globl _fmodf");
	  asm("movel (%sp)+,(-112,%fp)");

	  asm("movef (%sp)+,%fp0"); // backwards order, reverse it
	  asm("movef (%sp)+,%fp1"); 
	  asm("movef %fp0,-(%sp)"); 
	  asm("movef %fp1,-(%sp)"); 

	  asm("modf (%sp)+,(%sp)+,-(%sp)");
	  asm("movef (%sp)+,%fp0");
	  asm("addwl #-8,%sp");
	  asm("movel (-112,%fp),-(%sp)");
	  asm("rts");



}





