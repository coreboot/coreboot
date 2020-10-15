ifeq ($(CONFIG_INTEL_CBNT_SUPPORT),y)

ifneq ($(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY),"")
cbfs-files-y += boot_policy_manifest.bin
boot_policy_manifest.bin-file := $(CONFIG_INTEL_CBNT_BOOT_POLICY_MANIFEST_BINARY)
boot_policy_manifest.bin-type := raw
boot_policy_manifest.bin-align := 0x10

INTERMEDIATE+=add_bpm_fit
add_bpm_fit: $(obj)/coreboot.pre $(IFITTOOL)
	$(IFITTOOL) -r COREBOOT -a -n boot_policy_manifest.bin -t 12 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif

ifneq ($(CONFIG_INTEL_CBNT_KEY_MANIFEST_BINARY),"")
cbfs-files-y += key_manifest.bin
key_manifest.bin-file := $(CONFIG_INTEL_CBNT_KEY_MANIFEST_BINARY)
key_manifest.bin-type := raw
key_manifest.bin-align := 0x10

INTERMEDIATE+=add_km_fit
add_km_fit: $(obj)/coreboot.pre $(IFITTOOL)
	$(IFITTOOL) -r COREBOOT -a -n key_manifest.bin -t 11 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<
endif

endif # CONFIG_INTEL_CBNT_SUPPORT
