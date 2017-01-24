CC:=gcc
OBJ:=bucts.o
VERSION:=$(shell git describe)
CFLAGS+=-Wall

ifeq ($(shell uname), FreeBSD)
	CFLAGS += -I/usr/local/include
	LDFLAGS += -L/usr/local/lib
endif

all: bucts

bucts: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) -lpci

%.o: %.c
	$(CC) $(CFLAGS) -DVERSION='"$(VERSION)"' -c $<

.PHONY: clean
clean:
	rm -f bucts $(OBJ)
