/* Emacs style mode select   -*- C++ -*- 
 *-----------------------------------------------------------------------------
 *
 * $Id: r_sky.c,v 1.1.1.1 2001/08/23 10:48:18 bob Exp $
 *
 *  LxDoom, a Doom port for Linux/Unix
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *   and Colin Phipps
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *  Sky rendering. The DOOM sky is a texture map like any
 *  wall, wrapping around. A 1024 columns equal 360 degrees.
 *  The default sky map is 256 columns and repeats 4 times
 *  on a 320 screen?
 *
 *-----------------------------------------------------------------------------*/

static const char
rcsid[] = "$Id: r_sky.c,v 1.1.1.1 2001/08/23 10:48:18 bob Exp $";

#ifdef __GNUG__
#pragma implementation "r_sky.h"
#endif
#include "r_sky.h"

//
// sky mapping
//
int skyflatnum;
int skytexture;
int skytexturemid;

//
// R_InitSkyMap
// Called whenever the view size changes.
//
void R_InitSkyMap (void)
{
  skytexturemid = 100*FRACUNIT;
}

/*----------------------------------------------------------------------------
 *
 * $Log: r_sky.c,v $
 * Revision 1.1.1.1  2001/08/23 10:48:18  bob
 * Two test games for X
 *
 * Revision 1.1  2000/09/22 13:19:38  bob
 * Really Bad Fucking Program -- this CVS
 *
 * Revision 1.1  2000/08/07 18:16:20  bob
 * Doom port
 *
 * Revision 1.2  1999/10/12 13:01:14  cphipps
 * Changed header to GPL
 *
 * Revision 1.1  1998/09/13 16:49:50  cphipps
 * Initial revision
 *
 * Revision 1.6  1998/05/03  23:01:06  killough
 * beautification
 *
 * Revision 1.5  1998/05/01  14:14:24  killough
 * beautification
 *
 * Revision 1.4  1998/02/05  12:14:31  phares
 * removed dummy comment
 *
 * Revision 1.3  1998/01/26  19:24:49  phares
 * First rev with no ^Ms
 *
 * Revision 1.2  1998/01/19  16:17:59  rand
 * Added dummy line to be removed later.
 *
 * Revision 1.1.1.1  1998/01/19  14:03:07  rand
 * Lee's Jan 19 sources
 *
 *----------------------------------------------------------------------------*/
