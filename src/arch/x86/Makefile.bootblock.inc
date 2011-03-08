#######################################################################
# Build the final rom image

$(obj)/coreboot.pre: $(obj)/coreboot.romstage $(obj)/coreboot.pre1 $(CBFSTOOL)
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
	rm -f $@
	cp $(obj)/coreboot.pre1 $@
	$(CBFSTOOL) $@ add-stage $(obj)/romstage.elf $(CONFIG_CBFS_PREFIX)/romstage x 0x$(shell cat $(obj)/location.txt)
#FIXME: location.txt might require an offset of header size

#######################################################################
# Build the bootblock

$(obj)/coreboot.bootblock: $(obj)/bootblock.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY) -O binary $< $@

bootblock_lds = $(src)/arch/x86/init/ldscript_failover.lb
bootblock_lds += $(src)/cpu/x86/16bit/entry16.lds
bootblock_lds += $(src)/cpu/x86/16bit/reset16.lds
bootblock_lds += $(src)/arch/x86/lib/id.lds
bootblock_lds += $(chipset_bootblock_lds)

bootblock_inc = $(src)/arch/x86/init/prologue.inc
bootblock_inc += $(src)/cpu/x86/16bit/entry16.inc
bootblock_inc += $(src)/cpu/x86/16bit/reset16.inc
bootblock_inc += $(src)/cpu/x86/32bit/entry32.inc
bootblock_inc += $(src)/arch/x86/lib/id.inc
bootblock_inc += $(chipset_bootblock_inc)

ifeq ($(CONFIG_SSE),y)
bootblock_inc += $(src)/cpu/x86/sse_enable.inc
endif
bootblock_inc += $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.inc
bootblock_inc += $(src)/arch/x86/lib/walkcbfs.S

bootblock_romccflags := -mcpu=i386 -O2 -D__PRE_RAM__
ifeq ($(CONFIG_SSE),y)
bootblock_romccflags := -mcpu=k7 -msse -O2 -D__PRE_RAM__
endif

$(obj)/bootblock/ldscript.ld: $$(bootblock_lds) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	mkdir -p $(obj)/bootblock
	printf '$(foreach ldscript,ldoptions $(bootblock_lds),INCLUDE "$(ldscript)"\n)' > $@

$(obj)/bootblock/bootblock.S: $$(bootblock_inc)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	mkdir -p $(obj)/bootblock
	printf '$(foreach crt0,config.h $(bootblock_inc),#include "$(crt0)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/bootblock.o: $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/bootblock.s: $(obj)/bootblock/bootblock.S
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -DASSEMBLY -E -I$(src)/include -I$(src)/arch/x86/include -I$(obj) -I$(obj)/bootblock -include $(obj)/config.h -I. -I$(src) $< -o $@

$(obj)/mainboard/$(MAINBOARDDIR)/bootblock.inc: $(src)/arch/x86/init/$(subst ",,$(CONFIG_BOOTBLOCK_SOURCE)) $(objutil)/romcc/romcc $(OPTION_TABLE_H)
	@printf "    ROMCC      $(subst $(obj)/,,$(@))\n"
	$(CC) -MM -MT$(obj)/mainboard/$(MAINBOARDDIR)/bootblock.inc \
		$< > $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.inc.d
	$(ROMCC) -c -S $(bootblock_romccflags) $(ROMCCFLAGS) -I. $(INCLUDES) $< -o $@

$(obj)/bootblock.elf: $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.o $(obj)/bootblock/ldscript.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(CC) -nostdlib -nostartfiles -static -o $@ -L$(obj) -T $(obj)/bootblock/ldscript.ld $<
	$(NM) -n $(obj)/bootblock.elf | sort > $(obj)/bootblock.map
	$(OBJCOPY) --only-keep-debug $@ $(obj)/bootblock.debug
	$(OBJCOPY) --strip-debug $@
	$(OBJCOPY) --add-gnu-debuglink=$(obj)/bootblock.debug $@

#######################################################################
# Build the romstage
$(obj)/coreboot.romstage: $(obj)/coreboot.pre1 $$(romstage-objs) $(obj)/romstage/ldscript.ld
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	printf "CONFIG_ROMBASE = 0x0;\nAUTO_XIP_ROM_BASE = 0x0;\n" > $(obj)/location.ld
	$(CC) -nostdlib -nostartfiles -static -o $(obj)/romstage.elf -L$(obj) -T $(obj)/romstage/ldscript.ld $(romstage-objs)
	$(OBJCOPY) -O binary $(obj)/romstage.elf $(obj)/romstage.bin
	printf "CONFIG_ROMBASE = 0x" > $(obj)/location.ld
	$(CBFSTOOL) $(obj)/coreboot.pre1 locate $(obj)/romstage.bin $(CONFIG_CBFS_PREFIX)/romstage $(CONFIG_XIP_ROM_SIZE) > $(obj)/location.txt
	cat $(obj)/location.txt >> $(obj)/location.ld
	printf ';\nAUTO_XIP_ROM_BASE = CONFIG_ROMBASE & ~(CONFIG_XIP_ROM_SIZE - 1);\n' >> $(obj)/location.ld
	$(CC) -nostdlib -nostartfiles -static -o $(obj)/romstage.elf -L$(obj) -T $(obj)/romstage/ldscript.ld $(romstage-objs)
	$(NM) -n $(obj)/romstage.elf | sort > $(obj)/romstage.map
	$(OBJCOPY) --only-keep-debug $(obj)/romstage.elf $(obj)/romstage.debug
	$(OBJCOPY) --strip-debug $(obj)/romstage.elf
	$(OBJCOPY) --add-gnu-debuglink=$(obj)/romstage.debug $(obj)/romstage.elf
	$(OBJCOPY) -O binary $(obj)/romstage.elf $@

$(obj)/romstage/ldscript.ld: $$(ldscripts) $(obj)/ldoptions
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	mkdir -p $(obj)/romstage
	printf '$(foreach ldscript,ldoptions location.ld $(ldscripts),INCLUDE "$(ldscript:$(obj)/%=%)"\n)' > $@

$(obj)/romstage/crt0.S: $$(crt0s)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	mkdir -p $(obj)/romstage
	printf '$(foreach crt0,config.h $(crt0s),#include "$(crt0:$(obj)/%=%)"\n)' > $@

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.romstage.o: $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -I$(obj) -Wa,-acdlns -c -o $@ $<  > $(dir $@)/crt0.disasm

$(obj)/mainboard/$(MAINBOARDDIR)/crt0.s: $(obj)/romstage/crt0.S
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -x assembler-with-cpp -DASSEMBLY -E -I$(src)/include -I$(src)/arch/x86/include -I$(obj) -I$(obj)/romstage -include $(obj)/config.h -I. -I$(src) $< -o $@

