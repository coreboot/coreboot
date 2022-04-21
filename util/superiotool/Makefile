# SPDX-License-Identifier: GPL-2.0-or-later

PROGRAM = superiotool

CC      ?= gcc
INSTALL ?= /usr/bin/env install
PREFIX  ?= /usr/local

# Set the superiotool version string to the output of 'git describe'.

VERSION := -D'SUPERIOTOOL_VERSION="$(shell git describe 2>/dev/null)"'

CFLAGS += -O2 -Wall -Wstrict-prototypes -Wundef -Wstrict-aliasing \
         -Werror-implicit-function-declaration -ansi -pedantic $(VERSION)
LDFLAGS += -lz

OBJS = superiotool.o serverengines.o ali.o exar.o fintek.o ite.o nsc.o \
       nuvoton.o smsc.o winbond.o infineon.o aspeed.o

OS_ARCH ?= $(shell uname)
ifeq ($(OS_ARCH), Darwin)
LIBS = -framework IOKit -framework DirectHW -lpci -lz
endif
ifeq ($(OS_ARCH), FreeBSD)
CFLAGS = -O2 -Wall -Werror -Wstrict-prototypes -Wundef -Wstrict-aliasing \
         -Werror-implicit-function-declaration -ansi $(VERSION) \
         -I/usr/local/include
LDFLAGS += -L/usr/local/lib
LIBS = -lz
endif
ifeq ($(OS_ARCH), NetBSD)
CFLAGS += -I/usr/pkg/include
LDFLAGS += -L/usr/pkg/lib -Wl,-rpath-link,/usr/pkg/lib
LIBS = -lz -l$(shell uname -p)
endif

# Support for PCI-attached "Super I/Os" (e.g. in VIA VT82686A/B).
CONFIG_PCI = yes

ifeq ($(CONFIG_PCI), yes)
CFLAGS += -DPCI_SUPPORT
OBJS += pci.o via.o amd.o
LIBS += -lpci
ifeq ($(OS_ARCH),NetBSD)
LIBS += -lpciutils
endif
endif

all: pciutils $(PROGRAM)

superiotool.o: *.c superiotool.h

$(PROGRAM): $(OBJS) superiotool.h
	$(CC) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LIBS)

install: $(PROGRAM)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/share/man/man8
	$(INSTALL) -p -m644 $(PROGRAM).8 $(DESTDIR)$(PREFIX)/share/man/man8

clean:
	rm -f $(PROGRAM) *.o junit.xml

distclean: clean

.PHONY: all install clean distclean

ifeq ($(CONFIG_PCI), yes)
define LIBPCI_TEST
/* Avoid a failing test due to libpci header symbol shadowing breakage */
#define index shadow_workaround_index
#ifdef __NetBSD__
#include <pciutils/pci.h>
#else
#include <pci/pci.h>
#endif
struct pci_access *pacc;
int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;
	pacc = pci_alloc();
	return 0;
}
endef
export LIBPCI_TEST

pciutils:
	@printf "\nChecking for pciutils and zlib... "
	@echo "$$LIBPCI_TEST" > .test.c
	@$(CC) $(CFLAGS) .test.c -o .test $(LIBS) $(LDFLAGS) >/dev/null 2>&1 &&	  \
		printf "found.\n" || ( printf "not found.\n\n";		  \
		printf "Please install pciutils-devel and zlib-devel.\n"; \
		printf "See README for more information.\n\n";		  \
		rm -f .test.c .test; exit 1)
	@rm -rf .test.c .test .test.dSYM
endif
