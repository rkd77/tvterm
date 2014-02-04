/*
Copyright (C) 2009 Bryan Christ

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*
This library is based on ROTE written by Bruno Takahashi C. de Oliveira
*/

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_csi.h"

/* Interpret DEC SM (set mode) */
void interpret_dec_SM(vterm_t *vterm,int param[],int pcount)
{
   int i;

   if(pcount==0) return;

   for(i=0;i < pcount;i++)
   {
      /* civis is actually "normal" for rxvt */
      if (param[i] == 25) vterm->state &= ~STATE_CURSOR_INVIS;
      else if (param[i] == 9) vterm->state |= STATE_MOUSE;
   }
}
