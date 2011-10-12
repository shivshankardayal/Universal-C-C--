/* $TOG: fileio.c /main/6 1998/02/09 10:48:16 kaleb $ */

/*

Copyright 1991, 1998  The Open Group

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

#include <fntfilio.h>
#include <X11/Xos.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif

FontFilePtr
FontFileOpen (name)
    char    *name;
{
    int		fd;
    int		len;
    BufFilePtr	raw, cooked;

    fd = open (name, O_BINARY);
    if (fd < 0)
	return 0;
    raw = BufFileOpenRead (fd);
    if (!raw)
    {
	close (fd);
	return 0;
    }
    len = strlen (name);
    if (len > 2 && !strcmp (name + len - 2, ".Z")) {
	cooked = BufFilePushCompressed (raw);
	if (!cooked) {
	    BufFileClose (raw, TRUE);
	    return 0;
	}
	raw = cooked;
#ifdef X_GZIP_FONT_COMPRESSION
    } else if (len > 3 && !strcmp (name + len - 3, ".gz")) {
	cooked = BufFilePushZIP (raw);
	if (!cooked) {
	    BufFileClose (raw, TRUE);
	    return 0;
	}
	raw = cooked;
#endif
    }
    return (FontFilePtr) raw;
}

FontFileClose (f)
    FontFilePtr	f;
{
    BufFileClose ((BufFilePtr) f, TRUE);
}

