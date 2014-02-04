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
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "vterm.h"
#include "vterm_private.h"
#include "vterm_render.h"

ssize_t vterm_read_pipe(vterm_t *vterm)
{
   struct pollfd     fd_array;
   char 					*buf=NULL;
   char              *pos;
   int               bytes_peek=0;
	size_t				bytes_waiting;
   ssize_t				bytes_read=0;
   size_t            bytes_remaining=0;
   size_t            bytes_total=0;
   gint              retval;
   pid_t             child_pid;
   int               pid_status;
   int               errcpy=0;

   // make sure pty file descriptor is good
   if(vterm->pty_fd < 0) return -1;

   // check to see if child pid has exited
   child_pid=waitpid(vterm->child_pid,&pid_status,WNOHANG);

   if(child_pid==vterm->child_pid || child_pid==-1)
   {
      vterm->state |= STATE_CHILD_EXITED;
      return -1;
   }

   fd_array.fd=vterm->pty_fd;
   fd_array.events=POLLIN;

   // wait 10 millisecond for data on pty file descriptor.
   retval=poll(&fd_array,1,10);

   // no data or poll() error.
   if(retval <= 0)
   {
      if(errno == EINTR) return 0;
      return retval;
   }

#ifdef FIONREAD
	retval=ioctl(vterm->pty_fd,FIONREAD,&bytes_peek);
#else
	retval=ioclt(vterm->pty_fd,TIOCINQ,&bytes_peek);
#endif

   if(retval == -1) return 0;
	if(bytes_peek == 0) return 0;

   bytes_waiting=bytes_peek;
   if(bytes_waiting > SSIZE_MAX) bytes_waiting=SSIZE_MAX;
   bytes_remaining=bytes_waiting;

	buf=(char*)calloc(bytes_waiting+10,sizeof(char));	/* 10 byte padding	*/
   pos=buf;

   do
   {
      bytes_read=read(vterm->pty_fd,pos,bytes_remaining);
      if(bytes_read == -1)
      {
         if(errno == EINTR) bytes_read=0;
         else errcpy=errno;
      }

      if(bytes_read <= 0) break;

      bytes_remaining-=bytes_read;
      bytes_total+=bytes_read;
      pos+=bytes_read;
   }
   while(bytes_remaining > 0);

//fprintf(stderr, "bytes_remaining = %d, bytes_waiting = %d,bytes_read = %d\n", bytes_remaining, bytes_waiting, bytes_read);
   // render the data to the offscreen terminal buffer.
   if((bytes_waiting > 0) && (bytes_read != -1))
   {
      vterm_render(vterm,buf,bytes_read);
   }

   // release memory
   if(buf!=NULL) free(buf);

   if(bytes_read == -1 && errcpy != EINTR) return -1;

   return bytes_waiting-bytes_remaining;
}


