/* $TOG: photospace.c /main/3 1998/02/06 15:12:51 kaleb $ */

/*

Copyright 1993, 1998  The Open Group

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

#include "XIElibint.h"



XiePhotospace
XieCreatePhotospace (display)

Display	*display;

{
    xieCreatePhotospaceReq	*req;
    char			*pBuf;
    XiePhotospace		id;

    LockDisplay (display);

    id = XAllocID (display);

    GET_REQUEST (CreatePhotospace, pBuf);

    BEGIN_REQUEST_HEADER (CreatePhotospace, pBuf, req);

    STORE_REQUEST_HEADER (CreatePhotospace, req);
    req->nameSpace = id;

    END_REQUEST_HEADER (CreatePhotospace, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (id);
}


void
XieDestroyPhotospace (display, photospace)

Display		*display;
XiePhotospace	photospace;

{
    xieDestroyPhotospaceReq	*req;
    char			*pBuf;

    LockDisplay (display);

    GET_REQUEST (DestroyPhotospace, pBuf);

    BEGIN_REQUEST_HEADER (DestroyPhotospace, pBuf, req);

    STORE_REQUEST_HEADER (DestroyPhotospace, req);
    req->nameSpace = photospace;

    END_REQUEST_HEADER (DestroyPhotospace, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


void
XieExecuteImmediate (display, photospace, flo_id,
    notify, elem_list, elem_count)

Display		*display;
XiePhotospace	photospace;
unsigned long	flo_id;
Bool		notify;
XiePhotoElement	*elem_list;
int		elem_count;

{
    xieExecuteImmediateReq	*req;
    char			*pBuf, *pStart;
    unsigned			size;
    int				i;

    LockDisplay (display);

    size = _XiePhotofloSize (elem_list, elem_count);

    GET_REQUEST (ExecuteImmediate, pBuf);

    BEGIN_REQUEST_HEADER (ExecuteImmediate, pBuf, req);

    STORE_REQUEST_EXTRA_HEADER (ExecuteImmediate, size, req);
    req->nameSpace = photospace;
    req->floID = flo_id;
    req->numElements = elem_count;
    req->notify = notify;

    END_REQUEST_HEADER (ExecuteImmediate, pBuf, req);

    pBuf = pStart = _XAllocScratch (display, size);

    for (i = 0; i < elem_count; i++) 
	(*_XieElemFuncs[elem_list[i].elemType - 1]) (&pBuf, &elem_list[i]);

    Data (display, pStart, size);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


