CC	= gcc
CFLAGS	= -O0 -g -Wall
LD	= gcc
LDFLAGS	=
OBJS	= jglib.o common.o category.o junog_waves.o alsa.o
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
