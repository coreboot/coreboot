## SPDX-License-Identifier: GPL-2.0-only
PROGRAM = archive
HOSTCC ?= gcc
WERROR=-Werror
CFLAGS=-O2 -Wall -Wextra -Wshadow ${WERROR}

SRCS = $(PROGRAM).c

all: $(PROGRAM)

$(PROGRAM): $(SRCS)
	$(HOSTCC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(PROGRAM) *.o

distclean: clean

help:
	@echo "${PROGRAM}: Concatenate files and create an archive"
	@echo "Targets: all, clean, distclean, help"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean help
