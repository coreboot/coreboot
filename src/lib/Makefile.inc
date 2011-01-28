ramstage-y += clog2.c
ramstage-y += uart8250.c
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
ramstage-y += cbmem.c

romstage-$(CONFIG_HAVE_ACPI_RESUME) += cbmem.c
romstage-y += uart8250.c
romstage-y += memset.c
romstage-y += memcpy.c
romstage-y += memcmp.c
romstage-y += cbfs.c
romstage-y += lzma.c
romstage-$(CONFIG_CACHE_AS_RAM) += ramtest.c
#romstage-y += lzmadecode.c
romstage-$(CONFIG_CONSOLE_NE2K) += ne2k.c
romstage-$(CONFIG_CONSOLE_NE2K) += compute_ip_checksum.c

driver-$(CONFIG_CONSOLE_NE2K) += ne2k.c

romstage-$(CONFIG_USBDEBUG) += usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += usbdebug.c

ramstage-$(CONFIG_BOOTSPLASH) += jpeg.c

smm-y += memcpy.c

$(obj)/lib/version.ramstage.o : $(obj)/build.h
