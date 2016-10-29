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
################################################################################

################################################################################
# Take care of subdirectories
################################################################################

subdirs-y += armv8/

################################################################################
# ARM specific options
################################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM64),y)
check-ramstage-overlap-regions += postram_cbfs_cache stack ttb
endif

################################################################################
# bootblock
################################################################################

ifeq ($(CONFIG_ARCH_BOOTBLOCK_ARM64),y)

bootblock-y += div0.c
bootblock-y += id.S
$(call src-to-obj,bootblock,$(dir)/id.S): $(obj)/build.h

bootblock-y += boot.c
bootblock-y += eabi_compat.c
bootblock-y += transition.c transition_asm.S

bootblock-y += memset.S
bootblock-y += memcpy.S
bootblock-y += memmove.S

# Build the bootblock

$(objcbfs)/bootblock.debug: $$(bootblock-objs) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(bootblock-objs)) --end-group -T $(call src-to-obj,bootblock,src/mainboard/$(MAINBOARDDIR)/memlayout.ld)

endif # CONFIG_ARCH_BOOTBLOCK_ARM64

###############################################################################
# verification stage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM64),y)

$(objcbfs)/verstage.debug: $(objgenerated)/libverstage.a $$(verstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_verstage) $(LDFLAGS_verstage) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(verstage-objs)) $(objgenerated)/libverstage.a --end-group -T $(call src-to-obj,verstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld)

verstage-y += boot.c
verstage-y += div0.c
verstage-y += eabi_compat.c
verstage-y += ../../lib/memset.c
verstage-y += ../../lib/memcpy.c
verstage-y += ../../lib/memmove.c

verstage-y += transition.c transition_asm.S

endif # CONFIG_ARCH_VERSTAGE_ARM64

################################################################################
# romstage
################################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)

romstage-y += boot.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S
romstage-y += transition.c transition_asm.S

rmodules_arm64-y += memset.S
rmodules_arm64-y += memcpy.S
rmodules_arm64-y += memmove.S
rmodules_arm64-y += eabi_compat.c

$(objcbfs)/romstage.debug: $$(romstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) $(LDFLAGS_romstage) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) --end-group -T $(call src-to-obj,romstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld)

endif # CONFIG_ARCH_ROMSTAGE_ARM64

################################################################################
# ramstage
################################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM64),y)

ramstage-y += div0.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += memset.S
ramstage-y += memcpy.S
ramstage-y += memmove.S
ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += arm_tf.c
ramstage-y += transition.c transition_asm.S

rmodules_arm64-y += memset.S
rmodules_arm64-y += memcpy.S
rmodules_arm64-y += memmove.S
rmodules_arm64-y += eabi_compat.c

ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

# Build the ramstage

$(objcbfs)/ramstage.debug: $$(ramstage-objs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) $(LDFLAGS_ramstage) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) --end-group -T $(call src-to-obj,ramstage,src/mainboard/$(MAINBOARDDIR)/memlayout.ld)

# Build ARM Trusted Firmware (BL31)

ifeq ($(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE),y)

BL31_SOURCE := $(top)/3rdparty/arm-trusted-firmware
BL31_BUILD := $(abspath $(obj)/3rdparty/arm-trusted-firmware)
BL31_TARGET := $(BL31_BUILD)/bl31/bl31.elf

ifeq ($(V),1)
BL31_MAKEARGS += V=1
endif

# Build ARM TF in debug mode (with serial output) if coreboot uses serial
ifeq ($(CONFIG_CONSOLE_SERIAL),y)
BL31_MAKEARGS += DEBUG=1
else
# Turn off NOTICE messages from BL31 if coreboot does not use serial
BL31_MAKEARGS += LOG_LEVEL=0
endif # CONFIG_CONSOLE_SERIAL

# Avoid build/release|build/debug distinction by overriding BUILD_PLAT directly
BL31_MAKEARGS += BUILD_PLAT="$(BL31_BUILD)"

# Set a consistent build timestamp: the same coreboot has
# The \# \" complications exist to satisfy both gnu make's parser and editors
# with non-semantic quote-handling (that would assume that this line starts a
# multi line string.
BL31_MAKEARGS += BUILD_MESSAGE_TIMESTAMP='"$(shell grep "\#define COREBOOT_BUILD\>" $(obj)/build.h |cut -d\" -f2 \# \")"'

BL31_CFLAGS := -fno-pic -fno-stack-protector
BL31_LDFLAGS := --emit-relocs

BL31 := $(obj)/bl31.elf

$(BL31): $(obj)/build.h
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	CROSS_COMPILE="$(CROSS_COMPILE_arm64)" \
	CFLAGS="$(BL31_CFLAGS)" \
	LDFLAGS="$(BL31_LDFLAGS)" \
	$(MAKE) -C $(BL31_SOURCE) $(BL31_MAKEARGS) $(BL31_TARGET)
	mv $(BL31_TARGET) $@

.PHONY: $(BL31)

BL31_CBFS := $(CONFIG_CBFS_PREFIX)/bl31
$(BL31_CBFS)-file := $(BL31)
$(BL31_CBFS)-type := payload
$(BL31_CBFS)-compression := $(CBFS_COMPRESS_FLAG)
cbfs-files-y += $(BL31_CBFS)

check-ramstage-overlap-files += $(BL31_CBFS)

ifeq ($(CONFIG_ARM64_USE_SECURE_OS),y)

SECURE_OS_FILE := $(CONFIG_ARM64_SECURE_OS_FILE)
SECURE_OS_FILE_CBFS := $(CONFIG_CBFS_PREFIX)/secure_os
$(SECURE_OS_FILE_CBFS)-file := $(SECURE_OS_FILE)
$(SECURE_OS_FILE_CBFS)-type := stage
cbfs-files-y += $(SECURE_OS_FILE_CBFS)

check-ramstage-overlap-files += $(SECURE_OS_FILE_CBFS)

endif # CONFIG_ARM64_USE_SECURE_OS

endif # CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE

endif # CONFIG_ARCH_RAMSTAGE_ARM64
