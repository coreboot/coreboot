# SPDX-License-Identifier: BSD-3-Clause

HOSTCC ?= cc

SRC = amdfwtool.c data_parse.c
OBJ = $(SRC:%.c=%.o)
TARGET = amdfwtool
CFLAGS=-O2 -Wall -Wextra -Wshadow

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -f $(TARGET) $(OBJ)

distclean: clean
