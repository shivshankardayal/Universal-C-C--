/* $TOG: CharSet.h /main/6 1998/02/06 15:41:16 kaleb $ */

/* 

Copyright 1988, 1998  The Open Group

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
 * The interfaces described by this header file are for miscellaneous utilities
 * and are not part of the Xlib standard.
 */

#ifndef _XMU_CHARSET_H_
#define _XMU_CHARSET_H_

#include <X11/Xfuncproto.h>

_XFUNCPROTOBEGIN

extern void XmuCopyISOLatin1Lowered(
#if NeedFunctionPrototypes
    char *		/* dst_return */,
    _Xconst char *	/* src */
#endif
);

extern void XmuCopyISOLatin1Uppered(
#if NeedFunctionPrototypes
    char *		/* dst_return */,
    _Xconst char *	/* src */
#endif
);

extern int XmuCompareISOLatin1(
#if NeedFunctionPrototypes
    _Xconst char *	/* first */,
    _Xconst char *	/* second */
#endif
);

_XFUNCPROTOEND

#endif /* _XMU_CHARSET_H_ */
