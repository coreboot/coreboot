# Makefile for the kernel
#
# Copyright (C) 2003 Stefan Reinauer
#
# See the file "COPYING" for further information about
# the copyright and warranty status of this work.
#

OBJS_KERNEL   =  lib.o legacybios.o pcibios.o malloc.o x86glue.o
INCLUDES := -I$(TOPDIR)/include -I$(BUILDDIR) -I $(TOPDIR)/arch/$(ARCH)/include -I$(TOPDIR)/arch/x86emu/include

include $(TOPDIR)/Rules.make

all:	core $(OBJS_KERNEL)

core:
	echo -e "\nBuilding common core files for architecture $(ARCH)"

# dependencies. these are so small that we write them manually.
lib.o: types.h lib.c
legacybios.o: config.h types.h legacybios.c

