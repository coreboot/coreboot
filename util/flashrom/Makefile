#
# Makefile for flash_rom
# 
# redone by Stefan Reinauer <stepan@openbios.org>
#

PROGRAM = flashrom

CC      = gcc
STRIP	= strip
#CFLAGS  = -O2 -g -Wall -Werror
CFLAGS  = -Os -Wall -Werror -DDISABLE_DOC
LDFLAGS = -lpci -static


OBJS  = flash_enable.o udelay.o jedec.o sst28sf040.o am29f040b.o mx29f002.o  \
	sst39sf020.o m29f400bt.o w49f002u.o 82802ab.o msys_doc.o pm49fl004.o \
	sst49lf040.o sst_fwhub.o layout.o lbtable.o flashchips.o flash_rom.o

all: dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
	$(STRIP) -s $(PROGRAM)

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(PROGRAM) .dependencies
	
dep:
	@$(CC) -MM *.c > .dependencies

.PHONY: all clean distclean dep 

-include .dependencies

