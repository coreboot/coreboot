# SPDX-License-Identifier: BSD-3-Clause

VBOOT_BUILD_DIR ?= $(abspath $(obj)/external/vboot)
VBOOT_FW_LIB = $(VBOOT_BUILD_DIR)/vboot_fw.a
TLCL_LIB = $(VBOOT_BUILD_DIR)/tlcl.a

vboot_fw-objs += $(VBOOT_FW_LIB)
tlcl-objs += $(TLCL_LIB)

kconfig-to-binary=$(if $(strip $(1)),1,0)
vboot-fixup-includes = $(filter -I$(coreboottop)/%, $(1)) \
		       $(patsubst -I%,-I$(top)/%,\
		       $(patsubst include/%.h,$(top)/include/%.h,\
		       $(filter-out -I$(obj),\
		       $(filter-out -I$(coreboottop)/%,$(1)))))

VBOOT_FIRMWARE_ARCH-$(CONFIG_LP_ARCH_ARM) := arm
VBOOT_FIRMWARE_ARCH-$(CONFIG_LP_ARCH_X86_32) := x86
VBOOT_FIRMWARE_ARCH-$(CONFIG_LP_ARCH_X86_64) := x86_64
VBOOT_FIRMWARE_ARCH-$(CONFIG_LP_ARCH_ARM64) := arm64

ifneq ($(CONFIG_LP_ARCH_MOCK),)
VBOOT_FIRMWARE_ARCH-y := mock
else
VBOOT_CFLAGS += $(call vboot-fixup-includes,$(CFLAGS))
VBOOT_CFLAGS += -I$(abspath $(obj))
endif

ifeq ($(VBOOT_FIRMWARE_ARCH-y),)
$(error vboot requires architecture to be set in the configuration)
endif

# Enable vboot debug by default
VBOOT_CFLAGS += -DVBOOT_DEBUG

ifeq ($(CONFIG_LP_VBOOT_X86_RSA_ACCELERATION),y)
CPPFLAGS_common += -DVB2_X86_RSA_ACCELERATION
endif

$(VBOOT_FW_LIB): $(obj)/libpayload-config.h
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	+$(Q) FIRMWARE_ARCH="$(VBOOT_FIRMWARE_ARCH-y)" \
		CC="$(CC)" \
		CFLAGS="$(VBOOT_CFLAGS)" \
		$(MAKE) -C "$(VBOOT_SOURCE)" \
		TPM2_MODE=$(call kconfig-to-binary, $(CONFIG_LP_VBOOT_TPM2_MODE)) \
		X86_SHA_EXT=$(call kconfig-to-binary, $(CONFIG_LP_VBOOT_X86_SHA_EXT)) \
		VB2_X86_RSA_ACCELERATION=$(call kconfig-to-binary, $(CONFIG_LP_VBOOT_X86_RSA_ACCELERATION)) \
		ARMV8_CRYPTO_EXT=$(call kconfig-to-binary, $(CONFIG_LP_VBOOT_SHA_ARMV8_CE)) \
		ARM64_RSA_ACCELERATION=$(call kconfig-to-binary, $(CONFIG_LP_VBOOT_ARM64_RSA_ACCELERATION)) \
		UNROLL_LOOPS=1 \
		BUILD="$(VBOOT_BUILD_DIR)" \
		V=$(V) \
		$(VBOOT_BUILD_DIR)/vboot_fw.a tlcl

$(TLCL_LIB): $(VBOOT_FW_LIB)

.PHONY: $(VBOOT_FW_LIB) $(TLCL_LIB)
