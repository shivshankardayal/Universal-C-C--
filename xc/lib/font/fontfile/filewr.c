/* $TOG: filewr.c /main/5 1998/02/09 10:48:21 kaleb $ */

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

FontFilePtr
FontFileOpenWrite (name)
    char    *name;
{
    int	fd;

#ifdef WIN32
    fd = open (name, O_CREAT|O_TRUNC|O_RDWR|O_BINARY, 0666);
#else
    fd = creat (name, 0666);
#endif
    if (fd < 0)
	return 0;
    return (FontFilePtr) BufFileOpenWrite (fd);
}

FontFilePtr
FontFileOpenWriteFd (fd)
{
    return (FontFilePtr) BufFileOpenWrite (fd);
}

FontFilePtr
FontFileOpenFd (fd)
    int	fd;
{
    return (FontFilePtr) BufFileOpenRead (fd);
}
