VERSION	= 0.1
NAME	= fsex
PKG	= $(NAME)-$(VERSION)

CC	= gcc
CFLAGS	= -O0 -g -Wall -DVERSION=\"$(VERSION)\"
LD	= gcc
LDFLAGS	=
SEX_OBJ	= common.o library.o category.o sex.o manufacturer.o midi_alsa.o fsex.o
LIB_OBJ	= common.o library.o category.o lib.o junog_waves.o fantom_waves.o \
	  xv5080_waves.o flib.o
OBJS	= $(SEX_OBJ) $(LIB_OBJ)
DFILES	= Makefile README TODO common.h midi.h xv.h library.h lib.h sex.h \
	  fsex.1 alsa-junog.patch

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $<

all: fsex flib

fsex: $(SEX_OBJ)
	$(LD) -o$@ $(LDFLAGS) $+ -lasound

flib: $(LIB_OBJ)
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
