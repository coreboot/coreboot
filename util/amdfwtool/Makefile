# SPDX-License-Identifier: BSD-3-Clause

HOSTCC ?= cc

READ_SRC = amdfwread.c
READ_OBJ = $(READ_SRC:%.c=%.o)
TOOL_SRC = amdfwtool.c data_parse.c
TOOL_OBJ = $(TOOL_SRC:%.c=%.o)
HEADER=amdfwtool.h
TARGETS = amdfwread amdfwtool
WERROR=-Werror
CFLAGS=-O2 -Wall -Wextra -Wshadow ${WERROR}

all: $(TARGETS)

amdfwread: $(READ_OBJ)
	$(CC) $(READ_OBJ) $(LDFLAGS) -o $@

amdfwtool: $(TOOL_OBJ)
	$(CC) $(TOOL_OBJ) $(LDFLAGS) -o $@

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm -f $(TARGETS) $(READ_OBJ) $(TOOL_OBJ)

distclean: clean

help:
	@echo "${TARGETS}: Tools to create and read from AMD firmware combinations"
	@echo "Targets: all, clean, distclean, help"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean help
