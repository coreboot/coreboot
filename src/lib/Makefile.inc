

romstage-y += memset.c
romstage-y += memcpy.c
romstage-y += memcmp.c
romstage-y += cbfs.c
romstage-y += lzma.c
#romstage-y += lzmadecode.c
romstage-$(CONFIG_CACHE_AS_RAM) += ramtest.c
romstage-$(CONFIG_HAVE_ACPI_RESUME) += cbmem.c
romstage-$(CONFIG_CONSOLE_SERIAL8250) += uart8250.c
romstage-$(CONFIG_CONSOLE_NE2K) += ne2k.c
romstage-$(CONFIG_CONSOLE_NE2K) += compute_ip_checksum.c
romstage-$(CONFIG_USBDEBUG) += usbdebug.c

ramstage-y += memset.c
ramstage-y += memcpy.c
ramstage-y += memcmp.c
ramstage-y += memmove.c
ramstage-y += malloc.c
ramstage-y += delay.c
ramstage-y += fallback_boot.c
ramstage-y += compute_ip_checksum.c
ramstage-y += version.c
ramstage-y += cbfs.c
ramstage-y += lzma.c
#ramstage-y += lzmadecode.c
ramstage-y += gcc.c
ramstage-y += clog2.c
ramstage-y += cbmem.c
ramstage-$(CONFIG_CONSOLE_SERIAL8250) += uart8250.c
ramstage-$(CONFIG_USBDEBUG) += usbdebug.c
ramstage-$(CONFIG_BOOTSPLASH) += jpeg.c

driver-$(CONFIG_CONSOLE_NE2K) += ne2k.c

smm-y += memcpy.c
smm-$(CONFIG_CONSOLE_SERIAL8250) += uart8250.c

$(obj)/lib/version.ramstage.o : $(obj)/build.h
