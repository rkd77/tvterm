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

#include <stdlib.h>
#include <pty.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <utmp.h>
#include <termios.h>
#include <signal.h>

#include <sys/types.h>

#include <glib.h>

#include "vterm.h"
#include "vterm_colors.h"
#include "vterm_private.h"
#include "vterm_write.h"

vterm_t* vterm_create(guint width,guint height,guint flags)
{
   vterm_t        *vterm;
   struct passwd  *user_profile;
   const char     *user_shell=NULL;
   pid_t          child_pid;
   int            master_fd;
   struct winsize ws={0};//.ws_xpixel=0,.ws_ypixel=0};
   // int            cell_count;
   guint          i;

   if(height <= 0 || width <= 0) return NULL;

   vterm=(vterm_t*)g_malloc0(sizeof(vterm_t));

   /* record dimensions */
   vterm->rows=height;
   vterm->cols=width;

   // cell_count=width*height;

   /* create the cell matrix */
   vterm->cells=(vterm_cell_t**)g_malloc0(sizeof(vterm_cell_t*)*height);

   for(i=0;i < height;i++)
   {
      vterm->cells[i]=(vterm_cell_t*)g_malloc0(sizeof(vterm_cell_t)*width);
   }

   // initialize all cells with defaults
   vterm_erase(vterm);

   // initialization of other public fields
   vterm->crow=0;
   vterm->ccol=0;

   // default active colors
   vterm->curattr = A_NORMAL;//COLOR_PAIR_TV(vterm->colors);

//fprintf(stderr, "vterm->curattr = %d\n", vterm->curattr);

   // initial scrolling area is the whole window
   vterm->scroll_min=0;
   vterm->scroll_max=height-1;

   vterm->flags=flags;

   memset(&ws,0,sizeof(ws));
   ws.ws_row=height;
   ws.ws_col=width;

   child_pid=forkpty(&master_fd,NULL,NULL,&ws);
   vterm->pty_fd=master_fd;

   if(child_pid < 0)
   {
      vterm_destroy(vterm);
      return NULL;
   }

   if(child_pid==0)
   {
      signal(SIGINT,SIG_DFL);

      // default is rxvt emulation
      setenv("TERM","rxvt",1);

      if(flags & VTERM_FLAG_VT100)
      {
         setenv("TERM","vt100",1);
      }

      user_profile=getpwuid(getuid());
      if(user_profile==NULL) user_shell="/bin/sh";
      else if(user_profile->pw_shell==NULL) user_shell="/bin/sh";
      else user_shell=user_profile->pw_shell;

      if(user_shell==NULL) user_shell="/bin/sh";

//fprintf(stderr, "execl\n");
      // start the shell
      if(execl(user_shell,user_shell,"-l",NULL)==-1)
      {
         //fprintf(stderr, "FAIL\n");
         exit(EXIT_FAILURE);
      }
	//fprintf(stderr, "OK\n");
      exit(EXIT_SUCCESS);
   }

   vterm->child_pid=child_pid;

   if(ttyname_r(master_fd,vterm->ttyname,sizeof(vterm->ttyname)-1) !=0)
   {
      snprintf(vterm->ttyname,sizeof(vterm->ttyname)-1,"vterm");
   }

   if(flags & VTERM_FLAG_VT100) vterm->write=vterm_write_vt100;
   else vterm->write=vterm_write_rxvt;

   return vterm;
}

void vterm_destroy(vterm_t *vterm)
{
   int   i;

   if(vterm==NULL) return;

   for(i=0;i < vterm->rows;i++) g_free(vterm->cells[i]);
   g_free(vterm->cells);

   g_free(vterm);

   return;
}

pid_t vterm_get_pid(vterm_t *vterm)
{
   if(vterm==NULL) return -1;

   return vterm->child_pid;
}

gint vterm_get_pty_fd(vterm_t *vterm)
{
   if(vterm==NULL) return -1;

   return vterm->pty_fd;
}

const gchar* vterm_get_ttyname(vterm_t *vterm)
{
   if(vterm == NULL) return NULL;

   return (const gchar*)vterm->ttyname;
}
