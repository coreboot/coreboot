CC=gcc
CFLAGS=-Wall -Werror -g

LZMA_OBJ := lzma/LZMAEncoder.o lzma/LZInWindow.o
LZMA_OBJ += lzma/RangeCoderBit.o lzma/StreamUtils.o
LZMA_OBJ += lzma/OutBuffer.o lzma/Alloc.o
LZMA_OBJ += lzma/CRC.o
LZMA_OBJ += lzma/lzma-compress.o

COMMON= common.o compress.o $(LZMA_OBJ)

all: rom-mkstage rom-mkpayload

rom-mkstage: rom-mkstage.o $(COMMON)
	$(CXX) -g -o $@ rom-mkstage.o $(COMMON)

rom-mkpayload: rom-mkpayload.o $(COMMON)
	$(CXX) -o $@ rom-mkpayload.o $(COMMON)

include lzma/Makefile

%.o: %.c
	$(CC) -Wall -Werror -g -c -o $@ $<

clean:
	@ rm -f rom-mkpayload.o rom-mkstage.o $(COMMON)
	@ rm -f rom-mkpayload rom-mkstage
