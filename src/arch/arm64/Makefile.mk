## SPDX-License-Identifier: GPL-2.0-only

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

decompressor-y += boot.c
bootblock-y += boot.c
decompressor-y += div0.c
bootblock-y += div0.c
decompressor-y += eabi_compat.c
bootblock-y += eabi_compat.c

decompressor-$(CONFIG_ARM64_USE_ARCH_TIMER) += arch_timer.c
bootblock-$(CONFIG_ARM64_USE_ARCH_TIMER) += arch_timer.c
bootblock-y += transition.c transition_asm.S

decompressor-y += memset.S
bootblock-y += memset.S
decompressor-y += memcpy.S
bootblock-y += memcpy.S
decompressor-y += memmove.S
bootblock-y += memmove.S

# Build the bootblock

$(objcbfs)/bootblock.debug: $$(bootblock-objs) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(bootblock-objs)) --end-group -T $(call src-to-obj,bootblock,$(CONFIG_MEMLAYOUT_LD_FILE))

$(objcbfs)/decompressor.debug: $$(decompressor-objs) $(obj)/config.h
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_bootblock) $(LDFLAGS_bootblock) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(decompressor-objs)) --end-group -T $(call src-to-obj,decompressor,$(CONFIG_MEMLAYOUT_LD_FILE))

endif # CONFIG_ARCH_BOOTBLOCK_ARM64

###############################################################################
# verification stage
###############################################################################

ifeq ($(CONFIG_ARCH_VERSTAGE_ARM64),y)

$(objcbfs)/verstage.debug: $$(verstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_verstage) $(LDFLAGS_verstage) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(verstage-objs)) --end-group -T $(call src-to-obj,verstage,$(CONFIG_MEMLAYOUT_LD_FILE))

verstage-y += boot.c
verstage-y += div0.c
verstage-y += eabi_compat.c
verstage-$(CONFIG_ARM64_USE_ARCH_TIMER) += arch_timer.c
verstage-y += memset.S
verstage-y += memcpy.S
verstage-y += memmove.S

verstage-y += transition.c transition_asm.S

endif # CONFIG_ARCH_VERSTAGE_ARM64

################################################################################
# romstage
################################################################################

ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)

romstage-y += boot.c
romstage-y += div0.c
romstage-y += eabi_compat.c
romstage-$(CONFIG_ARM64_USE_ARCH_TIMER) += arch_timer.c
romstage-y += memset.S
romstage-y += memcpy.S
romstage-y += memmove.S
romstage-y += ramdetect.c
romstage-y += romstage.c
romstage-y += transition.c transition_asm.S

rmodules_arm64-y += memset.S
rmodules_arm64-y += memcpy.S
rmodules_arm64-y += memmove.S
rmodules_arm64-y += eabi_compat.c

$(objcbfs)/romstage.debug: $$(romstage-objs)
	@printf "    LINK       $(subst $(obj)/,,$(@))\n"
	$(LD_romstage) $(LDFLAGS_romstage) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(romstage-objs)) --end-group -T $(call src-to-obj,romstage,$(CONFIG_MEMLAYOUT_LD_FILE))

endif # CONFIG_ARCH_ROMSTAGE_ARM64

################################################################################
# ramstage
################################################################################

ifeq ($(CONFIG_ARCH_RAMSTAGE_ARM64),y)

ramstage-y += div0.c
ramstage-y += eabi_compat.c
ramstage-y += boot.c
ramstage-y += tables.c
ramstage-y += ramdetect.c
ramstage-$(CONFIG_ARM64_USE_ARCH_TIMER) += arch_timer.c
ramstage-y += memset.S
ramstage-y += memcpy.S
ramstage-y += memmove.S
ramstage-$(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE) += bl31.c
ramstage-y += transition.c transition_asm.S
ramstage-$(CONFIG_PAYLOAD_FIT_SUPPORT) += fit_payload.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-y += dma.c

rmodules_arm64-y += memset.S
rmodules_arm64-y += memcpy.S
rmodules_arm64-y += memmove.S
rmodules_arm64-y += eabi_compat.c
rmodules_arm64-$(CONFIG_ARM64_USE_ARCH_TIMER) += arch_timer.c

ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/mainboard.c)

# Build the ramstage

$(objcbfs)/ramstage.debug: $$(ramstage-objs)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(LD_ramstage) $(LDFLAGS_ramstage) -o $@ -L$(obj) --whole-archive --start-group $(filter-out %.ld,$(ramstage-objs)) --end-group -T $(call src-to-obj,ramstage,$(CONFIG_MEMLAYOUT_LD_FILE))

# Build ARM Trusted Firmware (BL31)

ifeq ($(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE),y)

ifeq ($(CONFIG_ARM64_BL31_EXTERNAL_FILE),"")

BL31_SOURCE := $(top)/3rdparty/arm-trusted-firmware
BL31_BUILD := $(abspath $(obj)/3rdparty/arm-trusted-firmware)
BL31_TARGET := $(BL31_BUILD)/bl31/bl31.elf

ifeq ($(V),1)
BL31_MAKEARGS += V=1
endif

# Build ARM TF in debug mode (with assertions) if coreboot has hard assertions
ifeq ($(CONFIG_FATAL_ASSERTS),y)
BL31_MAKEARGS += DEBUG=1
endif # CONFIG_FATAL_ASSERTS

# ARM TF's VERBOSE (50) is *very* spammy, so default to INFO (40)
BL31_MAKEARGS += LOG_LEVEL=40

# Always enable crash reporting, even on a release build
BL31_MAKEARGS += CRASH_REPORTING=1

# Enable coreboot-specific features like CBMEM console support
BL31_MAKEARGS += COREBOOT=1

# Avoid build/release|build/debug distinction by overriding BUILD_PLAT directly
BL31_MAKEARGS += BUILD_PLAT="$(BL31_BUILD)"

# Force making .d files and output directories even though target is not 'bl31'
BL31_MAKEARGS += IS_ANYTHING_TO_BUILD=1

# Set a consistent build timestamp: the same coreboot has
BL31_MAKEARGS += BUILD_MESSAGE_TIMESTAMP='"$(shell sed -n 's/^.define COREBOOT_BUILD\>.*"\(.*\)".*/\1/p' $(obj)/build.h)"'

BL31_CFLAGS := -fno-pic -fno-stack-protector -Wno-deprecated-declarations -Wno-unused-function
BL31_LDFLAGS := -Wl,--emit-relocs

BL31 := $(obj)/bl31.elf

$(BL31): $(obj)/build.h
	printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	+unset AS AR CC CPP OC OD LD; \
	CROSS_COMPILE="$(CROSS_COMPILE_arm64)" \
	CFLAGS="$(BL31_CFLAGS)" \
	LDFLAGS="$(BL31_LDFLAGS)" \
	$(MAKE) -C $(BL31_SOURCE) $(BL31_MAKEARGS) $(BL31_TARGET) DISABLE_PEDANTIC=1
	mv $(BL31_TARGET) $@

.PHONY: $(BL31)

else

BL31 := $(call strip_quotes,$(CONFIG_ARM64_BL31_EXTERNAL_FILE))

endif # CONFIG_ARM64_BUILD_ARM_TRUSTED_FIRMWARE


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
