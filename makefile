VERSION	= 1.0
CC	= gcc
CFLAGS	= -Wall -g -D_REENTRANT -DVERSION=\"$(VERSION)\"
LDFLAGS	= -lm

OBJGEN	= pixelgen.o functions.o myhead.h
OBJWRI	= writepic.o functions.o myhead.h

all: pixelgen writepic clean

pixelgen: $(OBJGEN)
	$(CC) $(CFLAGS) -o pixelgen $(OBJGEN) $(LDFLAGS)

writepic: $(OBJWRI)
	$(CC) $(CFLAGS) -o writepic $(OBJWRI) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm pixelgen.o writepic.o functions.o
