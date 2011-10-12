/* Emacs style mode select   -*- C++ -*- 
 *-----------------------------------------------------------------------------
 *
 * $Id: r_plane.h,v 1.1.1.1 2001/08/23 10:48:18 bob Exp $
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
 *      Refresh, visplane stuff (floor, ceilings).
 *
 *-----------------------------------------------------------------------------*/

#ifndef __R_PLANE__
#define __R_PLANE__

#include "r_data.h"

#ifdef __GNUG__
#pragma interface
#endif

/* killough 10/98: special mask indicates sky flat comes from sidedef */
#define PL_SKYFLAT (0x80000000)

/* Visplane related. */
extern  short *lastopening;

extern short floorclip[], ceilingclip[];
extern fixed_t yslope[], distscale[];

void R_InitPlanes(void);
void R_ClearPlanes(void);
void R_DrawPlanes (void);

visplane_t *R_FindPlane(
                        fixed_t height, 
                        int picnum,
                        int lightlevel,
                        fixed_t xoffs,  /* killough 2/28/98: add x-y offsets */
                        fixed_t yoffs
                       );

visplane_t *R_CheckPlane(visplane_t *pl, int start, int stop);

#endif

/*----------------------------------------------------------------------------
 *
 * $Log: r_plane.h,v $
 * Revision 1.1.1.1  2001/08/23 10:48:18  bob
 * Two test games for X
 *
 * Revision 1.1  2000/09/22 13:19:38  bob
 * Really Bad Fucking Program -- this CVS
 *
 * Revision 1.4  2000/08/19 15:46:25  bob
 * Fresh update to old source
 *
 * Revision 1.1  2000/08/07 18:16:20  bob
 * Doom port
 *
 * Revision 1.3  1999/10/12 13:00:57  cphipps
 * Changed header to GPL, converted C++ comments to C
 *
 * Revision 1.2  1999/02/02 09:17:47  cphipps
 * Add constant to mark enhanced skies from MBF
 *
 * Revision 1.1  1998/09/13 16:49:50  cphipps
 * Initial revision
 *
 * Revision 1.6  1998/04/27  01:48:34  killough
 * Program beautification
 *
 * Revision 1.5  1998/03/02  11:47:16  killough
 * Add support for general flats xy offsets
 *
 * Revision 1.4  1998/02/09  03:16:06  killough
 * Change arrays to use MAX height/width
 *
 * Revision 1.3  1998/02/02  14:20:45  killough
 * Made some functions static
 *
 * Revision 1.2  1998/01/26  19:27:42  phares
 * First rev with no ^Ms
 *
 * Revision 1.1.1.1  1998/01/19  14:03:03  rand
 * Lee's Jan 19 sources
 *
 *
 *----------------------------------------------------------------------------*/