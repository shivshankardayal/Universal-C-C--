/* $TOG: globals.c /main/6 1998/02/06 14:51:01 kaleb $ */
/*

Copyright 1989, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from The Open Group.

*/
/*
 * This file should contain only those objects which must be predefined.
 */
#include <X11/Xlib.h>
#include <X11/extensions/Xext.h>
#ifndef X_NOT_STDC_ENV
#include <stddef.h>			/* for definition of NULL */
#else
#include <sys/param.h>
#endif

/*
 * If possible, it is useful to have the global data default to a null value.
 * Some shared library implementations are *much* happier if there isn't any
 * global initialized data.
 */
#ifdef NULL_NOT_ZERO			/* then need to initialize */
#define SetZero(t,var,z) t var = z
#else 
#define SetZero(t,var,z) t var
#endif

#ifdef ATTSHAREDLIB			/* then need extra variables */
/*
 * If we need to define extra variables for each global
 */
#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define ZEROINIT(t,var,val) SetZero(t,var,val); \
  SetZero (long, _libX_##var##Flag, 0); \
  SetZero (void *, _libX_##var##Ptr, NULL)
#else /* else pcc concatenation */
#define ZEROINIT(t,var,val) SetZero(t,var,val); \
  SetZero (long, _libX_/**/var/**/Flag, 0); \
  SetZero (void *, _libX_/**/var/**/Ptr, NULL)
#endif /* concat ANSI C vs. pcc */

#else /* else not ATTSHAREDLIB */
/*
 * no extra crud
 */
#define ZEROINIT(t,var,val) SetZero (t, var, val)

#endif /* ATTSHAREDLIB */


/*
 * Error handlers; used to be in XlibInt.c
 */
typedef int (*funcptr)();
ZEROINIT (funcptr, _XExtensionErrorFunction, NULL);

/*
 * NOTE: any additional external definition NEED
 * to be inserted BELOW this point!!!
 */

/*
 * NOTE: any additional external definition NEED
 * to be inserted ABOVE this point!!!
 */
