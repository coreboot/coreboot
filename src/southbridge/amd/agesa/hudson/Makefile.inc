ramstage-y += hudson.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += sm.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += hda.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c
ramstage-y += reset.c
romstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
romstage-y += early_setup.c

ramstage-$(CONFIG_HAVE_ACPI_RESUME) += spi.c

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# EC ROM should be 64K aligned.
HUDSON_FWM_POSITION=$(shell printf %u $(CONFIG_HUDSON_FWM_POSITION))

#assume the cbfs header is less than 128 bytes.
ROMSIG_SIZE=16
ifeq ($(CONFIG_HUDSON_XHCI_FWM), y)
HUDSON_XHCI_POSITION=$(shell echo $(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) 128 | awk '{print $$1 + $$2 + $$3}')
XHCI_FWM_SIZE=$(word 5,$(shell ls -l $(CONFIG_HUDSON_XHCI_FWM_FILE)))
else
HUDSON_XHCI_POSITION=0
XHCI_FWM_SIZE=0
endif

ifeq ($(CONFIG_HUDSON_GEC_FWM), y)
HUDSON_GEC_POSITION=$(shell echo $(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) 128 \
		$(XHCI_FWM_SIZE) 128 | awk '{print $$1 + $$2 + $$3 + $$4 + $$5}')
GEC_FWM_SIZE=$(word 5,$(shell ls -l $(CONFIG_HUDSON_GEC_FWM_FILE)))
else
HUDSON_GEC_POSITION=0
GEC_FWM_SIZE=0
endif

ifeq ($(CONFIG_HUDSON_IMC_FWM), y)
HUDSON_IMC_POSITION_UNALIGN=$(shell echo $(HUDSON_FWM_POSITION) $(ROMSIG_SIZE) 128 \
		$(XHCI_FWM_SIZE) 128 \
		$(GEC_FWM_SIZE) 128 65535 | awk '{print $$1 + $$2 + $$3 + $$4 + $$5 + $$6 + $$7 + $$8}')
HUDSON_IMC_POSITION=$(shell echo $(HUDSON_IMC_POSITION_UNALIGN) | awk '{print $$1 - $$1 % 65536}')
else
HUDSON_IMC_POSITION=0
endif

$(obj)/coreboot_hudson_romsig.bin: $(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE)) \
			$(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE)) \
			$(obj)/config.h
	echo "    Hudson FW  $@"
	for fwm in 1437226410 \
		$(HUDSON_IMC_POSITION) \
		$(HUDSON_GEC_POSITION) \
		$(HUDSON_XHCI_POSITION) ; do \
		echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done > $@

ifeq ($(CONFIG_HUDSON_FWM), y)
cbfs-files-y += hudson/fwm
hudson/fwm-file := $(obj)/coreboot_hudson_romsig.bin
hudson/fwm-position := $(HUDSON_FWM_POSITION)
hudson/fwm-type := raw
endif

ifeq ($(CONFIG_HUDSON_XHCI_FWM), y)
cbfs-files-y += hudson/xhci
hudson/xhci-file := $(call strip_quotes, $(CONFIG_HUDSON_XHCI_FWM_FILE))
hudson/xhci-position := $(HUDSON_XHCI_POSITION)
hudson/xhci-type := raw

# check to see if the XHCI firmware exists
$(call strip_quotes,$(CONFIG_HUDSON_XHCI_FWM_FILE)):
	$(info )
	$(info ###########################################################################)
	$(info # ERROR: The XHCI firmware file specified does not exist:)
	$(info # $(CONFIG_HUDSON_XHCI_FWM_FILE) )
	$(info # Please enable binary blobs or configure a valid XHCI firmware path.)
	$(info ###########################################################################)
	$(info )
	__XHCI_FIRMWARE_ERROR___END_BUILD__
endif

ifeq ($(CONFIG_HUDSON_IMC_FWM), y)
cbfs-files-y += hudson/imc
hudson/imc-file := $(call strip_quotes, $(CONFIG_HUDSON_IMC_FWM_FILE))
hudson/imc-position := $(HUDSON_IMC_POSITION)
hudson/imc-type := raw

# check to see if the IMC firmware exists
$(call strip_quotes,$(CONFIG_HUDSON_IMC_FWM_FILE)):
	$(info )
	$(info ###########################################################################)
	$(info # ERROR: The IMC firmware file specified does not exist:)
	$(info # $(CONFIG_HUDSON_IMC_FWM_FILE) )
	$(info # Please enable binary blobs or config a valid IMC firmware path.)
	$(info ###########################################################################)
	$(info )
	__IMC_FIRMWARE_ERROR___END_BUILD__
endif

ifeq ($(CONFIG_HUDSON_GEC_FWM), y)
cbfs-files-y += hudson/gec
hudson/gec-file := $(call strip_quotes, $(CONFIG_HUDSON_GEC_FWM_FILE))
hudson/gec-position := $(HUDSON_GEC_POSITION)
hudson/gec-type := raw

# check to see if the GEC firmware exists
$(call strip_quotes,$(CONFIG_HUDSON_GEC_FWM_FILE)):
	$(info )
	$(info ###########################################################################)
	$(info # ERROR: The GEC firmware file specified does not exist:)
	$(info # $(CONFIG_HUDSON_GEC_FWM_FILE) )
	$(info # Please configure a valid GEC firmware path. If you don't have)
	$(info # this file, it may be obtained from your AMD representative.)
	$(info ###########################################################################)
	$(info )
	__GEC_FIRMWARE_ERROR___END_BUILD__
endif

ifdef CONFIG_HUDSON_AHCI_ROM
stripped_ahci_rom_id = $(call strip_quotes,$(CONFIG_AHCI_ROM_ID))
cbfs-files-y += pci$(stripped_ahci_rom_id).rom
pci$(stripped_ahci_rom_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_ROM_FILE))
pci$(stripped_ahci_rom_id).rom-type := optionrom

# check to see if the AHCI rom exists
$(call strip_quotes,$(CONFIG_AHCI_ROM_FILE)):
	$(info )
	$(info ###########################################################################)
	$(info # ERROR: The AHCI rom specified does not exist:)
	$(info # $(CONFIG_AHCI_ROM_FILE) )
	$(info # Please configure a valid AHCI rom path.  If you don't have)
	$(info # this file, it may be obtained from your AMD representative.)
	$(info ###########################################################################)
	$(info )
	__AHCI_ROM_ERROR___END_BUILD__
endif
