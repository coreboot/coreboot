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

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# In our coreboot implementation, we concatenate the ROM images
# next to the ROMSIG. EC ROM should be 64K aligned.
# Final hudson.bin:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |USB3 ROM    |
# +-----------+---------------+----------------+------------+
# |                   USB3 ROM                              |
# +---------------------------------------------------------+
# |                   GEC ROM                               |
# +---------------------------------------------------------+
# |                   EC ROM                                |
# +---------------------------------------------------------+
#
$(obj)/hudson.bin: $(top)/$(strip $(patsubst "%", %, $(CONFIG_HUDSON_XHCI_FWM_FILE))) \
			$(top)/$(strip $(patsubst "%", %, $(CONFIG_HUDSON_IMC_FWM_FILE)))
	echo "    Hudson FW  $@"
	( \
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
	{ \
		printf ("%d ", cbstrtonum($$1)); \
	} ' ; \
	for fwm in "$(CONFIG_HUDSON_IMC_FWM_FILE)" "$(CONFIG_HUDSON_GEC_FWM_FILE)" "$(CONFIG_HUDSON_XHCI_FWM_FILE)" ; do \
		if [ -z $$fwm ]; then \
			awk 'BEGIN {printf ("0 ");}'; \
		else \
			[ -r $$fwm ] || { echo "$$fwm not found"; exit 1; };  \
			ls -l $$fwm | awk '{printf("%d ", $$5);}'; \
		fi \
	done ) | \
	awk ' \
	function print_raw_dword(number) \
	{ \
		printf ("%c%c%c%c", number % 0x100, number/0x100 % 0x100, number/0x10000 % 0x100, number/0x1000000); \
	} \
	{       # $$1=ROMSIG address, $$2=EC_ROM_size, $$3=GEC_ROM_size, $$4=USB3_ROM_size \
		print_raw_dword(0x55AA55AA); \
		imc_offset = $$1 + 16 + $$3 + 65535; \
		imc_offset = imc_offset - (imc_offset) % 65536; #align 64K \
		if ($$2) print_raw_dword(imc_offset);     else print_raw_dword(0); \
		if ($$3) print_raw_dword($$1 + 16 + $$3); else print_raw_dword(0); \
		if ($$4) print_raw_dword($$1 + 16);       else print_raw_dword(0); \
	} ' >  $@.tmp
	[ -z $(CONFIG_HUDSON_XHCI_FWM_FILE) ] || cat $(CONFIG_HUDSON_XHCI_FWM_FILE) >> $@.tmp
	[ -z $(CONFIG_HUDSON_GEC_FWM_FILE) ] || cat $(CONFIG_HUDSON_GEC_FWM_FILE) >> $@.tmp
	ls -l $@.tmp | awk '{ print $$5 }' | LC_ALL=C awk '{for (i=$$1; i<($$1+65535) - ($$1+65535)%65536; i++) {printf "%c", 255}}' >> $@.tmp # fill stub to make EC ROM aligned.
	[ -z $(CONFIG_HUDSON_IMC_FWM_FILE) ] || cat $(CONFIG_HUDSON_IMC_FWM_FILE) >> $@.tmp
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
