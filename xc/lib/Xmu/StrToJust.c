/* $TOG: StrToJust.c /main/10 1998/02/06 15:45:44 kaleb $ */

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
void
XmuCvtStringToJustify(args, num_args, fromVal, toVal)
    XrmValuePtr args;		/* unused */
    Cardinal	*num_args;	/* unused */
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static XtJustify	e;
    static XrmQuark	XrmQEleft;
    static XrmQuark	XrmQEcenter;
    static XrmQuark	XrmQEright;
    static int		haveQuarks;
    XrmQuark    q;
    char	*s = (char *) fromVal->addr;
    char        lowerName[1000];

    if (s == NULL) return;

    if (!haveQuarks) {
	XrmQEleft   = XrmPermStringToQuark(XtEleft);
	XrmQEcenter = XrmPermStringToQuark(XtEcenter);
	XrmQEright  = XrmPermStringToQuark(XtEright);
	haveQuarks = 1;
    }

    XmuCopyISOLatin1Lowered(lowerName, s);

    q = XrmStringToQuark(lowerName);

    toVal->size = sizeof(XtJustify);
    toVal->addr = (XPointer) &e;

    if (q == XrmQEleft)   { e = XtJustifyLeft;   return; }
    if (q == XrmQEcenter) { e = XtJustifyCenter; return; }
    if (q == XrmQEright)  { e = XtJustifyRight;  return; }

    toVal->size = 0;
    toVal->addr = NULL;
}
