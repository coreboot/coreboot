obj ?= $(shell pwd)

HOSTCXX ?= g++
HOSTCC ?= gcc

BINARY:=$(obj)/cbfstool

COMMON:=common.o compress.o minilzma.o
COMMON+=LZMAEncoder.o LZInWindow.o
COMMON+=RangeCoderBit.o StreamUtils.o
COMMON+=OutBuffer.o Alloc.o CRC.o
COMMON+=cbfs-mkstage.o cbfs-mkpayload.o cbfstool.o

COMMON:=$(addprefix $(obj)/,$(COMMON))

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

all: $(BINARY)

clean:
	rm -f $(COMMON) $(BINARY)

tags: 
	ctags *.[ch]

CXXFLAGS=-DCOMPACT -g
CFLAGS=-g
LDFLAGS=-g

$(obj)/cbfstool:$(COMMON)
	$(HOSTCXX) $(LDFLAGS) -o $@ $^
