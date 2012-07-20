################################################################################
## Subdirectories
################################################################################
subdirs-y += amd
subdirs-y += intel
subdirs-y += via

################################################################################
## Rules for building the microcode blob in CBFS
################################################################################

ifneq ($(CONFIG_CPU_MICROCODE_CBFS_NONE), y)

cbfs-files-y += cpu_microcode_blob.bin

cpu_microcode_blob.bin-type = 0x53

# External microcode file, or are we generating one ?
ifeq ($(CONFIG_CPU_MICROCODE_CBFS_EXTERNAL), y)
  cpu_microcode_blob.bin-file = $(CONFIG_CPU_MICROCODE_FILE)
else
  cpu_microcode_blob.bin-file = $(obj)/cpu_microcode_blob.bin
endif

$(obj)/cpu_microcode_blob.o: $$(cpu_microcode-objs)
	@printf "  CACAVACA     $< -> $@ \n"
	$(warning $(cpu_microcode-objs))
	$(warning $$(cpu_microcode-objs))
	cat $< > $@

$(obj)/cpu_microcode_blob.bin: $(obj)/cpu_microcode_blob.o
	@printf "  MICROCODE    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY) -j .data -O binary $< $@

endif