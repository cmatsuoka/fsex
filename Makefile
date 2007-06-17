CC	= gcc
CFLAGS	= -O0 -g -Wall
LD	= gcc
LDFLAGS	=
LIBS	=


.c.o:
	$(CC) -c $(CFLAGS) -o $*.o $<


all: jglib

jglib: jglib.o common.o category.o
	$(LD) -o$@ $(LDFLAGS) $+ $(LIBS)

clean:
	rm -f core *.o *~
