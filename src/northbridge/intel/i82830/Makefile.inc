driver-y += northbridge.c
driver-y += vga.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += i82830_smihandler.c
