################################################################################
## Subdirectories
################################################################################
subdirs-y += amd
subdirs-y += dmp
subdirs-y += armltd
subdirs-y += intel
subdirs-y += samsung
subdirs-y += ti
subdirs-y += via
subdirs-y += x86
subdirs-$(CONFIG_CPU_QEMU_X86) += qemu-x86

################################################################################
## Rules for building the microcode blob in CBFS
################################################################################

cpu_ucode_cbfs_name = cpu_microcode_blob.bin

# External microcode file, or are we generating one ?
ifeq ($(CONFIG_CPU_MICROCODE_CBFS_EXTERNAL), y)
cpu_ucode_cbfs_file = $(call strip_quotes,$(CONFIG_CPU_MICROCODE_FILE))
cbfs_include_ucode = y
endif

ifeq ($(CONFIG_CPU_MICROCODE_CBFS_GENERATE), y)
cpu_ucode_cbfs_file = $(obj)/cpu_microcode_blob.bin
cbfs_include_ucode = y
endif

# In case we have more than one "source" (cough) files containing microcode, we
# link them together in one large blob, so that we get all the microcode updates
# in one file. This makes it easier for objcopy in the final step.
# The --entry=0 is just here to suppress the LD warning. It does not affect the
# final microcode file.
$(obj)/cpu_microcode_blob.o: $$(cpu_microcode-objs)
	@printf "    LD         $(subst $(obj)/,,$(@))\n"
	$(LD) -static --entry=0 $< -o $@

# We have a lot of useless data in the large blob, and we are only interested in
# the data section, so we only copy that part to the final microcode file
$(obj)/cpu_microcode_blob.bin: $(obj)/cpu_microcode_blob.o
	@printf "  MICROCODE    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY) -j .data -O binary $< $@

ifeq ($(cbfs_include_ucode),y)
# Add CPU microcode to specified rom image $(1)
add-cpu-microcode-to-cbfs = \
	$(CBFSTOOL) $(1) locate -f $(cpu_ucode_cbfs_file) -n $(cpu_ucode_cbfs_name) -a 16 | xargs $(CBFSTOOL) $(1) add -n $(cpu_ucode_cbfs_name) -f $(cpu_ucode_cbfs_file) -t 0x53 -b
else
add-cpu-microcode-to-cbfs = true
endif
