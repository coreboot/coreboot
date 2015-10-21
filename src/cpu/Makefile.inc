################################################################################
## Subdirectories
################################################################################
subdirs-y += allwinner
subdirs-y += amd
subdirs-y += dmp
subdirs-y += armltd
subdirs-y += imgtec
subdirs-y += intel
subdirs-y += ti
subdirs-y += via
subdirs-$(CONFIG_ARCH_X86) += x86
subdirs-$(CONFIG_CPU_QEMU_X86) += qemu-x86

$(eval $(call create_class_compiler,cpu_microcode,x86_32))
################################################################################
## Rules for building the microcode blob in CBFS
################################################################################

# External microcode file, or are we generating one ?
ifeq ($(CONFIG_CPU_MICROCODE_CBFS_EXTERNAL), y)
cpu_ucode_cbfs_file = $(call strip_quotes,$(CONFIG_CPU_MICROCODE_FILE))
cbfs_include_ucode = y
endif

ifeq ($(CONFIG_CPU_MICROCODE_CBFS_GENERATE), y)
ifneq ($(CONFIG_CPU_MICROCODE_MULTIPLE_FILES), y)
cpu_ucode_cbfs_file = $(obj)/cpu_microcode_blob.bin
cbfs_include_ucode = y
endif
endif

# We just mash all microcode binaries together into one binary to rule them all.
# This approach assumes that the microcode binaries are properly padded, and
# their headers specify the correct size. This works fairly well on isolatied
# updates, such as Intel and some AMD microcode, but won't work very well if the
# updates are wrapped in a container, like AMD's microcode update container. If
# there is only one microcode binary (i.e. one container), then we don't have
# this issue, and this rule will continue to work.
$(obj)/cpu_microcode_blob.bin: $$(cpu_microcode_bins)
	@printf "    MICROCODE  $(subst $(obj)/,,$(@))\n"
	@echo $(cpu_microcode_bins)
	cat /dev/null $+ > $@

cbfs-files-$(cbfs_include_ucode) += cpu_microcode_blob.bin
cpu_microcode_blob.bin-file := $(cpu_ucode_cbfs_file)
cpu_microcode_blob.bin-type := microcode

ifneq ($(CONFIG_CPU_MICROCODE_CBFS_LOC),)
cpu_microcode_blob.bin-position := $(CONFIG_CPU_MICROCODE_CBFS_LOC)
else
cpu_microcode_blob.bin-align := 16
endif
