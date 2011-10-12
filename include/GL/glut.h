#ifndef __glut_h__
#define __glut_h__

/* Copyright (c) Mark J. Kilgard, 1994, 1995, 1996, 1998. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

/************* modified for ICVM *************/

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif


/* Define GLUTAPIENTRY and GLUTCALLBACK to nothing if we aren't on Win32. */
#  define GLUTAPIENTRY
#  define GLUTAPIENTRYV
#  define GLUT_APIENTRY_DEFINED
#  define GLUTCALLBACK
#  define GLUTAPI extern
/* Prototype exit for the non-Win32 case (see above). */
/*extern void exit(int);  this screws up gcc -ansi -pedantic! */


/**
 GLUT API revision history:

 GLUT_API_VERSION is updated to reflect incompatible GLUT
 API changes (interface changes, semantic changes, deletions,
 or additions).

 GLUT_API_VERSION=1  First public release of GLUT.  11/29/94

 GLUT_API_VERSION=2  Added support for OpenGL/GLX multisampling,
 extension.  Supports new input devices like tablet, dial and button
 box, and Spaceball.  Easy to query OpenGL extensions.

 GLUT_API_VERSION=3  glutMenuStatus added.

 GLUT_API_VERSION=4  glutInitDisplayString, glutWarpPointer,
 glutBitmapLength, glutStrokeLength, glutWindowStatusFunc, dynamic
 video resize subAPI, glutPostWindowRedisplay, glutKeyboardUpFunc,
 glutSpecialUpFunc, glutIgnoreKeyRepeat, glutSetKeyRepeat,
 glutJoystickFunc, glutForceJoystickFunc (NOT FINALIZED!).
**/
#ifndef GLUT_API_VERSION  /* allow this to be overriden */
#define GLUT_API_VERSION		3
#endif

/**
 GLUT implementation revision history:

 GLUT_XLIB_IMPLEMENTATION is updated to reflect both GLUT
 API revisions and implementation revisions (ie, bug fixes).

 GLUT_XLIB_IMPLEMENTATION=1  mjk's first public release of
 GLUT Xlib-based implementation.  11/29/94

 GLUT_XLIB_IMPLEMENTATION=2  mjk's second public release of
 GLUT Xlib-based implementation providing GLUT version 2
 interfaces.

 GLUT_XLIB_IMPLEMENTATION=3  mjk's GLUT 2.2 images. 4/17/95

 GLUT_XLIB_IMPLEMENTATION=4  mjk's GLUT 2.3 images. 6/?/95

 GLUT_XLIB_IMPLEMENTATION=5  mjk's GLUT 3.0 images. 10/?/95

 GLUT_XLIB_IMPLEMENTATION=7  mjk's GLUT 3.1+ with glutWarpPoitner.  7/24/96

 GLUT_XLIB_IMPLEMENTATION=8  mjk's GLUT 3.1+ with glutWarpPoitner
 and video resize.  1/3/97

 GLUT_XLIB_IMPLEMENTATION=9 mjk's GLUT 3.4 release with early GLUT 4 routines.

 GLUT_XLIB_IMPLEMENTATION=11 Mesa 2.5's GLUT 3.6 release.

 GLUT_XLIB_IMPLEMENTATION=12 mjk's GLUT 3.6 release with early GLUT 4 routines + signal handling.

 GLUT_XLIB_IMPLEMENTATION=13 mjk's GLUT 3.7 beta with GameGLUT support.

 GLUT_XLIB_IMPLEMENTATION=14 mjk's GLUT 3.7 beta with f90gl friend interface.

 GLUT_XLIB_IMPLEMENTATION=15 mjk's GLUT 3.7 beta sync'ed with Mesa <GL/glut.h>
**/
#ifndef GLUT_XLIB_IMPLEMENTATION  /* Allow this to be overriden. */
#define GLUT_XLIB_IMPLEMENTATION	15
#endif

/* Display mode bit masks. */
#define GLUT_RGB			0
#define GLUT_RGBA			GLUT_RGB
#define GLUT_INDEX			1
#define GLUT_SINGLE			0
#define GLUT_DOUBLE			2
#define GLUT_ACCUM			4
#define GLUT_ALPHA			8
#define GLUT_DEPTH			16
#define GLUT_STENCIL			32
#if (GLUT_API_VERSION >= 2)
#define GLUT_MULTISAMPLE		128
#define GLUT_STEREO			256
#endif
#if (GLUT_API_VERSION >= 3)
#define GLUT_LUMINANCE			512
#endif

