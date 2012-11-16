ramstage-y += northbridge.c
ramstage-y += vga.c

smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c
