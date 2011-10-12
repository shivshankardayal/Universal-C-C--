/* $TOG: StrToLong.c /main/6 1998/02/06 15:45:51 kaleb $ */

/*
 
Copyright 1989, 1998  The Open Group

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

#define done(address, type) \
        { (*toVal).size = sizeof(type); (*toVal).addr = (XPointer) address; }

void XmuCvtStringToLong (args, num_args, fromVal, toVal)
    XrmValuePtr args;
    Cardinal    *num_args;
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static long l;

    if (*num_args != 0)
        XtWarningMsg("wrongParameters","cvtStringToLong","XtToolkitError",
                  "String to Long conversion needs no extra arguments",
                  (String *) NULL, (Cardinal *)NULL);
    if (sscanf((char *)fromVal->addr, "%ld", &l) == 1) {
        done(&l, long);
    } else {
        XtStringConversionWarning((char *) fromVal->addr, XtRLong);
    }
}
