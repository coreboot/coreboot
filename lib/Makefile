##
## This file is part of the coreboot project.
##
## Copyright (C) 2006-2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

DECOMPRESSORS :=
ifeq ($(CONFIG_COMPRESSION_LZMA),y)
DECOMPRESSORS += lzma.c
endif
ifeq ($(CONFIG_COMPRESSION_NRV2B),y)
DECOMPRESSORS += nrv2b.c
endif

#
# This part takes care of compression. It should build the compression modules
# in a way that they can be put in the lar.
#

ifeq ($(CONFIG_COMPRESSION_LZMA),y)
lzma: $(obj)/util/lzma/lzma $(obj)/util/lzma/lzma-compress.o
else
lzma:
endif

ifeq ($(CONFIG_COMPRESSION_NRV2B),y)
nrv2b: $(obj)/util/nrv2b/nrv2b $(obj)/util/nrv2b/nrv2b-compress.o
else
nrv2b:
endif

$(obj)/lib/%.o: $(src)/lib/%.c
	$(Q)mkdir -p $(dir $@)
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

