ifeq ($(CONFIG_INTEL_TXT),y)

cbfs-files-y += txt_bios_acm.bin
txt_bios_acm.bin-file := $(CONFIG_INTEL_TXT_BIOSACM_FILE)
txt_bios_acm.bin-type := raw
txt_bios_acm.bin-align := $(CONFIG_INTEL_TXT_BIOSACM_ALIGNMENT)

ifneq ($(CONFIG_INTEL_TXT_SINITACM_FILE),"")
cbfs-files-y += txt_sinit_acm.bin
txt_sinit_acm.bin-file := $(CONFIG_INTEL_TXT_SINITACM_FILE)
txt_sinit_acm.bin-type := raw
txt_sinit_acm.bin-align := 0x10
txt_sinit_acm.bin-compression := lzma
endif

INTERMEDIATE+=add_acm_fit
add_acm_fit: $(obj)/coreboot.pre $(IFITTOOL)
	$(IFITTOOL) -r COREBOOT -a -n txt_bios_acm.bin -t 2 -s $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES) -f $<

endif
