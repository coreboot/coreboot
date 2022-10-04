## SPDX-License-Identifier: GPL-2.0-only
subdirs-y += ./*

romstage-y += cpu.c

ramstage-y += cpu.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_SVI2) += svi2.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_SVI3) += svi3.c
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_UCODE) += update_microcode.c

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_UCODE),y)

define add-ucode-as-cbfs

# check for duplicate microcode files. Same sbin and bin ucode is allowed here though, because mendocino has a duplicate.
ifeq ($(cpu_microcode_$(2).bin-file), $(obj)/cpu_microcode_$(2).$(3))
    $$(info Tried to add ucode: $(1))
    $$(error Error: The cbfs filename "cpu_microcode_$(2).bin" is already used. Check your microcode patches for duplicates.)
endif

# offset 0x14 contains the size of the unwrapped ucode file
# .sbin files contain a 256 wrapper around the usual microcode file
$(obj)/cpu_microcode_$(2).$(3): $(1)
	echo $$< "->" $$@
	if   [ $(3) = "bin" ]; then \
	  cp $$< $$@; \
	elif [ $(3) = "sbin" ]; then \
	  size=$$$$(od --endian little --address-radix n --read-bytes 4 --skip-bytes 0x14 --format u4 $$<); \
	  dd status=none ibs=1 skip=256 count=$$$$((size)) if=$$< of=$$@; \
	fi

# if there is both a sbin and bin microcode only include the bin one to keep the old behaviour
ifeq ($(cpu_microcode_$(2).bin-file),)
    cbfs-files-y += cpu_microcode_$(2).bin
    cpu_microcode_$(2).bin-file := $(obj)/cpu_microcode_$(2).$(3)
    cpu_microcode_$(2).bin-type := microcode

    ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA),y)
        cpu_microcode_$(2).bin-align := 64
    else
        cpu_microcode_$(2).bin-align := 16
    endif
endif

endef

amd_microcode_bins  += $(wildcard ${FIRMWARE_LOCATION}/*U?odePatch*.bin)
amd_microcode_sbins += $(wildcard ${FIRMWARE_LOCATION}/*UcodePatch_*.sbin)

# Function to grab bytes from a file and format them as desired
# $(call extract-bytes,filename,bytes-to-read,offset-to-bytes,output-format)
extract-bytes = $(shell echo $(shell od --endian little --address-radix n --read-bytes $(2) --skip-bytes $(3) --format $(4) $(1)))

$(foreach ucode, $(amd_microcode_bins), \
    $(eval $(call add-ucode-as-cbfs,$(ucode),$(call extract-bytes,$(ucode),2,0x18,x2),bin)))

$(foreach ucode, $(amd_microcode_sbins), \
    $(eval $(call add-ucode-as-cbfs,$(ucode),$(call extract-bytes,$(ucode),2,0x118,x2),sbin)))

endif #ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_UCODE),y)
