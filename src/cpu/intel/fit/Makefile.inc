bootblock-y += fit.S

FIT_ENTRY=$(call strip_quotes, $(CONFIG_INTEL_TOP_SWAP_FIT_ENTRY_FMAP_REG))

ifneq ($(CONFIG_UPDATE_IMAGE),y) # never update the bootblock

ifneq ($(CONFIG_CPU_MICROCODE_CBFS_NONE),y)

PHONY+=add_mcu_fit
INTERMEDIATE+=add_mcu_fit
add_mcu_fit: $(obj)/coreboot.pre $(IFITTOOL)
	@printf "    UPDATE-FIT Microcode\n"
	$(IFITTOOL) -f $< -a -n cpu_microcode_blob.bin -t 1 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -r COREBOOT

# Second FIT in TOP_SWAP bootblock
ifeq ($(CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK),y)

PHONY+=add_ts_mcu_fit
INTERMEDIATE+=add_ts_mcu_fit
add_ts_mcu_fit: $(obj)/coreboot.pre $(IFITTOOL)
	@printf "    UPDATE-FIT Top Swap: Microcode\n"
ifneq ($(FIT_ENTRY),)
	$(IFITTOOL) -f $< -A -n $(FIT_ENTRY) -t 1 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) $(TS_OPTIONS) -r COREBOOT
endif # FIT_ENTRY
	$(IFITTOOL) -f $< -a -n cpu_microcode_blob.bin -t 1 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) $(TS_OPTIONS) -r COREBOOT

endif # CONFIG_INTEL_ADD_TOP_SWAP_BOOTBLOCK

endif # CONFIG_CPU_MICROCODE_CBFS_NONE

endif # CONFIG_UPDATE_IMAGE
