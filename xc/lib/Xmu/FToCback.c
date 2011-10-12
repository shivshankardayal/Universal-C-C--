/* $TOG: FToCback.c /main/6 1998/02/06 15:43:49 kaleb $ */

/* 
 
Copyright 1988,1998  The Open Group

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

#include	<X11/Intrinsic.h>

/* ARGSUSED */
void
XmuCvtFunctionToCallback(args, num_args, fromVal, toVal)
    XrmValue	*args;		/* unused */
    Cardinal	*num_args;	/* unused */
    XrmValuePtr fromVal;
    XrmValuePtr toVal;
{
    static XtCallbackRec callback[2]; /* K&R: initialized to NULL */
    static XtCallbackList callback_list = callback;

    callback[0].callback = *(XtCallbackProc *)fromVal->addr;

    toVal->size = sizeof(XtCallbackList);
    toVal->addr = (XPointer)&callback_list;
}
