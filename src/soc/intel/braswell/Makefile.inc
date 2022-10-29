## SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SOC_INTEL_BRASWELL),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo
subdirs-y += ../../../cpu/intel/common

bootblock-y += gpio_support.c
bootblock-y += bootblock/bootblock.c
bootblock-y += lpc_init.c
bootblock-y += pmutil.c
bootblock-y += tsc_freq.c

romstage-y += gpio_support.c
romstage-y += iosf.c
romstage-y += memmap.c
romstage-y += pmutil.c
romstage-y += smbus.c
romstage-y += tsc_freq.c

postcar-y += memmap.c
postcar-y += iosf.c
postcar-y += tsc_freq.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += emmc.c
ramstage-y += fadt.c
ramstage-y += gpio.c
ramstage-y += gfx.c
ramstage-y += smbus.c

ramstage-y += gpio_support.c
ramstage-y += iosf.c
ramstage-y += lpe.c
ramstage-y += lpss.c
ramstage-y += memmap.c
ramstage-y += northcluster.c
ramstage-y += pcie.c
ramstage-y += pmutil.c
ramstage-y += ramstage.c
ramstage-y += sata.c
ramstage-y += scc.c
ramstage-y += sd.c
ramstage-y += smm.c
ramstage-y += southcluster.c
ramstage-y += tsc_freq.c
ramstage-y += xhci.c

# Remove as ramstage gets fleshed out
ramstage-y += placeholders.c
smm-y += lpc_init.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += tsc_freq.c

verstage-y += pmutil.c
verstage-y += tsc_freq.c

CPPFLAGS_common += -I$(src)/soc/intel/braswell/
CPPFLAGS_common += -I$(src)/soc/intel/braswell/include
CPPFLAGS_common += -I$(call strip_quotes,$(CONFIG_FSP_HEADER_PATH))

CPPFLAGS_common += -I3rdparty/blobs/mainboard/$(MAINBOARDDIR)

cpu_microcode_bins += $(wildcard 3rdparty/intel-microcode/intel-ucode/06-4c-*)

ifneq ($(CONFIG_VGA_BIOS_FILE),)
#we will assume that the vbios names will remain as they are now: vgabios.bin and vgabios_c0.bin
BRASWELL_C0_VBIOS= $(subst .bin,_c0.bin,$(call strip_quotes,$(CONFIG_VGA_BIOS_FILE)))

cbfs-files-$(CONFIG_VGA_BIOS) += pci8086,22b1.rom
pci8086,22b1.rom-file := $(BRASWELL_C0_VBIOS)
pci8086,22b1.rom-type := optionrom
endif # ifneq ($(CONFIG_VGA_BIOS_FILE),)

endif # ifeq ($(CONFIG_SOC_INTEL_BRASWELL),y)
