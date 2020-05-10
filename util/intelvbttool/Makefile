##
## SPDX-License-Identifier: GPL-2.0-only

PROGRAM = intelvbttool
CC ?= gcc
CFLAGS ?= -O2 -g
CFLAGS += -Wall -Werror
CFLAGS += -I../../src/commonlib/include -I ../../src/commonlib/bsd/include

all: $(PROGRAM)

$(PROGRAM): $(PROGRAM).c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(PROGRAM).c -o $(PROGRAM)

clean:
	rm -f $(PROGRAM) junit.xml

distclean: clean

.PHONY: all clean distclean
