/* $TOG: DrArcs.c /main/14 1998/02/06 17:18:02 kaleb $ */
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

#define arc_scale (SIZEOF(xArc) / 4)

XDrawArcs(dpy, d, gc, arcs, n_arcs)
register Display *dpy;
Drawable d;
GC gc;
XArc *arcs;
int n_arcs;
{
    register xPolyArcReq *req;
    register long len;

    LockDisplay(dpy);
    FlushGC(dpy, gc);
    GetReq(PolyArc,req);
    req->drawable = d;
    req->gc = gc->gid;
    len = ((long)n_arcs) * arc_scale;
    SetReqLen(req, len, 1);
    len <<= 2; /* watch out for macros... */
    Data16 (dpy, (short *) arcs, len);
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}
