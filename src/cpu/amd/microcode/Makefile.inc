romstage-$(CONFIG_CPU_MICROCODE_MULTIPLE_FILES) += microcode.c
ramstage-$(CONFIG_CPU_MICROCODE_MULTIPLE_FILES) += microcode.c

romstage-$(CONFIG_CPU_AMD_FAM16H_MICROCODE) += microcode_fam16h.c
ramstage-$(CONFIG_CPU_AMD_FAM16H_MICROCODE) += microcode_fam16h.c

# Microcode for Family 16h
cbfs-files-$(CONFIG_CPU_AMD_FAM16H_MICROCODE) += microcode_amd.bin
microcode_amd.bin-file := src/mainboard/pcengines/apu2/microcode_amd
microcode_amd.bin-type := microcode
