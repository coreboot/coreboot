cbfsobj :=
cbfsobj += cbfstool.o
cbfsobj += common.o
cbfsobj += compress.o
cbfsobj += cbfs_image.o
cbfsobj += cbfs-mkstage.o
cbfsobj += cbfs-mkpayload.o
# LZMA
cbfsobj += lzma.o
cbfsobj += LzFind.o
cbfsobj += LzmaDec.o
cbfsobj += LzmaEnc.o

CBFSTOOLFLAGS=-D_7ZIP_ST -g

ifeq ($(shell uname -s | cut -c-7 2>/dev/null), MINGW32)
CBFSTOOLFLAGS+=-mno-ms-bitfields
endif

$(objutil)/cbfstool:
	mkdir -p $@

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(CBFSTOOLFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/%.cc
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@))\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) $(HOSTCXXFLAGS) -c -o $@ $<

$(objutil)/cbfstool/%.o: $(top)/util/cbfstool/lzma/C/%.c
	printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) $(CBFSTOOLFLAGS) $(HOSTCFLAGS) -c -o $@ $<

$(objutil)/cbfstool/cbfstool: $(objutil)/cbfstool $(addprefix $(objutil)/cbfstool/,$(cbfsobj))
	printf "    HOSTCXX    $(subst $(objutil)/,,$(@)) (link)\n"
	$(HOSTCXX) $(CBFSTOOLFLAGS) -o $@ $(addprefix $(objutil)/cbfstool/,$(cbfsobj))

