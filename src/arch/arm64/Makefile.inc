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
## Foundation, Inc.
##
################################################################################

################################################################################
# Take care of subdirectories
################################################################################

subdirs-y += armv8/ cpu/

################################################################################
# ARM specific options
################################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)
CBFSTOOL_PRE1_OPTS = -m arm64 -s $(CONFIG_CBFS_SIZE)
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
bootblock-y += cpu-stubs.c
bootblock-y += stages.c
bootblock-y += eabi_compat.c
bootblock-y += transition.c transition_asm.S
bootblock-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

bootblock-y += ../../lib/memset.c
bootblock-y += ../../lib/memcpy.c
bootblock-y += ../../lib/memmove.c

# Build the bootblock

$(objcbfs)/bootblock.debug: $$(bootblock-objs) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) --gc-sections -static -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(bootblock-objs)) --end-group -T $(obj)/mainboard/$(MAINBOARDDIR)/memlayout.bootblock.ld

endif # CONFIG_ARCH_BOOTBLOCK_ARM64

###############################################################################
# verification stage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM64),y)

$(objcbfs)/verstage.debug: $(objgenerated)/libverstage.a $$(verstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_verstage) --gc-sections -static -o $@ -L$(obj) --whole-archive --start-group $(objgenerated)/libverstage.a $$(verstage-objs) --end-group -T $(obj)/mainboard/$(MAINBOARDDIR)/memlayout.verstage.ld

verstage-$(CONFIG_EARLY_CONSOLE) += early_console.c
verstage-y += boot.c
verstage-y += div0.c
verstage-y += eabi_compat.c
verstage-y += ../../lib/memset.c
verstage-y += ../../lib/memcpy.c
verstage-y += ../../lib/memmove.c
verstage-y += stages.c
verstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

endif # CONFIG_ARCH_VERSTAGE_ARM64

################################################################################
# romstage
################################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)

romstage-y += boot.c
romstage-y += c_entry.c
romstage-y += stage_entry.S
romstage-y += cpu-stubs.c
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

$(objcbfs)/romstage.debug: $$(romstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) -nostdlib --gc-sections -static -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) --end-group -T $(obj)/mainboard/$(MAINBOARDDIR)/memlayout.romstage.ld

endif # CONFIG_ARCH_ROMSTAGE_ARM64

################################################################################
# ramstage
################################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM64),y)

ramstage-y += c_entry.c
ramstage-y += stages.c
ramstage-y += startup.c
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
ramstage-y += cpu-stubs.c
ramstage-$(CONFIG_ARM64_USE_SPINTABLE) += spintable.c spintable_asm.S
ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += arm_tf.c
ramstage-y += transition.c transition_asm.S

rmodules_arm64-y += ../../lib/memset.c
rmodules_arm64-y += ../../lib/memcpy.c
rmodules_arm64-y += ../../lib/memmove.c
rmodules_arm64-y += eabi_compat.c

secmon-y += stage_entry.S
secmon-y += cpu-stubs.c
secmon-y += startup.c
secmon-y += ../../lib/malloc.c
secmon-y += ../../lib/memset.c
secmon-y += ../../lib/memcmp.c
secmon-y += ../../lib/memcpy.c

ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c

ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

# Build the ramstage

$(objcbfs)/ramstage.debug: $$(ramstage-objs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) -nostdlib --gc-sections -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) --end-group -T $(obj)/mainboard/$(MAINBOARDDIR)/memlayout.ramstage.ld

# Build ARM Trusted Firmware (BL31)

ifeq ($(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE),y)

BL31_SOURCE := $(top)/3rdparty/arm-trusted-firmware

BL31_MAKEARGS := PLAT=$(call strip_quotes,$(CONFIG_ARM_TF_PLATFORM_NAME))

ifeq ($(V),1)
BL31_MAKEARGS += V=1
endif

# Build ARM TF in debug mode (with serial output) if coreboot uses serial
ifeq ($(CONFIG_CONSOLE_SERIAL),y)
BL31_MAKEARGS += DEBUG=1
endif # CONFIG_CONSOLE_SERIAL

# Avoid build/release|build/debug distinction by overriding BUILD_PLAT directly
BL31_MAKEARGS += BUILD_PLAT="$(top)/$(obj)/3rdparty/arm-trusted-firmware"

BL31_CFLAGS := -fno-pic -fno-stack-protector
BL31_LDFLAGS := --emit-relocs

BL31 := $(obj)/3rdparty/arm-trusted-firmware/bl31/bl31.elf

$(BL31):
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	CROSS_COMPILE="$(CROSS_COMPILE_arm64)" \
	CFLAGS="$(BL31_CFLAGS)" \
	LDFLAGS="$(BL31_LDFLAGS)" \
	$(MAKE) -C $(BL31_SOURCE) $(BL31_MAKEARGS) bl31

.PHONY: $(BL31)

BL31_CBFS := $(call strip_quotes,$(CONFIG_CBFS_PREFIX))/bl31
$(BL31_CBFS)-file := $(BL31)
$(BL31_CBFS)-type := stage
$(BL31_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(BL31_CBFS)

endif # CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE

endif # CONFIG_ARCH_RAMSTAGE_ARM64
