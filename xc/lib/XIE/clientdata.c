/* $TOG: clientdata.c /main/5 1998/02/06 15:11:30 kaleb $ */

/*

Copyright 1993, 1994, 1998  The Open Group

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

#include <stdio.h>


void
XiePutClientData (display, name_space, flo_id, element, final,
    band_number, data, nbytes)

Display      	*display;
unsigned long  	name_space;
unsigned long  	flo_id;
XiePhototag	element;
Bool         	final;
unsigned     	band_number;
unsigned char  	*data;
unsigned     	nbytes;

{
    xiePutClientDataReq	*req;
    char		*pBuf;

    LockDisplay (display);

    GET_REQUEST (PutClientData, pBuf);

    BEGIN_REQUEST_HEADER (PutClientData, pBuf, req);

    STORE_REQUEST_EXTRA_HEADER (PutClientData, nbytes, req);
    req->nameSpace = name_space;
    req->floID = flo_id;
    req->element = element;
    req->final = final;
    req->bandNumber = band_number;
    req->byteCount = nbytes;

    END_REQUEST_HEADER (PutClientData, pBuf, req);

    Data (display, (char *) data, nbytes);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


Status
XieGetClientData (display, name_space, flo_id, element, max_bytes,
    terminate, band_number, new_state_ret, data_ret, nbytes_ret)

Display      	*display;
unsigned long  	name_space;
unsigned long  	flo_id;
XiePhototag	element;
unsigned  	max_bytes;
Bool		terminate;
unsigned     	band_number;
XieExportState 	*new_state_ret;
unsigned char   **data_ret;
unsigned     	*nbytes_ret;

{
    xieGetClientDataReq		*req;
    xieGetClientDataReply	rep;
    char			*pBuf;

    LockDisplay (display);

    GET_REQUEST (GetClientData, pBuf);

    BEGIN_REQUEST_HEADER (GetClientData, pBuf, req);

    STORE_REQUEST_HEADER (GetClientData, req);
    req->nameSpace = name_space;
    req->floID = flo_id;
    req->maxBytes = max_bytes;
    req->element = element;
    req->terminate = terminate;
    req->bandNumber = band_number;

    END_REQUEST_HEADER (GetClientData, pBuf, req);

    if (_XReply (display, (xReply *)&rep, 0, xFalse) == 0)
	/* V4.13 moved the data length word into the reply */
    {
        UnlockDisplay (display);
	SYNC_HANDLE (display);

   	*nbytes_ret = 0;
	*data_ret = NULL;

	return (0);
    }


    *new_state_ret = rep.newState;
    *nbytes_ret = rep.byteCount;

    *data_ret = (unsigned char *) Xmalloc (PADDED_BYTES (rep.byteCount));

    _XReadPad (display, (char *) *data_ret, rep.byteCount);


    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (1);
}
