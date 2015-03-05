INCLUDE_DIRS = 
LIB_DIRS = 
CC=gcc

CDEFS=

# Use this for unoptimized code
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)

# Use this to optimize and generate vector instructions
# CFLAGS= -O3 -msse3 $(INCLUDE_DIRS) $(CDEFS)

LIBS=

HFILES= 
CFILES= brighten.c

SRCS= ${HFILES} ${CFILES}
COBJS= ${CILES:.c=.o}

all:	brighten

clean:
	-rm -f *.o *.d brighter.ppm brightc.asm
	-rm -f brighten

distclean:
	-rm -f *.o *.d

brighten: brighten.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o $(LIBS)

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<
