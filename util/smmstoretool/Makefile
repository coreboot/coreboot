# SPDX-License-Identifier: GPL-2.0-or-later

PRG  := smmstoretool
TOP  ?= $(abspath ../..)
ROOT := $(TOP)/src
MDE  := $(ROOT)/vendorcode/intel/edk2/UDK2017/MdePkg/Include/

CC      ?= $(CROSS_COMPILE)gcc
HOSTCC  ?= $(CC)
INSTALL ?= /usr/bin/env install
PREFIX  ?= /usr/local

HOSTCFLAGS ?= $(CFLAGS)
HOSTCFLAGS += -Wall -Wextra -MMD -MP -O3
HOSTCFLAGS += -I $(ROOT)/commonlib/bsd/include
HOSTCFLAGS += -I $(ROOT)/vendorcode/intel/edk2/
HOSTCFLAGS += -I $(TOP)/util/cbfstool/flashmap/
HOSTCFLAGS += -I $(MDE)

HOSTLDFLAGS ?= $(LDFLAGS)

MACHINE := $(shell uname -m)
ifeq ($(MACHINE),x86_64)
    HOSTCFLAGS += -I $(MDE)/X64
else ifeq ($(patsubst i%86,Ia32,$(MACHINE)),Ia32)
    HOSTCFLAGS += -I $(MDE)/Ia32
else
    $(error Unsupported machine: '$(MACHINE)')
endif

# there files are in this directory
SRC := data.c fv.c guids.c main.c storage.c utils.c vs.c
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

clean:
	-$(RM) $(PRG) $(OBJ) $(DEP)

$(PRG): $(OBJ)
	$(HOSTCC) -o $@ $^ $(HOSTLDFLAGS)

%.o: %.c
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ -MF $@.d $<

%.o: $(TOP)/util/cbfstool/flashmap/%.c
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ -MF $@.d $<

-include $(DEP)
