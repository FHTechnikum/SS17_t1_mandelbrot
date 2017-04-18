VERSION	= 1.0
CC	= gcc
CFLAGS	= -Wall -g -D_REENTRANT -DVERSION=\"$(VERSION)\"
LDFLAGS	= -lm

OBJGEN	= pixelgen.o functions.o myhead.h
OBJWRI	= writepic.o functions.o myhead.h

all: pixelgen movepixelgen writepic movewritepic clean

pixelgen: $(OBJGEN)
	$(CC) $(CFLAGS) $(OBJGEN) $(LDFLAGS)

movepixelgen:
	mv a.out pixelgen

writepic: $(OBJWRI)
	$(CC) $(CFLAGS) $(OBJWRI) $(LDFLAGS)

movewritepic:
	mv a.out writepic

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm pixelgen.o writepic.o functions.o
