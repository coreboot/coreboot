##
## This file is part of the bayou project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License version 2 as
## published by the Free Software Foundation.
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

PBUILDOBJS := config.o create.o show.o main.o
INCLUDES := -Iliblar/
HOSTCXX=g++
HOSTCC=gcc
HOSTCFLAGS=-Wall -Werror -g

all: pbuilder

LZMA_OBJ := lzma/LZMAEncoder.o lzma/LZInWindow.o lzma/RangeCoderBit.o
LZMA_OBJ += lzma/StreamUtils.o lzma/OutBuffer.o lzma/Alloc.o lzma/CRC.o
LZMA_OBJ += lzma/lzma-compress.o

lzma/lzma-compress.o: lzma/minilzma.cc
	$(HOSTCXX) -o $@ -c -DCOMPACT $<

lzma/%.o: lzma/C/7zip/Compress/LZMA/%.cpp
	$(HOSTCXX) -o $@ -c $<

lzma/%.o: lzma/C/7zip/Compress/LZ/%.cpp
	$(HOSTCXX) -o $@ -c $<

lzma/%.o: lzma/C/7zip/Compress/RangeCoder/%.cpp
	$(HOSTCXX) -o $@ -c $<

lzma/%.o: lzma/C/7zip/Decompress/%.cpp
	$(HOSTCXX) -o $@ -c $<

lzma/%.o: lzma/C/7zip/Common/%.cpp
	$(HOSTCXX) -o $@ -c $<

lzma/%.o: lzma/C/Common/%.cpp
	$(HOSTCXX) -o $@ -c $<

lzma/%.o: lzma/%.cc
	$(HOSTCXX) -o $@ -c $<

pbuilder: $(PBUILDOBJS) $(LZMA_OBJ) liblar/liblar.a
	$(HOSTCXX) -o $@ $(PBUILDOBJS) $(LZMA_OBJ) liblar/liblar.a -lexpat

liblar/liblar.a:
	$(MAKE) -C liblar

%.o: %.c
	$(HOSTCC) -c $(HOSTCFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -f pbuilder *.o lzma/*.o
	$(MAKE) -C liblar clean

