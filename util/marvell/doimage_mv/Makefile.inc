## SPDX-License-Identifier: GPL-2.0-only
DOIMAGE_FOLDER = marvell/doimage_mv
DOIMAGE_BINARY = doimage

CFLAGS  = -g -O1 -I./../inc -DMV_CPU_LE

$(objutil)/$(DOIMAGE_FOLDER):
	mkdir -p $@

$(objutil)/$(DOIMAGE_FOLDER)/doimage.o: $(top)/util/marvell/doimage_mv/doimage.c $(objutil)/$(DOIMAGE_FOLDER)
	$(HOSTCC) $(CFLAGS) -c $< -o $@

$(objutil)/$(DOIMAGE_FOLDER)/$(DOIMAGE_BINARY): $(objutil)/$(DOIMAGE_FOLDER)/doimage.o
	$(HOSTCC) $(CFLAGS) -o $@ $^
