/* $TOG: StrToOrnt.c /main/10 1998/02/06 15:45:56 kaleb $ */

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
#include <X11/StringDefs.h>
#include "Converters.h"
#include "CharSet.h"


#define	done(address, type) \
	{ (*toVal).size = sizeof(type); (*toVal).addr = (XPointer) address; }

/* ARGSUSED */
void
XmuCvtStringToOrientation(args, num_args, fromVal, toVal)
    XrmValuePtr args;		/* unused */
    Cardinal	*num_args;	/* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
    static XtOrientation orient;
    static	XrmQuark  XtQEhorizontal;
    static	XrmQuark  XtQEvertical;
    static	int	  haveQuarks = 0;
    XrmQuark	q;
    char	lowerName[1000];

    if (!haveQuarks) {
	XtQEhorizontal = XrmPermStringToQuark(XtEhorizontal);
	XtQEvertical   = XrmPermStringToQuark(XtEvertical);
	haveQuarks = 1;
    }
    XmuCopyISOLatin1Lowered(lowerName, (char *) fromVal->addr);
    q = XrmStringToQuark(lowerName);
    if (q == XtQEhorizontal) {
    	orient = XtorientHorizontal;
	done(&orient, XtOrientation);
	return;
    }
    if (q == XtQEvertical) {
    	orient = XtorientVertical;
	done(&orient, XtOrientation);
	return;
    }
}
