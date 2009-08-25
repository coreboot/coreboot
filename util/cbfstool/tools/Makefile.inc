
tobj ?= $(obj)/util/cbfstool/tools
tsrc ?= $(top)/util/cbfstool/tools

include $(tsrc)/lzma/Makefile.inc

COMMON = common.o compress.o $(LZMA_OBJ)

ifdef POST_EVALUATION

$(tobj)/cbfs-mkstage: $(tobj)/cbfs-mkstage.o $(patsubst %,$(tobj)/%,$(COMMON))
	printf "    HOSTCXX    $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ $^

$(tobj)/cbfs-mkpayload: $(tobj)/cbfs-mkpayload.o $(patsubst %,$(tobj)/%,$(COMMON))
	printf "    HOSTCXX    $(subst $(obj)/,,$(@)) (link)\n"
	$(HOSTCXX) $(HOSTCFLAGS) -o $@ $^

$(tobj)/%.o: $(tsrc)/%.c
	printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<

endif
