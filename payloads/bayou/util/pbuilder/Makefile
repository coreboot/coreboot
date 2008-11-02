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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

CC=gcc
PBUILDOBJS := config.o create.o show.o main.o
INCLUDES := -Iliblar/
CFLAGS=-Wall -Werror -g

all: pbuilder

LZMA_OBJ := lzma/LZMAEncoder.o lzma/LZInWindow.o lzma/RangeCoderBit.o
LZMA_OBJ += lzma/StreamUtils.o lzma/OutBuffer.o lzma/Alloc.o lzma/CRC.o
LZMA_OBJ += lzma/lzma-compress.o

lzma/lzma-compress.o: lzma/minilzma.cc
	g++ -o $@ -c -DCOMPACT $<

lzma/%.o: lzma/C/7zip/Compress/LZMA/%.cpp
	g++ -o $@ -c $<

lzma/%.o: lzma/C/7zip/Compress/LZ/%.cpp
	g++ -o $@ -c $<

lzma/%.o: lzma/C/7zip/Compress/RangeCoder/%.cpp
	g++ -o $@ -c $<

lzma/%.o: lzma/C/7zip/Decompress/%.cpp
	g++ -o $@ -c $<

lzma/%.o: lzma/C/7zip/Common/%.cpp
	g++ -o $@ -c $<

lzma/%.o: lzma/C/Common/%.cpp
	g++ -o $@ -c $<

lzma/%.o: lzma/%.cc
	g++ -o $@ -c $<

pbuilder: $(PBUILDOBJS) $(LZMA_OBJ) liblar/liblar.a
	$(CXX) -o $@ $(PBUILDOBJS) $(LZMA_OBJ) -L ./liblar -llar -lexpat

liblar/liblar.a:
	make -C liblar

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -f pbuilder *.o lzma/*.o
	make -C liblar clean