/* Mouse buttons. */
#define GLUT_LEFT_BUTTON		0
#define GLUT_MIDDLE_BUTTON		1
#define GLUT_RIGHT_BUTTON		2

/* Mouse button  state. */
#define GLUT_DOWN			0
#define GLUT_UP				1

#if (GLUT_API_VERSION >= 2)
/* function keys */
#define GLUT_KEY_F1			1
#define GLUT_KEY_F2			2
#define GLUT_KEY_F3			3
#define GLUT_KEY_F4			4
#define GLUT_KEY_F5			5
#define GLUT_KEY_F6			6
#define GLUT_KEY_F7			7
#define GLUT_KEY_F8			8
#define GLUT_KEY_F9			9
#define GLUT_KEY_F10			10
#define GLUT_KEY_F11			11
#define GLUT_KEY_F12			12
/* directional keys */
#define GLUT_KEY_LEFT			100
#define GLUT_KEY_UP			101
#define GLUT_KEY_RIGHT			102
#define GLUT_KEY_DOWN			103
#define GLUT_KEY_PAGE_UP		104
#define GLUT_KEY_PAGE_DOWN		105
#define GLUT_KEY_HOME			106
#define GLUT_KEY_END			107
#define GLUT_KEY_INSERT			108
#endif

/* Entry/exit  state. */
#define GLUT_LEFT			0
#define GLUT_ENTERED			1

/* Menu usage  state. */
#define GLUT_MENU_NOT_IN_USE		0
#define GLUT_MENU_IN_USE		1

/* Visibility  state. */
#define GLUT_NOT_VISIBLE		0
#define GLUT_VISIBLE			1

/* Window status  state. */
#define GLUT_HIDDEN			0
#define GLUT_FULLY_RETAINED		1
#define GLUT_PARTIALLY_RETAINED		2
#define GLUT_FULLY_COVERED		3

/* Color index component selection values. */
#define GLUT_RED			0
#define GLUT_GREEN			1
#define GLUT_BLUE			2

/* Layers for use. */
#define GLUT_NORMAL			0
#define GLUT_OVERLAY			1

#define GLUT_STROKE_ROMAN			glutGLUT_STROKE_ROMAN()	
#define GLUT_STROKE_MONO_ROMAN 		glutGLUT_STROKE_MONO_ROMAN()
#define GLUT_BITMAP_9_BY_15			glutGLUT_BITMAP_9_BY_15()
#define GLUT_BITMAP_8_BY_13			glutGLUT_BITMAP_8_BY_13()
#define GLUT_BITMAP_TIMES_ROMAN_10	glutGLUT_BITMAP_TIMES_ROMAN_10()
#define GLUT_BITMAP_TIMES_ROMAN_24	glutGLUT_BITMAP_TIMES_ROMAN_24()
#define GLUT_BITMAP_HELVETICA_10	glutGLUT_BITMAP_HELVETICA_10()
#define GLUT_BITMAP_HELVETICA_12	glutGLUT_BITMAP_HELVETICA_12()
#define GLUT_BITMAP_HELVETICA_18	glutGLUT_BITMAP_HELVETICA_18()

/* glutGet parameters. */
#define GLUT_WINDOW_X			100
#define GLUT_WINDOW_Y			101
#define GLUT_WINDOW_WIDTH		102
#define GLUT_WINDOW_HEIGHT		103
#define GLUT_WINDOW_BUFFER_SIZE		104
#define GLUT_WINDOW_STENCIL_SIZE	105
#define GLUT_WINDOW_DEPTH_SIZE		106
#define GLUT_WINDOW_RED_SIZE		107
#define GLUT_WINDOW_GREEN_SIZE		108
#define GLUT_WINDOW_BLUE_SIZE		109
#define GLUT_WINDOW_ALPHA_SIZE		110
#define GLUT_WINDOW_ACCUM_RED_SIZE	111
#define GLUT_WINDOW_ACCUM_GREEN_SIZE	112
#define GLUT_WINDOW_ACCUM_BLUE_SIZE	113
#define GLUT_WINDOW_ACCUM_ALPHA_SIZE	114
#define GLUT_WINDOW_DOUBLEBUFFER	115
#define GLUT_WINDOW_RGBA		116
#define GLUT_WINDOW_PARENT		117
#define GLUT_WINDOW_NUM_CHILDREN	118
#define GLUT_WINDOW_COLORMAP_SIZE	119
#if (GLUT_API_VERSION >= 2)
#define GLUT_WINDOW_NUM_SAMPLES		120
#define GLUT_WINDOW_STEREO		121
#endif
#if (GLUT_API_VERSION >= 3)
#define GLUT_WINDOW_CURSOR		122
#endif
#define GLUT_SCREEN_WIDTH		200
#define GLUT_SCREEN_HEIGHT		201
#define GLUT_SCREEN_WIDTH_MM		202
#define GLUT_SCREEN_HEIGHT_MM		203
#define GLUT_MENU_NUM_ITEMS		300
#define GLUT_DISPLAY_MODE_POSSIBLE	400
#define GLUT_INIT_WINDOW_X		500
#define GLUT_INIT_WINDOW_Y		501
#define GLUT_INIT_WINDOW_WIDTH		502
#define GLUT_INIT_WINDOW_HEIGHT		503
#define GLUT_INIT_DISPLAY_MODE		504
#if (GLUT_API_VERSION >= 2)
#define GLUT_ELAPSED_TIME		700
#endif
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
#define GLUT_WINDOW_FORMAT_ID		123
#endif

