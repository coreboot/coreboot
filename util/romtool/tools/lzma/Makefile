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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

LZMA_OBJ := lzma/LZMAEncoder.o lzma/LZInWindow.o
LZMA_OBJ += lzma/RangeCoderBit.o lzma/StreamUtils.o
LZMA_OBJ += lzma/OutBuffer.o lzma/Alloc.o
LZMA_OBJ += lzma/CRC.o
LZMA_OBJ += lzma/lzma-compress.o

$(obj)/lzma/lzma-compress.o: lzma/minilzma.cc
	g++ -o $@ -c -DCOMPACT $<

$(obj)/lzma/%.o: lzma/C/7zip/Compress/LZMA/%.cpp
	g++ -o $@ -c $<

$(obj)/lzma/%.o: lzma/C/7zip/Compress/LZ/%.cpp
	g++ -o $@ -c $<

$(obj)/lzma/%.o: lzma/C/7zip/Compress/RangeCoder/%.cpp
	g++ -o $@ -c $<

$(obj)/lzma/%.o: lzma/C/7zip/Decompress/%.cpp
	g++ -o $@ -c $<

$(obj)/lzma/%.o: lzma/C/7zip/Common/%.cpp
	g++ -o $@ -c $<

$(obj)/lzma/%.o: lzma/C/Common/%.cpp
	g++ -o $@ -c $<

$(obj)/lzma/%.o: lzma/%.cc
	g++ -o $@ -c $<

