## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += fit.c

# The FIT table is generated as a separate CBFS file.
# The FIT pointer is reserved in fit.c and updated to point to the 'intel_fit'
# CBFS file using 'ifittool -F'.
# With a TOP_SWAP enabled bootblock the FIT pointer at the top swap offset
# will point to the 'intel_fit_ts' CBFS file.

cbfs-files-y += intel_fit
intel_fit-file := fit_table.c:struct
intel_fit-type := intel_fit
intel_fit-align := 16

ifeq ($(CONFIG_INTEL_TOP_SWAP_SEPARATE_REGIONS),y)
regions-for-file-intel_fit = BOOTBLOCK
regions-for-file-intel_fit_ts = TOPSWAP

TS_MCU_REGION = COREBOOT_TS
else
TS_MCU_REGION = COREBOOT
endif

$(call add_intermediate, set_fit_ptr, $(IFITTOOL))
	@printf "    UPDATE-FIT set FIT pointer to table\n"
	$(IFITTOOL) -f $< -F -n intel_fit -r $(BB_FIT_REGION) -c

FIT_ENTRY=$(call strip_quotes, $(CONFIG_INTEL_TOP_SWAP_FIT_ENTRY_FMAP_REG))

ifneq ($(CONFIG_UPDATE_IMAGE),y) # never update the bootblock

ifneq ($(CONFIG_CPU_MICROCODE_CBFS_NONE)$(CONFIG_CPU_INTEL_MICROCODE_CBFS_SPLIT_BINS),y)

$(call add_intermediate, add_mcu_fit, set_fit_ptr $(IFITTOOL))
	@printf "$(call regions-for-file,$(cpu_microcode_blob.bin))"
	@printf "    UPDATE-FIT Microcode\n"
	$(IFITTOOL) -f $< -a -n cpu_microcode_blob.bin -t 1 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -r $(BB_FIT_REGION) -R COREBOOT

# Second FIT in TOP_SWAP bootblock
ifeq ($(CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK),y)

$(call add_intermediate, set_ts_fit_ptr, $(IFITTOOL))
	@printf "    UPDATE-FIT Top Swap: set FIT pointer to table\n"
	$(IFITTOOL) -f $< -F -n intel_fit_ts -r $(TS_FIT_REGION) $(TS_OPTIONS)

$(call add_intermediate, add_ts_mcu_fit, set_ts_fit_ptr $(IFITTOOL))
	@printf "    UPDATE-FIT Top Swap: Microcode\n"
ifneq ($(FIT_ENTRY),)
	$(IFITTOOL) -f $< -A -n $(FIT_ENTRY) -t 1 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) $(TS_OPTIONS) -r $(TS_FIT_REGION)
endif # FIT_ENTRY
	$(IFITTOOL) -f $< -a -n cpu_microcode_blob.bin -t 1 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) $(TS_OPTIONS) -r $(TS_FIT_REGION) -R $(TS_MCU_REGION)

cbfs-files-y += intel_fit_ts
intel_fit_ts-file := fit_table.c:struct
intel_fit_ts-type := intel_fit
intel_fit_ts-align := 16

endif # CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK

endif # CONFIG_CPU_MICROCODE_CBFS_NONE

# Platform Boot Policy
ifeq ($(CONFIG_HAVE_PBP_BIN),y)

cbfs-files-y += pbp.bin
pbp.bin-file := $(call strip_quotes,$(CONFIG_PBP_BIN_PATH))
pbp.bin-type := raw

$(call add_intermediate, add_pbp_fit, set_fit_ptr $(IFITTOOL))
	@printf "    UPDATE-FIT Platform Boot Policy binary\n"
	$(IFITTOOL) -f $< -a -n pbp.bin -t 4 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -r COREBOOT
ifeq ($(CONFIG_INTEL_TOP_SWAP_SEPARATE_REGIONS),y)
	$(IFITTOOL) -f $< -a -n pbp.bin -t 4 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -r COREBOOT_TS
endif

endif # CONFIG_HAVE_PBP_BIN

endif # CONFIG_UPDATE_IMAGE