#if (GLUT_API_VERSION >= 2)
/* glutDeviceGet parameters. */
#define GLUT_HAS_KEYBOARD		600
#define GLUT_HAS_MOUSE			601
#define GLUT_HAS_SPACEBALL		602
#define GLUT_HAS_DIAL_AND_BUTTON_BOX	603
#define GLUT_HAS_TABLET			604
#define GLUT_NUM_MOUSE_BUTTONS		605
#define GLUT_NUM_SPACEBALL_BUTTONS	606
#define GLUT_NUM_BUTTON_BOX_BUTTONS	607
#define GLUT_NUM_DIALS			608
#define GLUT_NUM_TABLET_BUTTONS		609
#endif
#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 13)
#define GLUT_DEVICE_IGNORE_KEY_REPEAT   610
#define GLUT_DEVICE_KEY_REPEAT          611
#define GLUT_HAS_JOYSTICK		612
#define GLUT_OWNS_JOYSTICK		613
#define GLUT_JOYSTICK_BUTTONS		614
#define GLUT_JOYSTICK_AXES		615
#define GLUT_JOYSTICK_POLL_RATE		616
#endif

#if (GLUT_API_VERSION >= 3)
/* glutLayerGet parameters. */
#define GLUT_OVERLAY_POSSIBLE           800
#define GLUT_LAYER_IN_USE		801
#define GLUT_HAS_OVERLAY		802
#define GLUT_TRANSPARENT_INDEX		803
#define GLUT_NORMAL_DAMAGED		804
#define GLUT_OVERLAY_DAMAGED		805

#if (GLUT_API_VERSION >= 4 || GLUT_XLIB_IMPLEMENTATION >= 9)
/* glutVideoResizeGet parameters. */
#define GLUT_VIDEO_RESIZE_POSSIBLE	900
#define GLUT_VIDEO_RESIZE_IN_USE	901
#define GLUT_VIDEO_RESIZE_X_DELTA	902
#define GLUT_VIDEO_RESIZE_Y_DELTA	903
#define GLUT_VIDEO_RESIZE_WIDTH_DELTA	904
#define GLUT_VIDEO_RESIZE_HEIGHT_DELTA	905
#define GLUT_VIDEO_RESIZE_X		906
#define GLUT_VIDEO_RESIZE_Y		907
#define GLUT_VIDEO_RESIZE_WIDTH		908
#define GLUT_VIDEO_RESIZE_HEIGHT	909
#endif

/* glutUseLayer parameters. */
#define GLUT_NORMAL			0
#define GLUT_OVERLAY			1

/* glutGetModifiers return mask. */
#define GLUT_ACTIVE_SHIFT               1
#define GLUT_ACTIVE_CTRL                2
#define GLUT_ACTIVE_ALT                 4

