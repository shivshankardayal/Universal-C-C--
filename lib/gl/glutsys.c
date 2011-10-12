
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

	MAKESYSCALL("glutDestroyMenu",3072);
	MAKESYSCALL("glutCreateMenu",3073);


/*********************/
	MAKESYSCALL("glutGameModeGet",3640);
	MAKESYSCALL("glutLeaveGameMode",3641);
	MAKESYSCALL("glutEnterGameMode",3642);
	MAKESYSCALL("glutGameModeString",3643);
	MAKESYSCALL("glutForceJoystickFunc",3644);
	MAKESYSCALL("glutSetKeyRepeat",3645);
	MAKESYSCALL("glutIgnoreKeyRepeat",3646);
	MAKESYSCALL("glutReportErrors",3647);
	MAKESYSCALL("glutVideoPan",3648);
	MAKESYSCALL("glutVideoResize",3649);
	MAKESYSCALL("glutStopVideoResizing",3650);
	MAKESYSCALL("glutSetupVideoResizing",3651);
	MAKESYSCALL("glutVideoResizeGet",3652);
	MAKESYSCALL("glutSolidIcosahedron",3653);
	MAKESYSCALL("glutWireIcosahedron",3654);
	MAKESYSCALL("glutSolidTetrahedron",3655);
	MAKESYSCALL("glutWireTetrahedron",3656);
	MAKESYSCALL("glutSolidOctahedron",3657);
	MAKESYSCALL("glutWireOctahedron",3658);
	MAKESYSCALL("glutSolidTeapot",3659);
	MAKESYSCALL("glutWireTeapot",3660);
	MAKESYSCALL("glutSolidDodecahedron",3661);
	MAKESYSCALL("glutWireDodecahedron",3662);
	MAKESYSCALL("glutSolidTorus",3663);
	MAKESYSCALL("glutWireTorus",3664);
	MAKESYSCALL("glutSolidCube",3665);
	MAKESYSCALL("glutWireCube",3666);
	MAKESYSCALL("glutSolidCone",3667);
	MAKESYSCALL("glutWireCone",3668);
	MAKESYSCALL("glutSolidSphere",3669);
	MAKESYSCALL("glutWireSphere",3670);
	MAKESYSCALL("glutStrokeLength",3671);
	MAKESYSCALL("glutBitmapLength",3672);
	MAKESYSCALL("glutStrokeWidth",3673);
	MAKESYSCALL("glutStrokeCharacter",3674);
	MAKESYSCALL("glutBitmapWidth",3675);
	MAKESYSCALL("glutBitmapCharacter",3676);
	MAKESYSCALL("glutLayerGet",3677);
	MAKESYSCALL("glutGetModifiers",3678);
	MAKESYSCALL("glutExtensionSupported",3679);
	MAKESYSCALL("glutDeviceGet",3680);
	MAKESYSCALL("glutGet",3681);
	MAKESYSCALL("glutCopyColormap",3682);
	MAKESYSCALL("glutGetColor",3683);
	MAKESYSCALL("glutSetColor",3684);
	MAKESYSCALL("glutJoystickFunc",3685);
	MAKESYSCALL("glutSpecialUpFunc",3686);
	MAKESYSCALL("glutKeyboardUpFunc",3687);
	MAKESYSCALL("glutWindowStatusFunc",3688);
	MAKESYSCALL("glutOverlayDisplayFunc",3689);
	MAKESYSCALL("glutMenuStatusFunc",3690);
	MAKESYSCALL("glutTabletButtonFunc",3691);
	MAKESYSCALL("glutTabletMotionFunc",3692);
	MAKESYSCALL("glutDialsFunc",3693);
	MAKESYSCALL("glutButtonBoxFunc",3694);
	MAKESYSCALL("glutSpaceballButtonFunc",3695);
	MAKESYSCALL("glutSpaceballRotateFunc",3696);
	MAKESYSCALL("glutSpaceballMotionFunc",3697);
	MAKESYSCALL("glutSpecialFunc",3698);
	MAKESYSCALL("glutMenuStateFunc",3699);
	MAKESYSCALL("glutTimerFunc",3700);
	MAKESYSCALL("glutIdleFunc",3701);
	MAKESYSCALL("glutVisibilityFunc",3702);
	MAKESYSCALL("glutEntryFunc",3703);
	MAKESYSCALL("glutPassiveMotionFunc",3704);
	MAKESYSCALL("glutMotionFunc",3705);
	MAKESYSCALL("glutMouseFunc",3706);
	MAKESYSCALL("glutKeyboardFunc",3707);
	MAKESYSCALL("glutReshapeFunc",3708);
	MAKESYSCALL("glutDisplayFunc",3709);
	MAKESYSCALL("glutDetachMenu",3710);
	MAKESYSCALL("glutAttachMenu",3711);
	MAKESYSCALL("glutRemoveMenuItem",3712);
	MAKESYSCALL("glutChangeToSubMenu",3713);
	MAKESYSCALL("glutChangeToMenuEntry",3714);
	MAKESYSCALL("glutAddSubMenu",3715);
	MAKESYSCALL("glutAddMenuEntry",3716);
	MAKESYSCALL("glutSetMenu",3717);
	MAKESYSCALL("glutGetMenu",3718);
	MAKESYSCALL("glutHideOverlay",3719);
	MAKESYSCALL("glutShowOverlay",3720);
	MAKESYSCALL("glutPostWindowOverlayRedisplay",3721);
	MAKESYSCALL("glutPostOverlayRedisplay",3722);
	MAKESYSCALL("glutUseLayer",3723);
	MAKESYSCALL("glutRemoveOverlay",3724);
	MAKESYSCALL("glutEstablishOverlay",3725);
	MAKESYSCALL("glutWarpPointer",3726);
	MAKESYSCALL("glutSetCursor",3727);
	MAKESYSCALL("glutFullScreen",3728);
	MAKESYSCALL("glutHideWindow",3729);
	MAKESYSCALL("glutShowWindow",3730);
	MAKESYSCALL("glutIconifyWindow",3731);
	MAKESYSCALL("glutPushWindow",3732);
	MAKESYSCALL("glutPopWindow",3733);
	MAKESYSCALL("glutReshapeWindow",3734);
	MAKESYSCALL("glutPositionWindow",3735);
	MAKESYSCALL("glutSetIconTitle",3736);
	MAKESYSCALL("glutSetWindowTitle",3737);
	MAKESYSCALL("glutSetWindow",3738);
	MAKESYSCALL("glutGetWindow",3739);
	MAKESYSCALL("glutSwapBuffers",3740);
	MAKESYSCALL("glutPostWindowRedisplay",3741);
	MAKESYSCALL("glutPostRedisplay",3742);
	MAKESYSCALL("glutDestroyWindow",3743);
	MAKESYSCALL("glutCreateSubWindow",3744);
	MAKESYSCALL("glutCreateWindow",3745);
	MAKESYSCALL("glutMainLoop",3746);
	MAKESYSCALL("glutInitWindowSize",3747);
	MAKESYSCALL("glutInitWindowPosition",3748);
	MAKESYSCALL("glutInitDisplayString",3749);
	MAKESYSCALL("glutInitDisplayMode",3750);
	MAKESYSCALL("glutInit",3751);
	MAKESYSCALL("glutGLUT_BITMAP_HELVETICA_18",3752);
	MAKESYSCALL("glutGLUT_BITMAP_HELVETICA_12",3753);
	MAKESYSCALL("glutGLUT_BITMAP_HELVETICA_10",3754);
	MAKESYSCALL("glutGLUT_BITMAP_TIMES_ROMAN_24",3755);
	MAKESYSCALL("glutGLUT_BITMAP_TIMES_ROMAN_10",3756);
	MAKESYSCALL("glutGLUT_BITMAP_8_BY_13",3757);
	MAKESYSCALL("glutGLUT_BITMAP_9_BY_15",3758);
	MAKESYSCALL("glutGLUT_STROKE_MONO_ROMAN",3759);
	MAKESYSCALL("glutGLUT_STROKE_ROMAN",3760);


}





