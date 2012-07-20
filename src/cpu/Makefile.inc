################################################################################
## Subdirectories
################################################################################
subdirs-y += amd
subdirs-y += intel
subdirs-y += via


################################################################################
## Rules for building the microcode blob in CBFS
################################################################################

ifeq ($(CONFIG_CPU_MICROCODE_CBFS_EXTERNAL), y)
cbfs-files-y += cpu_microcode_blob.bin
microcode_blob.bin-file = $(CONFIG_CPU_MICROCODE_FILE)
microcode_blob.bin-type = 0x53
endif

ifneq ($(CONFIG_CPU_MICROCODE_CBFS_NONE), y)
cbfs-files-y += cpu_microcode_blob.bin
cpu_microcode_blob.bin-file = $(obj)/cpu_microcode_blob.bin
cpu_microcode_blob.bin-type = 0x53

$(obj)/cpu_microcode_blob.bin: $$(cpu_microcode-objs)
	@printf "    ASSM       $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -o $@ -L$(obj) $(cpu_microcode-objs)

endif
