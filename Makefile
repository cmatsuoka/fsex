VERSION	= 0.1
NAME	= fsex
PKG	= $(NAME)-$(VERSION)

CC	= gcc
CFLAGS	= -O0 -g -Wall -DVERSION=\"$(VERSION)\"
LD	= gcc
LDFLAGS	=
OBJS	= common.o library.o category.o roland.o midi_alsa.o junog_waves.o \
	  fantom_waves.o gsex.o
DFILES	= Makefile README TODO common.h midi.h xv.h
LIBS	= -lasound

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $<

all: gsex

gsex: $(OBJS)
	$(LD) -o$@ $(LDFLAGS) $+ $(LIBS)

clean:
	rm -f core *.o *~ depend gsex

depend:
	@echo Building dependencies...
	@$(CC) $(CFLAGS) -M $(OBJS:.o=.c) >$@

$(OBJS): Makefile

dist:
	@echo Create tarball...
	@rm -Rf $(PKG)
	@mkdir $(PKG)
	@cp -p $(DFILES) $(OBJS:.o=.c) $(PKG)/
	@tar cf - $(PKG) | gzip -c > $(PKG).tar.gz
	@rm -Rf $(PKG)
	@ls -l $(PKG).tar.gz
	
	
include depend
