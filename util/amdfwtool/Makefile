# SPDX-License-Identifier: BSD-3-Clause

top ?= $(abspath ../..)
ifneq ($(CC),)
HOSTCC ?= $(CC)
else
HOSTCC ?= cc
endif

TARGETS = amdfwread amdfwtool

all: $(TARGETS)

BUILD_ALL_TOOLS = 1
include ./Makefile.mk

amdfwread: $(amdfwreadobj)
	$(HOSTCC) $(amdfwreadobj) $(LDFLAGS) -o $@

amdfwtool: $(amdfwtoolobj)
	$(HOSTCC) $(amdfwtoolobj) $(LDFLAGS) -o $@

%.o: %.c $(amdfwheader)
	$(HOSTCC) $(AMDFWTOOLCFLAGS) -c -o $@ $<

clean:
	@rm -f $(TARGETS) $(amdfwreadobj) $(amdfwtoolobj)

distclean: clean

help:
	@echo "${TARGETS}: Tools to create and read from AMD firmware combinations"
	@echo "Targets: all, clean, distclean, help"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean help
