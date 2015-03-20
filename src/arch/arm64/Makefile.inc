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

ifeq ($(CONFIG_ARCH_ARM64),y)
stages_c = $(src)/arch/arm64/stages.c
stages_o = $(obj)/arch/arm64/stages.o

$(stages_o): $(stages_c) $(obj)/config.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_arm) -I. $(CPPFLAGS_arm) -c -o $@ $< -marm

endif

################################################################################
# bootblock
################################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM64),y)

bootblock-y += div0.c
bootblock-y += id.S
$(obj)/arch/arm64/id.bootblock.o: $(obj)/build.h

bootblock-y += boot.c
bootblock-y += c_entry.c
bootblock-y += stage_entry.S
bootblock-y += stages.c
bootblock-y += eabi_compat.c
bootblock-y += transition.c transition_asm.S

bootblock-y += ../../lib/memset.c
bootblock-y += ../../lib/memcpy.c
bootblock-y += ../../lib/memmove.c

# Build the bootblock

$(objcbfs)/bootblock.debug: $(src)/arch/arm64/bootblock.ld $(obj)/ldoptions $$(bootblock-objs) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) --gc-sections -static -o $@ -L$(obj) --start-group $(bootblock-objs) --end-group -T $(src)/arch/arm64/bootblock.ld

endif # CONFIG_ARCH_BOOTBLOCK_ARM64

################################################################################
# romstage
################################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)

romstage-y += boot.c
romstage-y += c_entry.c
romstage-y += stage_entry.S
romstage-y += stages.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-y += ../../lib/memset.c
romstage-y += ../../lib/memcpy.c
romstage-y += ../../lib/memmove.c
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
romstage-y += transition.c transition_asm.S

rmodules_arm64-y += ../../lib/memset.c
rmodules_arm64-y += ../../lib/memcpy.c
rmodules_arm64-y += ../../lib/memmove.c
rmodules_arm64-y += eabi_compat.c

# Build the romstage
VBOOT_STUB_DEPS += $(obj)/arch/arm/eabi_compat.rmodules_arm64.o

$(objcbfs)/romstage.debug: $$(romstage-objs) $(src)/arch/arm64/romstage.ld $(obj)/ldoptions
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) -nostdlib --gc-sections -static -o $@ -L$(obj) --start-group $(romstage-objs) --end-group -T $(src)/arch/arm64/romstage.ld

endif # CONFIG_ARCH_ROMSTAGE_ARM64

################################################################################
# ramstage
################################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM64),y)

ramstage-y += c_entry.c
ramstage-y += stages.c
ramstage-y += div0.c
ramstage-y += cpu.c
ramstage-y += cpu_ramstage.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += ../../lib/memset.c
ramstage-y += ../../lib/memcpy.c
ramstage-y += ../../lib/memmove.c
ramstage-y += stage_entry.S
ramstage-$(CONFIG_ARCH_SPINTABLE) += spintable.c spintable_asm.S
ramstage-y += transition.c transition_asm.S

rmodules_arm64-y += ../../lib/memset.c
rmodules_arm64-y += ../../lib/memcpy.c
rmodules_arm64-y += ../../lib/memmove.c
rmodules_arm64-y += eabi_compat.c

secmon-$(CONFIG_ARCH_USE_SECURE_MONITOR) += stage_entry.S
secmon-$(CONFIG_ARCH_USE_SECURE_MONITOR) += ../../lib/memset.c
secmon-$(CONFIG_ARCH_USE_SECURE_MONITOR) += ../../lib/memcmp.c
secmon-$(CONFIG_ARCH_USE_SECURE_MONITOR) += ../../lib/memcpy.c

ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

# Build the ramstage

$(objcbfs)/ramstage.debug: $$(ramstage-objs) $(src)/arch/arm64/ramstage.ld $(obj)/ldoptions
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) -nostdlib --gc-sections -o $@ -L$(obj) --start-group $(ramstage-objs) --end-group -T $(src)/arch/arm64/ramstage.ld

$(objgenerated)/ramstage.o: $(stages_o) $$(ramstage-objs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) -nostdlib --gc-sections -r -o $@ --start-group $(ramstage-objs) --end-group

endif # CONFIG_ARCH_RAMSTAGE_ARM64
