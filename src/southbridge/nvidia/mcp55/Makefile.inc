ifeq ($(CONFIG_SOUTHBRIDGE_NVIDIA_MCP55),y)

ramstage-y += mcp55.c
ramstage-y += azalia.c
ramstage-y += ht.c
ramstage-y += ide.c
ramstage-y += lpc.c
ramstage-y += nic.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += smbus.c
ramstage-y += usb2.c
ramstage-y += usb.c

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

ramstage-y += reset.c

romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c

ifeq ($(CONFIG_MCP55_USE_AZA),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c
endif

bootblock-y += romstrap.ld
bootblock-y += romstrap.S

endif
