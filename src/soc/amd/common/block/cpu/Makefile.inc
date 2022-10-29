## SPDX-License-Identifier: GPL-2.0-only
subdirs-y += ./*
ramstage-$(CONFIG_SOC_AMD_COMMON_BLOCK_UCODE) += update_microcode.c
romstage-y += cpu.c
ramstage-y += cpu.c

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_UCODE),y)
define add-ucode-as-cbfs
cbfs-files-y += cpu_microcode_$(2).bin
cpu_microcode_$(2).bin-file := $(1)
cpu_microcode_$(2).bin-type := microcode

ifeq ($(CONFIG_SOC_AMD_COMMON_BLOCK_LPC_SPI_DMA),y)
cpu_microcode_$(2).bin-align := 64
else
cpu_microcode_$(2).bin-align := 16
endif
endef

$(foreach ucode,$(amd_microcode_bins),$(eval $(call add-ucode-as-cbfs,$(ucode),$(shell hexdump -n 2 -s 0x18 -e '"%x"' $(ucode)))))
endif
