
CPPFLAGS_x86_32 += -I$(src)/southbridge/amd/agesa/hudson
CPPFLAGS_x86_64 += -I$(src)/southbridge/amd/agesa/hudson

romstage-y += smbus.c smbus_spd.c
ramstage-y += hudson.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += sm.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += hda.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sd.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-y += reset.c

ifneq ($(CONFIG_ROMCC_BOOTBLOCK),y)
bootblock-y += bootblock.c
bootblock-y += early_setup.c
bootblock-$(CONFIG_USBDEBUG) += enable_usbdebug.c
endif

romstage-y += enable_usbdebug.c
ramstage-y += enable_usbdebug.c
romstage-y += early_setup.c

ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-y += resume.c ramtop.c

romstage-y += ramtop.c
postcar-y += ramtop.c

romstage-y += imc.c
ramstage-y += imc.c

smm-y += smihandler.c smi_util.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c smi_util.c

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# EC ROM should be 64K aligned.

HUDSON_FWM_POSITION=$(call int-add, $(call int-subtract, 0xffffffff $(CONFIG_ROM_SIZE)) 0x20000 1)

ifdef CONFIG_HUDSON_AHCI_ROM
stripped_ahci_rom_id = $(call strip_quotes,$(CONFIG_AHCI_ROM_ID))
cbfs-files-y += pci$(stripped_ahci_rom_id).rom
pci$(stripped_ahci_rom_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_ROM_FILE))
pci$(stripped_ahci_rom_id).rom-type := optionrom
pci$(stripped_ahci_rom_id).rom-required := Hudson AHCI Option ROM (Contact your AMD representative)
endif

add_opt_prefix=$(if $(call strip_quotes, $(1)), $(2) $(call strip_quotes, $(1)), )

OPT_HUDSON_XHCI_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_XHCI_FWM_FILE), --xhci)
OPT_HUDSON_IMC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_IMC_FWM_FILE), --imc)
OPT_HUDSON_GEC_FWM_FILE=$(call add_opt_prefix, $(CONFIG_HUDSON_GEC_FWM_FILE), --gec)

$(obj)/amdfw.rom:	$(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE)) \
			$(AMDFWTOOL)
	rm -f $@
	@printf "    AMDFWTOOL  $(subst $(obj)/,,$(@))\n"
	$(AMDFWTOOL) \
		$(OPT_HUDSON_XHCI_FWM_FILE) \
		$(OPT_HUDSON_IMC_FWM_FILE) \
		$(OPT_HUDSON_GEC_FWM_FILE) \
		--flashsize $(CONFIG_ROM_SIZE) \
		--output	$@

cbfs-files-y += apu/amdfw
apu/amdfw-file := $(obj)/amdfw.rom
apu/amdfw-position := $(HUDSON_FWM_POSITION)
apu/amdfw-type := raw
