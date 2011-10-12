
#define MAKESYSCALL(name,callno)\
	  asm("\n_"name":");\
	  asm("movel (%sp)+,(-112,%fp)");\
	  asm("syscalll %0": : "i" (callno) );\
	  asm(".globl _"name);\
	  asm("movel (-112,%fp),-(%sp)");\
	  asm("rts");


/* surrounded with a dummy call so we can access system header files.
   these actually turn to jumps into the Host handlers 
   so we just need to compile
   the jump vectors.   */


/* basic operating level system calls. 
*/

static void dummy(void){

	MAKESYSCALL("gluCheckExtension",3512);
	MAKESYSCALL("gluGetString",3513);
	MAKESYSCALL("gluGetTessProperty",3514);
	MAKESYSCALL("gluTessCallback",3515);
	MAKESYSCALL("gluTessNormal",3516);
	MAKESYSCALL("gluTessProperty",3517);
	MAKESYSCALL("gluTessEndPolygon",3518);
	MAKESYSCALL("gluTessEndContour",3519);
	MAKESYSCALL("gluTessVertex",3520);
	MAKESYSCALL("gluTessBeginContour",3521);
	MAKESYSCALL("gluTessBeginPolygon",3522);
	MAKESYSCALL("gluDeleteTess",3523);
	MAKESYSCALL("gluNewTess",3524);
	MAKESYSCALL("gluNurbsCallback",3525);
	MAKESYSCALL("gluPwlCurve",3526);
	MAKESYSCALL("gluEndTrim",3527);
	MAKESYSCALL("gluBeginTrim",3528);
	MAKESYSCALL("gluNurbsSurface",3529);
	MAKESYSCALL("gluEndSurface",3530);
	MAKESYSCALL("gluBeginSurface",3531);
	MAKESYSCALL("gluNurbsCurve",3532);
	MAKESYSCALL("gluEndCurve",3533);
	MAKESYSCALL("gluBeginCurve",3534);
	MAKESYSCALL("gluGetNurbsProperty",3535);
	MAKESYSCALL("gluNurbsProperty",3536);
	MAKESYSCALL("gluLoadSamplingMatrices",3537);
	MAKESYSCALL("gluDeleteNurbsRenderer",3538);
	MAKESYSCALL("gluNewNurbsRenderer",3539);
	MAKESYSCALL("gluPartialDisk",3540);
	MAKESYSCALL("gluDisk",3541);
	MAKESYSCALL("gluSphere",3542);
	MAKESYSCALL("gluCylinder",3543);
	MAKESYSCALL("gluQuadricCallback",3544);
	MAKESYSCALL("gluQuadricTexture",3545);
	MAKESYSCALL("gluQuadricNormals",3546);
	MAKESYSCALL("gluQuadricOrientation",3547);
	MAKESYSCALL("gluQuadricDrawStyle",3548);
	MAKESYSCALL("gluDeleteQuadric",3549);
	MAKESYSCALL("gluNewQuadric",3550);
	MAKESYSCALL("gluBuild2DMipmaps",3551);
	MAKESYSCALL("gluBuild1DMipmaps",3552);
	MAKESYSCALL("gluScaleImage",3553);
	MAKESYSCALL("gluErrorString",3554);
	MAKESYSCALL("gluUnProject",3555);
	MAKESYSCALL("gluProject",3556);
	MAKESYSCALL("gluPickMatrix",3557);
	MAKESYSCALL("gluPerspective",3558);
	MAKESYSCALL("gluOrtho2D",3559);
	MAKESYSCALL("gluLookAt",3560);
	MAKESYSCALL("gluNextContour",3561);
	MAKESYSCALL("gluEndPolygon",3562);
	MAKESYSCALL("gluBeginPolygon",3563);


}





