ramstage-y += microcode.c
romstage-y += microcode.c

# Microcode for Family 16h
cbfs-files-$(CONFIG_CPU_AMD_FAM16H_MICROCODE) += microcode_amd.bin
microcode_amd.bin-file := src/mainboard/pcengines/apu2/microcode_amd
microcode_amd.bin-type := microcode
