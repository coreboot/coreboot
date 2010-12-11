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

$(obj)/crt0.S: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	printf '$(foreach crt0,config.h $(crt0s),#include "$(crt0:$(obj)/%=%)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.romstage.o: $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.s: $(obj)/crt0.S
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -x assembler-with-cpp -DASSEMBLY -E -I$(src)/include -I$(src)/arch/x86/include -I$(obj) -include $(obj)/config.h -I. -I$(src) $< -o $@

$(obj)/coreboot: $$(romstage-objs) $(obj)/ldscript.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(obj)/ldscript.ld $(romstage-objs)
	$(NM) -n $(obj)/coreboot | sort > $(obj)/coreboot.map
	$(OBJCOPY) --only-keep-debug $@ $(obj)/bootblock.debug
	$(OBJCOPY) --strip-debug $@
	$(OBJCOPY) --add-gnu-debuglink=$(obj)/bootblock.debug $@

