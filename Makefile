CC	= gcc
CFLAGS	= -O0 -g -Wall
LD	= gcc
LDFLAGS	=
OBJS	= common.o library.o category.o roland.o midi_alsa.o junog_waves.o \
	  fantom_waves.o
LIBS	= -lasound

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $<

all: gsex

gsex: $(OBJS) gsex.o
	$(LD) -o$@ $(LDFLAGS) $+ $(LIBS)

clean:
	rm -f core *.o *~ depend gsex

depend:
	@echo Building dependencies...
	@$(CC) $(CFLAGS) -M $(OBJS:.o=.c) gsex.c >$@

include depend
