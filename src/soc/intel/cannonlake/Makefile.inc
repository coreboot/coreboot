## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE_BASE),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/intel/common

bootblock-y += bootblock/bootblock.c
bootblock-y += bootblock/pch.c
bootblock-y += pmutil.c
bootblock-y += bootblock/report_platform.c
bootblock-y += gspi.c
bootblock-y += i2c.c
bootblock-y += spi.c
bootblock-y += lpc.c
bootblock-y += p2sb.c
bootblock-y += uart.c

romstage-y += cnl_memcfg_init.c
romstage-y += gspi.c
romstage-y += i2c.c
romstage-y += lpc.c
romstage-y += pmutil.c
romstage-y += reset.c
romstage-y += spi.c
romstage-y += uart.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-y += dptf.c
ramstage-y += elog.c
ramstage-y += finalize.c
ramstage-y += fsp_params.c
ramstage-y += graphics.c
ramstage-y += gspi.c
ramstage-y += i2c.c
ramstage-y += lockdown.c
ramstage-y += lpc.c
ramstage-y += me.c
ramstage-y += nhlt.c
ramstage-y += p2sb.c
ramstage-y += pmc.c
ramstage-y += pmutil.c
ramstage-y += reset.c
ramstage-y += spi.c
ramstage-y += systemagent.c
ramstage-y += uart.c
ramstage-y += vr_config.c
ramstage-y += sd.c
ramstage-y += xhci.c

smm-y += elog.c
smm-y += p2sb.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += uart.c
smm-y += xhci.c

postcar-y += pmutil.c
postcar-y += i2c.c
postcar-y += gspi.c
postcar-y += spi.c
postcar-y += uart.c

verstage-y += gspi.c
verstage-y += i2c.c
verstage-y += pmutil.c
verstage-y += spi.c
verstage-y += uart.c

ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE_PCH_H),y)
bootblock-y += gpio_cnp_h.c
romstage-y += gpio_cnp_h.c
ramstage-y += gpio_cnp_h.c
smm-y += gpio_cnp_h.c
verstage-y += gpio_cnp_h.c
else
bootblock-y += gpio.c
romstage-y += gpio.c
ramstage-y += gpio.c
smm-y += gpio.c
verstage-y += gpio.c
endif

bootblock-y += gpio_common.c
ramstage-y += gpio_common.c

ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE),y)
# Not yet in intel-microcode repo
#cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-66-*)
else ifeq ($(CONFIG_SOC_INTEL_COFFEELAKE),y)
ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE_PCH_H),y)
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0a
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0b
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0c
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-9e-0d
else
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8e-0a
endif
else ifeq ($(CONFIG_SOC_INTEL_WHISKEYLAKE),y)
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8e-0b
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8e-0c
else ifeq ($(CONFIG_SOC_INTEL_COMETLAKE),y)
ifeq ($(CONFIG_SOC_INTEL_CANNONLAKE_PCH_H),y)
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-a5-02
else
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-8e-0c
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-a6-00
cpu_microcode_bins += 3rdparty/intel-microcode/intel-ucode/06-a6-01
endif
endif

CPPFLAGS_common += -I$(src)/soc/intel/cannonlake
CPPFLAGS_common += -I$(src)/soc/intel/cannonlake/include

# DSP firmware settings files.
NHLT_BLOB_PATH = 3rdparty/blobs/soc/intel/cnl/nhlt-blobs
DMIC_1CH_48KHZ_16B = dmic-1ch-48khz-16b.bin
DMIC_2CH_48KHZ_16B = dmic-2ch-48khz-16b.bin
DMIC_4CH_48KHZ_16B = dmic-4ch-48khz-16b.bin
MAX98357_RENDER = max98357-render-2ch-48khz-24b.bin
DA7219_RENDER_CAPTURE = dialog-2ch-48khz-24b.bin
MAX98373_RENDER_24B = max98373-render-2ch-48khz-24b.bin
MAX98373_RENDER_16B = max98373-render-2ch-48khz-16b.bin

cbfs-files-$(CONFIG_NHLT_DMIC_1CH_16B) += $(DMIC_1CH_48KHZ_16B)
$(DMIC_1CH_48KHZ_16B)-file := $(NHLT_BLOB_PATH)/$(DMIC_1CH_48KHZ_16B)
$(DMIC_1CH_48KHZ_16B)-type := raw

cbfs-files-$(CONFIG_NHLT_DMIC_2CH_16B) += $(DMIC_2CH_48KHZ_16B)
$(DMIC_2CH_48KHZ_16B)-file := $(NHLT_BLOB_PATH)/$(DMIC_2CH_48KHZ_16B)
$(DMIC_2CH_48KHZ_16B)-type := raw

cbfs-files-$(CONFIG_NHLT_DMIC_4CH_16B) += $(DMIC_4CH_48KHZ_16B)
$(DMIC_4CH_48KHZ_16B)-file := $(NHLT_BLOB_PATH)/$(DMIC_4CH_48KHZ_16B)
$(DMIC_4CH_48KHZ_16B)-type := raw

cbfs-files-$(CONFIG_NHLT_MAX98357) += $(MAX98357_RENDER)
$(MAX98357_RENDER)-file := $(NHLT_BLOB_PATH)/$(MAX98357_RENDER)
$(MAX98357_RENDER)-type := raw

cbfs-files-$(CONFIG_NHLT_MAX98373) += $(MAX98373_RENDER_16B)
$(MAX98373_RENDER_16B)-file := $(NHLT_BLOB_PATH)/$(MAX98373_RENDER_16B)
$(MAX98373_RENDER_16B)-type := raw

cbfs-files-$(CONFIG_NHLT_MAX98373) += $(MAX98373_RENDER_24B)
$(MAX98373_RENDER_24B)-file := $(NHLT_BLOB_PATH)/$(MAX98373_RENDER_24B)
$(MAX98373_RENDER_24B)-type := raw

cbfs-files-$(CONFIG_NHLT_DA7219) += $(DA7219_RENDER_CAPTURE)
$(DA7219_RENDER_CAPTURE)-file := $(NHLT_BLOB_PATH)/$(DA7219_RENDER_CAPTURE)
$(DA7219_RENDER_CAPTURE)-type := raw

endif
