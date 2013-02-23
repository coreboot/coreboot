##
## This file is part of the coreboot project.
##
## Copyright (C) 2007 coresystems GmbH
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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

LZMA_OBJ := $(obj)/util/lzma/LZMAEncoder.o $(obj)/util/lzma/LZInWindow.o
LZMA_OBJ += $(obj)/util/lzma/RangeCoderBit.o $(obj)/util/lzma/StreamUtils.o
LZMA_OBJ += $(obj)/util/lzma/OutBuffer.o $(obj)/util/lzma/Alloc.o
LZMA_OBJ += $(obj)/util/lzma/CRC.o

$(obj)/util/lzma/lzma: $(obj)/util/lzma/ $(LZMA_OBJ) $(obj)/util/lzma/minilzma.o
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ $(LZMA_OBJ) $(obj)/util/lzma/minilzma.o

$(obj)/util/lzma/lzma-compress.o: $(src)/util/lzma/minilzma.cc
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c -DCOMPACT $<

$(obj)/util/lzma/:
	$(Q)printf "  BUILD   LZMA\n"
	$(Q)mkdir -p $(obj)/util/lzma/

$(obj)/util/lzma/%.o: $(src)/util/lzma/C/7zip/Compress/LZMA/%.cpp
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c $<

$(obj)/util/lzma/%.o: $(src)/util/lzma/C/7zip/Compress/LZ/%.cpp
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c $<

$(obj)/util/lzma/%.o: $(src)/util/lzma/C/7zip/Compress/RangeCoder/%.cpp
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c $<

$(obj)/util/lzma/%.o: $(src)/util/lzma/C/7zip/Decompress/%.cpp
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c $<

$(obj)/util/lzma/%.o: $(src)/util/lzma/C/7zip/Common/%.cpp
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c $<

$(obj)/util/lzma/%.o: $(src)/util/lzma/C/Common/%.cpp
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c $<

$(obj)/util/lzma/%.o: $(src)/util/lzma/%.cc
	$(Q)printf "  HOSTCXX $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ -c $<

