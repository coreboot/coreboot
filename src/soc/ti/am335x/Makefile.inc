## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_TI_AM335X),y)
bootblock-y	+= bootblock.c
bootblock-y	+= timer.c
bootblock-y	+= gpio.c
bootblock-y	+= pinmux.c
bootblock-y	+= mmc.c

romstage-y	+= cbmem.c
romstage-y	+= timer.c
romstage-y	+= mmc.c
romstage-y	+= sdram.c

ramstage-y	+= timer.c
ramstage-y	+= soc.c
ramstage-y	+= mmc.c

bootblock-y	+= uart.c
romstage-y	+= uart.c
ramstage-y	+= uart.c

$(call add-class,omap-header)
$(eval $(call create_class_compiler,omap-header,arm))

omap-header-generic-ccopts += -D__COREBOOT_ARM_ARCH__=7

real-target: $(obj)/MLO

header_ld := $(call src-to-obj,omap-header,$(dir)/header.ld)

get_header_size= \
	$(shell echo $$(wc -c < $(objcbfs)/bootblock.bin))

$(obj)/omap-header.bin: $$(omap-header-objs) $(objcbfs)/bootblock.bin
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_omap-header) -nostdlib -nostartfiles -static -include $(obj)/config.h \
		-Wl,--defsym,header_load_size=$(strip \
			$(call get_header_size,$(obj)/coreboot.rom) \
		) \
		-o $@.tmp $< -T $(header_ld)
	$(OBJCOPY_omap-header) --only-section=".header" -O binary $@.tmp $@

$(obj)/MLO: $(obj)/coreboot.rom $(obj)/omap-header.bin
	@printf "    HEADER     $(subst $(obj)/,,$(@))\n"
	$(Q)cat $(obj)/omap-header.bin $(obj)/coreboot.rom > $@

omap-header-y	+= header.c

omap-header-srcs += $(CONFIG_MEMLAYOUT_LD_FILE)
omap-header-y += header.ld
endif
