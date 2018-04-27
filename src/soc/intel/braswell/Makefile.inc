ifeq ($(CONFIG_SOC_INTEL_BRASWELL),y)

subdirs-y += romstage
subdirs-y += ../../../cpu/x86/lapic
subdirs-y += ../../../cpu/x86/mtrr
subdirs-y += ../../../cpu/x86/smm
subdirs-y += ../../../cpu/x86/tsc
subdirs-y += ../../../cpu/intel/microcode
subdirs-y += ../../../cpu/intel/turbo

romstage-y += gpio_support.c
romstage-y += iosf.c
romstage-y += lpc_init.c
romstage-y += memmap.c
romstage-y += pmutil.c
romstage-y += tsc_freq.c

ramstage-y += acpi.c
ramstage-y += chip.c
ramstage-y += cpu.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += emmc.c
ramstage-y += gpio.c
ifneq ($(CONFIG_RUN_FSP_GOP),y)
ramstage-y += gfx.c
endif
ramstage-y += gpio_support.c
ramstage-y += hda.c
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
ramstage-y += spi.c
ramstage-y += tsc_freq.c
ramstage-y += xhci.c

# Remove as ramstage gets fleshed out
ramstage-y += placeholders.c
smm-y += lpc_init.c
smm-y += pmutil.c
smm-y += smihandler.c
smm-y += spi.c
smm-y += tsc_freq.c

# cpu_microcode_bins += ???

CPPFLAGS_common += -I$(src)/soc/intel/braswell/
CPPFLAGS_common += -I$(src)/soc/intel/braswell/include
CPPFLAGS_common += -I$(src)/vendorcode/intel/fsp/fsp1_1/braswell

CPPFLAGS_common += -I3rdparty/blobs/mainboard/$(MAINBOARDDIR)

ifneq ($(CONFIG_VGA_BIOS_FILE),)
#we will assume that the vbios names will remain as they are now: vgabios.bin and vgabios_c0.bin
BRASWELL_C0_VBIOS= $(subst .bin,_c0.bin,$(call strip_quotes,$(CONFIG_VGA_BIOS_FILE)))

cbfs-files-$(CONFIG_VGA_BIOS) += pci8086,22b1.rom
pci8086,22b1.rom-file := $(BRASWELL_C0_VBIOS)
pci8086,22b1.rom-type := optionrom
endif # ifneq ($(CONFIG_VGA_BIOS_FILE),)

endif # ifeq ($(CONFIG_SOC_INTEL_BRASWELL),y)
