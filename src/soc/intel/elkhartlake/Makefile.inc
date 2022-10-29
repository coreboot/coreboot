## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_ELKHARTLAKE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

# all (bootblock, verstage, romstage, postcar, ramstage)
all-y += gspi.c
all-y += i2c.c
all-y += pmutil.c
all-y += spi.c
all-y += uart.c

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/pch.c
bootblock-y += bootblock/report_platform.c
bootblock-y += espi.c
bootblock-y += gpio.c
bootblock-y += p2sb.c

romstage-y += espi.c
romstage-y += gpio.c
romstage-y += meminit.c
romstage-y += reset.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += espi.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += gpio.c
ramstage-y += lockdown.c
ramstage-y += p2sb.c
ramstage-y += pmc.c
ramstage-y += reset.c
ramstage-y += systemagent.c
ramstage-y += sd.c
ramstage-$(CONFIG_EHL_TSN_DRIVER) += tsn_gbe.c
ramstage-y += me.c

smm-y += gpio.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c

verstage-y += gpio.c

CPPFLAGS_common += -I$(src)/soc/intel/elkhartlake
CPPFLAGS_common += -I$(src)/soc/intel/elkhartlake/include

# B0 stepping
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-96-01
cbfs-files-$(CONFIG_ADD_PSE_IMAGE_TO_CBFS) += pse.bin
pse.bin-file := $(CONFIG_PSE_IMAGE_FILE)
pse.bin-type := raw
pse.bin-compression := lzma

# Add a build time check if the PSE file size fits
ifeq ($(CONFIG_ADD_PSE_IMAGE_TO_CBFS),y)
ifeq ($(call int-gt,\
	$(call file-size,$(CONFIG_PSE_IMAGE_FILE))\
	$(shell printf "%d" $(call int-shift-left, $(CONFIG_PSE_FW_FILE_SIZE_KIB) 10))), 1)
$(error PSE binary larger than CONFIG_PSE_FW_FILE_SIZE_KIB.)
endif
endif # CONFIG_ADD_PSE_IMAGE_TO_CBFS

endif
