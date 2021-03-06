/* $TOG: minimain.c /main/5 1998/02/09 10:52:44 kaleb $ */

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
 
#include "ximager5.h"
 
main()
{
       XYspace S;
       path p;
 
       InitImager();
       S = Scale(IDENTITY, 300.0, -300.0);
       p = Join(Line(Loc(S, 0.0, 1.0)), Line(Loc(S, 1.0, 0.0)));
       Interior(ClosePath(p), EVENODDRULE);
}
 
void Trace()
{
}
 
void *DEFAULTDEVICE;
