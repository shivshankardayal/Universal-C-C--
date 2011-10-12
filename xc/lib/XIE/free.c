/* $TOG: free.c /main/4 1998/02/06 15:12:10 kaleb $ */

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

#define CHECK_AND_FREE(_ptr) if (_ptr) Xfree (_ptr)


void
XieFreeTechniques (techs, count)

XieTechnique 	*techs;
unsigned int	count;

{
    int i;

    if (techs)
    {
	for (i = 0; i < count; i++)
	    CHECK_AND_FREE (techs[i].name);

	Xfree ((char *) techs);
    }
}


void
XieFreePhotofloGraph (elements, count)

XiePhotoElement *elements;
unsigned int    count;

{
    /*
     * NOTE: We do not free the technique parameters here.
     * Most of the technique parameters should be freed by the
     * client using Xfree (exception: EncodeJPEGBaseline and
     * EncodeJPEGLossless, see functions below).  This is so
     * the client can reuse technique parameters between photoflos.
     */

    int i;

    if (!elements)
	return;

    for (i = 0; i < count; i++)
    {
	switch (elements[i].elemType)
	{
	case xieElemConvolve:
	    CHECK_AND_FREE ((char *) elements[i].data.Convolve.kernel);
	    break;
	case xieElemPasteUp:
	    CHECK_AND_FREE ((char *) elements[i].data.PasteUp.tiles);
	    break;
	default:
	    break;
	}
    }

    Xfree ((char *) elements);
}


void
XieFreeEncodeJPEGBaseline (param)

XieEncodeJPEGBaselineParam *param;

{
    if (param)
    {
	CHECK_AND_FREE ((char *) param->q_table);
	CHECK_AND_FREE ((char *) param->ac_table);
	CHECK_AND_FREE ((char *) param->dc_table);
	Xfree ((char *) param);
    }
}


void
XieFreeEncodeJPEGLossless (param)

XieEncodeJPEGLosslessParam *param;

{
    if (param)
    {
	CHECK_AND_FREE ((char *) param->table);
	Xfree ((char *) param);
    }
}


void 
XieFreePasteUpTiles (element)

XiePhotoElement	*element;

{
    XieTile *tiles= element->data.PasteUp.tiles;

    if (tiles)
    {
	Xfree (tiles);
	element->data.PasteUp.tiles=NULL;
    }
}
