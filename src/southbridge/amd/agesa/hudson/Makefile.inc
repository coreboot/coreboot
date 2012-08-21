driver-y += hudson.c
driver-y += usb.c
driver-y += lpc.c
driver-y += sm.c
driver-y += ide.c
driver-y += sata.c
driver-y += hda.c
driver-y += pci.c
driver-y += pcie.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c
ramstage-y += reset.c
romstage-y += enable_usbdebug.c
romstage-y += early_setup.c

ramstage-$(CONFIG_HAVE_ACPI_RESUME) += spi.c

$(obj)/hudson.bin: $(top)/$(call strip_quotes $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(top)/$(call strip_quotes $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(top)/$(call strip_quotes $(CONFIG_HUDSON_GEC_FWM_FILE))
	echo "    Hudson FW  $@"
	$(src)/southbridge/amd/agesa/hudson/hudson_fwm.sh $(CONFIG_HUDSON_FWM_POSITION) $@ \
		"$(CONFIG_HUDSON_XHCI_FWM_FILE)" \
		"$(CONFIG_HUDSON_IMC_FWM_FILE)"  \
		"$(CONFIG_HUDSON_GEC_FWM_FILE)" #Add quotes to avoid skipping NULL string.

ifeq ($(CONFIG_HUDSON_FWM), y)
cbfs-files-y += hudson/fwm
hudson/fwm-file := $(obj)/hudson.bin
hudson/fwm-position := $(CONFIG_HUDSON_FWM_POSITION)
hudson/fwm-type := raw
endif

#ifeq ($(CONFIG_HUDSON_SATA_AHCI), y)
ifdef CONFIG_HUDSON_AHCI_ROM
stripped_ahci_rom_id = $(call strip_quotes,$(CONFIG_AHCI_ROM_ID))
cbfs-files-y += pci$(stripped_ahci_rom_id).rom
pci$(stripped_ahci_rom_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_ROM_FILE))
pci$(stripped_ahci_rom_id).rom-type := optionrom
#endif
endif
