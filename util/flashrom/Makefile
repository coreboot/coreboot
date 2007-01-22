#
# Makefile for flash_rom
# 
# redone by Stefan Reinauer <stepan@openbios.org>
#

PROGRAM = flashrom

CC      = gcc
STRIP	= strip
INSTALL = /usr/bin/install
PREFIX  = /usr/local
#CFLAGS  = -O2 -g -Wall -Werror
CFLAGS  = -Os -Wall -Werror -DDISABLE_DOC # -DTS5300
LDFLAGS = -lpci -lz -static 


OBJS  = flash_enable.o udelay.o jedec.o sst28sf040.o am29f040b.o mx29f002.o  \
	sst39sf020.o m29f400bt.o w49f002u.o 82802ab.o msys_doc.o pm49fl004.o \
	sst49lf040.o sst49lfxxxc.o sst_fwhub.o layout.o lbtable.o \
	flashchips.o flash_rom.o sharplhf00l04.o

all: pciutils dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
	$(STRIP) -s $(PROGRAM)

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(PROGRAM) .dependencies
	
dep:
	@$(CC) -MM *.c > .dependencies

pciutils:
	@echo; echo -n "Checking for pciutils... "
	@$(shell ( echo "#include <pci/pci.h>";		   \
		   echo "struct pci_access *pacc;";	   \
		   echo "int main(int argc, char **argv)"; \
		   echo "{ pacc = pci_alloc(); return 0; }"; ) > .test.c )
	@$(CC) $(CFLAGS) .test.c -o .test $(LDFLAGS) &>/dev/null &&	\
		echo "found." || ( echo "not found."; echo;		\
		echo "Please install pciutils and pciutils-devel.";	\
		echo "See README for more information."; echo;		\
		rm -f .test.c .test; exit 1)
	@rm -f .test.c .test

install: $(PROGRAM)
	$(INSTALL) flashrom $(PREFIX)/bin

.PHONY: all clean distclean dep pciutils

-include .dependencies

