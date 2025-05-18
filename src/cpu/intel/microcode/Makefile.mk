## SPDX-License-Identifier: GPL-2.0-only

bootblock-$(CONFIG_MICROCODE_UPDATE_PRE_RAM) += microcode_asm.S

bootblock-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c
ramstage-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c
romstage-$(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS) += microcode.c

# Pack individual microcodes per CPUID from CONFIG_CPU_INTEL_UCODE_SPLIT_BINARIES directory into the CBFS.
ifeq ($(CONFIG_CPU_INTEL_MICROCODE_CBFS_SPLIT_BINS),y)
microcode-params-dir := $(call strip_quotes,$(CONFIG_CPU_INTEL_UCODE_SPLIT_BINARIES))/
microcode-params := $(shell find "$(microcode-params-dir)" -type f -exec basename {} \;)

# Ensure microcode-params is not empty
ifeq ($(microcode-params),)
$(error "microcode-params is empty. Ensure CONFIG_CPU_INTEL_UCODE_SPLIT_BINARIES is set correctly and contains valid files.")
endif

# Make "cpu_microcode_$(CPUID).bin" file entry into the FIT table
$(call add_intermediate, add_mcu_fit, set_fit_ptr $(IFITTOOL))
	$(foreach params, $(microcode-params), $(shell $(IFITTOOL) -f $< -a -n $(params) -t 1 \
		-s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -r COREBOOT)) true

# Add "cpu_microcode_$(CPUID).bin" file into the CBFS
$(foreach params,$(microcode-params), \
	$(eval cbfs-files-y += $(params)) \
	$(eval $(params)-file := $(microcode-params-dir)/$(params)) \
	$(eval $(params)-type := microcode) \
	$(eval $(params)-align := 16) \
)

endif
