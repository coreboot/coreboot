##
## This file is part of the coreboot project.
##
## Copyright 2017-present Facebook, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

ifeq ($(CONFIG_SOC_CAVIUM_COMMON),y)

CFLAGS_arm64   += -Wstack-usage=$(CONFIG_STACK_SIZE)

bootblock-$(CONFIG_BOOTBLOCK_CUSTOM) += bootblock.c

################################################################################
# romstage

romstage-y += cbmem.c
romstage-y += bdk-coreboot.c

################################################################################
# ramstage

ramstage-y += cbmem.c

CPPFLAGS_common += -Isrc/soc/cavium/common/include

ROM_HEADER_BIN := $(objgenerated)/rom_header.bin
ROM_HEADER_SOURCES += rom_clib_s_nbl1fw
ROM_HEADER_SOURCES += rom_csib_s_nbl1fw

ROM_HEADER_DEPS := $(foreach f, $(ROM_HEADER_SOURCES), src/soc/cavium/common/$(f).bin.hex)

# Include ROM header
$(ROM_HEADER_BIN): $(ROM_HEADER_DEPS)
	for f in $+; \
		do for c in $$(cat $$f | grep -v ^#); \
			do printf $$(printf '\%o' 0x$$c); \
		done; \
	done > $@

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin $(ROM_HEADER_BIN)
	@printf "    GEN        $(subst $(obj)/,,$(@))\n"
	dd if=/dev/zero ibs=1 count=$$(($(shell stat --printf="%s" $(objcbfs)/bootblock.raw.bin) + 0x10000)) of=$@ status=none
	# Insert CLIB at 0x0 and CSIB at 0x100
	dd if=$(ROM_HEADER_BIN) of=$@ bs=1 seek=0 conv=notrunc status=none
	# Insert bootblock at 0x10000
	dd if=$(objcbfs)/bootblock.raw.bin of=$@ bs=1 seek=$$((0x10000)) conv=notrunc status=none

endif
