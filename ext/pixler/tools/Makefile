CFLAGS += -std=c99 -Wall -Wextra -Os
BIN = png2chr chr2png lz4x

.phony: all clean

all: $(BIN)

clean:
	-rm -r *.o *.exe $(BIN)

slib.o: slib.h

LDFLAGS=-lpng -lz -static
png2chr: png2chr.o slib.o shared.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

chr2png: chr2png.o slib.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

lz4x: lz4x.cpp
	$(CC) -o $@ $<
