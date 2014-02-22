CFLAGS = -Wall -g3 -O2
DEFS = -D_REENTRANT
GLIB_INCLUDES = $(shell pkg-config --cflags glib-2.0)
GLIB_LIBS = $(shell pkg-config --libs glib-2.0)
PREFIX = /usr/local
libdir = /usr/lib
includedir = /usr/include
RHTV_LIBS = $(shell rhtv-config --dlibs)
RHTV_INCLUDES = $(shell rhtv-config --include)
OBJS = vterm.o vterm_colors.o vterm_csi.o vterm_csi_CUP.o \
vterm_csi_CUx.o vterm_csi_DCH.o vterm_csi_DECSTBM.o vterm_csi_DL.o \
vterm_csi_ECH.o vterm_csi_ED.o vterm_csi_EL.o vterm_csi_ICH.o \
vterm_csi_IL.o vterm_csi_RESTORECUR.o vterm_csi_SAVECUR.o vterm_csi_SGR.o \
vterm_dec_RM.o vterm_dec_SM.o vterm_erase.o vterm_escape.o vterm_misc.o \
vterm_read.o vterm_render.o vterm_resize.o vterm_scroll.o vterm_wnd.o vterm_write.o vshell.o

all: tvterm

.c.o:
	$(CXX) $(CFLAGS) $(DEFS) -c $< $(GLIB_INCLUDES)

vshell.o:	demo/vshell.cc
	$(CXX) $(CFLAGS) $(DEFS) -c $< $(RHTV_INCLUDES) $(GLIB_INCLUDES) -I.

tvterm: $(OBJS)
	$(CXX) -o $@ $(OBJS) -lutil $(RHTV_LIBS) $(GLIB_LIBS)

clean:
	rm -f *.o demo/*.o

install:
	install tvterm $(DESTDIR)/$(PREFIX)/bin
