## SPDX-License-Identifier: GPL-2.0-only
CC:=gcc
OBJ:=bucts.o
TARGET=bucts
VERSION:=$(shell git describe)
WERROR=-Werror
CFLAGS=-O2 -Wall -Wextra -Wshadow ${WERROR}

ifeq ($(shell uname), FreeBSD)
	CFLAGS += -I/usr/local/include
	LDFLAGS += -L/usr/local/lib
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) -lpci

%.o: %.c
	$(CC) $(CFLAGS) -DVERSION='"$(VERSION)"' -c $<

clean:
	rm -f bucts $(OBJ)

distclean: clean

help:
	@echo "${TARGET}: tool to manipulate the BUC.TS bit on Intel targets."
	@echo "Targets: all, clean, distclean, help"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean help
