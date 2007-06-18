CC	= gcc
CFLAGS	= -O0 -g -Wall
LD	= gcc
LDFLAGS	=
OBJS	= jglib.o common.o category.o
LIBS	=

.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $<

all: jglib

jglib: $(OBJS)
	$(LD) -o$@ $(LDFLAGS) $+ $(LIBS)

clean:
	rm -f core *.o *~ depend

depend:
	@echo Building dependencies...
	@$(CC) $(CFLAGS) -M $(OBJS:.o=.c) >$@

include depend
