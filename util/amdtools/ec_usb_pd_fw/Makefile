# SPDX-License-Identifier: GPL-2.0-or-later

PRG  := ec_usb_pd_fw
TOP  ?= $(abspath ../../..)
ROOT := $(TOP)/src

CC      ?= $(CROSS_COMPILE)gcc
HOSTCC  ?= $(CC)
INSTALL ?= /usr/bin/env install
PREFIX  ?= /usr/local

HOSTCFLAGS ?= $(CFLAGS)
HOSTCFLAGS += -Wall -Wextra -MMD -MP -O3
HOSTCFLAGS += -I $(TOP)/util/cbfstool/flashmap/
HOSTCFLAGS += -I $(ROOT)/commonlib/bsd/include

HOSTLDFLAGS ?= $(LDFLAGS)

# there files are in this directory
SRC := main.c utils.c rom.c
# and these are in $(TOP)/util/cbfstool/flashmap/
SRC += fmap.c kv_pair.c valstr.c

OBJ := $(SRC:.c=.o)
DEP := $(SRC:.c=.o.d)

.PHONY: all debug clean install

all: $(PRG)

debug: HOSTCFLAGS += -O0 -g
debug: HOSTLDFLAGS += -g
debug: all

install: $(PRG)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/bin/
	$(INSTALL) $^ $(DESTDIR)$(PREFIX)/bin/

uninstall:
	$(RM) $(PRG) $(DESTDIR)$(PREFIX)/bin/
	$(RMDIR) $(DESTDIR)$(PREFIX)/bin/

clean:
	-$(RM) $(PRG) $(OBJ) $(DEP)

$(PRG): $(OBJ)
	$(HOSTCC) -o $@ $^ $(HOSTLDFLAGS)

%.o: %.c
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ -MF $@.d $<

%.o: $(TOP)/util/cbfstool/flashmap/%.c
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ -MF $@.d $<

-include $(DEP)
