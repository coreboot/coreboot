obj ?= $(shell pwd)

HOSTCXX  ?= g++
HOSTCC   ?= gcc
CFLAGS   ?= -g -Wall
CXXFLAGS +=-DCOMPACT $(CFLAGS)
LDFLAGS  ?= -g

BINARY:=$(obj)/cbfstool

COMMON:=common.o compress.o minilzma.o
COMMON+=LZMAEncoder.o LZInWindow.o
COMMON+=RangeCoderBit.o StreamUtils.o
COMMON+=OutBuffer.o Alloc.o CRC.o
COMMON+=cbfs-mkstage.o cbfs-mkpayload.o cbfstool.o

COMMON:=$(addprefix $(obj)/,$(COMMON))

all: dep $(BINARY)

$(obj)/%.o: %.c
	$(HOSTCC) $(CFLAGS) -c -o $@ $<

$(obj)/%.o: lzma/%.cc
	$(HOSTCXX) $(CXXFLAGS) -c -o $@ $<

$(obj)/%.o: lzma/C/7zip/Compress/LZMA/%.cpp
	$(HOSTCXX) $(CXXFLAGS) -c -o $@ $<

$(obj)/%.o: lzma/C/7zip/Compress/LZ/%.cpp
	$(HOSTCXX) $(CXXFLAGS) -c -o $@ $<

$(obj)/%.o: lzma/C/7zip/Compress/RangeCoder/%.cpp
	$(HOSTCXX) $(CXXFLAGS) -c -o $@ $<

$(obj)/%.o: lzma/C/7zip/Common/%.cpp
	$(HOSTCXX) $(CXXFLAGS) -c -o $@ $<

$(obj)/%.o: lzma/C/Common/%.cpp
	$(HOSTCXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(COMMON) $(BINARY)

tags:
	ctags *.[ch]

$(obj)/cbfstool:$(COMMON)
	$(HOSTCXX) $(LDFLAGS) -o $@ $^

dep:
	@$(HOSTCC) $(CFLAGS) -MM *.c > .dependencies
	@$(HOSTCC) $(CFLAGS) -MM lzma/C/7zip/Decompress/*.c >> .dependencies
	@$(HOSTCXX) $(CXXFLAGS) -MM lzma/C/7zip/Compress/*/*.cpp >> .dependencies
	@$(HOSTCXX) $(CXXFLAGS) -MM lzma/C/7zip/Common/*.cpp >> .dependencies
	@$(HOSTCXX) $(CXXFLAGS) -MM lzma/C/Common/*.cpp >> .dependencies
	@$(HOSTCXX) $(CXXFLAGS) -MM lzma/*.cc >> .dependencies

-include .dependencies
