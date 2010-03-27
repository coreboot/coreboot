cbfsobj :=
cbfsobj += common.o
cbfsobj += compress.o
cbfsobj += minilzma.o
cbfsobj += LZMAEncoder.o
cbfsobj += LZInWindow.o
cbfsobj += RangeCoderBit.o
cbfsobj += StreamUtils.o
cbfsobj += OutBuffer.o
cbfsobj += Alloc.o
cbfsobj += CRC.o
cbfsobj += cbfs-mkstage.o
cbfsobj += cbfs-mkpayload.o
cbfsobj += cbfstool.o

CBFSTOOLFLAGS=-DCOMPACT -g

$(obj)/util/cbfstool:
	mkdir -p $@

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(CBFSTOOLFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/lzma/%.cc
	printf "    HOSTCXX    $(subst $(obj)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Compress/LZMA/%.cpp
	printf "    HOSTCXX    $(subst $(obj)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Compress/LZ/%.cpp
	printf "    HOSTCXX    $(subst $(obj)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Compress/RangeCoder/%.cpp
	printf "    HOSTCXX    $(subst $(obj)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Common/%.cpp
	printf "    HOSTCXX    $(subst $(obj)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/Common/%.cpp
	printf "    HOSTCXX    $(subst $(obj)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/cbfstool: $(obj)/util/cbfstool $(addprefix $(obj)/util/cbfstool/,$(cbfsobj))
	printf "    HOSTCXX    $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) -o $@ $(addprefix $(obj)/util/cbfstool/,$(cbfsobj))

