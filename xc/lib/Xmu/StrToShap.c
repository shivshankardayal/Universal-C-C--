/* $TOG: StrToShap.c /main/6 1998/02/06 15:46:01 kaleb $ */

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

#include <X11/Intrinsic.h>
#include "Converters.h"
#include "CharSet.h"

/* ARGSUSED */
#define	done(type, value) \
	{							\
	    if (toVal->addr != NULL) {				\
		if (toVal->size < sizeof(type)) {		\
		    toVal->size = sizeof(type);			\
		    return False;				\
		}						\
		*(type*)(toVal->addr) = (value);		\
	    }							\
	    else {						\
		static type static_val;				\
		static_val = (value);				\
		toVal->addr = (XtPointer)&static_val;		\
	    }							\
	    toVal->size = sizeof(type);				\
	    return True;					\
	}


Boolean XmuCvtStringToShapeStyle(dpy, args, num_args, from, toVal, data)
    Display *dpy;
    XrmValue *args;		/* unused */
    Cardinal *num_args;		/* unused */
    XrmValue *from;
    XrmValue *toVal;
    XtPointer *data;		/* unused */
{
    if (   XmuCompareISOLatin1((char*)from->addr, XtERectangle) == 0
	|| XmuCompareISOLatin1((char*)from->addr, "ShapeRectangle") == 0)
	done( int, XmuShapeRectangle );
    if (   XmuCompareISOLatin1((char*)from->addr, XtEOval) == 0
	|| XmuCompareISOLatin1((char*)from->addr, "ShapeOval") == 0)
	done( int, XmuShapeOval );
    if (   XmuCompareISOLatin1((char*)from->addr, XtEEllipse) == 0
	|| XmuCompareISOLatin1((char*)from->addr, "ShapeEllipse") == 0)
	done( int, XmuShapeEllipse );
    if (   XmuCompareISOLatin1((char*)from->addr, XtERoundedRectangle) == 0
	|| XmuCompareISOLatin1((char*)from->addr, "ShapeRoundedRectangle") == 0)
	done( int, XmuShapeRoundedRectangle );
    {
	int style = 0;
	char ch, *p = (char*)from->addr;
	while (ch = *p++) {
	    if (ch >= '0' && ch <= '9') {
		style *= 10;
		style += ch - '0';
	    }
	    else break;
	}
	if (ch == '\0' && style <= XmuShapeRoundedRectangle)
	    done( int, style );
    }
    XtDisplayStringConversionWarning( dpy, (char*)from->addr, XtRShapeStyle );
    return False;
}
