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

#include <glib.h>
#include <stdio.h>

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_colors.h"

extern struct colors col[];
extern short default_bg, default_fg;

int vterm_set_colors(vterm_t *vterm,short fg,short bg)
{
//   short colors;

//fprintf(stderr, "vterm_set_colors\n");
   if(vterm==NULL) return -1;
   //if(has_colors()==FALSE) return -1;

//   colors=find_color_pair(fg,bg);
  // if(colors==-1) colors=0;

   vterm->colors = (default_fg)|(default_bg << 4);

   return 0;
}

short vterm_get_colors(vterm_t *vterm)
{
//fprintf(stderr, "vterm_get_colors\n");
   if(vterm==NULL) return -1;
   //if(has_colors()==FALSE) return -1;

   return vterm->colors;
}

void pair_content_tv(short c, short *fg, short *bg)
{
	*fg = col[c].fg;
	*bg = col[c].bg;
//	fprintf(stderr, "pair_content_tv: c=%d fg=%d bg=%d\n", c, *fg, *bg);
}

short find_color_pair(short fg,short bg)
{
//   short fg_color,bg_color;
//   int   i;
	short i;

//fprintf(stderr, "find_color_pair before: fg=%d bg=%d\n", fg, bg);

//	fg = color_values[fg];
//	bg = color_values[bg];

//fprintf(stderr, "find_color_pair after: fg=%d bg=%d\n", fg, bg);

	for (i = 0; i < 64; ++i)
	{
		if (col[i].fg == fg && col[i].bg == bg)
		{
			//fprintf(stderr, "return %d\n", i);
			return i;
		}
	}
	//fprintf(stderr, "return 0\n");
	return 0;

	//short res = (fg & 15) | (bg & 7) << 4;

   //if(has_colors()==FALSE) return -1;

//fprintf(stderr, "find_color_pair: fg=%d bg=%d res=%d\n", fg, bg, res);

   //return res;

#if 0


//   for(i=1;i<COLOR_PAIRS;i++)
//   {
//      pair_content(i,&fg_color,&bg_color);
//      if(fg_color==fg && bg_color==bg) break;
//   }

//   if(i==COLOR_PAIRS) return -1;

//   return i;
#endif
}
