VERSION	= 0.1
NAME	= fsex
PKG	= $(NAME)-$(VERSION)

CC	= gcc
CFLAGS	= -O0 -g -Wall -DVERSION=\"$(VERSION)\" -D_TRACE
LD	= gcc
LDFLAGS	=
CMN_OBJ	= common.o library.o category.o id.o patch.o
SEX_OBJ	= sex.o manufacturer.o midi_alsa.o fsex.o
LIB_OBJ	= lib.o junog_waves.o fantom_waves.o xv5080_waves.o flib.o
OBJS	= $(CMN_OBJ) $(SEX_OBJ) $(LIB_OBJ)
DFILES	= Makefile README TODO common.h midi.h xv.h library.h lib.h sex.h \
	  id.h patch.h fsex.1 flib.1 alsa-junog.patch

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $<

all: fsex flib

fsex: $(CMN_OBJ) $(SEX_OBJ)
	$(LD) -o$@ $(LDFLAGS) $+ -lasound

flib: $(CMN_OBJ) $(LIB_OBJ)
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
