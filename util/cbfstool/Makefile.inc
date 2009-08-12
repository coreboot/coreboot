#
#
#

ifdef POST_EVALUATION

cbfsobj :=
# commands
cbfsobj += create.o
cbfsobj += bootblock.o
cbfsobj += delete.o
cbfsobj += extract.o 
cbfsobj += add.o 
cbfsobj += print.o
cbfsobj += resize.o
# main tool
cbfsobj += cbfstool.o
cbfsobj += util.o
cbfsobj += fs.o

cbfsinc := cbfstool.h cbfs.h

$(obj)/util/cbfstool:
	$(Q)mkdir -p $@
	$(Q)mkdir -p $@/tools/lzma

$(obj)/util/cbfstool/%.o: $(top)/util/cbfstool/%.c
	$(Q)printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<

$(obj)/util/cbfstool/cbfstool: $(obj)/util/cbfstool $(obj)/util/cbfstool/tools/cbfs-mkpayload $(obj)/util/cbfstool/tools/cbfs-mkstage $(addprefix $(obj)/util/cbfstool/,$(cbfsobj))
	$(Q)printf "    HOSTCC     $(subst $(obj)/,,$(@)) (link)\n"
	$(Q)$(HOSTCC) -o $@ $(addprefix $(obj)/util/cbfstool/,$(cbfsobj))

endif

include $(top)/util/cbfstool/tools/Makefile.inc


