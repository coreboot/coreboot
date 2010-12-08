driver-y += northbridge.c
driver-y += vga.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
