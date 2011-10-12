/* $TOG: fontmisc.h /main/8 1998/02/09 10:49:18 kaleb $ */

/*

Copyright 1991, 1998  The Open Group

All Rights Reserved.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/

/*
 * Author:  Keith Packard, MIT X Consortium
 */

#ifndef _FONTMISC_H_
#define _FONTMISC_H_

#include <X11/Xfuncs.h>

typedef unsigned char	*pointer;
typedef int		Bool;

#ifndef X_PROTOCOL
#ifndef _XSERVER64
typedef unsigned long	Atom;
typedef unsigned long	XID;
#else
#include <X11/Xmd.h>
typedef CARD32 XID;
typedef CARD32 Atom;
#endif 
#endif

#ifndef LSBFirst
#define LSBFirst	0
#define MSBFirst	1
#endif

#ifndef None
#define None	0l
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

extern char	    *NameForAtom ();

#define xalloc(n)   Xalloc ((unsigned) n)
#define xfree(p)    Xfree ((pointer) p)
#define xrealloc(p,n)	Xrealloc ((pointer)p,n)
#define lowbit(x) ((x) & (~(x) + 1))

#define assert(x)

#endif /* _FONTMISC_H_ */
