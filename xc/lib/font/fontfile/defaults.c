/* $TOG: defaults.c /main/4 1998/02/09 10:48:02 kaleb $ */

/*

Copyright 1990, 1998  The Open Group

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

/*
 * Author:  Keith Packard, MIT X Consortium
 */

#include    <X11/X.h>
#include    <X11/Xproto.h>
#include    <servermd.h>

#ifndef DEFAULT_BIT_ORDER
#ifdef BITMAP_BIT_ORDER
#define DEFAULT_BIT_ORDER BITMAP_BIT_ORDER
#else
#define DEFAULT_BIT_ORDER MSBFirst
#endif
#endif

#ifndef DEFAULT_BYTE_ORDER
#ifdef IMAGE_BYTE_ORDER
#define DEFAULT_BYTE_ORDER IMAGE_BYTE_ORDER
#else
#define DEFAULT_BYTE_ORDER MSBFirst
#endif
#endif

#ifndef DEFAULT_GLYPH_PAD
#ifdef GLYPHPADBYTES
#define DEFAULT_GLYPH_PAD GLYPHPADBYTES
#else
#define DEFAULT_GLYPH_PAD 4
#endif
#endif

#ifndef DEFAULT_SCAN_UNIT
#define DEFAULT_SCAN_UNIT 1
#endif

FontDefaultFormat (bit, byte, glyph, scan)
    int	    *bit, *byte, *glyph, *scan;
{
    *bit = DEFAULT_BIT_ORDER;
    *byte = DEFAULT_BYTE_ORDER;
    *glyph = DEFAULT_GLYPH_PAD;
    *scan = DEFAULT_SCAN_UNIT;
}