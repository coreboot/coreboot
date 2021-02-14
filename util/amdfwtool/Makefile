# SPDX-License-Identifier: BSD-3-Clause

HOSTCC ?= cc

SRC = amdfwtool.c data_parse.c
OBJ = $(SRC:%.c=%.o)
TARGET = amdfwtool
WERROR=-Werror
CFLAGS=-O2 -Wall -Wextra -Wshadow ${WERROR}

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -f $(TARGET) $(OBJ)

distclean: clean

help:
	@echo "${TARGET}: Create AMD Firmware combination"
	@echo "Targets: all, clean, distclean, help"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean help
