##
## This file is part of the LinuxBIOS project.
##
## Copyright (C) 2006-2007 coresystems GmbH
## Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
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

COMPRESSORS :=
ifdef $(CONFIG_COMPRESSION_LZMA)
COMPRESSORS += lzma.o
endif
ifdef $(CONFIG_COMPRESSION_NRV2B)
COMPRESSORS += nrv2b.o
endif

#
# This part takes care of compression. It should build the compression modules
# in a way that they can be put in the lar.
#

lzma:
	$(Q)echo "Building lzma... skipped"

$(obj)/%.o: $(src)/lib/%.c
	$(Q)$(CC) $(INITCFLAGS) -c $< -o $@

