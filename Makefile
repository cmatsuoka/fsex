VERSION	= 0.2
NAME	= fsex
PKG	= $(NAME)-$(VERSION)

CC	= gcc
CFLAGS	= -O0 -g -Wall -DVERSION=\"$(VERSION)\" #-D_TRACE
LD	= gcc
LDFLAGS	=
CMN_OBJ	= common.o library.o category.o id.o patch.o
SEX_OBJ	= sex.o manufacturer.o midi_alsa.o fsex.o
LIB_OBJ	= lib.o junog_waves.o fantom_waves.o flib.o
CVT_OBJ	= conv.o fconv.o junog_waves.o fantom_waves.o
OBJS	= $(CMN_OBJ) $(SEX_OBJ) $(LIB_OBJ) $(CVT_OBJ)
DFILES	= Makefile README TODO LICENSE common.h midi.h xv.h library.h lib.h \
	  sex.h id.h patch.h fsex.1 flib.1 fconv.1 linux-2.6.21-junog.patch \
	  xv5080_waves.c

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $<

all: fsex flib fconv

fsex: $(CMN_OBJ) $(SEX_OBJ)
	$(LD) -o$@ $(LDFLAGS) $+ -lasound

flib: $(CMN_OBJ) $(LIB_OBJ)
	$(LD) -o$@ $(LDFLAGS) $+

fconv: $(CMN_OBJ) $(CVT_OBJ)
	$(LD) -o$@ $(LDFLAGS) $+

clean:
	rm -f core *.o *~ depend fsex flib

$(OBJS): Makefile

dist:
	@echo Create tarball...
	@rm -Rf $(PKG)
	@mkdir $(PKG)
	@cp -p $(DFILES) $(OBJS:.o=.c) $(PKG)/
	@tar cf - $(PKG) | gzip -c > $(PKG).tar.gz
	@rm -Rf $(PKG)
	@ls -l $(PKG).tar.gz
	
depend:
	@echo Building dependencies...
	@$(CC) $(CFLAGS) -M $(OBJS:.o=.c) >$@

include depend
