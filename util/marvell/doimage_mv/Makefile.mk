## SPDX-License-Identifier: GPL-2.0-only
TOP            = $(abspath ../../..)
DOIMAGE_FOLDER = marvell/doimage_mv
DOIMAGE_BINARY = doimage

CFLAGS_doimage  = -g -O1 -I./../inc -DMV_CPU_LE -I $(TOP)/src/commonlib/bsd/include

$(objutil)/$(DOIMAGE_FOLDER):
	mkdir -p $@

$(objutil)/$(DOIMAGE_FOLDER)/doimage.o: $(top)/util/marvell/doimage_mv/doimage.c $(objutil)/$(DOIMAGE_FOLDER)
	$(HOSTCC) $(CFLAGS_doimage) -c $< -o $@

$(objutil)/$(DOIMAGE_FOLDER)/$(DOIMAGE_BINARY): $(objutil)/$(DOIMAGE_FOLDER)/doimage.o
	$(HOSTCC) $(CFLAGS_doimage) -o $@ $^
