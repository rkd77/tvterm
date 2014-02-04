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

/*
   VT100 SGR documentation
   From http://vt100.net/docs/vt510-rm/SGR table 5-16
   0  All attributes off
   1  Bold
   4  Underline
   5  Blinking
   7  Negative image
   8  Invisible image
   10    The ASCII character set is the current 7-bit
         display character set (default) - SCO Console only.
   11    Map Hex 00-7F of the PC character set codes
         to the current 7-bit display character set
         - SCO Console only.
   12    Map Hex 80-FF of the current character set to
         the current 7-bit display character set - SCO
         Console only.
   22    Bold off
   24    Underline off
   25    Blinking off
   27    Negative image off
   28    Invisible image off
*/

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_csi.h"
#include "vterm_colors.h"

#include <stdio.h>

short default_fg = 7;
short default_bg = 0;

/* interprets a 'set attribute' (SGR) CSI escape sequence */
void interpret_csi_SGR(vterm_t *vterm, int param[], int pcount)
{
   int   i;

//fprintf(stderr, "interpret_csi_SGR pcount=%d\n", pcount);
   if(pcount==0)
   {
      vterm->curattr=A_NORMAL;                        // reset attributes
      vterm->colors = (default_fg|default_bg << 4);
      vterm->fg = 7;
      vterm->bg = 0;
      return;
   }

   for(i=0;i<pcount;i++)
   {
      if(param[i]==0)                                 // reset attributes
      {
         vterm->curattr=A_NORMAL;
         vterm->colors = (default_fg|default_bg << 4);
         vterm->fg = 7;
         vterm->bg = 0;
         continue;
      }

      if(param[i]==1 || param[i]==2 || param[i]==4)   // bold on
      {
         vterm->curattr |= A_BOLD;
         vterm->colors |= 8;
         continue;
      }

      if(param[i]==5)                                 // blink on
      {
         vterm->curattr |= A_BLINK;
         vterm->colors |= 128;
         continue;
      }

      if(param[i]==7)                 // reverse on
      {
         vterm->curattr |= A_REVERSE;
         continue;
      }

	if (param[i] == 27)
	{
		vterm->curattr &= ~A_REVERSE; // reverse off
		continue;
	}
      
      

      if(param[i]==8)                                 // invisible on
      {
         vterm->curattr |= A_INVIS;
         continue;
      }

      if(param[i]==10)                                // rmacs
      {
         vterm->state &= ~STATE_ALT_CHARSET;
         //fprintf(stderr, "~STATE_ALT_CHARSET 2\n");
         continue;
      }

		if(param[i]==11)                                // smacs
      {
         vterm->state |= STATE_ALT_CHARSET;
         //fprintf(stderr, "STATE_ALT_CHARSET 2\n");
         continue;
      }

      if(param[i]==22 || param[i]==24)                // bold off
      {
         vterm->curattr &= ~A_BOLD;
         vterm->colors &= ~8;
         continue;
      }

      if(param[i]==25)                                // blink off
      {
         vterm->curattr &= ~A_BLINK;
         vterm->colors &= ~128;
         continue;
      }

      if(param[i]==28)                                // invisible off
      {
         vterm->curattr &= ~A_INVIS;
         continue;
      }

      if(param[i] >= 30 && param[i] <= 37)            // set fg color
      {
         vterm->fg = color_values[param[i]-30] & 15;
         vterm->colors = vterm->fg | (vterm->bg << 4);
         if (vterm->curattr & A_BOLD) vterm->colors |= 8;
         if (vterm->curattr & A_BLINK) vterm->colors |= 128;
         //colors=find_color_pair(vterm->fg,vterm->bg);
         //if(colors==-1) colors=0;
         //vterm->curattr |= COLOR_PAIR_TV(colors);
         continue;
      }

      if(param[i] >= 40 && param[i] <= 47)            // set bg color
      {
         vterm->bg = color_values[param[i]-40] & 7;
         vterm->colors = vterm->fg | (vterm->bg << 4);
         if (vterm->curattr & A_BOLD) vterm->colors |= 8;
         if (vterm->curattr & A_BLINK) vterm->colors |= 128;
         //colors=find_color_pair(vterm->fg,vterm->bg);
         //if(colors==-1) colors=0;
         //vterm->curattr |= COLOR_PAIR_TV(colors);
         continue;
      }

      if(param[i]==39)                                // reset fg color
      {
         //pair_content_tv(vterm->colors,&default_fg,&default_bg);
         vterm->fg=default_fg;
         vterm->colors = vterm->fg | (vterm->bg << 4);
         if (vterm->curattr & A_BOLD) vterm->colors |= 8;
         if (vterm->curattr & A_BLINK) vterm->colors |= 128;
         //colors=find_color_pair(vterm->fg,vterm->bg);
         //if(colors==-1) colors=0;
         //vterm->curattr |= COLOR_PAIR_TV(colors);
         continue;
      }

      if(param[i]==49)                                // reset bg color
      {
         //pair_content_tv(vterm->colors,&default_fg,&default_bg);
         vterm->bg=default_bg;
         vterm->colors = vterm->fg | (vterm->bg << 4);
         if (vterm->curattr & A_BOLD) vterm->colors |= 8;
         if (vterm->curattr & A_BLINK) vterm->colors |= 128;
         //colors=find_color_pair(vterm->fg,vterm->bg);
         //if(colors==-1) colors=0;
         //vterm->curattr |= COLOR_PAIR_TV(colors);
         continue;
      }
   }
}