/* glutSetCursor parameters. */
/* Basic arrows. */
#define GLUT_CURSOR_RIGHT_ARROW		0
#define GLUT_CURSOR_LEFT_ARROW		1
/* Symbolic cursor shapes. */
#define GLUT_CURSOR_INFO		2
#define GLUT_CURSOR_DESTROY		3
#define GLUT_CURSOR_HELP		4
#define GLUT_CURSOR_CYCLE		5
#define GLUT_CURSOR_SPRAY		6
#define GLUT_CURSOR_WAIT		7
#define GLUT_CURSOR_TEXT		8
#define GLUT_CURSOR_CROSSHAIR		9
/* Directional cursors. */
#define GLUT_CURSOR_UP_DOWN		10
#define GLUT_CURSOR_LEFT_RIGHT		11
/* Sizing cursors. */
#define GLUT_CURSOR_TOP_SIDE		12
#define GLUT_CURSOR_BOTTOM_SIDE		13
#define GLUT_CURSOR_LEFT_SIDE		14
#define GLUT_CURSOR_RIGHT_SIDE		15
#define GLUT_CURSOR_TOP_LEFT_CORNER	16
#define GLUT_CURSOR_TOP_RIGHT_CORNER	17
#define GLUT_CURSOR_BOTTOM_RIGHT_CORNER	18
#define GLUT_CURSOR_BOTTOM_LEFT_CORNER	19
/* Inherit from parent window. */
#define GLUT_CURSOR_INHERIT		100
/* Blank cursor. */
#define GLUT_CURSOR_NONE		101
/* Fullscreen crosshair (if available). */
#define GLUT_CURSOR_FULL_CROSSHAIR	102
#endif

/* GLUT device control sub-API. */
/* glutSetKeyRepeat modes. */
#define GLUT_KEY_REPEAT_OFF		0
#define GLUT_KEY_REPEAT_ON		1
#define GLUT_KEY_REPEAT_DEFAULT		2

/* Joystick button masks. */
#define GLUT_JOYSTICK_BUTTON_A		1
#define GLUT_JOYSTICK_BUTTON_B		2
#define GLUT_JOYSTICK_BUTTON_C		4
#define GLUT_JOYSTICK_BUTTON_D		8


/* GLUT game mode sub-API. */
/* glutGameModeGet. */
#define GLUT_GAME_MODE_ACTIVE           0
#define GLUT_GAME_MODE_POSSIBLE         1
#define GLUT_GAME_MODE_WIDTH            2
#define GLUT_GAME_MODE_HEIGHT           3
#define GLUT_GAME_MODE_PIXEL_DEPTH      4
#define GLUT_GAME_MODE_REFRESH_RATE     5
#define GLUT_GAME_MODE_DISPLAY_CHANGED  6

/*********** general API - ICVM system calls *******/
void glutDestroyMenu(int menu) ICVMCALL(3072);
int glutCreateMenu(void (GLUTCALLBACK *func)(int)) ICVMCALL(3073);

/*********************************************************/

