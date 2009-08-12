
tobj ?= $(obj)/util/cbfstool/tools
tsrc ?= $(top)/util/cbfstool/tools

include $(tsrc)/lzma/Makefile.inc

COMMON = common.o compress.o $(LZMA_OBJ)

ifdef POST_EVALUATION

$(tobj)/cbfs-mkstage: $(tobj)/cbfs-mkstage.o $(patsubst %,$(tobj)/%,$(COMMON))
	$(Q)printf "    HOSTCXX    $(subst $(obj)/,,$(@)) (link)\n"
	$(Q)$(HOSTCXX) $(HOSTCXXFLAGS) -o $@ $^

$(tobj)/cbfs-mkpayload: $(tobj)/cbfs-mkpayload.o $(patsubst %,$(tobj)/%,$(COMMON))
	$(Q)printf "    HOSTCXX    $(subst $(obj)/,,$(@)) (link)\n"
	$(Q)$(HOSTCXX) $(HOSTCFLAGS) -o $@ $^

$(tobj)/%.o: $(tsrc)/%.c
	$(Q)printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<

endif
