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

$(objutil)/cbfstool:
	mkdir -p $@

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(CBFSTOOLFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/%.cc
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Compress/LZMA/%.cpp
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Compress/LZ/%.cpp
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Compress/RangeCoder/%.cpp
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/7zip/Common/%.cpp
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/Common/%.cpp
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(objutil)/cbfstool/cbfstool: $(objutil)/cbfstool $(addprefix $(objutil)/cbfstool/,$(cbfsobj))
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(cbfsobj))

