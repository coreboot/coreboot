################################################################################
## Subdirectories
################################################################################
subdirs-y += amd
subdirs-y += armltd
subdirs-y += intel
subdirs-y += ti
subdirs-$(CONFIG_ARCH_X86) += x86
subdirs-$(CONFIG_CPU_QEMU_X86) += qemu-x86
subdirs-$(CONFIG_CPU_POWER9) += power9

$(eval $(call create_class_compiler,cpu_microcode,x86_32))
################################################################################
## Rules for building the microcode blob in CBFS
################################################################################

cbfs-files-$(CONFIG_USE_CPU_MICROCODE_CBFS_BINS) += cpu_microcode_blob.bin

ifeq ($(CONFIG_CPU_MICROCODE_CBFS_EXTERNAL_HEADER),y)
cbfs-files-y  += cpu_microcode_blob.bin
cpu_microcode_blob.bin-file = $(objgenerated)/microcode.bin

$(objgenerated)/microcode.bin: $(call strip_quotes,$(CONFIG_CPU_MICROCODE_HEADER_FILES))
	echo "   util/scripts/ucode_h_to_bin.sh $(objgenerated)/microcode.bin \"$(CONFIG_CPU_MICROCODE_HEADER_FILES)\""
	util/scripts/ucode_h_to_bin.sh $(objgenerated)/microcode.bin $(CONFIG_CPU_MICROCODE_HEADER_FILES)
endif

ifeq ($(CONFIG_CPU_MICROCODE_CBFS_EXTERNAL_BINS),y)
$(obj)/cpu_microcode_blob.bin: cpu_microcode_bins := $(call strip_quotes,$(CONFIG_CPU_UCODE_BINARIES))
endif
# otherwise `cpu_microcode_bins` should be filled by platform makefiles

# We just mash all microcode binaries together into one binary to rule them all.
# This approach assumes that the microcode binaries are properly padded, and
# their headers specify the correct size. This works fairly well on isolatied
# updates, such as Intel and some AMD microcode, but won't work very well if the
# updates are wrapped in a container, like AMD's microcode update container. If
# there is only one microcode binary (i.e. one container), then we don't have
# this issue, and this rule will continue to work.
$(obj)/cpu_microcode_blob.bin: $$(wildcard $$(cpu_microcode_bins)) $(DOTCONFIG)
	for bin in $(cpu_microcode_bins); do \
		if [ ! -f "$$bin" ]; then \
			echo "Microcode error: $$bin does not exist"; \
			NO_MICROCODE_FILE=1; \
		fi; \
	done; \
	if [ -n "$$NO_MICROCODE_FILE" ]; then \
		if [ -z "$(CONFIG_USE_BLOBS)" ] && [ -n "$(CONFIG_CPU_MICROCODE_CBFS_DEFAULT_BINS)" ]; then \
			echo "Try enabling binary-only repository in Kconfig 'General setup' menu."; \
		fi; \
		false; \
	fi
	$(if $(cpu_microcode_bins),,false) # fail if no file is given at all
	@printf "    MICROCODE  $(subst $(obj)/,,$(@))\n"
	@echo $(cpu_microcode_bins)
	cat $(cpu_microcode_bins) > $@

cpu_microcode_blob.bin-file ?= $(obj)/cpu_microcode_blob.bin
cpu_microcode_blob.bin-type := microcode
# The AMD LPC SPI DMA controller requires source files to be 64 byte aligned.
ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA),y)
cpu_microcode_blob.bin-align := 64
else
cpu_microcode_blob.bin-align := 16
endif

ifneq ($(CONFIG_CPU_MICROCODE_CBFS_LOC),)
cpu_microcode_blob.bin-COREBOOT-position := $(CONFIG_CPU_MICROCODE_CBFS_LOC)
endif
