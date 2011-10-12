/* $TOG: SetState.c /main/6 1998/02/06 17:53:15 kaleb $ */
/*

Copyright 1986, 1998  The Open Group

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

#include "Xlibint.h"

XSetState(dpy, gc, foreground, background, function, planemask)
register Display *dpy;
GC gc;
int function;
unsigned long planemask;
unsigned long foreground, background;
{
    XGCValues *gv = &gc->values;

    LockDisplay(dpy);

    if (function != gv->function) {
	gv->function = function;
	gc->dirty |= GCFunction;
    }
    if (planemask != gv->plane_mask) {
	gv->plane_mask = planemask;
	gc->dirty |= GCPlaneMask;
    }
    if (foreground != gv->foreground) {
	gv->foreground = foreground;
	gc->dirty |= GCForeground;
    }
    if (background != gv->background) {
	gv->background = background;
	gc->dirty |= GCBackground;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}