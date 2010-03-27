#######################################################################
# Build the final rom image

$(obj)/coreboot.pre: $(obj)/coreboot.bootblock $(CBFSTOOL)
	rm -f $@
	$(CBFSTOOL) $@ create $(CONFIG_COREBOOT_ROMSIZE_KB)K $(obj)/coreboot.bootblock

#######################################################################
# Build the bootblock

$(obj)/coreboot.bootblock: $(obj)/coreboot
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY) -O binary $< $@

$(obj)/ldscript.ld: $$(ldscripts) $(obj)/ldoptions
	printf 'INCLUDE "ldoptions"\n' > $@
	printf '$(foreach ldscript,$(ldscripts),INCLUDE "$(ldscript:$(obj)/%=%)"\n)' >> $@

$(obj)/crt0_includes.h: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,config.h $(crt0s),#include "$(crt0:$(obj)/%=%)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.initobj.o: $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.s: $(src)/arch/i386/init/crt0.S.lb $(obj)/crt0_includes.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -x assembler-with-cpp -DASSEMBLY -E -I$(src)/include -I$(src)/arch/i386/include -I$(obj) -include $(obj)/config.h -I. -I$(src) $< > $@.new && mv $@.new $@

$(obj)/coreboot: $$(initobjs) $(obj)/ldscript.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(obj)/ldscript.ld $(initobjs)
	$(NM) -n $(obj)/coreboot | sort > $(obj)/coreboot.map

