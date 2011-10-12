/*
 * $TOG: Alloc.c /main/3 1998/02/06 14:38:48 kaleb $
 *
 * 
Copyright 1989, 1998  The Open Group

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
 * *
 * Author:  Keith Packard, MIT X Consortium
 */

/* stubs for use when Xalloc, Xrealloc and Xfree are not defined */

extern char	*malloc (), *realloc ();

long *
Xalloc (amount)
    unsigned	amount;
{
    if (amount == 0)
	amount = 1;
    return (long *) malloc (amount);
}

long *
Xrealloc (old, amount)
    long	*old;
    unsigned	amount;
{
    if (amount == 0)
	amount = 1;
    if (!old)
	return (long *) malloc (amount);
    return (long *) realloc ((char *) old, amount);
}

void
Xfree (old)
    long    *old;
{
    if (old)
	free ((char *) old);
}