/* $TOG: bitmap.h /main/6 1998/02/09 10:48:57 kaleb $ */

/*

Copyright 1990, 1998  The Open Group

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

/*
 * Author:  Keith Packard, MIT X Consortium
 */

#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <fntfilio.h>
#include <stdio.h>  /* just for NULL */

/*
 * Internal format used to store bitmap fonts
 */

typedef struct _BitmapExtra {
    Atom       *glyphNames;
    int        *sWidths;
    CARD32      bitmapsSizes[GLYPHPADOPTIONS];
    FontInfoRec info;
}           BitmapExtraRec, *BitmapExtraPtr;

typedef struct _BitmapFont {
    unsigned    version_num;
    int         num_chars;
    int         num_tables;
    CharInfoPtr metrics;	/* font metrics, including glyph pointers */
    xCharInfo  *ink_metrics;	/* ink metrics */
    char       *bitmaps;	/* base of bitmaps, useful only to free */
    CharInfoPtr *encoding;	/* array of char info pointers */
    CharInfoPtr pDefault;	/* default character */
    BitmapExtraPtr bitmapExtra;	/* stuff not used by X server */
}           BitmapFontRec, *BitmapFontPtr;

extern int  bitmapReadFont(), bitmapReadFontInfo();
extern int  bitmapGetGlyphs(), bitmapGetMetrics();
extern int  bitmapGetBitmaps(), bitmapGetExtents();
extern void bitmapUnloadFont();

extern void bitmapComputeFontBounds();
extern void bitmapComputeFontInkBounds();

#endif				/* _BITMAP_H_ */
