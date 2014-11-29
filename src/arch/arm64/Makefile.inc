################################################################################
##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc.
## Copyright (C) 2012-2013 The ChromiumOS Authors
## Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
## Copyright (C) 2009-2010 coresystems GmbH
## Copyright (C) 2009 Ronald G. Minnich
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##
################################################################################

################################################################################
# Take care of subdirectories
################################################################################

subdirs-y += armv8/

################################################################################
# ARM specific options
################################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)
CBFSTOOL_PRE1_OPTS = -m arm64 -b $(CONFIG_BOOTBLOCK_ROM_OFFSET) -H $(CONFIG_CBFS_HEADER_ROM_OFFSET) -o $(CONFIG_CBFS_ROM_OFFSET)
endif

################################################################################
# bootblock
################################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM64),y)

bootblock-y += div0.c
bootblock-$(CONFIG_BOOTBLOCK_CONSOLE) += early_console.c

bootblock-y += id.S
$(obj)/arch/arm64/id.bootblock.o: $(obj)/build.h

bootblock-y += stages.c
bootblock-y += eabi_compat.c

bootblock-y += memset.S
bootblock-y += memcpy.S
bootblock-y += memmove.S

# Build the bootblock

$(objcbfs)/bootblock.debug: $(src)/arch/arm64/bootblock.ld $(obj)/ldoptions $$(bootblock-objs) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) -nostdlib --gc-sections -static -o $@ -L$(obj) --start-group $(bootblock-objs) --end-group -T $(src)/arch/arm64/bootblock.ld

endif # CONFIG_ARCH_BOOTBLOCK_ARM64

################################################################################
# romstage
################################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)

romstage-y += stages.c
romstage-y += div0.c
romstage-y += early_console.c
romstage-y += eabi_compat.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

# Build the romstage

$(objcbfs)/romstage.debug: $$(romstage-objs) $(src)/arch/arm64/romstage.ld $(obj)/ldoptions
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) -nostdlib --gc-sections -static -o $@ -L$(obj) --start-group $(romstage-objs) --end-group -T $(src)/arch/arm64/romstage.ld

endif # CONFIG_ARCH_ROMSTAGE_ARM64

################################################################################
# ramstage
################################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM64),y)

ramstage-y += stages.c
ramstage-y += div0.c
ramstage-y += cpu.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += memset.S
ramstage-y += memcpy.S
ramstage-y += memmove.S

rmodules_$(ARCH-ramstage-y) += memset.S
rmodules_$(ARCH-ramstage-y) += memcpy.S
rmodules_$(ARCH-ramstage-y) += memmove.S
rmodules_$(ARCH-ramstage-y) += eabi_compat.c

VBOOT_STUB_DEPS += $(obj)/arch/arm64/eabi_compat.rmodules.o

$(eval $(call create_class_compiler,rmodules,arm64))

ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/mainboard.c

# Build the ramstage

$(objcbfs)/ramstage.debug: $$(ramstage-objs) $(src)/arch/arm64/ramstage.ld $(obj)/ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) -nostdlib --gc-sections -o $@ --start-group $(ramstage-objs) --end-group -T $(src)/arch/arm64/ramstage.ld

endif # CONFIG_ARCH_RAMSTAGE_ARM64
