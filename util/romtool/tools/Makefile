obj ?= $(shell pwd)

CC=gcc
CFLAGS=-Wall -W -Werror -g

all: rom-mkstage rom-mkpayload

include lzma/Makefile

COMMON= common.o compress.o $(LZMA_OBJ)


$(obj)/rom-mkstage: $(obj)/rom-mkstage.o $(patsubst %,$(obj)/%,$(COMMON))
	$(CXX) -g -o $@ $(obj)/rom-mkstage.o $(patsubst %,$(obj)/%,$(COMMON))

$(obj)/rom-mkpayload: $(obj)/rom-mkpayload.o $(patsubst %,$(obj)/%,$(COMMON))
	$(CXX) -o $@ $(obj)/rom-mkpayload.o $(patsubst %,$(obj)/%,$(COMMON))

$(obj)/%.o: %.c
	$(CC) -Wall -Werror -g -c -o $@ $<

clean:
	@ rm -f rom-mkpayload.o rom-mkstage.o $(COMMON)
	@ rm -f rom-mkpayload rom-mkstage

