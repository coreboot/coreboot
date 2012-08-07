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

$(obj)/hudson.bin:
	echo $(CONFIG_HUDSON_FWM_POSITION) | LC_ALL=C awk ' \
	function cbstrtonum(str,    n,ret,i,c,k) \
	{ \
		str = substr(str, 3); \
		n = length(str); \
		ret = 0; \
		for (i = 1; i <= n; i++) { \
			c = substr(str, i, 1); \
			c = tolower(c); \
			k = index("123456789abcdef", c); \
			ret = ret * 16 + k; \
		} \
		return ret; \
	} \
	function print_raw_32bit(number) \
	{ \
		printf ("%c%c%c%c", number % 0x100, number/0x100 % 0x100, number/0x10000 % 0x100, number/0x1000000); \
	} \
	{ #main \
		print_raw_32bit(0x55AA55AA) \
		print_raw_32bit(cbstrtonum($$1) + 65536) #imc \
		print_raw_32bit(0)  #gec \
		print_raw_32bit(cbstrtonum($$1) + 16) #xhci \
	}' >  $@.tmp
	[ -z $(CONFIG_HUDSON_XHCI_FWM_FILE) ] || cat $(CONFIG_HUDSON_XHCI_FWM_FILE) >> $@.tmp
	expr 65536 - `ls -l $@.tmp | awk '{ print $$5 }'`  | LC_ALL=C awk '{for (i=0; i<$$1; i++) {printf "%c", 255}}' >> $@.tmp # fill stub
	[ -z $(CONFIG_HUDSON_IMC_FWM_FILE) ] || cat $(CONFIG_HUDSON_IMC_FWM_FILE) >> $@.tmp
	#TODO: GEC
	mv -f $@.tmp $@

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