int glutGameModeGet(GLenum mode) ICVMCALL(3640);
void glutLeaveGameMode(void) ICVMCALL(3641);
int glutEnterGameMode(void) ICVMCALL(3642);
void glutGameModeString(const char *string) ICVMCALL(3643);
void glutForceJoystickFunc(void) ICVMCALL(3644);
void glutSetKeyRepeat(int repeatMode) ICVMCALL(3645);
void glutIgnoreKeyRepeat(int ignore) ICVMCALL(3646);
void glutReportErrors(void) ICVMCALL(3647);
void glutVideoPan(int x,int y,int width,int height) ICVMCALL(3648);
void glutVideoResize(int x,int y,int width,int height) ICVMCALL(3649);
void glutStopVideoResizing(void) ICVMCALL(3650);
void glutSetupVideoResizing(void) ICVMCALL(3651);
int glutVideoResizeGet(GLenum param) ICVMCALL(3652);
void glutSolidIcosahedron(void) ICVMCALL(3653);
void glutWireIcosahedron(void) ICVMCALL(3654);
void glutSolidTetrahedron(void) ICVMCALL(3655);
void glutWireTetrahedron(void) ICVMCALL(3656);
void glutSolidOctahedron(void) ICVMCALL(3657);
void glutWireOctahedron(void) ICVMCALL(3658);
void glutSolidTeapot(GLdouble size) ICVMCALL(3659);
void glutWireTeapot(GLdouble size) ICVMCALL(3660);
void glutSolidDodecahedron(void) ICVMCALL(3661);
void glutWireDodecahedron(void) ICVMCALL(3662);
void glutSolidTorus(GLdouble innerRadius,GLdouble outerRadius,GLint sides,GLint rings) ICVMCALL(3663);
void glutWireTorus(GLdouble innerRadius,GLdouble outerRadius,GLint sides,GLint rings) ICVMCALL(3664);
void glutSolidCube(GLdouble size) ICVMCALL(3665);
void glutWireCube(GLdouble size) ICVMCALL(3666);
void glutSolidCone(GLdouble base,GLdouble height,GLint slices,GLint stacks) ICVMCALL(3667);
void glutWireCone(GLdouble base,GLdouble height,GLint slices,GLint stacks) ICVMCALL(3668);
void glutSolidSphere(GLdouble radius,GLint slices,GLint stacks) ICVMCALL(3669);
void glutWireSphere(GLdouble radius,GLint slices,GLint stacks) ICVMCALL(3670);
int glutStrokeLength(void *font,const unsigned char *string) ICVMCALL(3671);
int glutBitmapLength(void *font,const unsigned char *string) ICVMCALL(3672);
int glutStrokeWidth(void *font,int character) ICVMCALL(3673);
void glutStrokeCharacter(void *font,int character) ICVMCALL(3674);
int glutBitmapWidth(void *font,int character) ICVMCALL(3675);
void glutBitmapCharacter(void *font,int character) ICVMCALL(3676);
int glutLayerGet(GLenum type) ICVMCALL(3677);
int glutGetModifiers(void) ICVMCALL(3678);
int glutExtensionSupported(const char *name) ICVMCALL(3679);
int glutDeviceGet(GLenum type) ICVMCALL(3680);
int glutGet(GLenum type) ICVMCALL(3681);
void glutCopyColormap(int win) ICVMCALL(3682);
GLfloat glutGetColor(int ndx,int component) ICVMCALL(3683);
void glutSetColor(int,GLfloat red,GLfloat green,GLfloat blue) ICVMCALL(3684);
void glutJoystickFunc(void (GLUTCALLBACK *func)(unsigned int buttonMask,int x,int y,int z),int pollInterval) ICVMCALL(3685);
void glutSpecialUpFunc(void (GLUTCALLBACK *func)(int key,int x,int y)) ICVMCALL(3686);
void glutKeyboardUpFunc(void (GLUTCALLBACK *func)(unsigned char key,int x,int y)) ICVMCALL(3687);
void glutWindowStatusFunc(void (GLUTCALLBACK *func)(int state)) ICVMCALL(3688);
void glutOverlayDisplayFunc(void (GLUTCALLBACK *func)(void)) ICVMCALL(3689);
void glutMenuStatusFunc(void (GLUTCALLBACK *func)(int status,int x,int y)) ICVMCALL(3690);
void glutTabletButtonFunc(void (GLUTCALLBACK *func)(int button,int state,int x,int y)) ICVMCALL(3691);
void glutTabletMotionFunc(void (GLUTCALLBACK *func)(int x,int y)) ICVMCALL(3692);
void glutDialsFunc(void (GLUTCALLBACK *func)(int dial,int value)) ICVMCALL(3693);
void glutButtonBoxFunc(void (GLUTCALLBACK *func)(int button,int state)) ICVMCALL(3694);
void glutSpaceballButtonFunc(void (GLUTCALLBACK *func)(int button,int state)) ICVMCALL(3695);
void glutSpaceballRotateFunc(void (GLUTCALLBACK *func)(int x,int y,int z)) ICVMCALL(3696);
void glutSpaceballMotionFunc(void (GLUTCALLBACK *func)(int x,int y,int z)) ICVMCALL(3697);
void glutSpecialFunc(void (GLUTCALLBACK *func)(int key,int x,int y)) ICVMCALL(3698);
void glutMenuStateFunc(void (GLUTCALLBACK *func)(int state)) ICVMCALL(3699);
void glutTimerFunc(unsigned int millis,void (GLUTCALLBACK *func)(int value),int value) ICVMCALL(3700);
void glutIdleFunc(void (GLUTCALLBACK *func)(void)) ICVMCALL(3701);
void glutVisibilityFunc(void (GLUTCALLBACK *func)(int state)) ICVMCALL(3702);
void glutEntryFunc(void (GLUTCALLBACK *func)(int state)) ICVMCALL(3703);
void glutPassiveMotionFunc(void (GLUTCALLBACK *func)(int x,int y)) ICVMCALL(3704);
void glutMotionFunc(void (GLUTCALLBACK *func)(int x,int y)) ICVMCALL(3705);
void glutMouseFunc(void (GLUTCALLBACK *func)(int button,int state,int x,int y)) ICVMCALL(3706);
void glutKeyboardFunc(void (GLUTCALLBACK *func)(unsigned char key,int x,int y)) ICVMCALL(3707);
void glutReshapeFunc(void (GLUTCALLBACK *func)(int width,int height)) ICVMCALL(3708);
void glutDisplayFunc(void (GLUTCALLBACK *func)(void)) ICVMCALL(3709);
void glutDetachMenu(int button) ICVMCALL(3710);
void glutAttachMenu(int button) ICVMCALL(3711);
void glutRemoveMenuItem(int item) ICVMCALL(3712);
void glutChangeToSubMenu(int item,const char *label,int submenu) ICVMCALL(3713);
void glutChangeToMenuEntry(int item,const char *label,int value) ICVMCALL(3714);
void glutAddSubMenu(const char *label,int submenu) ICVMCALL(3715);
void glutAddMenuEntry(const char *label,int value) ICVMCALL(3716);
void glutSetMenu(int menu) ICVMCALL(3717);
int glutGetMenu(void) ICVMCALL(3718);
void glutHideOverlay(void) ICVMCALL(3719);
void glutShowOverlay(void) ICVMCALL(3720);
void glutPostWindowOverlayRedisplay(int win) ICVMCALL(3721);
void glutPostOverlayRedisplay(void) ICVMCALL(3722);
void glutUseLayer(GLenum layer) ICVMCALL(3723);
void glutRemoveOverlay(void) ICVMCALL(3724);
void glutEstablishOverlay(void) ICVMCALL(3725);
void glutWarpPointer(int x,int y) ICVMCALL(3726);
void glutSetCursor(int cursor) ICVMCALL(3727);
void glutFullScreen(void) ICVMCALL(3728);
void glutHideWindow(void) ICVMCALL(3729);
void glutShowWindow(void) ICVMCALL(3730);
void glutIconifyWindow(void) ICVMCALL(3731);
void glutPushWindow(void) ICVMCALL(3732);
void glutPopWindow(void) ICVMCALL(3733);
void glutReshapeWindow(int width,int height) ICVMCALL(3734);
void glutPositionWindow(int x,int y) ICVMCALL(3735);
void glutSetIconTitle(const char *title) ICVMCALL(3736);
void glutSetWindowTitle(const char *title) ICVMCALL(3737);
void glutSetWindow(int win) ICVMCALL(3738);
int glutGetWindow(void) ICVMCALL(3739);
void glutSwapBuffers(void) ICVMCALL(3740);
void glutPostWindowRedisplay(int win) ICVMCALL(3741);
void glutPostRedisplay(void) ICVMCALL(3742);
void glutDestroyWindow(int win) ICVMCALL(3743);
int glutCreateSubWindow(int win,int x,int y,int width,int height) ICVMCALL(3744);
int glutCreateWindow(const char *title) ICVMCALL(3745);
void glutMainLoop(void) ICVMCALL(3746);
void glutInitWindowSize(int width,int height) ICVMCALL(3747);
void glutInitWindowPosition(int x,int y) ICVMCALL(3748);
void glutInitDisplayString(const char *string) ICVMCALL(3749);
void glutInitDisplayMode(unsigned int mode) ICVMCALL(3750);
void glutInit(int *argcp,char **argv) ICVMCALL(3751);
void* glutGLUT_BITMAP_HELVETICA_18(void) ICVMCALL(3752);
void* glutGLUT_BITMAP_HELVETICA_12(void) ICVMCALL(3753);
void* glutGLUT_BITMAP_HELVETICA_10(void) ICVMCALL(3754);
void* glutGLUT_BITMAP_TIMES_ROMAN_24(void) ICVMCALL(3755);
void* glutGLUT_BITMAP_TIMES_ROMAN_10(void) ICVMCALL(3756);
void* glutGLUT_BITMAP_8_BY_13(void) ICVMCALL(3757);
void* glutGLUT_BITMAP_9_BY_15(void) ICVMCALL(3758);
void* glutGLUT_STROKE_MONO_ROMAN(void) ICVMCALL(3759);
void* glutGLUT_STROKE_ROMAN(void) ICVMCALL(3760);


#ifdef __cplusplus
}
#endif

#if 0
#ifdef GLUT_APIENTRY_DEFINED
# undef GLUT_APIENTRY_DEFINED
# undef APIENTRY
#endif

#ifdef GLUT_WINGDIAPI_DEFINED
# undef GLUT_WINGDIAPI_DEFINED
# undef WINGDIAPI
#endif

#ifdef GLUT_DEFINED___CDECL
# undef GLUT_DEFINED___CDECL
# undef __cdecl
#endif

#ifdef GLUT_DEFINED__CRTIMP
# undef GLUT_DEFINED__CRTIMP
# undef _CRTIMP
#endif
#endif

#endif                  /* __glut_h__ */
