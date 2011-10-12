/* $TOG: register.c /main/5 1998/02/09 10:48:49 kaleb $ */

/*

Copyright 1994, 1998  The Open Group

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
 * This is in a separate source file so that small programs
 * such as mkfontdir that want to use the fontfile utilities don't
 * end up dragging in code from all the renderers, which is not small.
 */

FontFileRegisterFpeFunctions()
{
    BitmapRegisterFontFileFunctions ();

#ifndef LOWMEMFTPT

#ifndef CRAY
    SpeedoRegisterFontFileFunctions ();
    Type1RegisterFontFileFunctions();
#endif

#endif /* ifndef LOWMEMFTPT */

    FontFileRegisterLocalFpeFunctions ();
}
